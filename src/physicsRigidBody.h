#ifndef PHYSICSRIGIDBODY_H
#define PHYSICSRIGIDBODY_H

#include "physicsBodyShared.h"
#include "physicsCollision.h"
#include "physicsConstraint.h"
#include "skeleton.h"
#include "mat3.h"
#include <stddef.h>

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

	// Physical collider.
	collider hull;  // The body's convex collider.

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

} physRigidBodyLocal;

typedef struct physRigidBody {

	// The rigid body this instance is derived from, in local space.
	const physRigidBodyLocal *local;

	// Physical collider.
	collider hull;  // The body's global, transformed collider.
	cAABB aabb;     // The body's global, transformed bounding box.

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

	// The body's ID in the physics solver.
	physicsBodyIndex_t id;

	// Various flags for the rigid body.
	flags_t flags;

} physRigidBody;

// Physics rigid body functions.
void physRigidBodyLocalInit(physRigidBodyLocal *const restrict local);
void physRigidBodyLocalGenerateMassProperties(physRigidBodyLocal *const restrict local, const float **const vertexMassArray);
return_t physRigidBodyLocalLoad(physRigidBodyLocal **const restrict bodies, const skeleton *const restrict skl, const char *const restrict prgPath, const char *const restrict filePath);
void physRigidBodyLocalDelete(physRigidBodyLocal *const restrict local);

// Physics rigid body instance functions.
void physRigidBodyInit(physRigidBody *const restrict body);
return_t physRigidBodyInstantiate(physRigidBody *const restrict body, physRigidBodyLocal *const restrict local, bone *const restrict configuration);

return_t physRigidBodyAddConstraint(physRigidBody *const restrict body, physConstraint *const c);

physSeparation *physRigidBodyFindSeparation(physRigidBody *const restrict body, const physicsBodyIndex_t id, physSeparation **const previous);
physSeparation *physRigidBodyCacheSeparation(physRigidBody *const restrict body, physSeparation *const restrict previous);
void physRigidBodyRemoveSeparation(physRigidBody *const restrict body, physSeparation *const restrict separation, const physSeparation *const restrict previous);

void physRigidBodyUpdateCollisionMesh(physRigidBody *const restrict body);

void physRigidBodyApplyLinearForce(physRigidBody *const restrict body, const vec3 *const restrict F);
void physRigidBodyApplyAngularForceGlobal(physRigidBody *const restrict body, const vec3 *const restrict F, const vec3 *const restrict r);
void physRigidBodyApplyForceGlobal(physRigidBody *const restrict body, const vec3 *const restrict F, const vec3 *const restrict r);
/*void physRigidBodyApplyLinearImpulse(physRigidBody *const restrict body, const vec3 *const restrict j);
void physRigidBodyApplyAngularImpulse(physRigidBody *const restrict body, const vec3 *const restrict T);
void physRigidBodyApplyImpulseAtGlobalPoint(physRigidBody *const restrict body, const vec3 *const restrict F, const vec3 *const restrict r);*/

void physRigidBodyIntegrateVelocity(physRigidBody *const restrict body, const float dt);
void physRigidBodyIntegrateConfiguration(physRigidBody *const restrict body, const float dt);

void physRigidBodyResolveCollisionGS(physRigidBody *const restrict body1, physRigidBody *const restrict body2, const cContact *const restrict cm);

void physRigidBodyDelete(physRigidBody *const restrict body);

#endif
