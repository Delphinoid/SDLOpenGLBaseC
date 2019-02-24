#ifndef PHYSICSRIGIDBODY_H
#define PHYSICSRIGIDBODY_H

#include "physicsShared.h"
#include "skeleton.h"
#include "mat3.h"

#define PHYSICS_BODY_ASLEEP             0x00
#define PHYSICS_BODY_UNINITIALIZED      0x01  // Whether or not the simulation has just begun on this frame.
#define PHYSICS_BODY_SIMULATE_LINEAR    0x02  // Simulate linear velocity.
#define PHYSICS_BODY_SIMULATE_ANGULAR   0x04  // Simulate angular velocity. Disabling this is useful for certain entities, such as players.
#define PHYSICS_BODY_SIMULATE           0x06  // Simulate both linear and angular velocity.
#define PHYSICS_BODY_COLLIDE            0x08  // Permit collisions.
#define PHYSICS_BODY_COLLISION_MODIFIED 0x10  // The collision flag was modified.
#define PHYSICS_BODY_TRANSLATED         0x20  // The body was translated this frame.
#define PHYSICS_BODY_ROTATED            0x40  // The body was rotated this frame.
#define PHYSICS_BODY_TRANSFORMED        0x60  // The body was transformed this frame.

#ifndef PHYSICS_BODY_DEFAULT_STATE
	#define PHYSICS_BODY_DEFAULT_STATE PHYSICS_BODY_UNINITIALIZED | PHYSICS_BODY_SIMULATE | PHYSICS_BODY_COLLIDE | PHYSICS_BODY_COLLISION_MODIFIED
#endif

typedef struct physCollider physCollider;
typedef struct physJoint physJoint;
typedef struct physIsland physIsland;

typedef struct {

	// Physical collider.
	physCollider *hull;  // The body's convex collider SLink.

	// Physical mass properties.
	float mass;                 // The body's mass.
	float inverseMass;          // The reciprocal of the body's mass.
	float linearDamping;        // The body's linear damping ratio.
	float angularDamping;       // The body's angular damping ratio.
	vec3 centroid;              // The body's center of mass.
	mat3 inverseInertiaTensor;  // The inverse of the body's local inertia tensor.

	// The bone the body is associated with.
	physicsBodyIndex_t id;

	// Default flags.
	flags_t flags;

	/** char *name; **/

} physRigidBodyBase;

typedef struct physRigidBody {

	// Physical collider.
	physCollider *hull;  // The body's convex collider SLink.

	// Physical properties.
	float mass;                       // The body's mass.
	float inverseMass;                // The reciprocal of the body's mass.
	float linearDamping;              // The body's linear damping ratio.
	float angularDamping;             // The body's angular damping ratio.
	vec3 centroidLocal;               // The body's local center of mass.
	vec3 centroidGlobal;              // The body's global center of mass.
	mat3 inverseInertiaTensorLocal;   // The inverse of the body's local inertia tensor.
	mat3 inverseInertiaTensorGlobal;  // The inverse of the body's global inertia tensor.

	// Space properties.
	bone *configuration;   // Pointer to the current configuration of the body.
	vec3 linearVelocity;   // Current linear velocity.
	vec3 angularVelocity;  // Current angular velocity.
	vec3 netForce;         // Force accumulator.
	vec3 netTorque;        // Torque accumulator.

	// Physical constraints.
	physJoint *joints;  // A QLink of joints that the body owns.
	                    // Ordered by smallest child address to largest.

	// The rigid body this instance is derived from, in local space.
	const physRigidBodyBase *base;

	// Various flags for the rigid body.
	flags_t flags;

} physRigidBody;

// Physics rigid body functions.
void physRigidBodyBaseInit(physRigidBodyBase *const restrict local);
void physRigidBodyBaseGenerateMassProperties(physRigidBodyBase *const restrict local, const float **const vertexMassArray);
return_t physRigidBodyBaseLoad(physRigidBodyBase **const restrict bodies, const skeleton *const restrict skl, const char *const restrict prgPath, const char *const restrict filePath);
void physRigidBodyBaseDelete(physRigidBodyBase *const restrict local);

// Physics rigid body instance functions.
void physRigidBodyInit(physRigidBody *const restrict body);
return_t physRigidBodyInstantiate(physRigidBody *const restrict body, physRigidBodyBase *const restrict local, bone *const restrict configuration);

void physRigidBodySetInitialized(physRigidBody *const restrict body);
void physRigidBodySetAsleep(physRigidBody *const restrict body);
void physRigidBodySetAwake(physRigidBody *const restrict body, const flags_t flags);

void physRigidBodySimulateCollisions(physRigidBody *const restrict body);
void physRigidBodySimulateLinear(physRigidBody *const restrict body);
void physRigidBodySimulateAngular(physRigidBody *const restrict body);

void physRigidBodyIgnoreCollisions(physRigidBody *const restrict body);
void physRigidBodyIgnoreLinear(physRigidBody *const restrict body);
void physRigidBodyIgnoreAngular(physRigidBody *const restrict body);

return_t physRigidBodyIsUninitialized(const physRigidBody *const restrict body);
return_t physRigidBodyIsSimulated(const physRigidBody *const restrict body);
return_t physRigidBodyIsCollidable(const physRigidBody *const restrict body);
return_t physRigidBodyIsAsleep(physRigidBody *const restrict body);

return_t physRigidBodyUpdateColliders(physRigidBody *const restrict body, physIsland *const restrict island);

void physRigidBodyApplyLinearForce(physRigidBody *const restrict body, const vec3 *const restrict F);
void physRigidBodyApplyAngularForceGlobal(physRigidBody *const restrict body, const vec3 *const restrict F, const vec3 *const restrict r);
void physRigidBodyApplyForceGlobal(physRigidBody *const restrict body, const vec3 *const restrict F, const vec3 *const restrict r);
void physRigidBodyApplyImpulse(physRigidBody *const restrict body, const vec3 *const restrict x, const vec3 *const restrict J);
void physRigidBodyApplyImpulseInverse(physRigidBody *const restrict body, const vec3 *const restrict x, const vec3 *const restrict J);

void physRigidBodyIntegrateVelocity(physRigidBody *const restrict body, const float dt);
void physRigidBodyIntegrateConfiguration(physRigidBody *const restrict body, const float dt);

return_t physRigidBodyPermitCollision(const physRigidBody *const restrict body1, const physRigidBody *const restrict body2);

return_t physRigidBodyAddJoint(physRigidBody *const restrict body, physJoint *const c);

void physRigidBodyDelete(physRigidBody *const restrict body);

#endif
