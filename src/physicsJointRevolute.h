#ifndef PHYSICSJOINTREVOLUTE_H
#define PHYSICSJOINTREVOLUTE_H

#include "physicsSettings.h"

// Keeps the bodies at a fixed distance
// and allows rotation along only one axis.
// Acts as a hinge joint.

typedef struct physRigidBody physRigidBody;
typedef struct physJoint physJoint;
typedef struct {

	//

} physJointRevolute;

void physJointRevoluteInit(physJointRevolute *const restrict joint);
void physJointRevolutePresolveConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, const float dt);
void physJointRevoluteSolveVelocityConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB);
#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
return_t physJointRevoluteSolveConfigurationConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB);
#endif

#endif