#ifndef PHYSICSJOINTPRISMATIC_H
#define PHYSICSJOINTPRISMATIC_H

#include "settingsPhysics.h"

// Restricts relative translation between
// two bodies to a certain axis.

typedef struct physRigidBody physRigidBody;
typedef struct physJoint physJoint;

typedef struct {

	///

} physJointPrismaticBase;

typedef struct {

	///

} physJointPrismatic;

void physJointPrismaticInit(physJointPrismatic *const __RESTRICT__ joint);
void physJointPrismaticPresolveConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float dt_s);
void physJointPrismaticSolveVelocityConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
return_t physJointPrismaticSolveConfigurationConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);

#endif
