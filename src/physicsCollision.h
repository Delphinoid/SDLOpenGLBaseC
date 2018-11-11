#ifndef PHYSICSCOLLISION_H
#define PHYSICSCOLLISION_H

#include "physicsBodyShared.h"
#include "hitboxCollision.h"
#include "bone.h"

typedef uint_least8_t physColliderIndex_t;
typedef uint_least8_t physCollisionIndex_t;

/** Allow more than just convex meshes? **/
typedef struct {
	hbAABB aabb;  // The hull's bounding box.
	hitbox hb;    // The collision mesh in local space.
	vec3 centroid;
} physCollider;

typedef struct {
	physicsBodyIndex_t collisionID;  // An identifier for the other body involved in the collision.
	hbCollisionInfo info;
} physCollisionInfo;

float physColliderGenerateMass(physCollider *collider, float *vertexMassArray);
void physColliderGenerateMoment(const physCollider *collider, const vec3 *centroid, const float *vertexMassArray, float *inertiaTensor);
void physColliderUpdate(physCollider *collider, const physCollider *local, const bone *configuration);
void physColliderDelete(physCollider *collider);

#endif
