#ifndef PHYSICSRIGIDBODY_H
#define PHYSICSRIGIDBODY_H

#include "physicsBodyShared.h"
#include "physicsCollider.h"
#include "hitboxCollision.h"
#include "skeleton.h"
#include "mat3.h"
#include "typedefs.h"
#include <stdlib.h>

/** Remove PHYS_BODY_INITIALIZE? **/
#define PHYS_BODY_INACTIVE   0x00
#define PHYS_BODY_INITIALIZE 0x01  // Whether or not the simulation has just begun on this frame.
#define PHYS_BODY_SIMULATE   0x02  // Whether or not the body is active. Still listens for collisions.
#define PHYS_BODY_COLLIDE    0x04  // Whether or not the body will listen for collisions.
#define PHYS_BODY_DELETE     0x08  // Set by an object's deletion function so the body can be freed by the physics handler.

#define PHYS_CONSTRAINT_TYPE_1  0x01
#define PHYS_CONSTRAINT_TYPE_2  0x02
#define PHYS_CONSTRAINT_COLLIDE 0x04

#define PHYS_BODY_MAX_CONSTRAINTS 256
#define PHYS_BODY_MAX_CACHE_SIZE 256

typedef uint8_t colliderIndex_t;
typedef uint8_t constraintIndex_t;
typedef uint8_t cacheIndex_t;

typedef struct {

	/* Physical colliders. */
	/** Update functions for single collider. **/
	colliderIndex_t colliderNum;  // The body's number of convex colliders.
	physCollider *colliders;      // The body's convex colliders.
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
	flags_t flags;
	physicsBodyIndex_t constraintID;  // An identifier for the other body being constrained.
	vec3 constraintOffsetMin;
	vec3 constraintOffsetMax;
	vec3 constraintRotationMin;
	vec3 constraintRotationMax;
} physConstraint;

typedef struct {
	physicsBodyIndex_t collisionID;  // An identifier for the other body involved in the collision.
	hbCollisionInfo info;
} physCollisionInfo;

/** Finish physRBIStateCopy(). **/
typedef struct {

	/* Various flags for the rigid body. */
	flags_t flags;

	/* The body's ID in the physics solver. */
	physicsBodyIndex_t id;

	/* The rigid body this instance is derived from, in local space. */
	physRigidBody *local;

	/* Physical colliders. */
	hbAABB aabb;              // The body's global, transformed bounding box.
	physCollider *colliders;  // The body's global, transformed convex colliders.
	                          // Their hulls re-use indices allocated for the local colliders.

	/* Physical mass properties. */
	vec3 centroid;              // The body's global center of mass.
	mat3 inertiaTensor;         // The body's global inertia tensor.
	mat3 inverseInertiaTensor;  // The inverse of the body's global inertia tensor.

	/* Physical space properties. */
	bone *configuration;   // Pointers to current and last configurations of the body.
	vec3 linearVelocity;   // Current linear velocity.
	vec3 angularVelocity;  // Current angular velocity.
	vec3 netForce;         // Force accumulator.
	vec3 netTorque;        // Torque accumulator.

	/* Physical constraints. */
	constraintIndex_t constraintNum;
	constraintIndex_t constraintCapacity;
	physConstraint *constraints;  // An array of constraints for the kinematics
	                              // chain, ordered by constraintID.

	/* Separation caching. */
	cacheIndex_t separationNum;
	cacheIndex_t separationCapacity;
	physCollisionInfo *cache;  // An array of separations from previous
	                           // frames, ordered by collisionID.

} physRBInstance;

/* Physics constraint functions. */
void physConstraintInit(physConstraint *constraint);

/* Physics rigid body functions. */
void physRigidBodyInit(physRigidBody *body);
void physRigidBodyGenerateMassProperties(physRigidBody *body, float **vertexMassArrays);
signed char physRigidBodyLoad(physRigidBody *bodies, flags_t *flags, constraintIndex_t *constraintNum, physConstraint **constraints,
                              const skeleton *skl, const char *prgPath, const char *filePath);
void physRigidBodyDelete(physRigidBody *body);

/* Physics rigid body instance functions. */
void physRBIInit(physRBInstance *prbi);
signed char physRBIInstantiate(physRBInstance *prbi, physRigidBody *body, bone *configuration);
signed char physRBIStateCopy(physRBInstance *o, physRBInstance *c);

signed char physRBIAddConstraint(physRBInstance *prbi, physConstraint *c);
signed char physRBICacheSeparation(physRBInstance *prbi, physCollisionInfo *c);

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

void physRBIResolveCollisionGS(physRBInstance *body1, physRBInstance *body2, const hbCollisionContactManifold *cm);

void physRBIDelete(physRBInstance *prbi);

#endif
