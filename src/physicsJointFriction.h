#ifndef PHYSICSJOINTFRICTION_H
#define PHYSICSJOINTFRICTION_H

#include "vec3.h"
#include "mat2.h"

/*
** The friction joint is a special kind of motor.
** It is only intended to be used in physics contacts,
** when "PHYSICS_CONTACT_FRICTION_CONSTRAINT" is defined.
*/

typedef struct physRigidBody physRigidBody;

typedef struct physJoint physJoint;
typedef struct {

	// Halfway points transformed by the
	// bodies' global configurations.
	vec3 rA;
	vec3 rB;

	// Average contact normal.
	vec3 normal;

	// Contact tangents for simulating friction.
	vec3 tangent1;
	vec3 tangent2;

	// Friction inverse effective mass.
	mat2 tangentInverseEffectiveMass;
	float angularInverseEffectiveMass;

	// Impulse magnitude accumulators for friction.
	vec2 tangentImpulseAccumulator;
	float angularImpulseAccumulator;

	// Coefficient of friction.
	float friction;

} physJointFriction;

void physJointFrictionWarmStart(const physJointFriction *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB);
void physJointFrictionGenerateInverseEffectiveMass(physJointFriction *const restrict joint, const physRigidBody *const restrict bodyA, const physRigidBody *const restrict bodyB, const float inverseMassTotal);
void physJointFrictionSolveVelocityConstraints(physJointFriction *const restrict joint, physRigidBody *const bodyA, physRigidBody *const bodyB, const float normalImpulseTotal);

#endif
