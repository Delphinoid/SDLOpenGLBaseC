#ifndef HITBOX_H
#define HITBOX_H

#include "hitboxConvexMesh.h"
#include "hitboxCapsule.h"
#include "hitboxSphere.h"
#include "hitboxAABB.h"
#include <stdlib.h>

#define HB_TYPE_INVALID -1
#define HB_TYPE_MESH 0
#define HB_TYPE_CAPSULE 1
#define HB_TYPE_SPHERE 2
#define HB_TYPE_AABB 3

// The size of the largest collider type.
// In this case, it tied between the mesh and capsule at 28 bytes.
#define HB_MAX_COLLIDER_SIZE 28

typedef struct {
	signed char type;  // -1 = invalid, 0 = mesh, 1 = capsule, 2 = sphere, 3 = AABB
	char hull[HB_MAX_COLLIDER_SIZE];  // Stores a collider of the type specified by "type". Needs to be casted.
	                                  // May be affected by translation, rotation and scaling, depending on the type.
} hitbox;

typedef struct {
	size_t hitboxNum;
	hitbox *hitboxes;
} hbArray;

void hbInit(hitbox *hb, signed char type);
void hbDelete(hitbox *hb);

void hbArrayDelete(hbArray *hba);

#endif
