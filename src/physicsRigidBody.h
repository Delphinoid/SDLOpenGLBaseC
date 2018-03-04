#ifndef PHYSICSRIGIDBODY_H
#define PHYSICSRIGIDBODY_H

#include "hitboxAABB.h"
#include "hitboxConvexMesh.h"
#include "quat.h"
#include "mat3.h"
#include <stdlib.h>

#define PHYSICS_BODY_SIMULATE 0x01  // Whether or not the body is active. Still listens for collisions.
#define PHYSICS_BODY_COLLIDE  0x02  // Whether or not the body will listen for collisions.
#define PHYSICS_BODY_DELETE   0x04  // Set by an object's deletion function so the body can be freed by the physics handler.

typedef struct {
	hbAABB aabb;        // The global hull's bounding box.
	hbMesh localHull;   // The collision mesh in local space.
	/** Don't malloc for indices, re-use indices from localHull. **/
	hbMesh globalHull;  // The collision mesh transformed by the rigid body's position and orientation.
	vec3 localCentroid;
	vec3 globalCentroid;
} physCollider;

typedef struct physRigidBody physRigidBody;

typedef struct {
	signed char collide;          // Whether or not the bodies being constrained will collide.
	physRigidBody *body;          // Pointer to the body being constrained.
	//vec3 *constraintPosition;     // Pointer to the position that the body is constrained to.
	//quat *constraintOrientation;  // Pointer to the orientation that the body is constrained to.
	vec3 constraintOffsetMin;
	vec3 constraintOffsetMax;
	vec3 constraintRotationMin;
	vec3 constraintRotationMax;
} physConstraint;

typedef struct physRigidBody {

	// Various flags for the rigid body.
	signed char flags;

	/* Physical colliders. */
	/** Update functions for multiple colliders. **/
	hbAABB aabb;              // The body's global bounding box.
	size_t colliderNum;       // The body's number of convex colliders.
	physCollider *colliders;  // The body's convex colliders.
	//hbMesh hull;

	/* Physical mass properties. */
	float mass;                       // The body's mass.
	float inverseMass;                // The reciprocal of the body's mass.
	float coefficientOfRestitution;   // The ratio of energy kept after a collision.
	vec3 localCentroid;               // The body's center of mass.
	vec3 globalCentroid;              // The body's center of mass.
	mat3 localInertiaTensor;          // The body's local inertia tensor.
	mat3 globalInverseInertiaTensor;  // The inverse of the body's global inertia tensor, used for angular momentum.

	/* Physical space properties. */
	vec3 position;          // Current position.
	quat orientation;       // Current orientation.
	vec3 linearVelocity;    // Current linear velocity.
	vec3 angularVelocity;   // Current angular velocity.
	vec3 netForce;          // Force accumulator.
	vec3 netTorque;         // Torque accumulator.

	/* Physical constraints. */
	size_t constraintNum;
	physConstraint *constraints;  // An array of constraints for the kinematics chain.

} physRigidBody;

/** Finish functions for physIsland and remove physKinematicsChain. **/

typedef struct {
	/*
	** A network of rigid bodies. The first acts as the root, which
	** uses an offset to the main position, rotation, etc.
	*/
	signed char simulate;        // Whether or not the kinematics chain is to be simulated.
	size_t bodyNum;              // If this is part of an object, the number of bodies should equal the
	                             // number of bones in its skeleton, assuming the object has physics.
	physRigidBody *bodies;       // Depth-first vector of each rigid body in the chain.
	size_t constraintNum;
	physConstraint constraints;  // An array of constraints for the kinematics chain.
} physKinematicsChain;

void physColliderGenerateMassProperties(physCollider *collider, const float mass, mat3 *inertiaTensor);
void physRigidBodyGenerateMassProperties(physRigidBody *body, const float *mass, const mat3 *inertiaTensor);

void physRigidBodyUpdateCollisionMesh(physRigidBody *body);

void physRigidBodyApplyForceAtGlobalPoint(physRigidBody *body, const vec3 *F, const vec3 *r);
void physRigidBodyAddLinearVelocity(physRigidBody *body, const vec3 *impulse);
void physRigidBodyApplyLinearImpulse(physRigidBody *body, const vec3 *impulse);
void physRigidBodyAddAngularVelocity(physRigidBody *body, const float angle, const float x, const float y, const float z);

void physRigidBodyIntegrateEuler(physRigidBody *body, const float dt);
void physRigidBodyIntegrateLeapfrog(physRigidBody *body, const float dt);

void physColliderDelete(physCollider *collider);
void physRigidBodyDelete(physRigidBody *body);

#endif
