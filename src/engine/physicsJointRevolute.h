#ifndef PHYSICSJOINTREVOLUTE_H
#define PHYSICSJOINTREVOLUTE_H

#include "settingsPhysics.h"

// Keeps the bodies at a fixed distance
// and allows rotation along only one axis.
// Acts as a hinge joint.

typedef struct physRigidBody physRigidBody;
typedef struct physJoint physJoint;

typedef struct {

	//

} physJointRevoluteBase;

typedef struct {

	//

} physJointRevolute;

void physJointRevoluteInit(physJointRevolute *const __RESTRICT__ joint);
void physJointRevolutePresolveConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float dt_s);
void physJointRevoluteSolveVelocityConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
return_t physJointRevoluteSolveConfigurationConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);

#endif
