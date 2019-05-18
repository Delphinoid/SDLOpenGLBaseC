#ifndef PHYSICSCOLLISION_H
#define PHYSICSCOLLISION_H

#include "physicsShared.h"
#include "physicsConstraint.h"
#include "collision.h"

#ifndef PHYSICS_CONTACT_VELOCITY_SOLVER_ITERATIONS
	#define PHYSICS_CONTACT_VELOCITY_SOLVER_ITERATIONS 40
#endif

#ifndef PHYSICS_CONTACT_CONFIGURATION_SOLVER_ITERATIONS
	#define PHYSICS_CONTACT_CONFIGURATION_SOLVER_ITERATIONS 4
#endif

#ifndef PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS 0
#endif

#ifndef PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS 0
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
	vec3 halfwayA;
	vec3 halfwayB;

	#ifdef PHYSICS_GAUSS_SEIDEL_SOLVER

	// Contact point in both colliders' local spaces.
	// "Untransforms" them using the conjugates of their
	// colliders' orientations, as they will need to be
	// transformed into global space later when solving.
	vec3 pointA;
	vec3 pointB;

	// The "untransformed" normal relative to pointA.
	vec3 normalA;

	#endif

	// Penetration depth.
	float penetrationDepth;

	// Impulse magnitude denominators.
	float normalEffectiveMass;
	float tangentEffectiveMassA;
	float tangentEffectiveMassB;

	// Persistent impulse magnitude accumulator.
	float normalImpulseAccumulator;

	// Persistent impulse magnitude accumulators for friction.
	float tangentImpulseAccumulatorA;
	float tangentImpulseAccumulatorB;

	// Bias term for warm starting.
	float bias;

	// Key for temporal coherence.
	cContactKey key;

} physContactPoint;

typedef struct physContact {

	// Contact array.
	physContactPoint contacts[COLLISION_MANIFOLD_MAX_CONTACT_POINTS];

	// Contact normal.
	vec3 normal;

	// Contact tangents for simulating friction.
	vec3 tangentA;
	vec3 tangentB;

	float friction;
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

#if !defined PHYSICS_GAUSS_SEIDEL_SOLVER || defined PHYSICS_FORCE_VELOCITY_BAUMGARTE
void physContactUpdate(physContact *const restrict contact, physCollider *const restrict colliderA, physCollider *const restrict colliderB, const float dt);
#else
void physContactUpdate(physContact *const restrict contact, physCollider *const restrict colliderA, physCollider *const restrict colliderB);
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
#ifdef PHYSICS_GAUSS_SEIDEL_SOLVER
float physContactSolveConfigurationConstraints(physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, float error);
#endif

#endif
