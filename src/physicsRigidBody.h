#ifndef PHYSICSRIGIDBODY_H
#define PHYSICSRIGIDBODY_H

#include "physicsShared.h"
#include "bone.h"
#include "mat3.h"
#include "flags.h"
#include <stddef.h>

// physRigidBody defines a physical rigid body.
//
// Note: the definition of the rigid body may change
// depending on the preprocessor directives.
//
// If PHYSICS_BODY_SCALE_INERTIA_TENSORS is defined, rigid
// bodies and rigid body bases will store regular inertia
// tensors rather than inverses to make scaling more
// efficient. If your application will not be making much
// use of scaling or the effects of scaling on the
// simulation are negligible, please consider undefining
// this and manually invoking the physRigidBodyScale()
// function instead.

#define PHYSICS_BODY_UNINITIALIZED      0x01  // Whether or not the simulation has just begun on this frame.
#define PHYSICS_BODY_SIMULATE_LINEAR    0x02  // Simulate linear velocity.
#define PHYSICS_BODY_SIMULATE_ANGULAR   0x04  // Simulate angular velocity. Disabling this is useful for certain entities, such as players.
#define PHYSICS_BODY_SIMULATE           0x06  // Simulate both linear and angular velocity.
#define PHYSICS_BODY_COLLIDE            0x08  // Permit collisions.
#define PHYSICS_BODY_AWAKE              0x0E  // Whether or not the body is asleep.
#define PHYSICS_BODY_ASLEEP             0xF1  // Whether or not the body is asleep.
#define PHYSICS_BODY_COLLISION_MODIFIED 0x10  // The collision flag was modified.
#define PHYSICS_BODY_TRANSLATED         0x20  // The body was translated this frame.
#define PHYSICS_BODY_ROTATED            0x40  // The body was rotated this frame.
#define PHYSICS_BODY_TRANSFORMED        0x60  // The body was transformed this frame.
#define PHYSICS_BODY_INITIALIZED        0x80  // The body was initialized on this frame. Currently unused.

#ifndef PHYSICS_BODY_DEFAULT_STATE
	#define PHYSICS_BODY_DEFAULT_STATE PHYSICS_BODY_UNINITIALIZED | PHYSICS_BODY_SIMULATE | PHYSICS_BODY_COLLIDE | PHYSICS_BODY_COLLISION_MODIFIED
#endif

typedef struct physCollider physCollider;
typedef struct physJoint physJoint;
typedef struct physIsland physIsland;
typedef struct skeleton skeleton;

typedef struct physRigidBodyBase {

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

	/**char *name;**/

} physRigidBodyBase;

typedef struct physRigidBody {

	// Physical collider.
	physCollider *hull;  // The body's convex collider SLink.

	// Physical properties.
	float mass;                       // The body's mass.
	float inverseMass;                // The reciprocal of the body's mass.
	float linearDamping;              // The body's linear damping ratio.
	float angularDamping;             // The body's angular damping ratio.
	vec3 centroidGlobal;              // The body's global center of mass.
	#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
	vec3 centroidLocal;               // The body's local center of mass.
	#ifdef PHYSICS_BODY_SCALE_INERTIA_TENSORS
	mat3 inertiaTensorLocal;          // The body's local inertia tensor.
	#else
	mat3 inverseInertiaTensorLocal;   // The inverse of the body's local inertia tensor.
	#endif
	#endif
	mat3 inverseInertiaTensorGlobal;  // The inverse of the body's global inertia tensor.

	// Space properties.
	bone configuration;    // The current global configuration of the body.
	vec3 linearVelocity;   // Current linear velocity.
	vec3 angularVelocity;  // Current angular velocity.
	vec3 netForce;         // Force accumulator.
	vec3 netTorque;        // Torque accumulator.

	// Physical constraints.
	// joints is a QLink of all joints that the body is a part of,
	// ordered from smallest partner address to largest.
	// At the beginning of the QLink is all of the joints where this
	// body is the one with the larger address, similar to collider
	// contacts and separations.
	physJoint *joints;

	// The rigid body this instance is derived from, in local space.
	const physRigidBodyBase *base;

	// Various flags for the rigid body.
	flags_t flags;

} physRigidBody;

// Physics rigid body functions.
void physRigidBodyBaseInit(physRigidBodyBase *const __RESTRICT__ local);
void physRigidBodyBaseGenerateMassProperties(physRigidBodyBase *const __RESTRICT__ local, const float **const vertexMassArray);
return_t physRigidBodyBaseLoad(physRigidBodyBase **const __RESTRICT__ bodies, const skeleton *const __RESTRICT__ skl, const char *const __RESTRICT__ filePath, const size_t filePathLength);
void physRigidBodyBaseDelete(physRigidBodyBase *const __RESTRICT__ local);

// Physics rigid body instance functions.
void physRigidBodyInit(physRigidBody *const __RESTRICT__ body);
return_t physRigidBodyInstantiate(physRigidBody *const __RESTRICT__ body, physRigidBodyBase *const __RESTRICT__ local);

void physRigidBodySetUninitialized(physRigidBody *const __RESTRICT__ body);
void physRigidBodySetInitialized(physRigidBody *const __RESTRICT__ body);
void physRigidBodySetInitializedFull(physRigidBody *const __RESTRICT__ body);
void physRigidBodySetAsleep(physRigidBody *const __RESTRICT__ body);
void physRigidBodySetAwake(physRigidBody *const __RESTRICT__ body, const flags_t flags);

void physRigidBodySimulateCollisions(physRigidBody *const __RESTRICT__ body);
void physRigidBodySimulateLinear(physRigidBody *const __RESTRICT__ body);
void physRigidBodySimulateAngular(physRigidBody *const __RESTRICT__ body);

void physRigidBodyIgnoreCollisions(physRigidBody *const __RESTRICT__ body);
void physRigidBodyIgnoreLinear(physRigidBody *const __RESTRICT__ body);
void physRigidBodyIgnoreAngular(physRigidBody *const __RESTRICT__ body);

return_t physRigidBodyIsUninitialized(const physRigidBody *const __RESTRICT__ body);
return_t physRigidBodyIsSimulated(const physRigidBody *const __RESTRICT__ body);
return_t physRigidBodyIsCollidable(const physRigidBody *const __RESTRICT__ body);
return_t physRigidBodyIsAsleep(physRigidBody *const __RESTRICT__ body);
return_t physRigidBodyWasInitialized(const physRigidBody *const __RESTRICT__ body);

return_t physRigidBodyUpdateColliders(physRigidBody *const __RESTRICT__ body, physIsland *const __RESTRICT__ island);

void physRigidBodyApplyLinearForce(physRigidBody *const __RESTRICT__ body, const vec3 F);
void physRigidBodyApplyAngularForceGlobal(physRigidBody *const __RESTRICT__ body, const vec3 F, const vec3 r);
void physRigidBodyApplyForceGlobal(physRigidBody *const __RESTRICT__ body, const vec3 F, const vec3 r);

void physRigidBodyApplyVelocityImpulse(physRigidBody *const __RESTRICT__ body, const vec3 x, const vec3 J);
void physRigidBodyApplyVelocityImpulseInverse(physRigidBody *const __RESTRICT__ body, const vec3 x, const vec3 J);

void physRigidBodyApplyVelocityImpulseAngular(physRigidBody *const __RESTRICT__ body, const vec3 x, const vec3 J, const vec3 a);
void physRigidBodyApplyVelocityImpulseAngularInverse(physRigidBody *const __RESTRICT__ body, const vec3 x, const vec3 J, const vec3 a);

void physRigidBodyApplyConfigurationImpulse(physRigidBody *const __RESTRICT__ body, const vec3 x, const vec3 J);
void physRigidBodyApplyConfigurationImpulseInverse(physRigidBody *const __RESTRICT__ body, const vec3 x, const vec3 J);

#if defined(PHYSICS_BODY_STORE_LOCAL_TENSORS) && defined(PHYSICS_BODY_SCALE_INERTIA_TENSORS)
void physRigidBodyScale(physRigidBody *const __RESTRICT__ body, const vec3 scale);
void physRigidBodySetScale(physRigidBody *const __RESTRICT__ body, const vec3 scale);
#endif

void physRigidBodyCentroidFromPosition(physRigidBody *const __RESTRICT__ body);
void physRigidBodyPositionFromCentroid(physRigidBody *const __RESTRICT__ body);
void physRigidBodyGenerateGlobalInertia(physRigidBody *const __RESTRICT__ body);

void physRigidBodyUpdateConfiguration(physRigidBody *const __RESTRICT__ body);

void physRigidBodyResetAccumulators(physRigidBody *const __RESTRICT__ body);
void physRigidBodyIntegrateVelocity(physRigidBody *const __RESTRICT__ body, const float dt);
void physRigidBodyIntegrateConfiguration(physRigidBody *const __RESTRICT__ body, const float dt);

void physRigidBodyIntegrateSymplecticEuler(physRigidBody *const __RESTRICT__ body, const float dt);
void physRigidBodyIntegrateLeapfrog(physRigidBody *const __RESTRICT__ body, const float dt);
void physRigidBodyIntegrateLeapfrogTest(physRigidBody *const __RESTRICT__ body, const float dt);

return_t physRigidBodyPermitCollision(const physRigidBody *const __RESTRICT__ body1, const physRigidBody *const __RESTRICT__ body2);

#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
void physRigidBodyAddCollider(physRigidBody *const __RESTRICT__ body, physCollider *const c, const float **const vertexMassArray);
#endif
return_t physRigidBodyAddJoint(physRigidBody *const body, physJoint *const joint);

void physRigidBodyDelete(physRigidBody *const __RESTRICT__ body);

#endif
