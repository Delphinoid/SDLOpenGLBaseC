#ifndef PHYSICSJOINTFRICTION_H
#define PHYSICSJOINTFRICTION_H

#include "physicsSettings.h"
#include "vec3.h"
#include "mat2.h"

// The friction joint is a special kind of motor.
// It is only intended to be used in physics contacts,
// when "PHYSICS_CONTACT_FRICTION_CONSTRAINT" is defined.

typedef struct physRigidBody physRigidBody;
typedef struct {

	// Halfway points transformed by the
	// bodies' global configurations.
	vec3 rA;
	vec3 rB;

	// Average contact normal.
	vec3 normal;

	// Contact tangents for simulating friction.
	vec3 tangent[2];

	// Friction inverse effective mass.
	mat2 tangentInverseEffectiveMass;
	float angularInverseEffectiveMass;

	// Impulse magnitude accumulators for friction.
	vec2 tangentImpulseAccumulator;
	float angularImpulseAccumulator;

	// Coefficient of friction.
	float friction;

} physJointFriction;

#ifdef PHYSICS_JOINT_FRICTION_WARM_START
void physJointFrictionWarmStart(const physJointFriction *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
#endif
void physJointFrictionGenerateInverseEffectiveMass(physJointFriction *const __RESTRICT__ joint, const physRigidBody *const __RESTRICT__ bodyA, const physRigidBody *const __RESTRICT__ bodyB, const float inverseMassTotal);
void physJointFrictionSolveVelocityConstraints(physJointFriction *const __RESTRICT__ joint, physRigidBody *const bodyA, physRigidBody *const bodyB, const float normalImpulseTotal);

#endif
