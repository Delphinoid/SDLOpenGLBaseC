#ifndef PHYSICSJOINTSPHERE_H
#define PHYSICSJOINTSPHERE_H

#include "physicsSettings.h"
#include "mat3.h"
#include "flags.h"

#define PHYSICS_JOINT_SPHERE_CONE_LIMIT_DISABLED 0x00
#define PHYSICS_JOINT_SPHERE_CONE_LIMIT_ENABLED  0x01

/** This implementation seems utterly wrong. **/

// Allows bodies to rotate around each other
// while maintaining their relative rotation.
// The angle may be constrained by a cone.

typedef struct physRigidBody physRigidBody;
typedef struct physJoint physJoint;
typedef struct {

	// Local anchor transformations.
	vec3 anchorA;
	mat3 anchorOrientationA;
	vec3 anchorB;
	mat3 anchorOrientationB;

	// Transformed anchor points in global space.
	vec3 rA;
	vec3 rB;

	// Linear constraint effective mass and impulse accumulator.
	mat3 linearEffectiveMass;
	vec3 linearImpulseAccumulator;

	// The cone axis.
	vec3 coneLimitAxis;

	// Half of the cone angle in radians.
	float coneLimitAngle;

	// Cone constraint inverse effective mass and impulse accumulator.
	float coneLimitInverseEffectiveMass;
	float coneLimitImpulseAccumulator;

	// Current state of the cone constraint.
	flags_t coneLimitState;

} physJointSphere;

void physJointSphereInit(physJointSphere *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const vec3 anchorA, const vec3 anchorB, const flags_t coneLimitState, const vec3 coneAxis, const float coneAngle);
void physJointSpherePresolveConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float dt_s);
void physJointSphereSolveVelocityConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
return_t physJointSphereSolveConfigurationConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
#endif

#endif