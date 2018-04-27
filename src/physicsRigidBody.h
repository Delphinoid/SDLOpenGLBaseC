#ifndef PHYSICSRIGIDBODY_H
#define PHYSICSRIGIDBODY_H

#include "physCollider.h"
#include "bone.h"
#include <stdlib.h>

#define PHYSICS_BODY_INITIALIZE 0x01  // Whether or not the simulation has just begun on this frame.
#define PHYSICS_BODY_SIMULATE   0x02  // Whether or not the body is active. Still listens for collisions.
#define PHYSICS_BODY_COLLIDE    0x04  // Whether or not the body will listen for collisions.
#define PHYSICS_BODY_DELETE     0x08  // Set by an object's deletion function so the body can be freed by the physics handler.

typedef struct {
	signed char collide;          // Whether or not the bodies being constrained will collide.
	vec3 *constraintPosition;     // Pointer to the position that the body is constrained to.
	quat *constraintOrientation;  // Pointer to the orientation that the body is constrained to.
	vec3 constraintOffsetMin;
	vec3 constraintOffsetMax;
	vec3 constraintRotationMin;
	vec3 constraintRotationMax;
} physConstraint;

typedef struct {

	/* Physical colliders. */
	/** Update functions for single collider. **/
	size_t colliderNum;       // The body's number of convex colliders.
	physCollider *colliders;  // The body's convex colliders.
	//hbMesh hull;

	/* Physical mass properties. */
	float mass;                       // The body's mass.
	float inverseMass;                // The reciprocal of the body's mass.
	/** The following property is awaiting implementation. **/
	float coefficientOfRestitution;   // The ratio of energy kept after a collision.
	vec3 centroid;                    // The body's center of mass.
	mat3 inertiaTensor;               // The body's local inertia tensor.

} physRigidBody;

typedef struct {

	/* Various flags for the rigid body. */
	signed char flags;

	/* The rigid body this instance is derived from, in local space. */
	physRigidBody *local;

	/* Physical colliders. */
	hbAABB aabb;              // The body's global, transformed bounding box.
	physCollider *colliders;  // The body's global, transformed convex colliders.
	                          // Their hulls re-use indices allocated for the local colliders.

	/* Physical mass properties. */
	vec3 centroid;              // The body's global center of mass.
	mat3 inverseInertiaTensor;  // The inverse of the body's global inertia tensor, used for angular momentum.

	/* Physical space properties. */
	bone configuration[2];   // Current and last configurations of the body.
	vec3 linearVelocity;     // Current linear velocity.
	vec3 angularVelocity;    // Current angular velocity.
	vec3 netForce;           // Force accumulator.
	vec3 netTorque;          // Torque accumulator.

	/* Physical constraints. */
	size_t constraintNum;
	physConstraint *constraints;  // An array of constraints for the kinematics chain.

} physRBInstance;

/* Physics rigid body functions. */
void physRigidBodyGenerateMassProperties(physRigidBody *body, float **vertexMassArrays);
void physRigidBodyDelete(physRigidBody *body);

/* Physics rigid body instance functions. */
void physRBIInit(physRBInstance *prbi);
signed char physRBICreate(physRBInstance *prbi, physRigidBody *body);
signed char physRBIStateCopy(physRBInstance *o, physRBInstance *c);
signed char physRBIAddConstraint(physRBInstance *prbi, const physConstraint *c);

void physRBIUpdateCollisionMesh(physRBInstance *prbi);

void physRBIApplyForceAtGlobalPoint(physRBInstance *prbi, const vec3 *F, const vec3 *r);
void physRBIAddLinearVelocity(physRBInstance *prbi, const vec3 *impulse);
void physRBIApplyLinearImpulse(physRBInstance *prbi, const vec3 *impulse);
void physRBIAddAngularVelocity(physRBInstance *prbi, const float angle, const float x, const float y, const float z);

void physRBIIntegrateEuler(physRBInstance *prbi, const float dt);
void physRBIIntegrateLeapfrog(physRBInstance *prbi, const float dt);

void physRBIDelete(physRBInstance *prbi);

#endif
