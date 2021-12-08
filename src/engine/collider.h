#ifndef COLLIDER_H
#define COLLIDER_H

#include "colliderShared.h"
#include "colliderComposite.h"
#include "colliderHull.h"
#include "colliderCapsule.h"
#include "colliderSphere.h"
#include "colliderPoint.h"
#include "flags.h"

#define COLLIDER_TYPE_UNKNOWN  -1
#define COLLIDER_TYPE_HULL      0
#define COLLIDER_TYPE_CAPSULE   1
#define COLLIDER_TYPE_SPHERE    2
#define COLLIDER_TYPE_AABB      3
#define COLLIDER_TYPE_POINT     4
#define COLLIDER_TYPE_COMPOSITE 5
#define COLLIDER_TYPE_NUM       6

#define COLLIDER_INSTANCE 0x01

typedef int_least8_t colliderType_t;

typedef struct collider {
	// Calculate the size of the largest collider type.
	union {
		cHull mesh;
		cCapsule capsule;
		cSphere sphere;
		cAABB aabb;
		cPoint point;
		cComposite composite;
	} data;
	colliderType_t type;
	// Fits in for free next to colliderType_t.
	flags_t flags;
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
	const transform configuration
);

void cInit(collider *const __RESTRICT__ c, const colliderType_t type);
return_t cInstantiate(collider *const instance, const collider *const local);
cAABB cTransform(collider *const instance, const vec3 instanceCentroid, const collider *const local, const vec3 localCentroid, const transform configuration);
void cDelete(collider *const __RESTRICT__ c);

#endif
