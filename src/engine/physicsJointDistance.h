#ifndef PHYSICSJOINTDISTANCE_H
#define PHYSICSJOINTDISTANCE_H

#include "physicsSettings.h"
#include "vec3.h"

/** Velocity seems to be exploding under certain circumstances. Investigate? **/

#ifndef PHYSICS_JOINT_DISTANCE_LINEAR_SLOP
	#define PHYSICS_JOINT_DISTANCE_LINEAR_SLOP 0.005f
#endif
#ifdef PHYSICS_JOINT_DISTANCE_STABILIZER_BAUMGARTE
	#ifndef PHYSICS_JOINT_DISTANCE_BAUMGARTE_BIAS
		#define PHYSICS_JOINT_DISTANCE_BAUMGARTE_BIAS 0.1f
	#endif
#endif
#ifdef PHYSICS_JOINT_DISTANCE_STABILIZER_GAUSS_SEIDEL
	// Error threshold for NGS configuration solving.
	#ifndef PHYSICS_JOINT_DISTANCE_LINEAR_ERROR_THRESHOLD
		#define PHYSICS_JOINT_DISTANCE_LINEAR_ERROR_THRESHOLD (3.f * PHYSICS_JOINT_DISTANCE_LINEAR_SLOP)
	#endif
#endif

// Constrains two bodies so that they
// stay at a certain distance from each
// other. Can be rigid (by setting the
// stiffness to 0) or soft and "springy".

typedef struct physRigidBody physRigidBody;
typedef struct physJoint physJoint;

typedef struct {
	// Application points in both bodies' local spaces.
	vec3 anchorA;
	vec3 anchorB;
	// The distance that the joint attempts to maintain.
	float distance;
	// Spring stiffness coefficient, k.
	// The natural frequency of the joint is omega = sqrt(k/m).
	float stiffness;
	// Spring damping coefficient, c.
	// Critical damping occurs at c = 2*sqrt(km).
	float damping;
} physJointDistanceBase;

typedef struct {

	// Application points in both bodies' local spaces.
	vec3 anchorA;
	vec3 anchorB;

	// The distance that the joint attempts to maintain.
	float distance;

	// Spring stiffness coefficient, k.
	// The natural frequency of the joint is omega = sqrt(k/m).
	float stiffness;

	// Spring damping coefficient, c.
	// Critical damping occurs at c = 2*sqrt(km).
	float damping;

	// "Magic" constants from Erin Catto's GDC 2011 presentation
	// on soft constraints in ODE. The bias is simply a Baumgarte
	// term with a special value for beta.
	//     gamma = 1/(h(hk + c))
	//     beta  = hk/(hk + c)
	//     bias  = (beta/h)*C = C*h*k*gamma,
	// where h is the time step, k is the spring stiffness and
	// c is the damping coefficient. Note that the extra h in
	// the denominator for gamma is because we're using impulses
	// rather than forces.
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

void physJointDistanceInit(physJointDistance *const __RESTRICT__ joint, const vec3 anchorA, const vec3 anchorB, const float distance, const float stiffness, const float damping);
void physJointDistancePresolveConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float dt_s);
void physJointDistanceSolveVelocityConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
return_t physJointDistanceSolveConfigurationConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);

#endif
