#ifndef PHYSCOLLIDER_H
#define PHYSCOLLIDER_H

#include "hitboxAABB.h"
#include "hitboxConvexMesh.h"
#include "mat3.h"

typedef struct {
	hbAABB aabb;   // The hull's bounding box.
	hbMesh hull;   // The collision mesh in local space.
	vec3 centroid;
} physCollider;

/* Physics collider functions. */
float physColliderGenerateMassProperties(physCollider *collider, float *vertexMassArray);
void physColliderDelete(physCollider *collider);

#endif
