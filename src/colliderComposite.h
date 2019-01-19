#ifndef COLLIDERCOMPOSITE_H
#define COLLIDERCOMPOSITE_H

#include "memoryShared.h"
#include "colliderMesh.h"
#include "colliderCapsule.h"
#include "colliderSphere.h"
#include "colliderAABB.h"
#include "colliderPoint.h"
#include "return.h"

#define COLLIDER_COMPOSITE_MAX_COLLIDERS 255

typedef uint_least8_t cCompositeIndex_t;

// The composite collider is simply an array of convex colliders.
// This allows it to appear as though it is concave.
typedef struct collider collider;
typedef struct {
	collider *colliders;
	cCompositeIndex_t colliderNum;
} cComposite;

void cCompositeInit(cComposite *const restrict c);
return_t cCompositeInstantiate(cComposite *const restrict instance, const cComposite *const restrict local);
void cCompositeDeleteBase(cComposite *const restrict c);
void cCompositeDelete(cComposite *const restrict c);

#endif
