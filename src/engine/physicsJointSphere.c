#include "physicsJoint.h"
#include "physicsRigidBody.h"
#include "helpersMath.h"
#include <math.h>
#include <string.h>

#ifdef PHYSICS_JOINT_SPHERE_SWING_USE_ELLIPSE_NORMAL
#include "vec2.h"
#endif


#define PHYSICS_JOINT_SPHERE_LIMITS_FREE        0x00
#define PHYSICS_JOINT_SPHERE_LIMITS_SWING       0x01
#define PHYSICS_JOINT_SPHERE_LIMITS_TWIST_LOWER 0x02
#define PHYSICS_JOINT_SPHERE_LIMITS_TWIST_UPPER 0x04
#define PHYSICS_JOINT_SPHERE_LIMITS_TWIST       (PHYSICS_JOINT_SPHERE_LIMITS_TWIST_LOWER | PHYSICS_JOINT_SPHERE_LIMITS_TWIST_UPPER)

// ----------------------------------------------------------------------
//
// Spherical constraints involve a "point-to-point" constraint and
// a cone constraint that limits one rigid body's relative position
// and orientation to a cone about some user-specified axis.
// This constraint is similar to a ball and socket joint.
//
// ----------------------------------------------------------------------
//
// For the point-to-point constraint, we define rA and rB to be
// the offsets of the "socket" and "ball" from the rigid bodies
// respectively. Of course, these two points should coincide, so
//
// C1 : (pB + rB) - (pA + rA) = 0.
//
// Differentiate with respect to time to get a velocity constraint:
//
// C1' : 0 = (vB +   wB X rB) - (vA +   wA X rA)
//         = (vB -   rB X wB) - (vA -   rA X wA)
//         = (vB - [rB]_X*wB) - (vA - [rA]_X*wA),
//
// where "[.]_X" denotes the skew-symmetric "cross product" matrix.
//
//
// We also need to impose constraints on the relative orientation of the
// rigid bodies, so if "theta(a)" represents the angle of the relative
// orientation "((qB*RB) * conj(qA*RA))" along the axis a, then we require
//
// C2 : theta(s) - s_limit <= 0,
// C3 : theta(t) - t_upper <= 0,
// C4 : theta(t) - t_lower >= 0.
//
// where s denotes the swing axis, t denotes the twist axis,
// and s_limit, t_upper and t_lower are the swing and twist
// axis limits. The twist axis is chosen as body B's x-axis.
//
// The velocity constraints for these are
//
// C2' : (wB - wA) . s <= 0,
// C3' : (wB - wA) . t <= 0,
// C4' : (wB - wA) . t >= 0.
//
// Note that constraints C3' and C4' appear to conflict.
// This is fine, as we only solve these constraints when
// they're broken, and it's impossible to break both the
// upper and lower limits simultaneously.
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
// and the identity C' = JV, we can solve for the Jacobian J.
// We will use three separate Jacobians, J1, J2 and J3, for
// C1', C2' and C3'.
//
// J1 = [-I_3, [rA]_X, I_3, -[rB]_X],
// J2 = [   0,     -s,   0,       s],
// J3 = [   0,     -t,   0,       t] = J4.
//
// Note that "I_3" is the 3x3 identity matrix.
//
// ----------------------------------------------------------------------
//
// The effective mass for the constraint is given by JM^{-1}J^T,
// where M^{-1} is the inverse mass matrix and J^T is the transposed
// Jacobian.
//
//          [mA^{-1}    0       0       0   ]
//          [   0    IA^{-1}    0       0   ]
// M^{-1} = [   0       0    mB^{-1}    0   ],
//          [   0       0       0    IB^{-1}]
//
// Note that transposing the Jacobian transposes [rA]_X.
// As it's skew-symmetric, we need to flip its sign.
//
//        [ -I_3  ]
//        [-[rA]_X]
// J1^T = [  I_3  ].
//        [ [rB]_X]
//
//
// Evaluating this expression gives us the following matrix for our
// point-to-point constraint:
//
//               [-mA^{-1} * I_3   ]
//               [-IA^{-1} * [rA]_X]
// M^{-1}*J1^T = [ mB^{-1} * I_3   ],
//               [ IB^{-1} * [rB]_X]
//
// K1 = J1*M^{-1}*J1^T
//    = (mA^{-1} + mB^{-1})*I_3 - [rA]_X*IA^{-1}*[rA]_X - [rB]_X*IB^{-1}*[rB]_X,
//
// For our angular constraints, we simply get:
//
//               [     0    ]
//               [-IA^{-1}*s]
// M^{-1}*J2^T = [     0    ],
//               [ IB^{-1}*s]
//
// K2 = J2*M^{-1}*J2^T = s . (IA^{-1} + IB^{-1})*s,
//
// and similarly for K3 = K4. Note that K1 is a 3x3 matrix,
// but K2, K3 and K4 are simply scalars.
//
// ----------------------------------------------------------------------
//
// To limit the relative orientation of rigid body B with
// respect to rigid body A, we use swing-twist decomposition
// and constrain the swing and twist axes individually.
//
// Note that if a_limit is the axis limit and "theta(a)" is the angle of
// the relative orientation "((qB*RB) * conj(qA*RA))" along the axis a, then
//
// b = (theta(a) - a_limit)/dt = C/dt
//
// is the angular velocity required for the limit to be reached.
// Note that to prevent all rotation along a, we apply the impulse
//
// lambda = -JV/K <= 0.
//
// To prevent rotation along a only when the limit has
// been exceeded, we should instead apply the impulse
//
// lambda = -(JV + C/dt)/K = -(JV + b)/K.
//
// Note that it is generally unstable to add all of b back into the
// impulse, so we multiply it by a Baumgarte constant B in [0, 1]:
//
// b = B/dt * C.
//
// ----------------------------------------------------------------------

void physJointSphereInit(
	physJointSphere *const __RESTRICT__ joint,
	const vec3 anchorA, const vec3 anchorB,
	const quat anchorOrientationA, const quat anchorOrientationB,
	const float restitution,
	const float minX, const float maxX,
	const float minY, const float maxY,
	const float minZ, const float maxZ
){

	// The inputs for initializing the ball-and-socket joint shouldn't be too mysterious:
	//     anchorA          - Vector from rigid body A's point of reference to the socket.
	//     anchorB          - Vector from rigid body B's point of reference to the ball.
	//     {min,max}{X,Y,Z} - Angles used to clamp the relative orientation of body B.
	// Note that the minimum and maximum angles are given in radians.
	// The axes are taken relative to rigid body A's frame of reference.

	joint->anchorA = anchorA;
	joint->anchorB = anchorB;
	joint->anchorOrientationA = anchorOrientationA;
	joint->anchorOrientationB = anchorOrientationB;

	joint->limitStates = PHYSICS_JOINT_SPHERE_LIMITS_FREE;

	joint->restitution = restitution;

	joint->angularLimitsX[0] = minX;
	joint->angularLimitsX[1] = maxX;
	joint->angularLimitsY[0] = minY;
	joint->angularLimitsY[1] = maxY;
	joint->angularLimitsZ[0] = minZ;
	joint->angularLimitsZ[1] = maxZ;

	joint->linearImpulse = g_vec3Zero;
	joint->swingImpulse = 0.f;
	joint->twistImpulse = 0.f;

}

#ifdef PHYSICS_JOINT_SPHERE_SWING_USE_ELLIPSE_NORMAL
static __FORCE_INLINE__ vec3 physJointSphereSwingImpulseRemoveTwist(const vec3 twistAxis, const vec3 swingImpulse){

	// Given a swing impulse, remove the component parallel to the twist axis.
	// The ellipse normal may not be orthogonal to the twist axis, so we need
	// to filter it out. Otherwise, our swing impulses will introduce twisting.

	// Project the impulse onto the twist axis.
	const vec3 impulseTwistComponent = vec3VMultS(twistAxis, vec3Dot(swingImpulse, twistAxis));
	// Subtract the twist component from the impulse.
	return vec3VSubV(swingImpulse, impulseTwistComponent);

}
#endif

#ifdef PHYSICS_JOINT_SPHERE_WARM_START
static __FORCE_INLINE__ void physJointSphereWarmStart(physJointSphere *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	vec3 angularImpulse;

	// The angular impulse is the sum of the swing and twist impulses.
	// We recalculate them here because we need the new swing and twist axes.
	if(flagsAreSet(((physJointSphere *)joint)->limitStates, PHYSICS_JOINT_SPHERE_LIMITS_SWING)){
		angularImpulse = vec3VMultS(joint->swingAxis, joint->swingImpulse);
		#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
		angularImpulse = physJointSphereSwingImpulseRemoveTwist(joint->twistAxis, angularImpulse);
		#endif
	}else{
		angularImpulse = g_vec3Zero;
	}
	if(flagsAreSet(((physJointSphere *)joint)->limitStates, PHYSICS_JOINT_SPHERE_LIMITS_TWIST)){
		angularImpulse = vec3fmaf(joint->twistImpulse, joint->twistAxis, angularImpulse);
	}

	// Apply the accumulated angular impulse.
	physRigidBodyApplyBoostImpulseInverse(bodyA, joint->rA, joint->linearImpulse, angularImpulse);
	physRigidBodyApplyBoostImpulse(bodyB, joint->rB, joint->linearImpulse, angularImpulse);

}
#endif

#ifndef PHYSICS_JOINT_SPHERE_ANGULAR_CONSTRAINT_EULER
static flags_t physJointSphereGenerateSwingLimit(
	const quat swing,
	const quat anchorOrientationGlobalA, const vec3 twistAxis,
	const float angularLimitsY[2], const float angularLimitsZ[2],
	vec3 *const __RESTRICT__ swingAxis, float *const __RESTRICT__ swingBias
){

	// Calculate the swing axis and bias, and return the state of the swing limit.
	// The swing bias is positive when the constraint is violated, and zero otherwise.

	// Regardless of whether or not we use the ellipse normal
	// as the swing axis, we will always get an axis of (0, 0, 0)
	// when there is no relative rotation between the two bodies.
	// We don't actually care much about this case though, as our
	// constraint should always be satisfied.
	if(swing.w > 1.f - MATH_NORMALIZE_EPSILON){
		*swingAxis = quatBasisY(swing);
	}else{

		vec3 constraintAxis;
		#ifdef PHYSICS_JOINT_SPHERE_SWING_USE_ELLIPSE_NORMAL
		vec2 ellipseNormal;
		#endif
		float swingDeviation;

		// The constraint axis is rigid body B's x-axis
		// in the coordinate frame of rigid body A.
		// This is the axis we want to constrain within our cone.
		constraintAxis = quatConjugateRotateVec3FastApproximate(anchorOrientationGlobalA, twistAxis);
		// As rigid body B rotates around the y-axis of rigid
		// body A, the z-component of the constraint axis decreases.
		// Conversely, the y-component increases as we rotate about z.
		//
		// Note that we use different limits depending
		// on which quadrant of the ellispse we're in.
		#ifdef PHYSICS_JOINT_SPHERE_SWING_USE_ELLIPSE_NORMAL
		swingDeviation = quatAngle(swing) - clampEllipseDistanceNormalFast(
			constraintAxis.y, constraintAxis.z,
			constraintAxis.y > 0.f ?  angularLimitsZ[1] : -angularLimitsZ[0],
			constraintAxis.z > 0.f ? -angularLimitsY[0] :  angularLimitsY[1],
			&ellipseNormal
		);
		#else
		swingDeviation = quatAngle(swing) - clampEllipseDistanceFast(
			constraintAxis.y, constraintAxis.z,
			constraintAxis.y > 0.f ?  angularLimitsZ[1] : -angularLimitsZ[0],
			constraintAxis.z > 0.f ? -angularLimitsY[0] :  angularLimitsY[1]
		);
		#endif

		#ifdef PHYSICS_JOINT_SPHERE_SWING_USE_ELLIPSE_NORMAL
		// Using the ellipse normal should be more stable, although
		// we are using the angle for the swing axis, not this one.

		// The ellipse normal we compute is unit length, so since the
		// local swing axis is either of the form (1, 0, 0) or (0, -y, z),
		// we don't need to normalize the new swing axis.
		swingAxis->x = 0.f;
		swingAxis->y = -ellipseNormal.y;
		swingAxis->z = ellipseNormal.x;
		// Bring the swing axis from body B's space to global space.
		quatRotateVec3FastApproximateP(anchorOrientationGlobalA, swingAxis);
		#else
		quatAxisPR(&swing, swingAxis);
		#endif

		// If the constraint is broken, this term is positive.
		if(swingDeviation > 0.f){
			*swingBias = swingDeviation;
			return PHYSICS_JOINT_SPHERE_LIMITS_SWING;
		}

	}

	// If the constraint is satisfied, set the bias to 0.
	*swingBias = 0.f;
	return PHYSICS_JOINT_SPHERE_LIMITS_FREE;

}
#endif

static flags_t physJointSphereGenerateTwistLimit(
	const float angle, const float angularLimits[2],
	vec3 *const __RESTRICT__ axis, float *const __RESTRICT__ angleBias
){

	// Calculate the twist constraint and return which of the two specified
	// limit states it's in! We want a positive bias when the constraint is
	// broken, which means we may need to negate the constraint axis.

	// If the lower limit is broken, use it for the bias.
	// Note that the bias is negative when the lower limit is broken,
	// so to get a positive bias, we need to negate the constraint axis.
	if(angle < angularLimits[0]){
		*angleBias = angularLimits[0] - angle;
		vec3NegateP(axis);
		return PHYSICS_JOINT_SPHERE_LIMITS_TWIST_LOWER;

	// If the upper limit is broken, use it instead.
	// The bias is positive when the upper limit is broken.
	}else if(angle > angularLimits[1]){
		*angleBias = angle - angularLimits[1];
		return PHYSICS_JOINT_SPHERE_LIMITS_TWIST_UPPER;
	}

	*angleBias = 0.f;
	return PHYSICS_JOINT_SPHERE_LIMITS_FREE;

}

static __FORCE_INLINE__ void physJointSpherePersist(physJointSphere *const __RESTRICT__ joint, const physRigidBody *const __RESTRICT__ bodyA, const physRigidBody *const __RESTRICT__ bodyB){

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

}

static __FORCE_INLINE__ flags_t physJointSphereGenerateAngularBias(const physJointSphere *const __RESTRICT__ joint, const quat orientationA, const quat orientationB, vec3 *const __RESTRICT__ swingAxis, float *const __RESTRICT__ swingBias, vec3 *const __RESTRICT__ twistAxis, float *const __RESTRICT__ twistBias){

	// Calculate the new swing and twist
	// axes, as well as the angle limits.

	flags_t limitStates;

	vec3 xAxisA;
	quat swing, twist;
	quat anchorOrientationGlobalA, anchorOrientationGlobalB;
	quat qR;
	float twistAngle;

	// Calculate the swing bias.
	// Get the global constraint orientations for the rigid bodies.
	anchorOrientationGlobalA = quatQMultQ(orientationA, joint->anchorOrientationA);
	anchorOrientationGlobalB = quatQMultQ(orientationB, joint->anchorOrientationB);
	// Get the relative orientation from body A to body B:
	// qR = (qB*RB)*conj(qA*RA).
	qR = quatQMultQConjugate(anchorOrientationGlobalB, anchorOrientationGlobalA);
	// These represent the x-axis in the two rigid body's reference frames.
	// The swing is the quaternion giving the shortest arc between these vectors.
	xAxisA = quatBasisX(anchorOrientationGlobalA);
	*twistAxis = quatBasisX(anchorOrientationGlobalB);
	// Decompose the relative orientation into swing and twist components.
	// We take the twist component to be the rotation around the x-axis.
	quatSwingTwistFaster(qR, xAxisA, &twist, &swing);
	// Compute the constraint term for the swing limit,
	// C2 = (RB - RA) . s - s_lim.
	// The maximum swing limit is also computed
	// here using the limits on the y- and z-axes.
	limitStates = physJointSphereGenerateSwingLimit(
		swing,
		anchorOrientationGlobalA, *twistAxis,
		joint->angularLimitsY, joint->angularLimitsZ,
		swingAxis, swingBias
	);

	// Calculate the twist bias.
	twistAngle = quatAngle(twist);
	// Our quaternion angle function returns a value in [0, 2pi].
	// However, we want our angles in the interval [-pi, pi].
	if(twistAngle > M_PI){
		twistAngle -= 2.f*M_PI;
	}
	// The twist quaternion's axis should be parallel
	// to rigid body A's x-axis. If it's antiparallel,
	// we should negate the twist angle to compensate.
	if(vec3Dot(xAxisA, *((vec3 *)&twist.x)) < 0.f){
		twistAngle = -twistAngle;
	}
	// Compute the constraint term for the twist limit,
	// C2 = (RB - RA) . t - t_lim.
	flagsSet(limitStates, physJointSphereGenerateTwistLimit(
		twistAngle, joint->angularLimitsX, twistAxis, twistBias
	));

	return limitStates;

}

static __FORCE_INLINE__ vec3 physJointSphereGenerateLinearBias(const vec3 centroidA, const vec3 centroidB, const vec3 rA, const vec3 rB){
	// Calculate the displacement from the ball to the socket:
	// C1 = (pB + rB) - (pA + rA).
	const vec3 linearBias = vec3VSubV(vec3VAddV(centroidB, rB), vec3VAddV(centroidA, rA));
	return linearBias;
}

static __FORCE_INLINE__ void physJointSphereGenerateAngularInverseEffectiveMass(
	const physRigidBody *const __RESTRICT__ bodyA, const physRigidBody *const __RESTRICT__ bodyB,
	const vec3 swingAxis, const vec3 twistAxis,
	float *const __RESTRICT__ swingInvMass, float *const __RESTRICT__ twistInvMass
){
	// Calculate the inverse angular effective mass of the constraint, which won't
	// change between velocity iterations. We can just do it once per update.
	vec3 IABa;
	const mat3 inverseInertiaTensorSum = mat3MAddM(bodyA->inverseInertiaTensorGlobal, bodyB->inverseInertiaTensorGlobal);
	// Swing inverse effective mass (K2).
	IABa = mat3MMultV(inverseInertiaTensorSum, swingAxis);
	*swingInvMass = 1.f / vec3Dot(swingAxis, IABa);
	// Twist inverse effective mass (K3).
	IABa = mat3MMultV(inverseInertiaTensorSum, twistAxis);
	*twistInvMass = 1.f / vec3Dot(twistAxis, IABa);
}

static __FORCE_INLINE__ void physJointSphereGenerateLinearInverseEffectiveMass(
	const physRigidBody *const __RESTRICT__ bodyA, const physRigidBody *const __RESTRICT__ bodyB,
	const vec3 rA, const vec3 rB,
	mat3 *const __RESTRICT__ linearMass
){

	// Calculate the linear effective mass of the constraint, which won't
	// change between velocity iterations. We can just do it once per update.

	// Compute the linear effective mass given by
	//     K1 = (mA^{-1} + mB^{-1})*I_3 - [rA]_X*IA^{-1}*[rA]_X - [rB]_X*IB^{-1}*[rB]_X.
	// We don't invert the matrix here, as it's faster to solve
	// for lambda using Cramer's rule in the position solver step.

	// Compute body A's contribution to K1.
	{
		// Note that the matrix [rA]_X*IA^{-1}*[rA]_X is symmetric, so we
		// only need to compute the half of the off-diagonal elements.

		// These variables bring it down to 27 multiplications and 14 subtractions.
		// An alternative method gives 24 multiplications and 21 subtractions,
		// but float multiplication is generally faster and has less error.
		const float I[6] = {
			bodyA->inverseInertiaTensorGlobal.m[0][0], bodyA->inverseInertiaTensorGlobal.m[0][1], bodyA->inverseInertiaTensorGlobal.m[0][2],
			                                           bodyA->inverseInertiaTensorGlobal.m[1][1], bodyA->inverseInertiaTensorGlobal.m[1][2],
			                                                                                      bodyA->inverseInertiaTensorGlobal.m[2][2]
		};
		// Recall that the "cross product" matrix for rA is given by
		//              [    0, -rA_z,  rA_y]
		//     [rA]_X = [ rA_z,     0, -rA_x].
		//              [-rA_y,  rA_x,     0]
		// We begin by calculating A = -IA^{-1}*[rA]_X. We don't need
		// to compute the last element here, as it's only necessary
		// to compute the lower triangular components, and the final
		// matrix -[rA]_X*IA^{-1}*[rA]_X is guaranteed to be symmetric.
		const float a3xy = rA.z*I[1];
		const float A[8] = {
			rA.y*I[2] - a3xy,      rA.y*I[4] - rA.z*I[3], rA.y*I[5] - rA.z*I[4],
			rA.z*I[0] - rA.x*I[2], a3xy - rA.x*I[4],      rA.z*I[2] - rA.x*I[5],
			rA.x*I[1] - rA.y*I[0], rA.x*I[3] - rA.y*I[1]
		};

		// Compute -[rA]_X*IA^{-1}*[rA]_X. We can just set the lower
		// triangular components when we add body B's contribution.
		linearMass->m[0][0] = rA.y*A[2] - rA.z*A[1];
		linearMass->m[0][1] = rA.z*A[0] - rA.x*A[2];
		linearMass->m[0][2] = rA.x*A[1] - rA.y*A[0];
		linearMass->m[1][1] = rA.z*A[3] - rA.x*A[5];
		linearMass->m[1][2] = rA.x*A[4] - rA.y*A[3];
		linearMass->m[2][2] = rA.x*A[7] - rA.y*A[6];
	}

	// Compute body B's contribution to K1.
	{
		const float I[6] = {
			bodyB->inverseInertiaTensorGlobal.m[0][0], bodyB->inverseInertiaTensorGlobal.m[0][1], bodyB->inverseInertiaTensorGlobal.m[0][2],
			                                           bodyB->inverseInertiaTensorGlobal.m[1][1], bodyB->inverseInertiaTensorGlobal.m[1][2],
			                                                                                      bodyB->inverseInertiaTensorGlobal.m[2][2]
		};
		// Calculate A = -IB^{-1}*[rB]_X.
		const float a3xy = rB.z*I[1];
		const float A[8] = {
			rB.y*I[2] - a3xy,      rB.y*I[4] - rB.z*I[3], rB.y*I[5] - rB.z*I[4],
			rB.z*I[0] - rB.x*I[2], a3xy - rB.x*I[4],      rB.z*I[2] - rB.x*I[5],
			rB.x*I[1] - rB.y*I[0], rB.x*I[3] - rB.y*I[1]
		};
		const float inverseMass = bodyA->inverseMass + bodyB->inverseMass;

		// Add body B's contribution and the diagonal term to the effective mass:
		//    K1 += (mA^{-1} + mB^{-1})*I_3 - [rB]_X*IB^{-1}*[rB]_X.
		linearMass->m[0][0] += rB.y*A[2] - rB.z*A[1] + inverseMass;
		linearMass->m[0][1] += rB.z*A[0] - rB.x*A[2];
		linearMass->m[0][2] += rB.x*A[1] - rB.y*A[0];
		linearMass->m[1][0] = linearMass->m[0][1];
		linearMass->m[1][1] += rB.z*A[3] - rB.x*A[5] + inverseMass;
		linearMass->m[1][2] += rB.x*A[4] - rB.y*A[3];
		linearMass->m[2][0] = linearMass->m[0][2];
		linearMass->m[2][1] = linearMass->m[1][2];
		linearMass->m[2][2] += rB.x*A[7] - rB.y*A[6] + inverseMass;
	}

}

static __FORCE_INLINE__ void physJointSphereGenerateBias(physJointSphere *const __RESTRICT__ joint, const physRigidBody *const __RESTRICT__ bodyA, const physRigidBody *const __RESTRICT__ bodyB, const float dt_s){

	#ifdef PHYSICS_JOINT_SPHERE_STABILIZER_BAUMGARTE
	const float frequency = 1.f/dt_s;
	#endif
	flags_t changedLimits = joint->limitStates;

	// Angular constraints.
	joint->limitStates = physJointSphereGenerateAngularBias(
		joint,
		bodyA->configuration.orientation, bodyB->configuration.orientation,
		&joint->swingAxis, &joint->swingBias, &joint->twistAxis, &joint->twistBias
	);
	// Figure out which limits have been either enabled or disabled.
	changedLimits ^= joint->limitStates;
	physJointSphereGenerateAngularInverseEffectiveMass(
		bodyA, bodyB,
		joint->swingAxis,
		joint->twistAxis,
		&joint->swingInverseEffectiveMass,
		&joint->twistInverseEffectiveMass
	);
	// bias = (B/dt)*C
	// The angular slop lets us solve the velocity constraint,
	// but removes the bias term when we're sufficiently close
	// to the limits. This helps prevent jittering when resting
	// at the limit points.
	#ifdef PHYSICS_JOINT_SPHERE_STABILIZER_BAUMGARTE
		joint->swingBias = floatMax(
			PHYSICS_JOINT_SPHERE_BAUMGARTE_BIAS * frequency * (joint->swingBias - PHYSICS_ANGULAR_SLOP),
			0.f
		);
		joint->twistBias = floatMax(
			PHYSICS_JOINT_SPHERE_BAUMGARTE_BIAS * frequency * (joint->twistBias - PHYSICS_ANGULAR_SLOP),
			0.f
		);
	#else
		joint->swingBias = 0.f;
		joint->twistBias = 0.f;
	#endif

	// Linear constraint.
	// The performance of solving using Cramer's rule seems similar
	// to inverting. However, since we do our velocity solve step
	// multiple times using the same matrix for sequential impulse,
	// it's much faster to invert it here when we're presolving.
	physJointSphereGenerateLinearInverseEffectiveMass(bodyA, bodyB, joint->rA, joint->rB, &joint->linearInverseEffectiveMass);
	joint->linearInverseEffectiveMass = mat3Invert(joint->linearInverseEffectiveMass);
	#ifdef PHYSICS_JOINT_SPHERE_STABILIZER_BAUMGARTE
		// Compute the linear bias term.
		joint->linearBias = physJointSphereGenerateLinearBias(bodyA->centroid, bodyB->centroid, joint->rA, joint->rB);
		// bias = (B/dt)*C
		joint->linearBias = vec3VMultS(joint->linearBias, PHYSICS_JOINT_SPHERE_BAUMGARTE_BIAS * frequency);
	#else
		joint->linearBias = g_vec3Zero;
	#endif

	// Reset the accumulated impulses if the limits have changed state.
	if(flagsAreSet(changedLimits, PHYSICS_JOINT_SPHERE_LIMITS_SWING)){
		joint->swingImpulse = 0.f;
	}
	if(flagsAreSet(changedLimits, PHYSICS_JOINT_SPHERE_LIMITS_TWIST)){
		joint->twistImpulse = 0.f;
	}
	// Disable linear warmstarting while the swing constraint is violated.
	// This is a little hacky, but seems to give much more accurate results.
	if(flagsAreSet(joint->limitStates, PHYSICS_JOINT_SPHERE_LIMITS_SWING)){
		joint->linearImpulse = g_vec3Zero;
	}

}

void physJointSpherePresolveConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float dt_s){

	// Initialize the constraints.
	physJointSpherePersist((physJointSphere *)joint, bodyA, bodyB);
	physJointSphereGenerateBias((physJointSphere *)joint, bodyA, bodyB, dt_s);
	#ifdef PHYSICS_JOINT_SPHERE_WARM_START
	physJointSphereWarmStart((physJointSphere *)joint, bodyA, bodyB);
	#endif

}

static __FORCE_INLINE__ void physJointSphereAngularVelocityImpulse(
	const vec3 relativeVelocity, const vec3 axis,
	const float bias, const float inverseMass,
	float *const __RESTRICT__ impulseAccumulator, vec3 *const __RESTRICT__ impulse
){

	// Calculate the Lagrange multiplier.
	// lambda = -(JV + b)/(JM^{-1}J^T)
	const float lambda = -(vec3Dot(relativeVelocity, axis) + bias) * inverseMass;

	// Clamp the accumulated impulse magnitude.
	const float impulseOld = *impulseAccumulator;
	*impulseAccumulator = floatMin(impulseOld + lambda, 0.f);

	// Calculate the total impulse.
	vec3fmafP(*impulseAccumulator - impulseOld, &axis, impulse);
}

void physJointSphereSolveVelocityConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	vec3 impulse = g_vec3Zero;

	// Solve the angular swing and twist constraints.
	{
		// C' = JV = wB - wA
		vec3 relativeVelocity = vec3VSubV(bodyB->angularVelocity, bodyA->angularVelocity);
		// Solve the swing constraint.
		if(flagsAreSet(((physJointSphere *)joint)->limitStates, PHYSICS_JOINT_SPHERE_LIMITS_SWING)){
			physJointSphereAngularVelocityImpulse(
				relativeVelocity, ((physJointSphere *)joint)->swingAxis,
				((physJointSphere *)joint)->swingBias, ((physJointSphere *)joint)->swingInverseEffectiveMass,
				&((physJointSphere *)joint)->swingImpulse, &impulse
			);

			#ifdef PHYSICS_JOINT_SPHERE_SWING_USE_ELLIPSE_NORMAL
			impulse = physJointSphereSwingImpulseRemoveTwist(((physJointSphere *)joint)->twistAxis, impulse);
			#endif
		}
		// Solve the twist constraint.
		if(flagsAreSet(((physJointSphere *)joint)->limitStates, PHYSICS_JOINT_SPHERE_LIMITS_TWIST)){
			physJointSphereAngularVelocityImpulse(
				relativeVelocity, ((physJointSphere *)joint)->twistAxis,
				((physJointSphere *)joint)->twistBias, ((physJointSphere *)joint)->twistInverseEffectiveMass,
				&((physJointSphere *)joint)->twistImpulse, &impulse
			);
		}else{
			((physJointSphere *)joint)->twistImpulse = 0.f;
		}
		// Apply the correctional impulse.
		physRigidBodyApplyAngularImpulseInverse(bodyA, impulse);
		physRigidBodyApplyAngularImpulse(bodyB, impulse);
	}

	// Solve the linear point-to-point constraint last,
	// as it's more important that it's satisfied.
	{
		// v_socket   = vA + wA X rA
		// v_ball     = vB + wB X rB
		// v_relative = v_ball - v_socket
		impulse = vec3VSubV(
			vec3VAddV(vec3Cross(bodyB->angularVelocity, ((physJointSphere *)joint)->rB), bodyB->linearVelocity),
			vec3VAddV(vec3Cross(bodyA->angularVelocity, ((physJointSphere *)joint)->rA), bodyA->linearVelocity)
		);
		// Subtract the bias term from the negative relative velocity.
		impulse = vec3VSubV(impulse, ((physJointSphere *)joint)->linearBias);
		// Solve for the linear impulse:
		// J1*V + b1 = v_relative + b1,
		// K1*lambda = -(J1*V + b1).
		impulse = vec3VAddV(
			mat3MMultV(((physJointSphere *)joint)->linearInverseEffectiveMass, impulse),
			((physJointSphere *)joint)->linearImpulse
		);
		// Apply the correctional impulse.
		physRigidBodyApplyImpulseInverse(bodyA, ((physJointSphere *)joint)->rA, impulse);
		physRigidBodyApplyImpulse(bodyB, ((physJointSphere *)joint)->rB, impulse);
	}

}

return_t physJointSphereSolveConfigurationConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	#ifdef PHYSICS_JOINT_SPHERE_STABILIZER_GAUSS_SEIDEL

	vec3 impulse;
	float swingError, twistError, linearError;

	// Solve the angular constraints.
	{
		vec3 twistImpulse;
		float swingDeviation, twistDeviation;
		const flags_t limitStates = physJointSphereGenerateAngularBias(
			(physJointSphere *)joint,
			bodyA->configuration.orientation, bodyB->configuration.orientation,
			&impulse, &swingDeviation,
			&twistImpulse, &twistDeviation
		);
		// Solve the constraints if they're being violated.
		if(limitStates != PHYSICS_JOINT_SPHERE_LIMITS_FREE){
			float swingInverseMass;
			float twistInverseMass;
			// Calculate the angular errors.
			swingError = swingDeviation;
			twistError = fabsf(twistDeviation);
			// The Baumgarte bias stops us from adding the entire
			// correction in one step, which aids stability.
			swingDeviation = floatClamp(
				PHYSICS_JOINT_SPHERE_BAUMGARTE_BIAS * (swingDeviation - PHYSICS_JOINT_SPHERE_ANGULAR_SLOP),
				0.f, PHYSICS_JOINT_SPHERE_MAXIMUM_ANGULAR_CORRECTION
			);
			twistDeviation = floatClamp(
				PHYSICS_JOINT_SPHERE_BAUMGARTE_BIAS * (twistDeviation - PHYSICS_JOINT_SPHERE_ANGULAR_SLOP),
				0.f, PHYSICS_JOINT_SPHERE_MAXIMUM_ANGULAR_CORRECTION
			);
			physJointSphereGenerateAngularInverseEffectiveMass(bodyA, bodyB, impulse, twistImpulse, &swingInverseMass, &twistInverseMass);
			// lambda = -C/K
			impulse = vec3VMultS(impulse, -swingDeviation*swingInverseMass);
			#ifdef PHYSICS_JOINT_SPHERE_SWING_USE_ELLIPSE_NORMAL
			impulse = physJointSphereSwingImpulseRemoveTwist(twistImpulse, impulse);
			#endif
			impulse = vec3fmaf(-twistDeviation*twistInverseMass, twistImpulse, impulse);
			// Apply the correctional impulse.
			physRigidBodyApplyAngularConfigurationImpulseInverse(bodyA, impulse);
			physRigidBodyApplyAngularConfigurationImpulse(bodyB, impulse);
		}else{
			swingError = 0.f;
			twistError = 0.f;
		}
	}

	// Solve the linear point-to-point constraint.
	{
		// Retransform the anchor points.
		const vec3 rA = tfTransformDirection(
			bodyA->configuration,
			#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
			vec3VSubV(((physJointSphere *)joint)->anchorA, bodyA->centroidLocal)
			#else
			vec3VSubV(((physJointSphere *)joint)->anchorA, bodyA->base->centroid)
			#endif
		);
		const vec3 rB = tfTransformDirection(
			bodyB->configuration,
			#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
			vec3VSubV(((physJointSphere *)joint)->anchorB, bodyB->centroidLocal)
			#else
			vec3VSubV(((physJointSphere *)joint)->anchorB, bodyB->base->centroid)
			#endif
		);
		vec3 constraint;
		mat3 linearMass;
		// Calculate the displacement from the ball to the socket:
		// -C1 = (pA + rA) - (pB - rB).
		// Note that we want -C1, so we swap the inputs around.
		constraint = physJointSphereGenerateLinearBias(
			bodyB->centroidGlobal, bodyA->centroidGlobal, rB, rA
		);
		linearError = vec3Magnitude(constraint);
		#ifdef PHYSICS_JOINT_SPHERE_STABILIZER_BAUMGARTE
		constraint = vec3VMultS(constraint, PHYSICS_JOINT_SPHERE_BAUMGARTE_BIAS);
		#endif
		// Solve for the impulse:
		// K1*lambda = -C1.
		physJointSphereGenerateLinearInverseEffectiveMass(bodyA, bodyB, rA, rB, &linearMass);
		impulse = mat3Solve(linearMass, constraint);
		// Apply the correctional impulse.
		physRigidBodyApplyConfigurationImpulseInverse(bodyA, rA, impulse);
		physRigidBodyApplyConfigurationImpulse(bodyB, rB, impulse);
	}

	return
		swingError <= PHYSICS_JOINT_SPHERE_ANGULAR_ERROR_THRESHOLD &&
		twistError <= PHYSICS_JOINT_SPHERE_ANGULAR_ERROR_THRESHOLD &&
		linearError <= PHYSICS_JOINT_SPHERE_LINEAR_ERROR_THRESHOLD
	;

	#else
	return 1;
	#endif

}