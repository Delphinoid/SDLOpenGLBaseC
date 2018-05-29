#ifndef PHYSCOLLIDER_H
#define PHYSCOLLIDER_H

#include "hitbox.h"
#include "bone.h"

/** Allow more than just convex meshes. **/
typedef struct {
	hbAABB aabb;  // The hull's bounding box.
	hitbox hb;    // The collision mesh in local space.
	vec3 centroid;
} physCollider;

/* Physics collider functions. */
float physColliderGenerateMass(physCollider *collider, float *vertexMassArray);
void physColliderGenerateMoment(const physCollider *collider, const vec3 *centroid, const float *vertexMassArray, float *inertiaTensor);
void physColliderUpdate(physCollider *collider, const physCollider *local, const bone *configuration);

void physColliderDelete(physCollider *collider);

#endif
