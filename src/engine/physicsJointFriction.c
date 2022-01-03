#include "physicsJointFriction.h"
#include "physicsRigidBody.h"
#include "helpersMath.h"

// ----------------------------------------------------------------------
//
// Friction joints are based on three velocity constraints. The
// first two are the linear "point-to-point" constraints for
// tangential friction, while the third is an angular constraint.
//
// The magnitude of these three constraint expressions must be
// less than or equal to mu * lambda_total, where mu is the
// coefficient of friction and lambda_total is the total accumulated
// impulse magnitude over the length of the contact.
//
// This joint is generally only used if PHYSICS_CONTACT_FRICTION_CONSTRAINT
// is defined.
//
// ----------------------------------------------------------------------
//
// Differentiated tangential friction constraint equations:
//
// C1' : ((vB + wB X rB) - (vA + wA X rA)) . t1,
// C2' : ((vB + wB X rB) - (vA + wA X rA)) . t2,
//
// where t1, t2 are the contact tangents.
//
// Differentiated angular friction constraint equation:
//
// C3' : (wB - wA) . n,
//
// where n is the contact normal.
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
// and the identity
//
//       [((vB + wB X rB) - (vA + wA X rA)) . t1]
// JtV = [((vB + wB X rB) - (vA + wA X rA)) . t2],
//
// we can solve for the Jacobian Jt for the tangential constraint
// equations:
//
//      [-t1, -(rA X t1), t1, (rB X t1)]
// Jt = [-t2, -(rA X t2), t2, (rB X t2)].
//
// For the angular constraint equation, the Jacobian Ja is
//
// Ja = [0, -n, 0, n]
//
// ----------------------------------------------------------------------
//
// The effective mass for the tangential constraint is given by
// the 2x2 matrix (JtM^{-1})Jt^T, where M^{-1} is the inverse mass
// matrix and Jt^T is the transposed tangential Jacobian.
//
//          [mA^{-1}   0      0      0   ]
//          [   0   IA^{-1}   0      0   ]
// M^{-1} = [   0      0   mB^{-1}   0   ]
//          [   0      0      0   IB^{-1}],
//
//        [    -t1,       -t2    ]
//        [-(rA X t1), -(rA X t2)]
// Jt^T = [     t1,        t2    ]
//        [ (rB X t1),  (rB X t2)].
//
// Expanding results in
//
//                [mA^{-1} + mB^{-1} + (IA^{-1} * (rA X t1) . (rA X t1)) + (IB^{-1} * (rB X t1) . (rB X t1)),                 (IA^{-1} * (rA X t1) . (rA X t2)) + (IB^{-1} * (rB X t1) . (rB X t2))]
// (JtM^{-1})Jt^T = [                (IA^{-1} * (rA X t1) . (rA X t2)) + (IB^{-1} * (rB X t1) . (rB X t2)), mA^{-1} + mB^{-1} + (IA^{-1} * (rA X t2) . (rA X t2)) + (IB^{-1} * (rB X t2) . (rB X t2))].
//
// The effective mass for the angular constraint is given by
// the scalar (JaM^{-1})Ja^T:
//
// (JaM^{-1})Ja^T = (((IA^{-1} * n) . n) + ((IB^{-1} * n) . n)).
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

#ifndef PHYSICS_JOINT_FRICTION_EPSILON
	#define PHYSICS_JOINT_FRICTION_EPSILON 0.000001f
#endif

#ifdef PHYSICS_CONSTRAINT_WARM_START
__FORCE_INLINE__ void physJointFrictionWarmStart(const physJointFriction *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	// Warm-start the persistent friction constraints.

	const vec3 impulseTangent = vec3VAddV(
		vec3VMultS(joint->tangent1, joint->tangentImpulseAccumulator.x),
		vec3VMultS(joint->tangent2, joint->tangentImpulseAccumulator.y)
	);
	const vec3 impulseAngular = vec3VMultS(joint->normal, joint->angularImpulseAccumulator);

	physRigidBodyApplyVelocityImpulseAngularInverse(bodyA, joint->rA, impulseTangent, impulseAngular);
	physRigidBodyApplyVelocityImpulseAngular(bodyB, joint->rB, impulseTangent, impulseAngular);

}
#endif

__FORCE_INLINE__ void physJointFrictionGenerateInverseEffectiveMass(physJointFriction *const __RESTRICT__ joint, const physRigidBody *const __RESTRICT__ bodyA, const physRigidBody *const __RESTRICT__ bodyB, const float inverseMassTotal){

	// Tangential effective mass:
	//                  [mA^{-1} + mB^{-1} + (IA^{-1} * (rA X t1) . (rA X t1)) + (IB^{-1} * (rB X t1) . (rB X t1)),                     (IA^{-1} * (rA X t1) . (rA X t2)) + (IB^{-1} * (rB X t1) . (rB X t2))]
	// (JtM^{-1})Jt^T = [                    (IA^{-1} * (rA X t1) . (rA X t2)) + (IB^{-1} * (rB X t1) . (rB X t2)), mA^{-1} + mB^{-1} + (IA^{-1} * (rA X t2) . (rA X t2)) + (IB^{-1} * (rB X t2) . (rB X t2))]

	// Angular effective mass:
	// (JM^{-1})J^T = ((IA^{-1} * n) . n) + ((IB^{-1} * n) . n)

	const float angularMass = vec3Dot(
		mat3MMultV(
			mat3MAddM(bodyA->inverseInertiaTensorGlobal, bodyB->inverseInertiaTensorGlobal),
			joint->normal
		),
		joint->normal
	);

	const vec3 tangent1 = joint->tangent1;
	const vec3 tangent2 = joint->tangent2;

	const vec3 rAt1 = vec3Cross(joint->rA, tangent1);
	const vec3 rBt1 = vec3Cross(joint->rB, tangent1);
	const vec3 rAt2 = vec3Cross(joint->rA, tangent2);
	const vec3 rBt2 = vec3Cross(joint->rB, tangent2);

	const vec3 iArAt1 = mat3MMultV(bodyA->inverseInertiaTensorGlobal, rAt1);
	const vec3 iBrBt1 = mat3MMultV(bodyB->inverseInertiaTensorGlobal, rBt1);

	mat2 tangentMass;
	tangentMass.m[0][0] = inverseMassTotal + vec3Dot(iArAt1, rAt1) + vec3Dot(iBrBt1, rBt1);
	tangentMass.m[0][1] = vec3Dot(iArAt1, rAt2) + vec3Dot(iBrBt1, rBt2);
	tangentMass.m[1][0] = tangentMass.m[0][1];
	tangentMass.m[1][1] = inverseMassTotal + vec3Dot(mat3MMultV(bodyA->inverseInertiaTensorGlobal, rAt2), rAt2) + vec3Dot(mat3MMultV(bodyB->inverseInertiaTensorGlobal, rBt2), rBt2);

	joint->tangentInverseEffectiveMass = mat2Invert(tangentMass);
	joint->angularInverseEffectiveMass = angularMass > 0.f ? 1.f/angularMass : 0.f;

}

__FORCE_INLINE__ void physJointFrictionSolveVelocityConstraints(physJointFriction *const __RESTRICT__ joint, physRigidBody *const bodyA, physRigidBody *const bodyB, const float normalImpulseTotal){

	// Solves the friction constraint impulses.

	const float lambdaClamp = joint->friction * normalImpulseTotal;

	float angularImpulseAccumulatorNew, lambdaAngular;
	vec2 tangentImpulseAccumulatorNew, lambdaTangent;
	vec3 v, impulseTangent, impulseAngular;


	// Calculate the constraint velocity, i.e. the
	// difference in the velocities of the points on
	// both bodies.
	// ((wB x rB) + vB) - ((wA x rA) + vA)
	v = vec3VSubV(
		vec3VSubV(
			vec3VAddV(
				vec3Cross(bodyB->angularVelocity, joint->rB),
				bodyB->linearVelocity
			),
			vec3Cross(bodyA->angularVelocity, joint->rA)
		),
		bodyA->linearVelocity
	);

	// Calculate the tangent friction impulse magnitude,
	// i.e. the constraint's Lagrange multiplier.
	// (-JV)((JM^{-1})J^T)^{-1}
	lambdaTangent = mat2MMultV(joint->tangentInverseEffectiveMass, vec2New(-vec3Dot(v, joint->tangent1), -vec3Dot(v, joint->tangent2)));
	tangentImpulseAccumulatorNew = vec2VAddV(joint->tangentImpulseAccumulator, lambdaTangent);

	// Clamp the tangent friction impulse magnitude.
	// C' <= mu * lambda_total
	{
		const float tangentImpulseMagnitude = vec2Dot(tangentImpulseAccumulatorNew, tangentImpulseAccumulatorNew);
		if(tangentImpulseMagnitude > PHYSICS_JOINT_FRICTION_EPSILON && tangentImpulseMagnitude > lambdaClamp * lambdaClamp){
			tangentImpulseAccumulatorNew = vec2VMultS(tangentImpulseAccumulatorNew, lambdaClamp*rsqrtAccurate(tangentImpulseMagnitude));
		}
	}
	lambdaTangent = vec2VSubV(tangentImpulseAccumulatorNew, joint->tangentImpulseAccumulator);
	joint->tangentImpulseAccumulator = tangentImpulseAccumulatorNew;

	// Calculate the tangent friction impulse.
	impulseTangent = vec3VAddV(vec3VMultS(joint->tangent1, lambdaTangent.x), vec3VMultS(joint->tangent2, lambdaTangent.y));


	// Calculate the angular friction impulse magnitude,
	// i.e. the constraint's Lagrange multiplier.
	// -JV/((JM^{-1})J^T)
	lambdaAngular = vec3Dot(joint->normal, vec3VSubV(bodyA->angularVelocity, bodyB->angularVelocity)) * joint->angularInverseEffectiveMass;
	angularImpulseAccumulatorNew = joint->angularImpulseAccumulator + lambdaAngular;

	// Clamp the angular friction impulse magnitude.
	// C' <= mu * lambda_total
	if(angularImpulseAccumulatorNew <= -lambdaClamp){
		angularImpulseAccumulatorNew = -lambdaClamp;
	}else if(angularImpulseAccumulatorNew > lambdaClamp){
		angularImpulseAccumulatorNew = lambdaClamp;
	}
	lambdaAngular = angularImpulseAccumulatorNew - joint->angularImpulseAccumulator;
	joint->angularImpulseAccumulator = angularImpulseAccumulatorNew;

	// Calculate the angular friction impulse.
	impulseAngular = vec3VMultS(joint->normal, lambdaAngular);


	// Apply both of the frictional impulses.
	physRigidBodyApplyVelocityImpulseAngularInverse(bodyA, joint->rA, impulseTangent, impulseAngular);
	physRigidBodyApplyVelocityImpulseAngular(bodyB, joint->rB, impulseTangent, impulseAngular);

}