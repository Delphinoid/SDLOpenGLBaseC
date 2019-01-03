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
typedef struct {
	void *bodyA;  // Pointer to the reference body.
	void *bodyB;  // Pointer to the incident body.
	cContactManifold manifold;
} physContact;

typedef struct {
	physicsBodyIndex_t id;  // An identifier for the other body involved in the collision.
	cSeparationContainer separation;
} physSeparation;

void physContactSolve(physContact *contact);

#endif
