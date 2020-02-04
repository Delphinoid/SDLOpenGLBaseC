#ifndef PHYSICSJOINTFIXED_H
#define PHYSICSJOINTFIXED_H

#include "physicsSettings.h"
#include "return.h"
#include "qualifiers.h"

// Keeps two bodies rigidly connected, preventing
// relative translational or rotational changes
// from occurring between them.

typedef struct physRigidBody physRigidBody;
typedef struct physJoint physJoint;
typedef struct {

	//

} physJointFixed;

void physJointFixedInit(physJointFixed *const __RESTRICT__ joint);
void physJointFixedPresolveConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float dt);
void physJointFixedSolveVelocityConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
return_t physJointFixedSolveConfigurationConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
#endif

#endif