#ifndef PHYSICSCOLLISION_H
#define PHYSICSCOLLISION_H

#include "physicsShared.h"
#include "collision.h"

#ifndef PHYSICS_CONTACT_SOLVER_ITERATIONS
	#define PHYSICS_CONTACT_SOLVER_ITERATIONS 4
#endif

#ifndef PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS 0
#endif

#ifndef PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS 0
#endif

#ifndef PHYSICS_RESTITUTION_THRESHOLD
	#define PHYSICS_RESTITUTION_THRESHOLD 1.f
#endif

#ifndef PHYSICS_BAUMGARTE_TERM
	#define PHYSICS_BAUMGARTE_TERM 0.2f
#endif

#ifndef PHYSICS_PENETRATION_SLOP
	#define PHYSICS_PENETRATION_SLOP 0.05f
#endif

/*
** Contact constraints are separated from other constraints
** solely because of their size. The size of contact manifold
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

	// Contact points in both colliders' local spaces.
	vec3 pointA;
	vec3 pointB;

	// Penetration depth.
	float penetrationDepth;

	// Impulse magnitude denominators.
	float normalImpulseDenominator;
	float tangentImpulseDenominatorA;
	float tangentImpulseDenominatorB;

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
	physCollider *colliderA;
	physCollider *colliderB;

	// The previous and next contact pairs in collider A's array.
	physContactPair *prevA;
	physContactPair *nextA;

	// The previous and next contact pairs in collider B's array.
	physContactPair *prevB;
	physContactPair *nextB;

} physContactPair;

typedef struct cSeparation physSeparation;

typedef struct physSeparationPair physSeparationPair;
typedef struct physSeparationPair {

	// Cached pair data.
	physSeparation data;

	// How many steps have passed since the pair was updated.
	physPairTimestamp_t inactive;

	// Pointers to the reference and incident colliders.
	physCollider *colliderA;
	physCollider *colliderB;

	// The previous and next separation pairs in collider A's array.
	physSeparationPair *prevA;
	physSeparationPair *nextA;

	// The previous and next separation pairs in collider B's array.
	physSeparationPair *prevB;
	physSeparationPair *nextB;

} physSeparationPair;

void physContactUpdate(physContact *const restrict contact, physCollider *const restrict colliderA, physCollider *const restrict colliderB, const float dt);

void physContactPairDeactivate(void *const restrict pair);
void physSeparationPairDeactivate(void *const restrict pair);
void physContactPairInvalidate(void *const restrict pair);
void physSeparationPairInvalidate(void *const restrict pair);

return_t physContactPairIsActive(physContactPair *const restrict pair);
return_t physSeparationPairIsActive(physSeparationPair *const restrict pair);
return_t physContactPairIsInactive(physContactPair *const restrict pair);
return_t physSeparationPairIsInactive(physSeparationPair *const restrict pair);
return_t physContactPairIsInvalid(physContactPair *const restrict pair);
return_t physSeparationPairIsInvalid(physSeparationPair *const restrict pair);

void physContactPairRefresh(physContactPair *const restrict pair);
void physSeparationPairRefresh(physSeparationPair *const restrict pair);

void physContactPairInit(physContactPair *const pair, physCollider *const c1, physCollider *const c2, physContactPair *previous, physContactPair *next);
void physSeparationPairInit(physSeparationPair *const pair, physCollider *const c1, physCollider *const c2, physSeparationPair *previous, physSeparationPair *next);
void physContactPairDelete(physContactPair *const pair);
void physSeparationPairDelete(physSeparationPair *const pair);

return_t physCollisionQuery(aabbNode *const restrict n1, aabbNode *const restrict n2);

void physContactSolve(physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB);

#endif
