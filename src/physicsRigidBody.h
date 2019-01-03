#ifndef PHYSICSRIGIDBODY_H
#define PHYSICSRIGIDBODY_H

#include "physicsBodyShared.h"
#include "physicsCollider.h"
#include "physicsCollision.h"
#include "physicsConstraint.h"
#include "skeleton.h"
#include "mat3.h"
#include <stdlib.h>

/** Remove PHYSICS_BODY_INITIALIZE? **/
#define PHYSICS_BODY_ASLEEP           0x00
#define PHYSICS_BODY_INITIALIZE       0x01  // Whether or not the simulation has just begun on this frame.
#define PHYSICS_BODY_SIMULATE_LINEAR  0x02  // Simulate linear velocity.
#define PHYSICS_BODY_SIMULATE_ANGULAR 0x04  // Simulate angular velocity. Disabling this is useful for certain entities, such as players.
#define PHYSICS_BODY_SIMULATE         0x06  // Whether or not the body is active. Still listens for collisions.
#define PHYSICS_BODY_COLLIDE          0x08  // Whether or not the body will listen for collisions.
#define PHYSICS_BODY_DELETE           0x10  // Set by an object's deletion function so the body can be freed by the physics handler.

/**#define PHYSICS_BODY_MAX_CONSTRAINTS 256
#define PHYSICS_BODY_MAX_CACHE_SIZE  256**/

typedef struct {

	// Physical colliders.
	physCollider *colliders;  // The body's convex colliders.

	// Physical mass properties.
	float mass;                      // The body's mass.
	float inverseMass;               // The reciprocal of the body's mass.
	float coefficientOfRestitution;  // The ratio of energy kept after a collision.
	vec3 centroid;                   // The body's center of mass.
	mat3 inertiaTensor;              // The body's local inertia tensor.

	// Physical constraints.
	physConstraint *constraints;  // Default constraints.

	// The bone the body is associated with.
	physicsBodyIndex_t id;

	// Default flags.
	flags_t flags;

	/** char *name; **/

} physRigidBody;

typedef struct {

	// The rigid body this instance is derived from, in local space.
	const physRigidBody *local;

	// Physical colliders.
	cAABB aabb;               // The body's global, transformed bounding box.
	physCollider *colliders;  // The body's global, transformed convex colliders.
	                          // Their hulls re-use indices allocated for the local colliders.

	// Physical mass properties.
	vec3 centroid;              // The body's global center of mass.
	mat3 inertiaTensor;         // The body's global inertia tensor.
	mat3 inverseInertiaTensor;  // The inverse of the body's global inertia tensor.

	// Physical space properties.
	bone *configuration;   // Pointer to the current configuration of the body.
	vec3 linearVelocity;   // Current linear velocity.
	vec3 angularVelocity;  // Current angular velocity.
	vec3 netForce;         // Force accumulator.
	vec3 netTorque;        // Torque accumulator.

	// Physical constraints.
	physConstraint *constraints;  // An SLink of constraints for the kinematics
	                              // chain, ordered by id in increasing order.

	// Separation caching.
	physSeparation *cache;  // An SLink of separations from previous
	                        // frames, ordered by id in increasing order.
	                        // Freed if the rigid body doesn't pass the
	                        // collision broadphase.

	// Various flags for the rigid body.
	flags_t flags;

	// The body's ID in the physics solver.
	physicsBodyIndex_t id;

} physRBInstance;

// Physics rigid body functions.
void physRigidBodyInit(physRigidBody *const restrict body);
void physRigidBodyGenerateMassProperties(physRigidBody *const restrict body, float **const vertexMassArrays);
return_t physRigidBodyLoad(physRigidBody **const restrict bodies, const skeleton *const restrict skl, const char *const restrict prgPath, const char *const restrict filePath);
void physRigidBodyDelete(physRigidBody *const restrict body);

// Physics rigid body instance functions.
void physRBIInit(physRBInstance *const restrict prbi);
return_t physRBIInstantiate(physRBInstance *const restrict prbi, physRigidBody *const restrict body, bone *const restrict configuration);

return_t physRBIAddConstraint(physRBInstance *const restrict prbi, physConstraint *const c);

physSeparation *physRBIFindSeparation(physRBInstance *const restrict prbi, const physicsBodyIndex_t id, physSeparation **const previous);
physSeparation *physRBICacheSeparation(physRBInstance *const restrict prbi, physSeparation *const restrict previous);
void physRBIRemoveSeparation(physRBInstance *const restrict prbi, physSeparation *const restrict separation, const physSeparation *const restrict previous);

void physRBIUpdateCollisionMesh(physRBInstance *const restrict prbi);

void physRBIApplyLinearForce(physRBInstance *const restrict prbi, const vec3 *const restrict F);
void physRBIApplyAngularForceGlobal(physRBInstance *const restrict prbi, const vec3 *const restrict F, const vec3 *const restrict r);
void physRBIApplyForceGlobal(physRBInstance *const restrict prbi, const vec3 *const restrict F, const vec3 *const restrict r);
/*void physRBIApplyLinearImpulse(physRBInstance *const restrict prbi, const vec3 *const restrict j);
void physRBIApplyAngularImpulse(physRBInstance *const restrict prbi, const vec3 *const restrict T);
void physRBIApplyImpulseAtGlobalPoint(physRBInstance *const restrict prbi, const vec3 *const restrict F, const vec3 *const restrict r);*/

void physRBIIntegrateVelocity(physRBInstance *const restrict prbi, const float dt);
void physRBIIntegrateConfiguration(physRBInstance *const restrict prbi, const float dt);

void physRBIResolveCollisionGS(physRBInstance *const restrict body1, physRBInstance *const restrict body2, const cContactManifold *const restrict cm);

void physRBIDelete(physRBInstance *const restrict prbi);

#endif
