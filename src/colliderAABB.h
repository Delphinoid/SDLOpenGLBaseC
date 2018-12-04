#ifndef COLLIDERAABB_H
#define COLLIDERAABB_H

#include "return.h"

typedef struct {
	float left;
	float right;
	float top;
	float bottom;
	float front;
	float back;
} cAABB;

return_t cAABBCollision(const cAABB *c1, const cAABB *c2);

#endif
