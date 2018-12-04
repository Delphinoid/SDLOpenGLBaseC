#ifndef COLLIDER_H
#define COLLIDER_H

#include "memoryShared.h"
#include "colliderConvexMesh.h"
#include "colliderCapsule.h"
#include "colliderSphere.h"
#include "colliderAABB.h"

#define COLLIDER_TYPE_INVALID -1
#define COLLIDER_TYPE_MESH 0
#define COLLIDER_TYPE_CAPSULE 1
#define COLLIDER_TYPE_SPHERE 2
#define COLLIDER_TYPE_AABB 3
#define COLLIDER_TYPE_POINT 4
#define COLLIDER_TYPE_NUM 5

// The size of the largest collider type.
// In this case, it tied between the mesh and capsule at 28 bytes.
#define COLLIDER_MAX_SIZE 28

#define COLLIDER_ARRAY_MAX_SIZE 256

typedef int_least8_t  colliderType_t;
typedef uint_least8_t colliderIndex_t;

typedef struct {
	byte_t hull[COLLIDER_MAX_SIZE];  // Stores a collider of the type specified by "type". Needs to be casted.
	                                 // May be affected by translation, rotation and scaling, depending on the type.
	colliderType_t type;  // -1 = invalid, 0 = mesh, 1 = capsule, 2 = sphere, 3 = AABB, 4 = point
} collider;

typedef struct {
	collider *colliders;
	colliderIndex_t colliderNum;
} colliderArray;

void cInit(collider *c, const colliderType_t type);
void cDelete(collider *c);

void cArrayDelete(colliderArray *ca);

#endif
