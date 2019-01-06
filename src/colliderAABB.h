#ifndef COLLIDERAABB_H
#define COLLIDERAABB_H

#include "manifold.h"
#include "return.h"

typedef struct {
	float left;
	float right;
	float top;
	float bottom;
	float front;
	float back;
} cAABB;

return_t cAABBCollision(const cAABB *const restrict c1, const cAABB *const restrict c2);

#endif
