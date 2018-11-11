#ifndef HITBOXAABB_H
#define HITBOXAABB_H

#include "return.h"

typedef struct {
	float left;
	float right;
	float top;
	float bottom;
	float front;
	float back;
} hbAABB;

return_t hbAABBCollision(const hbAABB *c1, const hbAABB *c2);

#endif
