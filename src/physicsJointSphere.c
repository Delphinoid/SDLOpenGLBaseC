#include "physicsJoint.h"
#include "physicsRigidBody.h"
#include "physicsConstraint.h"
#include <math.h>
#include <string.h>

#define PHYSICS_JOINT_SPHERE_CONE_LIMIT_LOWER 0x02

// ----------------------------------------------------------------------
//
// Sphere joints involve a linear "point-to-point" constraint
// and potentially a cone constraint to limit the relative angle
// of the bodies with respect to some axis.
//
// ----------------------------------------------------------------------
//
// Cone constraint equation:
//
// C = (u2 . u1) - cos(coneAngle / 2) >= 0
//   = cos(upAngle) - cos(coneAngle / 2) >= 0.
//
// Differentiating so we can solve w.r.t. velocity:
//
// C' = dC/dt = (u2 . (w1 X u1)) + (u1 . (w2 X u2)) >= 0,
//
// where n = (u2 X u1) and u1, u2 are the second column vectors
// from the bodies' rotation matrices. The column vectors of a
// rotation matrix form an orthonormal basis, describing the
// transformation of the standard basis vectors. Hence u1 and u2
// are the local "up" vectors of the bodies.
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
// J = [0, -n, 0, n].
//
// Finally, adding a potential bias term, we have
//
// C' = JV + b >= 0.
//
// ----------------------------------------------------------------------
//
// The effective mass for the constraint is given by (JM^-1)J^T,
// where M^-1 is the inverse mass matrix and J^T is the transposed
// Jacobian.
//
//        [mA^-1  0    0    0  ]
//        [  0  IA^-1  0    0  ]
// M^-1 = [  0    0  mB^-1  0  ]
//        [  0    0    0  IB^-1],
//
//       [ 0]
//       [-n]
// J^T = [ 0]
//       [ n].
//
// Expanding results in
//
// (JM^-1)J^T = ((IA^-1 + IB^-1) X n) . n.
//
// ----------------------------------------------------------------------
//
// Semi-implicit Euler:
//
// V   = V_i + dt * M^-1 * F,
// V_f = V   + dt * M^-1 * P.
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
// J(V + dt * M^-1 * P) + b = 0
// JV + dt * (JM^-1)P + b = 0
// JV + dt * (JM^-1)J^T . lambda + b = 0
// dt * (JM^-1)J^T . lambda = -(JV + b)
// dt * lambda = -(JV + b)/((JM^-1)J^T)
// lambda' = -(JV + b)/((JM^-1)J^T).
//
// ----------------------------------------------------------------------

void physJointSphereInit(physJointSphere *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const vec3 anchorA, const vec3 anchorB, const flags_t coneLimitState, const vec3 coneAxis, const float coneAngle){

	mat3 rotation;

	// Generate an orientation matrix from the cone axis.
	// The columns of a rotation matrix form an orthonormal
	// basis representing the rotation of the standard basis
	// vectors.
	memcpy(rotation.m[1], &coneAxis, sizeof(vec3));
	vec3OrthonormalBasis(coneAxis, (vec3 *)&rotation.m[2], (vec3 *)&rotation.m[0]);

	// Transform this orientation into the bodies' local spaces.
	joint->anchorOrientationA = mat3MMultM(mat3Quaternion(bodyA->configuration.orientation), rotation);
	joint->anchorOrientationB = mat3MMultM(mat3Quaternion(bodyB->configuration.orientation), rotation);

	joint->anchorA = anchorA;
	joint->anchorB = anchorB;

	// Take into account the half coefficient in the constraint equation.
	joint->coneLimitAngle = 0.5f * coneAngle;
	joint->coneLimitState = coneLimitState;

	vec3ZeroP(&joint->rA);
	vec3ZeroP(&joint->rB);

	mat3ZeroP(&joint->linearEffectiveMass);
	vec3ZeroP(&joint->linearImpulseAccumulator);

	vec3ZeroP(&joint->coneLimitAxis);

	joint->coneLimitInverseEffectiveMass = 0.f;
	joint->coneLimitImpulseAccumulator = 0.f;

}

#ifdef PHYSICS_CONSTRAINT_WARM_START
static __FORCE_INLINE__ void physJointSphereWarmStart(physJointSphere *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	// Apply the accumulated linear impulse.
	physRigidBodyApplyVelocityImpulseInverse(bodyA, joint->rA, joint->linearImpulseAccumulator);
	physRigidBodyApplyVelocityImpulse(bodyB, joint->rB, joint->linearImpulseAccumulator);

	// Only apply cone limit impulses if the joint
	// rotation is near the edge of the cone limit.
	if(joint->coneLimitState == (PHYSICS_JOINT_SPHERE_CONE_LIMIT_ENABLED & PHYSICS_JOINT_SPHERE_CONE_LIMIT_LOWER)){

		const vec3 impulse = vec3VMultS(joint->coneLimitAxis, joint->coneLimitImpulseAccumulator);

		// Apply the accumulated angular cone limit impulse.
		bodyA->angularVelocity = vec3VSubV(bodyA->angularVelocity, mat3MMultVKet(bodyA->inverseInertiaTensorGlobal, impulse));
		bodyB->angularVelocity = vec3VAddV(bodyB->angularVelocity, mat3MMultVKet(bodyB->inverseInertiaTensorGlobal, impulse));

	}

}
#endif

static __FORCE_INLINE__ float physJointSphereConeLimitEffectiveMass(const mat3 inverseInertiaTensorA, const mat3 inverseInertiaTensorB, const vec3 normal){

	// Effective mass:
	// (JM^-1)J^T = ((IA^-1 + IB^-1) X n) . n
	return vec3Dot(
		mat3MMultVKet(
			mat3MAddM(inverseInertiaTensorA, inverseInertiaTensorB),
			normal
		),
		normal
	);

}

static __FORCE_INLINE__ mat3 physJointSphereLinearEffectiveMass(const vec3 pointA, const mat3 inverseInertiaTensorA, const vec3 pointB, const mat3 inverseInertiaTensorB, const float inverseMassTotal){

	const mat3 M = mat3DiagonalS(inverseMassTotal);
	const mat3 rA = mat3SkewSymmetric(pointA);
	const mat3 rA_T = mat3Transpose(rA);
	const mat3 rB = mat3SkewSymmetric(pointB);
	const mat3 rB_T = mat3Transpose(rB);
	return mat3MAddM(
		M,
		mat3MAddM(
			mat3MMultM(mat3MMultM(rA, inverseInertiaTensorA), rA_T),
			mat3MMultM(mat3MMultM(rB, inverseInertiaTensorB), rB_T)
		)
	);

}

static __FORCE_INLINE__ void physJointSpherePersist(physJointSphere *const __RESTRICT__ joint, const physRigidBody *const __RESTRICT__ bodyA, const physRigidBody *const __RESTRICT__ bodyB){

	// Transform the anchor points.
	joint->rA = quatRotateVec3FastApproximate(
		bodyA->configuration.orientation,
		vec3VMultV(
			bodyA->configuration.scale,
			#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
			vec3VSubV(joint->anchorA, bodyA->centroidLocal)
			#else
			vec3VSubV(joint->anchorA, bodyA->base->centroid)
			#endif
		)
	);
	joint->rB = quatRotateVec3FastApproximate(
		bodyB->configuration.orientation,
		vec3VMultV(
			bodyB->configuration.scale,
			#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
			vec3VSubV(joint->anchorB, bodyB->centroidLocal)
			#else
			vec3VSubV(joint->anchorB, bodyB->base->centroid)
			#endif
		)
	);

	// Calculate the linear effective mass matrix.
	joint->linearEffectiveMass = physJointSphereLinearEffectiveMass(
		joint->rA, bodyA->inverseInertiaTensorGlobal,
		joint->rB, bodyB->inverseInertiaTensorGlobal,
		bodyA->inverseMass + bodyB->inverseMass
	);

	if(flagsAreSet(joint->coneLimitState, PHYSICS_JOINT_SPHERE_CONE_LIMIT_ENABLED)){

		float coneLimitEffectiveMass;

		float cosine;
		float sine;
		float angle;

		// Get the second columns of the bodies' rotation matrices.
		const quat q1 = bodyA->configuration.orientation;
		const quat q2 = bodyB->configuration.orientation;
		const vec3 u1 = vec3New(
			q1.v.x*(q1.v.y+q1.v.y)-q1.w*(q1.v.z+q1.v.z),
			1.f-q1.v.x*(q1.v.x+q1.v.x)-q1.v.z*(q1.v.z+q1.v.z),
			q1.v.y*(q1.v.z+q1.v.z)+q1.w*(q1.v.x+q1.v.x)
		);
		const vec3 u2 = vec3New(
			q2.v.x*(q2.v.y+q2.v.y)-q2.w*(q2.v.z+q2.v.z),
			1.f-q2.v.x*(q2.v.x+q2.v.x)-q2.v.z*(q2.v.z+q2.v.z),
			q2.v.y*(q2.v.z+q2.v.z)+q2.w*(q2.v.x+q2.v.x)
		);
		joint->coneLimitAxis = vec3Cross(u2, u1);

		coneLimitEffectiveMass = physJointSphereConeLimitEffectiveMass(
			bodyA->inverseInertiaTensorGlobal, bodyB->inverseInertiaTensorGlobal, joint->coneLimitAxis
		);
		joint->coneLimitInverseEffectiveMass = (coneLimitEffectiveMass > 0.f ? 1.f/coneLimitEffectiveMass : 0.f);

		// Determine whether the joint rotation lies within the cone limit.
		cosine = vec3Dot(u2, u1);
		sine = vec3Magnitude(joint->coneLimitAxis);
		angle = atan2f(sine, cosine);

		// The half coefficient for coneLimitAngle is taken into account in the joint initialization function.
		if(joint->coneLimitAngle < angle){
			if(joint->coneLimitState != (PHYSICS_JOINT_SPHERE_CONE_LIMIT_ENABLED & PHYSICS_JOINT_SPHERE_CONE_LIMIT_LOWER)){
				// The joint rotation is sufficiently near the edge of the cone limit.
				// Reset the impulse accumulator and start applying velocity impulses.
				joint->coneLimitState = (PHYSICS_JOINT_SPHERE_CONE_LIMIT_ENABLED & PHYSICS_JOINT_SPHERE_CONE_LIMIT_LOWER);
				joint->coneLimitImpulseAccumulator = 0.f;
			}
		}else{
			// The joint rotation lies sufficiently within the cone limit.
			// There is currently no need for velocity impulses.
			joint->coneLimitState = PHYSICS_JOINT_SPHERE_CONE_LIMIT_ENABLED;
		}

	}else{
		joint->coneLimitState = PHYSICS_JOINT_SPHERE_CONE_LIMIT_ENABLED;
	}

}

void physJointSpherePresolveConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float dt){

	// Initialize the constraints.
	physJointSpherePersist((physJointSphere *)joint, bodyA, bodyB);
	#ifdef PHYSICS_CONSTRAINT_WARM_START
	physJointSphereWarmStart((physJointSphere *)joint, bodyA, bodyB);
	#endif

}

void physJointSphereSolveVelocityConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	// Calculate the linear impulse.
	vec3 impulse = mat3Solve(
		((physJointSphere *)joint)->linearEffectiveMass,
		vec3VSubV(
			vec3VSubV(
				vec3VAddV(
					vec3Cross(bodyB->angularVelocity, ((physJointSphere *)joint)->rB),
					bodyB->linearVelocity
				),
				vec3Cross(bodyA->angularVelocity, ((physJointSphere *)joint)->rA)
			),
			bodyA->linearVelocity
		)
	);
	((physJointSphere *)joint)->linearImpulseAccumulator = vec3VAddV(((physJointSphere *)joint)->linearImpulseAccumulator, impulse);

	// Apply the linear impulse.
	physRigidBodyApplyVelocityImpulseInverse(bodyA, ((physJointSphere *)joint)->rA, ((physJointSphere *)joint)->linearImpulseAccumulator);
	physRigidBodyApplyVelocityImpulse(bodyB, ((physJointSphere *)joint)->rB, ((physJointSphere *)joint)->linearImpulseAccumulator);

	if(((physJointSphere *)joint)->coneLimitState == (PHYSICS_JOINT_SPHERE_CONE_LIMIT_ENABLED & PHYSICS_JOINT_SPHERE_CONE_LIMIT_LOWER)){

		float coneLimitImpulseAccumulatorNew, lambda;
		vec3 impulse;

		// Evaluate the constraint expression, JV.
		// JV = C'
		lambda = vec3Dot(
			vec3VSubV(bodyB->angularVelocity, bodyA->angularVelocity),
			((physJointSphere *)joint)->coneLimitAxis
		);

		// Calculate the cone limit impulse magnitude,
		// i.e. the constraint's Lagrange multiplier.
		// -JV/((JM^-1)J^T)
		lambda *= -((physJointSphere *)joint)->coneLimitInverseEffectiveMass;

		// Clamp the cone limit impulse magnitude.
		// The constraint equation states that impulse magnitude >= 0.
		coneLimitImpulseAccumulatorNew = ((physJointSphere *)joint)->coneLimitImpulseAccumulator + lambda;
		coneLimitImpulseAccumulatorNew = coneLimitImpulseAccumulatorNew > 0.f ? coneLimitImpulseAccumulatorNew : 0.f;
		lambda = coneLimitImpulseAccumulatorNew - ((physJointSphere *)joint)->coneLimitImpulseAccumulator;
		((physJointSphere *)joint)->coneLimitImpulseAccumulator = coneLimitImpulseAccumulatorNew;

		// Calculate the cone limit impulse.
		impulse = vec3VMultS(((physJointSphere *)joint)->coneLimitAxis, lambda);

		// Apply the accumulated angular cone limit impulse.
		bodyA->angularVelocity = vec3VSubV(bodyA->angularVelocity, mat3MMultVKet(bodyA->inverseInertiaTensorGlobal, impulse));
		bodyB->angularVelocity = vec3VAddV(bodyB->angularVelocity, mat3MMultVKet(bodyB->inverseInertiaTensorGlobal, impulse));

	}


}

#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
return_t physJointSphereSolveConfigurationConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	float constraintLinear;
	float constraintLimit = 0.f;

	mat3 effectiveMass;
	vec3 impulse;

	// Retransform the anchor points.
	const vec3 rA = quatRotateVec3FastApproximate(
		bodyA->configuration.orientation,
		vec3VMultV(
			bodyA->configuration.scale,
			#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
			vec3VSubV(((physJointSphere *)joint)->anchorA, bodyA->centroidLocal)
			#else
			vec3VSubV(((physJointSphere *)joint)->anchorA, bodyA->base->centroid)
			#endif
		)
	);
	const vec3 rB = quatRotateVec3FastApproximate(
		bodyB->configuration.orientation,
		vec3VMultV(
			bodyB->configuration.scale,
			#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
			vec3VSubV(((physJointSphere *)joint)->anchorB, bodyB->centroidLocal)
			#else
			vec3VSubV(((physJointSphere *)joint)->anchorB, bodyB->base->centroid)
			#endif
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
	constraintLinear = vec3Magnitude(rAB);

	// Calculate the new linear effective mass.
	effectiveMass = physJointSphereLinearEffectiveMass(
		rA, bodyA->inverseInertiaTensorGlobal,
		rB, bodyB->inverseInertiaTensorGlobal,
		bodyA->inverseMass + bodyB->inverseMass
	);

	// Calculate the linear impulse.
	impulse = mat3Solve(effectiveMass, vec3Negate(rAB));

	// Apply the linear impulse.
	physRigidBodyApplyConfigurationImpulseInverse(bodyA, rA, impulse);
	physRigidBodyApplyConfigurationImpulse(bodyB, rB, impulse);

	if(flagsAreSet(((physJointSphere *)joint)->coneLimitState, PHYSICS_JOINT_SPHERE_CONE_LIMIT_ENABLED)){

		float coneLimitInverseEffectiveMass;

		float cosine;
		float sine;
		float angle;

		// Get the second columns of the bodies' rotation matrices.
		// Weird pointer tricks to get around "scalar" conversion.
		const vec3 u1 = quatRotateVec3FastApproximate(
			bodyA->configuration.orientation,
			*((vec3 *)&((physJointSphere *)joint)->anchorOrientationA.m[1])
		);
		const vec3 u2 = quatRotateVec3FastApproximate(
			bodyB->configuration.orientation,
			*((vec3 *)&((physJointSphere *)joint)->anchorOrientationB.m[1])
		);
		const vec3 coneLimitAxis = vec3Cross(u2, u1);

		coneLimitInverseEffectiveMass = physJointSphereConeLimitEffectiveMass(
			bodyA->inverseInertiaTensorGlobal, bodyB->inverseInertiaTensorGlobal, coneLimitAxis
		);
		coneLimitInverseEffectiveMass = (coneLimitInverseEffectiveMass > 0.f ? 1.f/coneLimitInverseEffectiveMass : 0.f);

		cosine = vec3Dot(u2, u1);
		sine = vec3Magnitude(((physJointSphere *)joint)->coneLimitAxis);
		angle = atan2f(sine, cosine);

		constraintLimit = -((physJointSphere *)joint)->coneLimitAngle + angle;
		if(constraintLimit > 0.f){

			// Allow some slop and clamp the constraint to prevent large corrections.
			float constraint = constraintLimit - PHYSICS_ANGULAR_SLOP;
			if(constraint >= PHYSICS_MAXIMUM_ANGULAR_CORRECTION){
				constraint = PHYSICS_MAXIMUM_ANGULAR_CORRECTION;
			}

			// Calculate the cone limit impulse.
			impulse = vec3VMultS(coneLimitAxis, constraint * coneLimitInverseEffectiveMass);

			// Apply the cone limit impulse.
			if(
				flagsAreSet(bodyA->flags, PHYSICS_BODY_SIMULATE_ANGULAR)// &&
				//(bodyA->angularVelocity.y != 0.f || bodyA->angularVelocity.z != 0.f || bodyA->angularVelocity.x != 0.f)
			){
				bodyA->configuration.orientation = quatNormalizeFastAccurate(
					quatQSubQ(
						bodyA->configuration.orientation,
						quatDifferentiate(
							bodyA->configuration.orientation,
							mat3MMultVKet(
								bodyA->inverseInertiaTensorGlobal,
								impulse
							)
						)
					)
				);
				physRigidBodyGenerateGlobalInertia(bodyA);
				flagsSet(bodyA->flags, PHYSICS_BODY_ROTATED);
			}
			if(
				flagsAreSet(bodyB->flags, PHYSICS_BODY_SIMULATE_ANGULAR)// &&
				//(bodyB->angularVelocity.y != 0.f || bodyB->angularVelocity.z != 0.f || bodyB->angularVelocity.x != 0.f)
			){
				bodyB->configuration.orientation = quatNormalizeFastAccurate(
					quatQAddQ(
						bodyB->configuration.orientation,
						quatDifferentiate(
							bodyB->configuration.orientation,
							mat3MMultVKet(
								bodyB->inverseInertiaTensorGlobal,
								impulse
							)
						)
					)
				);
				physRigidBodyGenerateGlobalInertia(bodyB);
				flagsSet(bodyB->flags, PHYSICS_BODY_ROTATED);
			}

		}

	}

	return constraintLinear <= PHYSICS_LINEAR_SLOP && constraintLimit <= PHYSICS_ANGULAR_SLOP;

}
#endif