#ifndef PHYSICSJOINT_H
#define PHYSICSJOINT_H

#include "physicsJointFixed.h"
#include "physicsJointDistance.h"
#include "physicsJointPrismatic.h"
#include "physicsJointRevolute.h"
#include "physicsJointSphere.h"
#include "flags.h"

#define PHYSICS_JOINT_TYPE_UNKNOWN  -1
#define PHYSICS_JOINT_TYPE_FIXED     0
#define PHYSICS_JOINT_TYPE_DISTANCE  1
#define PHYSICS_JOINT_TYPE_PRISMATIC 2
#define PHYSICS_JOINT_TYPE_REVOLUTE  3
#define PHYSICS_JOINT_TYPE_SPHERE    4
#define PHYSICS_JOINT_TYPE_NUM       5

#define PHYSICS_JOINT_COLLIDE 0x01

typedef int_least8_t physJointType_t;

typedef struct physRigidBody physRigidBody;
typedef struct physJoint {

	// Parent and child rigid body pointers.
	physRigidBody *parent;
	physRigidBody *child;

	// Calculate the size of the largest joint type.
	union {
		physJointFixed fixed;
		physJointDistance distance;
		physJointPrismatic prismatic;
		physJointRevolute revolute;
		physJointSphere sphere;
	} data;
	physJointType_t type;

	// Fits in for free next to physJointType_t.
	flags_t flags;

} physJoint;

extern void (* const physJointUpdateJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	const physJoint *const restrict joint
);

void physJointInit(physJoint *const restrict joint, const physJointType_t type);
void physJointDelete(physJoint *const restrict joint);

#endif
