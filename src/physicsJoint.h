#ifndef PHYSICSJOINT_H
#define PHYSICSJOINT_H

#include "physicsConstraint.h"
#include "physicsJointFixed.h"
#include "physicsJointDistance.h"
#include "physicsJointPrismatic.h"
#include "physicsJointRevolute.h"
#include "physicsJointSphere.h"
#include "flags.h"

#ifndef PHYSICS_JOINT_VELOCITY_SOLVER_ITERATIONS
	#define PHYSICS_JOINT_VELOCITY_SOLVER_ITERATIONS 4
#endif

#ifndef PHYSICS_JOINT_CONFIGURATION_SOLVER_ITERATIONS
	#define PHYSICS_JOINT_CONFIGURATION_SOLVER_ITERATIONS 4
#endif

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

	// Rigid body pointers.
	// Body A is guaranteed to have a greater address
	// than body B.
	physRigidBody *bodyA;
	physRigidBody *bodyB;

} physJoint;

extern void (* const physJointSolveVelocityConstraintsJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	const void *const restrict joint
);
#ifdef PHYSICS_GAUSS_SEIDEL_SOLVER
extern void (* const physJointSolveConfigurationConstraintsJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	const void *const restrict joint
);
#endif

void physJointInit(physJoint *const restrict joint, const physJointType_t type);
void physJointCreate(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, const physJointType_t type);
void physJointSolveVelocityConstraints(const physJoint *const restrict joint);
#ifdef PHYSICS_GAUSS_SEIDEL_SOLVER
void physJointSolveConfigurationConstraints(const physJoint *const restrict joint);
#endif
void physJointDelete(physJoint *const restrict joint);

#endif
