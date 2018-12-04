#ifndef PHYSICSRIGIDBODY_H
#define PHYSICSRIGIDBODY_H

#include "physicsBodyShared.h"
#include "physicsCollision.h"
#include "physicsConstraint.h"
#include "skeleton.h"
#include "mat3.h"
#include <stdlib.h>

/** Remove PHYSICS_BODY_INITIALIZE? **/
#define PHYSICS_BODY_INACTIVE   0x00
#define PHYSICS_BODY_INITIALIZE 0x01  // Whether or not the simulation has just begun on this frame.
#define PHYSICS_BODY_SIMULATE   0x02  // Whether or not the body is active. Still listens for collisions.
#define PHYSICS_BODY_COLLIDE    0x04  // Whether or not the body will listen for collisions.
#define PHYSICS_BODY_DELETE     0x08  // Set by an object's deletion function so the body can be freed by the physics handler.

/**#define PHYSICS_BODY_MAX_CONSTRAINTS 256
#define PHYSICS_BODY_MAX_CACHE_SIZE  256**/

typedef struct {

	/* Physical colliders. */
	/** Update functions for single collider. **/
	physCollider *colliders;  // The body's convex colliders.
	//hbMesh hull;

	/* Physical mass properties. */
	float mass;                       // The body's mass.
	float inverseMass;                // The reciprocal of the body's mass.
	/** The following property is awaiting implementation. **/
	float coefficientOfRestitution;   // The ratio of energy kept after a collision.
	vec3 centroid;                    // The body's center of mass.
	mat3 inertiaTensor;               // The body's local inertia tensor.

	/* Physical constraints. */
	physConstraint *constraints;  // Default constraints.

	/* Default flags. */
	flags_t flags;

	/* The bone the body is associated with. */
	physicsBodyIndex_t id;

	/** char *name; **/

} physRigidBody;

/** Finish physRBIStateCopy(). **/
typedef struct {

	/* The rigid body this instance is derived from, in local space. */
	physRigidBody *local;

	/* Physical colliders. */
	cAABB aabb;               // The body's global, transformed bounding box.
	physCollider *colliders;  // The body's global, transformed convex colliders.
	                          // Their hulls re-use indices allocated for the local colliders.

	/* Physical mass properties. */
	vec3 centroid;              // The body's global center of mass.
	mat3 inertiaTensor;         // The body's global inertia tensor.
	mat3 inverseInertiaTensor;  // The inverse of the body's global inertia tensor.

	/* Physical space properties. */
	bone *configuration;   // Pointer to the current configuration of the body.
	vec3 linearVelocity;   // Current linear velocity.
	vec3 angularVelocity;  // Current angular velocity.
	vec3 netForce;         // Force accumulator.
	vec3 netTorque;        // Torque accumulator.

	/* Physical constraints. */
	physConstraint *constraints;  // An array of constraints for the kinematics
	                              // chain, ordered by constraintID.

	/* Separation caching. */
	physCollisionInfo *cache;  // An array of separations from previous
	                           // frames, ordered by collisionID.

	/* Various flags for the rigid body. */
	flags_t flags;

	/* The body's ID in the physics solver. */
	physicsBodyIndex_t id;

} physRBInstance;

/* Physics rigid body functions. */
void physRigidBodyInit(physRigidBody *body);
void physRigidBodyGenerateMassProperties(physRigidBody *body, float **vertexMassArrays);
return_t physRigidBodyLoad(physRigidBody **bodies, const skeleton *skl, const char *prgPath, const char *filePath);
void physRigidBodyDelete(physRigidBody *body);

/* Physics rigid body instance functions. */
void physRBIInit(physRBInstance *prbi);
return_t physRBIInstantiate(physRBInstance *prbi, physRigidBody *body, bone *configuration);

return_t physRBIAddConstraint(physRBInstance *prbi, physConstraint *c);
return_t physRBICacheSeparation(physRBInstance *prbi, physCollisionInfo *c);

void physRBIUpdateCollisionMesh(physRBInstance *prbi);

void physRBIApplyLinearForce(physRBInstance *prbi, const vec3 *F);
void physRBIApplyAngularForceGlobal(physRBInstance *prbi, const vec3 *F, const vec3 *r);
void physRBIApplyForceGlobal(physRBInstance *prbi, const vec3 *F, const vec3 *r);
/*void physRBIApplyLinearImpulse(physRBInstance *prbi, const vec3 *j);
void physRBIApplyAngularImpulse(physRBInstance *prbi, const vec3 *T);
void physRBIApplyImpulseAtGlobalPoint(physRBInstance *prbi, const vec3 *F, const vec3 *r);*/

void physRBIBeginSimulation(physRBInstance *prbi);

void physRBIIntegrateEuler(physRBInstance *prbi, const float dt);
void physRBIIntegrateLeapfrog(physRBInstance *prbi, const float dt);
void physRBIIntegrateLeapfrogVelocity(physRBInstance *prbi, const float dt);
void physRBIIntegrateLeapfrogConstraints(physRBInstance *prbi, const float dt);

void physRBIResolveCollisionGS(physRBInstance *body1, physRBInstance *body2, const cCollisionContactManifold *cm);

void physRBIDelete(physRBInstance *prbi);

#endif
