#ifndef PHYSICSCONSTRAINT_H
#define PHYSICSCONSTRAINT_H

#include "physicsBodyShared.h"
#include "vec3.h"
#include "flags.h"

#define PHYSICS_CONSTRAINT_TYPE_1  0x01
#define PHYSICS_CONSTRAINT_TYPE_2  0x02
#define PHYSICS_CONSTRAINT_COLLIDE 0x04

typedef uint_least8_t physConstraintIndex_t;

typedef struct {
	flags_t flags;
	physicsBodyIndex_t ownerID;       // An identifier for the body that owns the constraint.
	physicsBodyIndex_t constraintID;  // An identifier for the other body being constrained.
	vec3 constraintOffsetMin;
	vec3 constraintOffsetMax;
	vec3 constraintRotationMin;
	vec3 constraintRotationMax;
} physConstraint;

void physConstraintInit(physConstraint *constraint);

#endif
