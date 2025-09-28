#ifndef PHYSICSCOLLISION_H
#define PHYSICSCOLLISION_H

#include "settingsPhysics.h"
#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT
#include "physicsJointFriction.h"
#endif
#include "collision.h"
#include "mat2.h"

#ifndef PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS 0
#endif
#ifndef PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS 0
#endif

#ifndef PHYSICS_SEPARATION_BIAS
	#define PHYSICS_SEPARATION_BIAS 0.f
#endif
#ifndef PHYSICS_CONTACT_LINEAR_SLOP
	#define PHYSICS_CONTACT_LINEAR_SLOP 0.05f
#endif
#ifndef PHYSICS_CONTACT_RESTITUTION_THRESHOLD
	#define PHYSICS_CONTACT_RESTITUTION_THRESHOLD 1.f
#endif
#ifndef PHYSICS_CONTACT_BAUMGARTE_BIAS
	#define PHYSICS_CONTACT_BAUMGARTE_BIAS 0.1f
#endif
#ifdef PHYSICS_CONTACT_STABILIZER_GAUSS_SEIDEL
	// Error threshold for NGS configuration solving.
	#ifndef PHYSICS_CONTACT_ERROR_THRESHOLD
		#define PHYSICS_CONTACT_ERROR_THRESHOLD (-3.f * PHYSICS_CONTACT_LINEAR_SLOP)
	#endif
	// Maximum linear correction for NGS configuration solving.
	#ifndef PHYSICS_CONTACT_MAXIMUM_LINEAR_CORRECTION
		#define PHYSICS_CONTACT_MAXIMUM_LINEAR_CORRECTION 0.2f
	#endif
#endif

// Contact constraints are separated from other constraints
// solely because of their size. The size of the contact manifold
// significantly exceeds the size of all other types of
// constraints, which would lead to an unacceptable amount of
// internal fragmentation.
///
// This, however, has an additional bonus of allowing contacts
// to be more conveniently solved separately to constraints.
// This means we can solve them after solving all regular joints,
// giving them "priority".

typedef struct aabbNode aabbNode;
typedef struct physRigidBody physRigidBody;
typedef struct physCollider physCollider;

typedef uint_least8_t physPairTimestamp_t;

typedef struct physContactPoint {

	// Point halfway between both contact points in both colliders' local spaces.
	vec3 rA;
	vec3 rB;

	#ifdef PHYSICS_CONTACT_STABILIZER_GAUSS_SEIDEL
	// Contact point in both colliders' local spaces.
	// "Untransforms" them using the conjugates of their
	// colliders' orientations, as they will need to be
	// transformed into global space later when solving.
	vec3 pointA;
	vec3 pointB;
	#endif

	#ifdef PHYSICS_CONTACT_STABILIZER_BAUMGARTE
	// Penetration depth. Always negative.
	float separation;
	#endif



	// Normal impulse magnitude denominator.
	float normalInverseEffectiveMass;

	// Persistent impulse magnitude accumulator.
	float normalImpulseAccumulator;

	#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT

	// Friction inverse effective mass.
	float tangentInverseEffectiveMass[2];

	// Impulse magnitude accumulators for friction.
	float tangentImpulseAccumulator[2];

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

	#ifdef PHYSICS_CONTACT_STABILIZER_GAUSS_SEIDEL

	// The "untransformed" average normal relative to the average pointA.
	vec3 normalA;

	#endif

	#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT

	// The friction constraint, if we're simulating friction using motors.
	physJointFriction frictionConstraint;

	#else

	// Contact tangents for simulating friction.
	vec3 tangent[2];

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

	// Previous and next pointers for collider A's
	// and collider B's contact pair arrays.
	#ifndef PHYSICS_CONTACT_USE_ALLOCATOR
	physContactPair *prevA, *nextA;
	physContactPair *prevB, *nextB;
	#endif

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

	// Previous and next pointers for collider A's
	// and collider B's separation pair arrays.
	#ifndef PHYSICS_CONTACT_USE_ALLOCATOR
	physSeparationPair *prevA, *nextA;
	physSeparationPair *prevB, *nextB;
	#endif

} physSeparationPair;

void physContactInit(physContact *const __RESTRICT__ contact, const cContact *const __RESTRICT__ manifold, const physRigidBody *const __RESTRICT__ bodyA, const physRigidBody *const __RESTRICT__ bodyB, const physCollider *const __RESTRICT__ colliderA, const physCollider *const __RESTRICT__ colliderB);
void physContactPersist(physContact *const __RESTRICT__ contact, const cContact *const __RESTRICT__ manifold, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const physCollider *const __RESTRICT__ colliderA, const physCollider *const __RESTRICT__ colliderB);

#ifdef PHYSICS_CONTACT_STABILIZER_BAUMGARTE
void physContactPresolveConstraints(physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float frequency);
#else
void physContactPresolveConstraints(physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
#endif
void physContactReset(physContact *const __RESTRICT__ contact);

void physContactPairRefresh(physContactPair *const __RESTRICT__ pair);
void physSeparationPairRefresh(physSeparationPair *const __RESTRICT__ pair);

void physContactPairInit(physContactPair *const pair, physCollider *const c1, physCollider *const c2, physContactPair *previous, physContactPair *next);
void physSeparationPairInit(physSeparationPair *const pair, physCollider *const c1, physCollider *const c2, physSeparationPair *previous, physSeparationPair *next);
void physContactPairDelete(physContactPair *const pair);
void physSeparationPairDelete(physSeparationPair *const pair);

void physContactSolveVelocityConstraints(physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
#ifdef PHYSICS_CONTACT_STABILIZER_GAUSS_SEIDEL
float physContactSolveConfigurationConstraints(physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, float separation);
#endif

#endif
