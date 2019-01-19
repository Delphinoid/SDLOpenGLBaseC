#ifndef PHYSICSCONSTRAINT_H
#define PHYSICSCONSTRAINT_H

#include "physicsShared.h"
#include "memoryShared.h"
#include "vec3.h"
#include "flags.h"

#define PHYSICS_CONSTRAINT_TYPE_MASK    0x0F
#define PHYSICS_CONSTRAINT_TYPE_UNKNOWN 0x0F
#define PHYSICS_CONSTRAINT_TYPE_CONTACT 0x01

#define PHYSICS_CONSTRAINT_FLAGS_MASK   0xF0
#define PHYSICS_CONSTRAINT_NO_COLLISION 0x10

// The size of the largest constraint type.
// In this case, it is the ???? at ? bytes.
#define PHYSICS_CONSTRAINT_MAX_SIZE 1

typedef int_least8_t physConstraintType_t;

// Forward declaration of physRigidBody.
//typedef struct physRigidBody physRigidBody;

typedef struct physConstraint {
	//physRigidBody *bodyA;
	//physRigidBody *bodyB;
	union {
		int temp;
	} data;
	flags_t flags;
} physConstraint;

void physConstraintInit(physConstraint *const restrict constraint);

#endif
