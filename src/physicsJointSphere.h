#ifndef PHYSICSJOINTSPHERE_H
#define PHYSICSJOINTSPHERE_H

/*
** Allows rotational changes, potentially
** limited by a cone, between two bodies,
** but no relative translations. Acts as
** a ball-and-socket joint.
*/

typedef struct physJoint physJoint;
typedef struct {

	// Application points in both bodies' local spaces.
	vec3 anchorA;
	vec3 anchorB;

	// The cone angle in radians.
	float angle;

} physJointSphere;

void physJointSpherePresolveConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, const float dt);
void physJointSphereSolveVelocityConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB);
#ifdef PHYSICS_SOLVER_GAUSS_SEIDEL
return_t physJointSphereSolveConfigurationConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB);
#endif

#endif