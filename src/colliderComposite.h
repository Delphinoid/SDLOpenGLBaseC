#ifndef COLLIDERCOMPOSITE_H
#define COLLIDERCOMPOSITE_H

#include "manifold.h"
#include "return.h"
#include "memoryShared.h"
#include "colliderMesh.h"
#include "colliderCapsule.h"
#include "colliderSphere.h"
#include "colliderAABB.h"
#include "colliderPoint.h"

#define COLLIDER_COMPOSITE_MAX_COLLIDERS 255

typedef int_least8_t  cCompositeType_t;
typedef uint_least8_t cCompositeIndex_t;

typedef union cSeparation cSeparation;
typedef struct {
	cSeparation *separations;
	cCompositeIndex_t separationNum;
} cCompositeSeparation;

// The composite collider is simply an array of convex colliders.
// This allows it to appear as though it is concave.
typedef struct collider collider;
typedef struct {
	collider *colliders;
	cCompositeIndex_t colliderNum;
} cComposite;

void cCompositeInit(cComposite *const restrict cc);
void cCompositeDelete(cComposite *const restrict cc);

#endif
