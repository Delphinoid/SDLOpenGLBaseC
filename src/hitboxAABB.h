#ifndef HITBOXAABB_H
#define HITBOXAABB_H

typedef struct {
	float left;
	float right;
	float top;
	float bottom;
	float front;
	float back;
} hbAABB;

signed char hbAABBCollision(const hbAABB *c1, const hbAABB *c2);

#endif
