#ifndef COLLIDER_H
#define COLLIDER_H

#include "colliderComposite.h"
#include "colliderMesh.h"
#include "colliderCapsule.h"
#include "colliderSphere.h"
#include "colliderAABB.h"
#include "colliderPoint.h"
#include "flags.h"

#define COLLIDER_TYPE_UNKNOWN  -1
#define COLLIDER_TYPE_MESH      0
#define COLLIDER_TYPE_CAPSULE   1
#define COLLIDER_TYPE_SPHERE    2
#define COLLIDER_TYPE_AABB      3
#define COLLIDER_TYPE_POINT     4
#define COLLIDER_TYPE_COMPOSITE 5
#define COLLIDER_TYPE_NUM       6

#define COLLIDER_INSTANCE 0x01

typedef int_least8_t  colliderType_t;
typedef uint_least8_t colliderIndex_t;

typedef struct collider {
	// Calculate the size of the largest collider type.
	union {
		cMesh mesh;
		cCapsule capsule;
		cSphere sphere;
		cAABB aabb;
		cPoint point;
		cComposite composite;
	} data;
	colliderType_t type;
	flags_t flags;  // Fits in for free next to colliderType_t.
} collider;

// Forward declarations for inlining.
extern return_t (* const cInstantiateJumpTable[COLLIDER_TYPE_NUM])(
	void *const instance,
	const void *const local
);
extern cAABB (* const cTransformJumpTable[COLLIDER_TYPE_NUM])(
	void *const instance,
	const vec3 instanceCentroid,
	const void *const local,
	const vec3 localCentroid,
	const vec3 position,
	const quat orientation,
	const vec3 scale
);

void cInit(collider *const restrict c, const colliderType_t type);
return_t cInstantiate(collider *const instance, const collider *const local);
cAABB cTransform(collider *const instance, const vec3 instanceCentroid, const collider *const local, const vec3 localCentroid, const vec3 position, const quat orientation, const vec3 scale);
void cDelete(collider *const restrict c);

#endif
