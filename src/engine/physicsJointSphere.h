#ifndef PHYSICSJOINTSPHERE_H
#define PHYSICSJOINTSPHERE_H

#include "settingsPhysics.h"
#include "constantsMath.h"
#include "mat3.h"
#include "flags.h"

#ifndef PHYSICS_JOINT_SPHERE_ANGULAR_SLOP
	#define PHYSICS_JOINT_SPHERE_ANGULAR_SLOP (2.f * RADIAN_RATIO)
#endif
#ifndef PHYSICS_JOINT_SPHERE_BAUMGARTE_BIAS
	#define PHYSICS_JOINT_SPHERE_BAUMGARTE_BIAS 0.3f
#endif
#ifdef PHYSICS_JOINT_SPHERE_STABILIZER_GAUSS_SEIDEL
	// Linear and angular error thresholds for NGS configuration solving.
	#ifndef PHYSICS_JOINT_SPHERE_LINEAR_ERROR_THRESHOLD
		#ifndef PHYSICS_JOINT_SPHERE_LINEAR_SLOP
			#define PHYSICS_JOINT_SPHERE_LINEAR_SLOP 0.005f
		#endif
		#define PHYSICS_JOINT_SPHERE_LINEAR_ERROR_THRESHOLD (3.f * PHYSICS_JOINT_SPHERE_LINEAR_SLOP)
	#endif
	#ifndef PHYSICS_JOINT_SPHERE_ANGULAR_ERROR_THRESHOLD
		#define PHYSICS_JOINT_SPHERE_ANGULAR_ERROR_THRESHOLD (3.f * PHYSICS_JOINT_SPHERE_ANGULAR_SLOP)
	#endif
	// Maximum angular correction for NGS configuration solving.
	#ifndef PHYSICS_JOINT_SPHERE_MAXIMUM_ANGULAR_CORRECTION
		#define PHYSICS_JOINT_SPHERE_MAXIMUM_ANGULAR_CORRECTION (8.f * RADIAN_RATIO)
	#endif
#endif

// Allows two bodies to rotate around each other while
// maintaining their relative orientations, similarly to
// a ball-and-socket joint. An angular constraint may
// also be specified, such as a conic constraint.
//
// In practice, this is similar to a distance joint with
// an additional angular constraint, although a different
// point-to-point constraint is used. This is because the
// one for distance joints seems unstable for 0 distance.

typedef struct physRigidBody physRigidBody;
typedef struct physJoint physJoint;

typedef struct {

	// Local anchor transformations, including the
	// relative orientations between the bodies.
	vec3 anchorA;
	vec3 anchorB;
	quat anchorOrientationA;
	quat anchorOrientationB;

	// Restitution of the joint; a value of 1 means the joint
	// will fully bounce back when hitting a limit, while a
	// value of 0 means the joint will stay at the limit.
	float restitution;

	// Stores the minimum and maximum angular
	// limits (in that order) for each axis.
	//
	// For Euler angle constraints, we use the
	// order XYZ for rotations. Therefore, to
	// guarantee a unique representation for any
	// orientation, we must limit X to [-pi, pi],
	// Y to [-pi/2, pi/2] and Z to [-pi, pi].
	float angularLimitsX[2];
	float angularLimitsY[2];
	float angularLimitsZ[2];

} physJointSphereBase;

typedef struct {

	// Local anchor transformations.
	vec3 anchorA;
	vec3 anchorB;
	quat anchorOrientationA;
	quat anchorOrientationB;

	// Transformed anchor points in global space.
	vec3 rA;
	vec3 rB;

	// Restitution of the joint; a value of 1 means the joint
	// will fully bounce back when hitting a limit, while a
	// value of 0 means the joint will stay at the limit.
	float restitution;

	// Offset of the ball (rigid body B)
	// from the socket (rigid body A).
	vec3 linearBias;

	// Stores the minimum and maximum angular
	// limits (in that order) for each axis.
	//
	// For Euler angle constraints, we use the
	// order XYZ for rotations. Therefore, to
	// guarantee a unique representation for any
	// orientation, we must limit X to [-pi, pi],
	// Y to [-pi/2, pi/2] and Z to [-pi, pi].
	float angularLimitsX[2];
	float angularLimitsY[2];
	float angularLimitsZ[2];
	// Swing and twist axes in global space.
	// Note that the twist axis is taken to
	// be rigid body B's transformed x-axis.
	vec3 swingAxis;
	vec3 twistAxis;
	// Difference between the current swing
	// and twist angles and their limits.
	float swingBias;
	float twistBias;

	// Inverse effective masses, (JMJ^T)^{-1}, for the
	// point-to-point (K1) and angular (K2, K3, K4) constraints.
	float swingInverseEffectiveMass;
	float twistInverseEffectiveMass;
	mat3 linearInverseEffectiveMass;

	// Accumulated impulses used for warm starting.
	float swingImpulse;
	float twistImpulse;
	vec3 linearImpulse;

	// Flags that tell us which limits need to be solved.
	// If we're using quaternion swing-twist limits, a single bit
	// is used to tell us whether or not the swing limit is broken.
	// Otherwise two bits are used for all other angular limits to
	// specify whether the upper or lower limits have been broken.
	flags_t limitStates;

} physJointSphere;

void physJointSphereInit(
	physJointSphere *const __RESTRICT__ joint,
	const vec3 anchorA, const vec3 anchorB,
	const quat anchorOrientationA, const quat anchorOrientationB,
	const float restitution,
	const float minX, const float maxX,
	const float minY, const float maxY,
	const float minZ, const float maxZ
);
void physJointSpherePresolveConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float dt_s);
void physJointSphereSolveVelocityConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
return_t physJointSphereSolveConfigurationConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);

#endif
