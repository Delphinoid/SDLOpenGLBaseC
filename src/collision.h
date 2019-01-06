#ifndef COLLISION_H
#define COLLISION_H

#include "collider.h"

// Stores a type of separation.
typedef union cSeparation {
	cMeshSeparation mesh;
	cCompositeSeparation composite;
} cSeparation;

// Forward declarations for inlining.
extern return_t (* const cCollisionJumpTable[COLLIDER_TYPE_NUM][COLLIDER_TYPE_NUM])(
	const void *const restrict,
	const void *const restrict,
	void *const restrict,
	cContact *const restrict
);
extern return_t (* const cSeparationJumpTable[COLLIDER_TYPE_NUM][COLLIDER_TYPE_NUM])(
	const void *const restrict,
	const void *const restrict,
	const void *const restrict
);

return_t cCheckCollision(const collider *const restrict c1, const collider *const restrict c2, cSeparation *const restrict sc, cContact *const restrict cm);
return_t cCheckSeparation(const collider *const restrict c1, const collider *const restrict c2, const cSeparation *const restrict sc);

#endif
