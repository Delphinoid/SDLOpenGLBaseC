#ifndef PHYSICSCOLLISION_H
#define PHYSICSCOLLISION_H

#include "physicsBodyShared.h"
#include "collision.h"

// Contact constraints are separated from other constraints
// solely because of their size. The size of contact manifolds
// significantly exceeds the size of all other types of
// constraints, which would lead to an unacceptable amount of
// internal fragmentation.
// This, however, has an additional bonus of allowing contacts
// to be more conveniently solved separately to constraints.
// This means we can solve them after solving all regular joints,
// giving them "priority".
typedef struct physRigidBody physRigidBody;
typedef struct {
	physRigidBody *bodyA;  // Pointer to the reference body.
	physRigidBody *bodyB;  // Pointer to the incident body.
	cContact manifold;
} physContact;

typedef struct {
	physicsBodyIndex_t id;  // An identifier for the other body involved in the collision.
	cSeparation separation;
} physSeparation;

typedef struct {
	int temp;
	///physCollider *incident;
	///physSeparation *separations;
} physCollisionCache;

void physContactSolve(physContact *contact);

return_t physCheckCollision(physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB,
                            physSeparation *const restrict sc, physContact *const restrict cm);
return_t physCheckSeparation(physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB,
                             physSeparation *const restrict sc);

#endif
