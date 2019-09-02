#include "physicsJoint.h"
#include "physicsRigidBody.h"
#include <math.h>
#include "inline.h"

static __FORCE_INLINE__ void physJointDistanceWarmStart(physJointDistance *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	const vec3 impulse = vec3VMultS(joint->rAB, joint->impulseAccumulator);

	// Apply the accumulated impulse.
	// This helps the constraint converge to a solution faster.
	physRigidBodyApplyVelocityImpulseInverse(bodyA, joint->rA, impulse);
	physRigidBodyApplyVelocityImpulse(bodyB, joint->rB, impulse);

}

static __FORCE_INLINE__ float physJointDistanceEffectiveMass(const vec3 pointA, const mat3 inverseInertiaTensorA, const vec3 pointB, const mat3 inverseInertiaTensorB, const vec3 normal, const float inverseMassTotal){

	/*
	** Calculates the effective mass for the constraint.
	** The inverse of this is used as the denominator for the
	** impulse magnitude (constraint Lagrange multiplier):
	**
	** 1 / ((JM^-1)J^T)
	**
	** Where J is the Jacobian row vector, J^T is its transpose
	** and M^-1 is the inverse mass matrix:
	**
	** J = [-p, -(rA X p), p, (rB X p)]
	**
	**       [    -p   ]
	**       [-(rA x p)]
	** J^T = [     p   ]
	**       [ (rB x p)]
	**
	**        [mA^-1  0    0    0  ]
	**        [  0  IA^-1  0    0  ]
	** M^-1 = [  0    0  mB^-1  0  ]
	**        [  0    0    0  IB^-1]
	**
	** Note that p = (pB - pA). J is derived from JV = C', where C is
	** the constraint equation and V is the velocity column vector:
	**
	** C = (1/2)((pB - pA)^2 - L^2) = 0
	** C' = dC/dt = ((vB + wB X rB) - (vA + wA X rA)) . (pB - pA) = 0
	**
	**     [vA]
	**     [wA]
	** V = [vB]
	**     [wB]
	**
	** Thus, our initial expression above expands to the following:
	**
	** 1 / (mA + mB + ((rA X p) . (IA(rA x p))) + ((rB x p) . (IB(rB x p))))
	*/

	const vec3 angularDeltaA = vec3Cross(pointA, normal);
	const vec3 angularDeltaB = vec3Cross(pointB, normal);

	// We don't invert the effective mass here, as
	// we may modify it in physJointGenerateBias().
	return inverseMassTotal +
	       vec3Dot(angularDeltaA, mat3MMultVBra(inverseInertiaTensorA, angularDeltaA)) +
	       vec3Dot(angularDeltaB, mat3MMultVBra(inverseInertiaTensorB, angularDeltaB));

}

static __FORCE_INLINE__ void physJointDistanceGenerateBias(physJointDistance *const restrict joint, const physRigidBody *const restrict bodyA, const physRigidBody *const restrict bodyB, const float dt){

	// Only use soft constraints if the angular frequency is greater than 0.
	if(joint->angularFrequency <= 0.f || joint->inverseEffectiveMass <= 0.f){

		joint->gamma = 0.f;
		joint->bias = 0.f;

	}else{

		const float inverseEffectiveMass = 1.f / joint->inverseEffectiveMass;
		// k = m_effective * omega^2
		const float stiffness = inverseEffectiveMass * joint->angularFrequency * joint->angularFrequency;

		// c = m_effective * d
		// gamma = 1/(hk + c)
		joint->gamma = 1.f / (dt * (dt * stiffness + inverseEffectiveMass * joint->damping));
		// beta = hk/(hk + c)
		// bias = beta/h * C(p)
		joint->bias *= dt * stiffness * joint->gamma;
		joint->inverseEffectiveMass = 1.f / (joint->inverseEffectiveMass + joint->gamma);

	}

}

static __FORCE_INLINE__ void physJointDistancePersist(physJointDistance *const restrict joint, const physRigidBody *const restrict bodyA, const physRigidBody *const restrict bodyB){

	float distance;

	// Transform the anchor points.
	joint->rA = quatRotateVec3FastApproximate(
		bodyA->configuration.orientation,
		vec3VMultV(bodyA->configuration.scale, joint->anchorA)
	);
	joint->rB = quatRotateVec3FastApproximate(
		bodyB->configuration.orientation,
		vec3VMultV(bodyB->configuration.scale, joint->anchorB)
	);

	// Find the bodies' relative positions.
	// This is the direction impulses will
	// be applied in.
	// p = (pB - pA)
	//   = (cB + rB - cA - rA)
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
		vec3VDivS(joint->rAB, distance);
	}else{
		vec3ZeroP(&joint->rAB);
	}

	// Calculate C(p) for the bias term.
	// beta = hk/(hk + c)
	// bias = beta/h * C(p)
	joint->bias = distance - joint->distance;

}

void physJointDistancePresolveConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, const float dt){

	// Initialize the constraints.
	physJointDistancePersist((physJointDistance *)joint, bodyA, bodyB);
	((physJointDistance *)joint)->inverseEffectiveMass = physJointDistanceEffectiveMass(
		((physJointDistance *)joint)->rA, bodyA->inverseInertiaTensorGlobal,
		((physJointDistance *)joint)->rB, bodyB->inverseInertiaTensorGlobal,
		((physJointDistance *)joint)->rAB, bodyA->inverseMass + bodyB->inverseMass
	);
	physJointDistanceGenerateBias((physJointDistance *)joint, bodyA, bodyB, dt);
	physJointDistanceWarmStart((physJointDistance *)joint, bodyA, bodyB);

}

void physJointDistanceSolveVelocityConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	float lambda;
	vec3 impulse;

	// Evaluate the constraint expression, JV.
	// C = (pB - pA) . p
	// C' = dC/dt = (((wB x rB) + vB) - ((wA x rA) + vA)) . p
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

#ifdef PHYSICS_SOLVER_GAUSS_SEIDEL
return_t physJointDistanceSolveConfigurationConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	// Only apply positional corrections if
	// soft constraints are not being used.
	if(((physJointDistance *)joint)->angularFrequency == 0.f){

		// Retransform the anchor points.
		const vec3 rA = quatRotateVec3FastApproximate(
			bodyA->configuration.orientation,
			vec3VMultV(bodyA->configuration.scale, ((physJointDistance *)joint)->anchorA)
		);
		const vec3 rB = quatRotateVec3FastApproximate(
			bodyB->configuration.orientation,
			vec3VMultV(bodyB->configuration.scale, ((physJointDistance *)joint)->anchorB)
		);

		// Find the bodies' relative positions.
		// This is the direction impulses will
		// be applied in.
		// p = (pB - pA)
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
			// Clamp the constraint to prevent large corrections.
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