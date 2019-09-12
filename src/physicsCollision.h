#ifndef PHYSICSCOLLISION_H
#define PHYSICSCOLLISION_H

#include "physicsShared.h"
#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT
#include "physicsJointFriction.h"
#endif
#include "physicsConstraint.h"
#include "collision.h"
#include "mat2.h"

#ifndef PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS 0
#endif

#ifndef PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS 0
#endif

#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
	// Error threshold for NGS configuration solving.
	#ifndef PHYSICS_CONTACT_ERROR_THRESHOLD
		#define PHYSICS_CONTACT_ERROR_THRESHOLD (-3.f * PHYSICS_LINEAR_SLOP)
	#endif
#endif

/*
** Contact constraints are separated from other constraints
** solely because of their size. The size of the contact manifold
** significantly exceeds the size of all other types of
** constraints, which would lead to an unacceptable amount of
** internal fragmentation.
**
** This, however, has an additional bonus of allowing contacts
** to be more conveniently solved separately to constraints.
** This means we can solve them after solving all regular joints,
** giving them "priority".
*/

typedef struct aabbNode aabbNode;
typedef struct physRigidBody physRigidBody;
typedef struct physCollider physCollider;

typedef uint_least8_t physPairTimestamp_t;

typedef struct physContactPoint {

	// Point halfway between both contact points in both colliders' local spaces.
	vec3 rA;
	vec3 rB;

	#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL

	// Contact point in both colliders' local spaces.
	// "Untransforms" them using the conjugates of their
	// colliders' orientations, as they will need to be
	// transformed into global space later when solving.
	vec3 pointA;
	vec3 pointB;

	#else

	// Penetration depth. Always negative.
	/** Rename to separation, as it's a negative quantity. **/
	float separation;

	#endif

	// Normal impulse magnitude denominator.
	float normalInverseEffectiveMass;

	// Persistent impulse magnitude accumulator.
	float normalImpulseAccumulator;

	#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT

	// Friction inverse effective mass.
	float tangentInverseEffectiveMass1;
	float tangentInverseEffectiveMass2;

	// Impulse magnitude accumulators for friction.
	float tangentImpulseAccumulator1;
	float tangentImpulseAccumulator2;

	#endif

	// Bias term for velocity warm starting.
	float bias;

	// Key for temporal coherence.
	cContactKey key;

} physContactPoint;

typedef struct physContact {

	// Contact array.
	physContactPoint contacts[COLLISION_MANIFOLD_MAX_CONTACT_POINTS];

	#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT

	// Halfway points transformed by the
	// bodies' global configurations.
	vec3 rA;
	vec3 rB;

	// Average contact normal.
	vec3 normal;

	#endif

	#if defined PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL

	// The "untransformed" average normal relative to the average pointA.
	vec3 normalA;

	#endif

	#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT

	// The friction constraint, if we're simulating friction using motors.
	physJointFriction frictionConstraint;

	#else

	// Contact tangents for simulating friction.
	vec3 tangent1;
	vec3 tangent2;

	// Coefficient of friction.
	float friction;

	#endif

	// Coefficient of restitution.
	float restitution;

	// Number of contacts.
	cContactPointIndex_t contactNum;

} physContact;

typedef struct physContactPair physContactPair;
typedef struct physContactPair {

	// Cached pair data.
	physContact data;

	// How many steps have passed since the pair was updated.
	physPairTimestamp_t inactive;

	// Pointers to the reference and incident colliders.
	// Collider A is guaranteed to have a greater address
	// than collider B.
	physCollider *colliderA;
	physCollider *colliderB;

} physContactPair;

typedef struct cSeparation physSeparation;

typedef struct physSeparationPair physSeparationPair;
typedef struct physSeparationPair {

	// Cached pair data.
	physSeparation data;

	// How many steps have passed since the pair was updated.
	physPairTimestamp_t inactive;

	// Pointers to the reference and incident colliders.
	// Collider A is guaranteed to have a greater address
	// than collider B.
	physCollider *colliderA;
	physCollider *colliderB;

} physSeparationPair;

#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
void physContactPresolveConstraints(physContact *const restrict contact, physCollider *const restrict colliderA, physCollider *const restrict colliderB, const float frequency);
#else
void physContactPresolveConstraints(physContact *const restrict contact, physCollider *const restrict colliderA, physCollider *const restrict colliderB);
#endif
void physContactReset(physContact *const restrict contact);

void physContactPairRefresh(physContactPair *const restrict pair);
void physSeparationPairRefresh(physSeparationPair *const restrict pair);

void physContactPairInit(physContactPair *const pair, physCollider *const c1, physCollider *const c2, physContactPair *previous, physContactPair *next);
void physSeparationPairInit(physSeparationPair *const pair, physCollider *const c1, physCollider *const c2, physSeparationPair *previous, physSeparationPair *next);
void physContactPairDelete(physContactPair *const pair);
void physSeparationPairDelete(physSeparationPair *const pair);

return_t physCollisionQuery(aabbNode *const n1, aabbNode *const n2);

void physContactSolveVelocityConstraints(physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB);
#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
float physContactSolveConfigurationConstraints(physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, float error);
#endif

#endif
