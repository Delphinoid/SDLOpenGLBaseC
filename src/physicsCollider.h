#ifndef PHYSICSCOLLIDER_H
#define PHYSICSCOLLIDER_H

#include "physicsBodyShared.h"
#include "colliderAABB.h"
#include "bone.h"

typedef uint_least8_t physColliderIndex_t;

typedef struct {
	cAABB aabb;     // The hull's bounding box.
	collider c;     // The collision mesh in local space.
	vec3 centroid;  // The collider's center of mass.
} physCollider;

float physColliderGenerateMass(physCollider *const restrict collider, const float *const vertexMassArray);
void physColliderGenerateMoment(const physCollider *const restrict collider, const vec3 *const restrict centroid, const float *const restrict vertexMassArray, float *const restrict inertiaTensor);
void physColliderUpdate(physCollider *const restrict collider, const physCollider *const restrict local, const bone *const restrict configuration);
void physColliderDelete(physCollider *const restrict collider);

#endif
