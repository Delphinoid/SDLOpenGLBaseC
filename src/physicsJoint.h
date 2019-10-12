#ifndef PHYSICSJOINT_H
#define PHYSICSJOINT_H

#include "physicsJointFixed.h"
#include "physicsJointDistance.h"
#include "physicsJointPrismatic.h"
#include "physicsJointRevolute.h"
#include "physicsJointSphere.h"

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

#define PHYSICS_JOINT_COLLISION_IGNORE  0x00
#define PHYSICS_JOINT_COLLISION         0x01

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
	// Body A is guaranteed to have a
	// greater address than body B.
	physRigidBody *bodyA;
	physRigidBody *bodyB;

} physJoint;

extern void (* const physJointPresolveConstraintsJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	physJoint *const restrict joint,
	physRigidBody *const restrict bodyA,
	physRigidBody *const restrict bodyB,
	const float dt
);
extern void (* const physJointSolveVelocityConstraintsJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	physJoint *const restrict joint,
	physRigidBody *const restrict bodyA,
	physRigidBody *const restrict bodyB
);
#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
extern return_t (* const physJointSolveConfigurationConstraintsJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	physJoint *const restrict joint,
	physRigidBody *const restrict bodyA,
	physRigidBody *const restrict bodyB
);
#endif

void physJointInit(physJoint *const restrict joint, const flags_t flags, const physJointType_t type);
void physJointPresolveConstraints(physJoint *const restrict joint, const float dt);
void physJointSolveVelocityConstraints(physJoint *const restrict joint);
#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
return_t physJointSolveConfigurationConstraints(physJoint *const restrict joint);
#endif
void physJointAdd(physJoint *const joint, physRigidBody *bodyA, physRigidBody *bodyB);
void physJointDelete(physJoint *const restrict joint);

#endif
