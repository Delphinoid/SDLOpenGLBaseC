#ifndef PHYSICSJOINTFIXED_H
#define PHYSICSJOINTFIXED_H

#include "physicsConstraint.h"

/*
** Keeps two bodies rigidly connected, preventing
** relative translational or rotational changes
** from occurring between them.
*/

typedef struct physRigidBody physRigidBody;
typedef struct physJoint physJoint;
typedef struct {

	//

} physJointFixed;

void physJointFixedInit(physJointFixed *const restrict joint);
void physJointFixedPresolveConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, const float dt);
void physJointFixedSolveVelocityConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB);
#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
return_t physJointFixedSolveConfigurationConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB);
#endif

#endif