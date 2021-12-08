#ifndef COLLIDERAABB_H
#define COLLIDERAABB_H

#include "transform.h"
#include "return.h"

typedef struct {
	vec3 min;
	vec3 max;
} cAABB;

void cAABBExpand(cAABB *const __RESTRICT__ c, const float addend);
void cAABBExpandVelocity(cAABB *const __RESTRICT__ c, const vec3 velocity, const float factor);

void cAABBCombine(cAABB *const __RESTRICT__ c1, cAABB *const __RESTRICT__ c2, cAABB *const __RESTRICT__ r);

float cAABBVolume(const cAABB *const __RESTRICT__ c);
float cAABBSurfaceArea(const cAABB *const __RESTRICT__ c);
float cAABBSurfaceAreaHalf(const cAABB *const __RESTRICT__ c);

float cAABBVolumeCombined(const cAABB *const __RESTRICT__ c1, const cAABB *const __RESTRICT__ c2);
float cAABBSurfaceAreaCombined(const cAABB *const __RESTRICT__ c1, const cAABB *const __RESTRICT__ c2);
float cAABBSurfaceAreaHalfCombined(const cAABB *const __RESTRICT__ c1, const cAABB *const __RESTRICT__ c2);

return_t cAABBEncapsulates(const cAABB *const __RESTRICT__ container, const cAABB *const __RESTRICT__ containee);
return_t cAABBCollision(const cAABB *const __RESTRICT__ c1, const cAABB *const __RESTRICT__ c2);

#endif
