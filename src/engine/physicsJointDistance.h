#ifndef PHYSICSJOINTDISTANCE_H
#define PHYSICSJOINTDISTANCE_H

#include "physicsSettings.h"
#include "vec3.h"

/** Velocity seems to be exploding under certain circumstances. Investigate? **/

// Constrains two bodies so that they
// stay at a certain distance from each
// other. Can be rigid (by setting the
// angular frequency to 0) or soft and
// "springy".

typedef struct physRigidBody physRigidBody;
typedef struct physJoint physJoint;
typedef struct {

	// Application points in both bodies' local spaces.
	vec3 anchorA;
	vec3 anchorB;

	// The distance that the joint attempts to maintain.
	float distance;

	// The natural (angular) frequency of the mass-spring-damper
	// system in radians per second. A value of 0 disables
	// spring softening.
	//     omega = 2 * pi * f
	//     f = frequency in hertz
	// Ideally, this frequency f should be less than half the
	// time step (the Nyquist frequency) to avoid introducing
	// errors.
	float angularFrequency;

	// Spring damping coefficient.
	//     d = 2 * omega * zeta
	//     omega = angular frequency
	//     zeta = damping ratio
	float damping;

	// "Magic" constants from Erin Catto's GDC 2011 presentation
	// on soft constraints in ODE. The bias is simply a Baumgarte
	// term with a special value for beta.
	//     gamma = 1/(h(hk + c))
	//     beta  = hk/(hk + c)
	//     bias  = beta/h * C
	//           = C * h * k * gamma
	//     h = time step
	//     k = spring stiffness
	//     c = damping coefficient
	float gamma;
	float bias;

	// Transformed anchor points in global space.
	vec3 rA;
	vec3 rB;

	// Anchor separation (rB - rA).
	vec3 rAB;

	// Inverse effective mass and impulse accumulator.
	float inverseEffectiveMass;
	float impulseAccumulator;

} physJointDistance;

void physJointDistanceInit(physJointDistance *const __RESTRICT__ joint, const vec3 anchorA, const vec3 anchorB, const float distance, const float frequency, const float damping);
void physJointDistancePresolveConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float dt_s);
void physJointDistanceSolveVelocityConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
return_t physJointDistanceSolveConfigurationConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
#endif
void physJointDistanceSetFrequency(physJointDistance *const __RESTRICT__ joint, const float frequency);
void physJointDistanceSetDamping(physJointDistance *const __RESTRICT__ joint, const float damping);

#endif