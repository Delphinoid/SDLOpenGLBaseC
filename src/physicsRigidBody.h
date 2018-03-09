#ifndef PHYSICSRIGIDBODY_H
#define PHYSICSRIGIDBODY_H

#include "hitboxAABB.h"
#include "hitboxConvexMesh.h"
#include "bone.h"
#include "mat3.h"
#include <stdlib.h>

#define PHYSICS_BODY_SIMULATE 0x01  // Whether or not the body is active. Still listens for collisions.
#define PHYSICS_BODY_COLLIDE  0x02  // Whether or not the body will listen for collisions.
#define PHYSICS_BODY_DELETE   0x04  // Set by an object's deletion function so the body can be freed by the physics handler.

typedef struct {
	hbAABB aabb;   // The hull's bounding box.
	hbMesh hull;   // The collision mesh in local space.
	vec3 centroid;
} physCollider;

typedef struct prbInstance prbInstance;
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
	float coefficientOfRestitution;   // The ratio of energy kept after a collision.
	vec3 centroid;                    // The body's center of mass.
	mat3 inertiaTensor;               // The body's local inertia tensor.

} physRigidBody;

typedef struct prbInstance {

	/* Various flags for the rigid body. */
	signed char flags;

	/* The rigid body this instance is derived from, in local space. */
	physRigidBody *local;

	/* Physical colliders. */
	hbAABB aabb;              // The body's global bounding box.
	physCollider *colliders;  // The body's global convex colliders.

	/* Physical mass properties. */
	vec3 centroid;              // The body's global center of mass.
	mat3 inverseInertiaTensor;  // The inverse of the body's global inertia tensor, used for angular momentum.

	/* Physical space properties. */
	bone configuration;      // Current configuration of the body.
	bone configurationLast;  // Previous configuration of the body.
	vec3 linearVelocity;     // Current linear velocity.
	vec3 angularVelocity;    // Current angular velocity.
	vec3 netForce;           // Force accumulator.
	vec3 netTorque;          // Torque accumulator.

	/* Physical constraints. */
	size_t constraintNum;
	physConstraint *constraints;  // An array of constraints for the kinematics chain.

} prbInstance;

/* Physics collider functions. */
void physColliderGenerateMassProperties(physCollider *collider, const float mass, mat3 *inertiaTensor);
void physColliderDelete(physCollider *collider);

/* Physics rigid body functions. */
void physRigidBodyGenerateMassProperties(physRigidBody *body, const float *mass, const mat3 *inertiaTensor);
void physRigidBodyDelete(physRigidBody *body);

/* Physics rigid body instance functions. */
void prbiUpdateCollisionMesh(prbInstance *prbi);

void prbiApplyForceAtGlobalPoint(prbInstance *prbi, const vec3 *F, const vec3 *r);
void prbiAddLinearVelocity(prbInstance *prbi, const vec3 *impulse);
void prbiApplyLinearImpulse(prbInstance *prbi, const vec3 *impulse);
void prbiAddAngularVelocity(prbInstance *prbi, const float angle, const float x, const float y, const float z);

void prbiIntegrateEuler(prbInstance *prbi, const float dt);
void prbiIntegrateLeapfrog(prbInstance *prbi, const float dt);

void prbiDelete(prbInstance *prbi);

#endif
