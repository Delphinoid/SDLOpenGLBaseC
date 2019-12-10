#ifndef PHYSICSJOINTPRISMATIC_H
#define PHYSICSJOINTPRISMATIC_H

#include "physicsSettings.h"

// Restricts relative translation between
// two bodies to a certain axis.

typedef struct physRigidBody physRigidBody;
typedef struct physJoint physJoint;
typedef struct {

	//

} physJointPrismatic;

void physJointPrismaticInit(physJointPrismatic *const restrict joint);
void physJointPrismaticPresolveConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, const float dt);
void physJointPrismaticSolveVelocityConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB);
#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
return_t physJointPrismaticSolveConfigurationConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB);
#endif

#endif