#include "physicsJoint.h"
#include "physicsRigidBody.h"
#include <math.h>

// ----------------------------------------------------------------------
//
// Distance constraint equation:
//
// C : ||pB - pA|| - L = 0.
//
// Differentiating so we can solve w.r.t. velocity:
//
// C' : dC/dt = (((wB X rB) + vB) - ((wA X rA) + vA)) . n = 0,
//
// where n = (pB - pA)/||pB - pA|| and the p terms are the
// transformed anchor points in global space.
//
// ----------------------------------------------------------------------
//
// Given the velocity vector
//
//     [vA]
//     [wA]
// V = [vB]
//     [wB]
//
// and the identity JV = C', we can solve for the Jacobian J:
//
// J = [-n, -(rA X n), n, (rB X n)].
//
// Finally, adding a potential bias term, we have
//
// C' : JV + b >= 0.
//
// ----------------------------------------------------------------------
//
// The effective mass for the constraint is given by (JM^{-1})J^T,
// where M^{-1} is the inverse mass matrix and J^T is the transposed
// Jacobian.
//
//          [mA^{-1}   0      0      0   ]
//          [   0   IA^{-1}   0      0   ]
// M^{-1} = [   0      0   mB^{-1}   0   ]
//          [   0      0      0   IB^{-1}],
//
//       [    -n   ]
//       [-(rA X n)]
// J^T = [     n   ]
//       [ (rB X n)].
//
// Expanding results in
//
// (JM^{-1})J^T = mA^{-1} + mB^{-1} + ((rA X n) . (IA^{-1} * (rA X n))) + ((rB X n) . (IB^{-1} * (rB X n))).
//
// ----------------------------------------------------------------------
//
// Semi-implicit Euler:
//
// V   = V_i + dt * M^{-1} * F,
// V_f = V   + dt * M^{-1} * P.
//
// Where V_i is the initial velocity vector, V_f is the final
// velocity vector, F is the external force on the body (e.g.
// gravity), P is the constraint force and dt is the timestep.
//
// Using P = J^T * lambda and lambda' = dt * lambda, we can
// solve for the impulse magnitude (constraint Lagrange
// multiplier) lambda':
//
// JV_f + b = 0
// J(V + dt * M^{-1} * P) + b = 0
// JV + dt * (JM^{-1})P + b = 0
// JV + dt * (JM^{-1})J^T . lambda + b = 0
// dt * (JM^{-1})J^T . lambda = -(JV + b)
// dt * lambda = -(JV + b)/((JM^{-1})J^T)
// lambda' = -(JV + b)/((JM^{-1})J^T).
//
// ----------------------------------------------------------------------

void physJointDistanceInit(physJointDistance *const __RESTRICT__ joint, const vec3 anchorA, const vec3 anchorB, const float distance, const float stiffness, const float damping){
	joint->anchorA = anchorA;
	joint->anchorB = anchorB;
	joint->distance = distance;
	joint->stiffness = stiffness;
	joint->damping = damping;
	joint->gamma = 0.f;
	joint->bias = 0.f;
	joint->rA = g_vec3Zero;
	joint->rB = g_vec3Zero;
	joint->rAB = g_vec3Zero;
	joint->inverseEffectiveMass = 0.f;
	joint->impulseAccumulator = 0.f;
}

#ifdef PHYSICS_DISTANCE_JOINT_WARM_START
static __FORCE_INLINE__ void physJointDistanceWarmStart(physJointDistance *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	const vec3 impulse = vec3VMultS(joint->rAB, joint->impulseAccumulator);

	// Apply the accumulated impulse.
	// This helps the constraint converge to a solution faster.
	physRigidBodyApplyImpulseInverse(bodyA, joint->rA, impulse);
	physRigidBodyApplyImpulse(bodyB, joint->rB, impulse);

}
#endif

static __FORCE_INLINE__ float physJointDistanceGenerateEffectiveMass(const vec3 pointA, const mat3 inverseInertiaTensorA, const vec3 pointB, const mat3 inverseInertiaTensorB, const vec3 normal, const float inverseMassTotal){

	// Effective mass:
	// (JM^{-1})J^T = mA^{-1} + mB^{-1} + ((rA X n) . (IA^{-1} * (rA X n))) + ((rB X n) . (IB^{-1} * (rB X n)))
	const vec3 angularDeltaA = vec3Cross(pointA, normal);
	const vec3 angularDeltaB = vec3Cross(pointB, normal);
	return inverseMassTotal +
	       vec3Dot(angularDeltaA, mat3MMultV(inverseInertiaTensorA, angularDeltaA)) +
	       vec3Dot(angularDeltaB, mat3MMultV(inverseInertiaTensorB, angularDeltaB));

}

static __FORCE_INLINE__ void physJointDistancePersist(physJointDistance *const __RESTRICT__ joint, const physRigidBody *const __RESTRICT__ bodyA, const physRigidBody *const __RESTRICT__ bodyB){

	float distance;

	// Transform the anchor points.
	joint->rA = tfTransformDirection(
		bodyA->configuration,
		#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
		vec3VSubV(joint->anchorA, bodyA->centroidLocal)
		#else
		vec3VSubV(joint->anchorA, bodyA->base->centroid)
		#endif
	);
	joint->rB = tfTransformDirection(
		bodyB->configuration,
		#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
		vec3VSubV(joint->anchorB, bodyB->centroidLocal)
		#else
		vec3VSubV(joint->anchorB, bodyB->base->centroid)
		#endif
	);

	// Find the bodies' relative positions.
	// This is the direction impulses will
	// be applied in.
	// n = (pB - pA)/||pB - pA||
	//   = (cB + rB - cA - rA)/||cB + rB - cA - rA||
	joint->rAB = vec3VSubV(
		vec3VAddV(bodyB->centroidGlobal, joint->rB),
		vec3VAddV(bodyA->centroidGlobal, joint->rA)
	);

	// Normalize rAB and handle singularities.
	distance = vec3Magnitude(joint->rAB);
	if(distance > PHYSICS_JOINT_DISTANCE_LINEAR_SLOP){
		joint->rAB = vec3VMultS(joint->rAB, 1.f/distance);
	}else{
		vec3ZeroP(&joint->rAB);
	}

	// Calculate C for the bias term.
	// beta = hk/(hk + c)
	// bias = (beta/h)*C = C*h*k*gamma
	joint->bias = distance - joint->distance;

}

static __FORCE_INLINE__ void physJointDistanceGenerateBias(physJointDistance *const __RESTRICT__ joint, const physRigidBody *const __RESTRICT__ bodyA, const physRigidBody *const __RESTRICT__ bodyB, const float dt_s){

	// Calculate the effective mass (not the inverse),
	// as we need it for generating the bias.
	joint->inverseEffectiveMass = physJointDistanceGenerateEffectiveMass(
		joint->rA, bodyA->inverseInertiaTensorGlobal,
		joint->rB, bodyB->inverseInertiaTensorGlobal,
		joint->rAB, bodyA->inverseMass + bodyB->inverseMass
	);

	// Only use soft constraints if the angular frequency is greater than 0.
	if(joint->stiffness <= 0.f){

		joint->gamma = 0.f;
		joint->bias = 0.f;

	}else{

		const float hk = joint->stiffness * dt_s;

		// gamma = 1/(h(hk + c))
		joint->gamma = dt_s * (hk + joint->damping);
		joint->gamma = (joint->gamma != 0.f ? 1.f/joint->gamma : 0.f);

		// beta = hk/(hk + c)
		// bias = (beta/h)*C = C*h*k*gamma
		// Bias is set to C in physJointDistancePersist().
		joint->bias *= hk * joint->gamma;
		// Note that this isn't the inverse effective mass just yet.
		joint->inverseEffectiveMass += joint->gamma;

	}

	joint->inverseEffectiveMass = (joint->inverseEffectiveMass > 0.f ? 1.f/joint->inverseEffectiveMass : 0.f);

}

void physJointDistancePresolveConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float dt_s){

	// Initialize the constraints.
	physJointDistancePersist((physJointDistance *)joint, bodyA, bodyB);
	physJointDistanceGenerateBias((physJointDistance *)joint, bodyA, bodyB, dt_s);
	#ifdef PHYSICS_DISTANCE_JOINT_WARM_START
	physJointDistanceWarmStart((physJointDistance *)joint, bodyA, bodyB);
	#endif

}

void physJointDistanceSolveVelocityConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	float lambda;
	vec3 impulse;

	// Evaluate the constraint expression, JV.
	// C = ||pB - pA|| - L
	// C' = dC/dt = (((wB x rB) + vB) - ((wA x rA) + vA)) . n
	// JV = C'
	lambda = vec3Dot(
		vec3VSubV(
			vec3VAddV(vec3Cross(bodyB->angularVelocity, ((physJointDistance *)joint)->rB), bodyB->linearVelocity),
			vec3VAddV(vec3Cross(bodyA->angularVelocity, ((physJointDistance *)joint)->rA), bodyA->linearVelocity)
		),
		((physJointDistance *)joint)->rAB
	);

	// Calculate the impulse magnitude, i.e.
	// the constraint's Lagrange multiplier.
	// lambda = -(JV + b)/((JM^{-1})J^T)
	lambda = -((physJointDistance *)joint)->inverseEffectiveMass * (
		lambda + ((physJointDistance *)joint)->bias +
		((physJointDistance *)joint)->gamma * ((physJointDistance *)joint)->impulseAccumulator
	);
	((physJointDistance *)joint)->impulseAccumulator += lambda;

	// Calculate the normal impulse.
	impulse = vec3VMultS(((physJointDistance *)joint)->rAB, lambda);

	// Apply the normal impulse.
	physRigidBodyApplyImpulseInverse(bodyA, ((physJointDistance *)joint)->rA, impulse);
	physRigidBodyApplyImpulse(bodyB, ((physJointDistance *)joint)->rB, impulse);

}

return_t physJointDistanceSolveConfigurationConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	#ifdef PHYSICS_JOINT_DISTANCE_STABILIZER_GAUSS_SEIDEL

	// Only apply positional corrections if
	// soft constraints are not being used.
	if(((physJointDistance *)joint)->stiffness <= 0.f){

		// Retransform the anchor points.
		const vec3 rA = tfTransformDirection(
			bodyA->configuration,
			#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
			vec3VSubV(((physJointDistance *)joint)->anchorA, bodyA->centroidLocal)
			#else
			vec3VSubV(((physJointDistance *)joint)->anchorA, bodyA->base->centroid)
			#endif
		);
		const vec3 rB = tfTransformDirection(
			bodyB->configuration,
			#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
			vec3VSubV(((physJointDistance *)joint)->anchorB, bodyB->centroidLocal)
			#else
			vec3VSubV(((physJointDistance *)joint)->anchorB, bodyB->base->centroid)
			#endif
		);

		// Find the bodies' relative positions.
		// This is the direction impulses will
		// be applied in.
		// n = (pB - pA)
		//   = (cB + rB - cA - rA)
		vec3 rAB = vec3VSubV(
			vec3VSubV(
				vec3VAddV(bodyB->centroidGlobal, rB),
				bodyA->centroidGlobal
			),
			rA
		);

		{

			const float distance = vec3Magnitude(rAB);
			const float constraint = distance - ((physJointDistance *)joint)->distance;
			// Don't clamp equality constraints!
			//if(constraint <= -PHYSICS_JOINT_DISTANCE_MAXIMUM_LINEAR_CORRECTION){
			//	constraint = -PHYSICS_JOINT_DISTANCE_MAXIMUM_LINEAR_CORRECTION;
			//}else if(constraint > PHYSICS_JOINT_DISTANCE_MAXIMUM_LINEAR_CORRECTION){
			//	constraint = PHYSICS_JOINT_DISTANCE_MAXIMUM_LINEAR_CORRECTION;
			//}
			if(distance > PHYSICS_JOINT_DISTANCE_LINEAR_SLOP){

				// Calculate the new effective mass.
				float effectiveMass;

				vec3VDivS(rAB, distance);
				effectiveMass = physJointDistanceGenerateEffectiveMass(
					rA, bodyA->inverseInertiaTensorGlobal,
					rB, bodyB->inverseInertiaTensorGlobal,
					rAB, bodyA->inverseMass + bodyB->inverseMass
				);

				if(effectiveMass > 0.f){

					// Normalize and multiply by the impulse magnitude,
					// i.e. the constraint's Lagrange multiplier.
					rAB = vec3VMultS(rAB, -constraint * effectiveMass);

					// Apply the normal impulse.
					physRigidBodyApplyConfigurationImpulseInverse(bodyA, rA, rAB);
					physRigidBodyApplyConfigurationImpulse(bodyB, rB, rAB);

				}

			}

			return fabsf(constraint) <= PHYSICS_JOINT_DISTANCE_LINEAR_ERROR_THRESHOLD;

		}

	}

	#endif

	return 1;

}
