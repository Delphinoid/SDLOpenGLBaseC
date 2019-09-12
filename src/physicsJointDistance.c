#include "physicsJoint.h"
#include "physicsRigidBody.h"
#include <math.h>
#include "inline.h"

/*
** ----------------------------------------------------------------------
**
** Distance constraint equation:
**
** C = ||pB - pA|| - L = 0.
**
** Differentiating so we can solve w.r.t. velocity:
**
** C' = dC/dt = (((wB X rB) + vB) - ((wA X rA) + vA)) . n = 0,
**
** where n = (pB - pA)/||pB - pA|| and the p terms are the
** transformed anchor points in global space.
**
** ----------------------------------------------------------------------
**
** Given the velocity vector
**
**     [vA]
**     [wA]
** V = [vB]
**     [wB]
**
** and the identity JV = C', we can solve for the Jacobian J:
**
** J = [-n, -(rA X n), n, (rB X n)].
**
** Finally, adding a potential bias term, we have
**
** C' = JV + b >= 0.
**
** ----------------------------------------------------------------------
**
** The effective mass for the constraint is given by (JM^-1)J^T,
** where M^-1 is the inverse mass matrix and J^T is the transposed
** Jacobian.
**
**        [mA^-1  0    0    0  ]
**        [  0  IA^-1  0    0  ]
** M^-1 = [  0    0  mB^-1  0  ]
**        [  0    0    0  IB^-1],
**
**       [    -n   ]
**       [-(rA X n)]
** J^T = [     n   ]
**       [ (rB X n)].
**
** Expanding results in
**
** (JM^-1)J^T = mA^-1 + mB^-1 + ((rA X n) . (IA^-1 * (rA X n))) + ((rB X n) . (IB^-1 * (rB X n))).
**
** ----------------------------------------------------------------------
**
** Semi-implicit Euler:
**
** V = v + M^-1 * P.
**
** Where P = lambda * J^T and lambda is the impulse magnitude
** (constraint Lagrange multiplier). Solving for lambda:
**
** lambda = -(JV + b)/((JM^-1)J^T).
**
** ----------------------------------------------------------------------
*/

void physJointDistanceInit(physJointDistance *const restrict joint, const vec3 anchorA, const vec3 anchorB, const float distance, const float frequency, const float damping){
	joint->distance = distance;
	joint->angularFrequency = 2.f * M_PI * frequency;
	joint->damping = 2.f * joint->angularFrequency * damping;
	joint->gamma = 0.f;
	joint->bias = 0.f;
	vec3ZeroP(&joint->rA);
	vec3ZeroP(&joint->rB);
	vec3ZeroP(&joint->rAB);
	joint->inverseEffectiveMass = 0.f;
	joint->impulseAccumulator = 0.f;
}

#ifdef PHYSICS_CONSTRAINT_WARM_START
static __FORCE_INLINE__ void physJointDistanceWarmStart(physJointDistance *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	const vec3 impulse = vec3VMultS(joint->rAB, joint->impulseAccumulator);

	// Apply the accumulated impulse.
	// This helps the constraint converge to a solution faster.
	physRigidBodyApplyVelocityImpulseInverse(bodyA, joint->rA, impulse);
	physRigidBodyApplyVelocityImpulse(bodyB, joint->rB, impulse);

}
#endif

static __FORCE_INLINE__ float physJointDistanceEffectiveMass(const vec3 pointA, const mat3 inverseInertiaTensorA, const vec3 pointB, const mat3 inverseInertiaTensorB, const vec3 normal, const float inverseMassTotal){

	// Effective mass:
	// (JM^-1)J^T = mA^-1 + mB^-1 + ((rA X n) . (IA^-1 * (rA X n))) + ((rB X n) . (IB^-1 * (rB X n)))
	const vec3 angularDeltaA = vec3Cross(pointA, normal);
	const vec3 angularDeltaB = vec3Cross(pointB, normal);
	return inverseMassTotal +
	       vec3Dot(angularDeltaA, mat3MMultVKet(inverseInertiaTensorA, angularDeltaA)) +
	       vec3Dot(angularDeltaB, mat3MMultVKet(inverseInertiaTensorB, angularDeltaB));

}

static __FORCE_INLINE__ void physJointDistanceGenerateBias(physJointDistance *const restrict joint, const physRigidBody *const restrict bodyA, const physRigidBody *const restrict bodyB, const float dt){

	// Only use soft constraints if the angular frequency is greater than 0.
	if(joint->angularFrequency <= 0.f){

		joint->gamma = 0.f;
		joint->bias = 0.f;

	}else{

		// joint->inverseEffectiveMass is set to the
		// effective mass in physJointDistancePersist().
		const float inverseEffectiveMass = (joint->inverseEffectiveMass > 0.f ? 1.f/joint->inverseEffectiveMass : 0.f);
		// k = m_effective * omega^2
		const float stiffness = inverseEffectiveMass * joint->angularFrequency * joint->angularFrequency;

		// c = m_effective * d
		// gamma = 1/(h(hk + c))
		joint->gamma = dt * (dt * stiffness + inverseEffectiveMass * joint->damping);
		joint->gamma = (joint->gamma != 0.f ? 1.f/joint->gamma : 0.f);
		// beta = hk/(hk + c)
		// bias = beta/h * C
		//      = C * h * k * gamma
		// Bias is set to C in physJointDistancePersist().
		joint->bias *= dt * stiffness * joint->gamma;
		joint->inverseEffectiveMass += joint->gamma;

	}

	joint->inverseEffectiveMass = (joint->inverseEffectiveMass > 0.f ? 1.f/joint->inverseEffectiveMass : 0.f);

}

static __FORCE_INLINE__ void physJointDistancePersist(physJointDistance *const restrict joint, const physRigidBody *const restrict bodyA, const physRigidBody *const restrict bodyB){

	float distance;

	// Transform the anchor points.
	joint->rA = quatRotateVec3FastApproximate(
		bodyA->configuration.orientation,
		vec3VMultV(
			bodyA->configuration.scale,
			vec3VSubV(((physJointDistance *)joint)->anchorA, bodyA->centroidLocal)
		)
	);
	joint->rB = quatRotateVec3FastApproximate(
		bodyB->configuration.orientation,
		vec3VMultV(
			bodyB->configuration.scale,
			vec3VSubV(((physJointDistance *)joint)->anchorB, bodyB->centroidLocal)
		)
	);

	// Find the bodies' relative positions.
	// This is the direction impulses will
	// be applied in.
	// n = (pB - pA)/||pB - pA||
	//   = (cB + rB - cA - rA)/||cB + rB - cA - rA||
	joint->rAB = vec3VSubV(
		vec3VSubV(
			vec3VAddV(bodyB->centroidGlobal, joint->rB),
			bodyA->centroidGlobal
		),
		joint->rA
	);

	// Normalize rAB and handle singularities.
	distance = vec3Magnitude(joint->rAB);
	if(distance > PHYSICS_LINEAR_SLOP){
		joint->rAB = vec3VMultS(joint->rAB, 1.f/distance);
	}else{
		vec3ZeroP(&joint->rAB);
	}

	// Calculate C for the bias term.
	// beta = hk/(hk + c)
	// bias = beta/h * C
	//      = gamma * h * k * C
	joint->bias = distance - joint->distance;

	// Calculate the effective mass (not the inverse).
	// This is used in physJointDistanceGenerateBias().
	joint->inverseEffectiveMass = physJointDistanceEffectiveMass(
		joint->rA, bodyA->inverseInertiaTensorGlobal,
		joint->rB, bodyB->inverseInertiaTensorGlobal,
		joint->rAB, bodyA->inverseMass + bodyB->inverseMass
	);

}

void physJointDistancePresolveConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, const float dt){

	// Initialize the constraints.
	physJointDistancePersist((physJointDistance *)joint, bodyA, bodyB);
	physJointDistanceGenerateBias((physJointDistance *)joint, bodyA, bodyB, dt);
	#ifdef PHYSICS_CONSTRAINT_WARM_START
	physJointDistanceWarmStart((physJointDistance *)joint, bodyA, bodyB);
	#endif

}

void physJointDistanceSolveVelocityConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	float lambda;
	vec3 impulse;

	// Evaluate the constraint expression, JV.
	// C = ||pB - pA|| - L
	// C' = dC/dt = (((wB X rB) + vB) - ((wA X rA) + vA)) . n
	// JV = C'
	lambda = vec3Dot(
		vec3VSubV(
			vec3VSubV(
				vec3VAddV(
					vec3Cross(bodyB->angularVelocity, ((physJointDistance *)joint)->rB),
					bodyB->linearVelocity
				),
				vec3Cross(bodyA->angularVelocity, ((physJointDistance *)joint)->rA)
			),
			bodyA->linearVelocity
		),
		((physJointDistance *)joint)->rAB
	);

	// Calculate the impulse magnitude, i.e.
	// the constraint's Lagrange multiplier.
	// lambda = -(JV + b)/((JM^-1)J^T)
	lambda = -((physJointDistance *)joint)->inverseEffectiveMass * (
		lambda + ((physJointDistance *)joint)->bias +
		((physJointDistance *)joint)->gamma * ((physJointDistance *)joint)->impulseAccumulator
	);
	((physJointDistance *)joint)->impulseAccumulator += lambda;

	// Calculate the normal impulse.
	impulse = vec3VMultS(((physJointDistance *)joint)->rAB, lambda);

	// Apply the normal impulse.
	physRigidBodyApplyVelocityImpulseInverse(bodyA, ((physJointDistance *)joint)->rA, impulse);
	physRigidBodyApplyVelocityImpulse(bodyB, ((physJointDistance *)joint)->rB, impulse);

}

#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
return_t physJointDistanceSolveConfigurationConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	// Only apply positional corrections if
	// soft constraints are not being used.
	if(((physJointDistance *)joint)->angularFrequency == 0.f){

		// Retransform the anchor points.
		const vec3 rA = quatRotateVec3FastApproximate(
			bodyA->configuration.orientation,
			vec3VMultV(
				bodyA->configuration.scale,
				vec3VSubV(((physJointDistance *)joint)->anchorA, bodyA->centroidLocal)
			)
		);
		const vec3 rB = quatRotateVec3FastApproximate(
			bodyB->configuration.orientation,
			vec3VMultV(
				bodyB->configuration.scale,
				vec3VSubV(((physJointDistance *)joint)->anchorB, bodyB->centroidLocal)
			)
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

		const float distance = vec3Magnitude(rAB);
		vec3VDivS(rAB, distance);

		{

			// Calculate the new effective mass.
			const float effectiveMass = physJointDistanceEffectiveMass(
				rA, bodyA->inverseInertiaTensorGlobal,
				rB, bodyB->inverseInertiaTensorGlobal,
				rAB, bodyA->inverseMass + bodyB->inverseMass
			);

			float constraint = distance - ((physJointDistance *)joint)->distance;
			// Allow some slop and clamp the constraint to prevent large corrections.
			if(constraint <= -PHYSICS_MAXIMUM_LINEAR_CORRECTION){
				constraint = -PHYSICS_MAXIMUM_LINEAR_CORRECTION;
			}else if(constraint > PHYSICS_MAXIMUM_LINEAR_CORRECTION){
				constraint = PHYSICS_MAXIMUM_LINEAR_CORRECTION;
			}

			if(effectiveMass > 0.f){

				// Normalize and multiply by the impulse magnitude,
				// i.e. the constraint's Lagrange multiplier.
				rAB = vec3VMultS(rAB, -constraint * effectiveMass);

				// Apply the normal impulse.
				physRigidBodyApplyConfigurationImpulseInverse(bodyA, rA, rAB);
				physRigidBodyApplyConfigurationImpulse(bodyB, rB, rAB);

			}

			return fabsf(constraint) < PHYSICS_LINEAR_SLOP;

		}

	}

	return 1;

}
#endif

void physJointDistanceSetFrequency(physJointDistance *const restrict joint, const float frequency){
	joint->damping /= joint->angularFrequency;
	joint->angularFrequency = 2.f * M_PI * frequency;
	joint->damping *= joint->angularFrequency;
}

void physJointDistanceSetDamping(physJointDistance *const restrict joint, const float damping){
	joint->damping = 2.f * joint->angularFrequency * damping;
}