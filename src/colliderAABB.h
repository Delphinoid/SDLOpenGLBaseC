#ifndef COLLIDERAABB_H
#define COLLIDERAABB_H

#include "vec3.h"
#include "return.h"

typedef struct {
	vec3 min;
	vec3 max;
} cAABB;

void cAABBExpand(cAABB *const restrict c, const float addend);
void cAABBExpandVelocity(cAABB *const restrict c, const vec3 velocity, const float factor);

void cAABBCombine(cAABB *const restrict c1, cAABB *const restrict c2, cAABB *const restrict r);

float cAABBVolume(const cAABB *const restrict c);
float cAABBSurfaceArea(const cAABB *const restrict c);
float cAABBSurfaceAreaHalf(const cAABB *const restrict c);

float cAABBVolumeCombined(const cAABB *const restrict c1, const cAABB *const restrict c2);
float cAABBSurfaceAreaCombined(const cAABB *const restrict c1, const cAABB *const restrict c2);
float cAABBSurfaceAreaHalfCombined(const cAABB *const restrict c1, const cAABB *const restrict c2);

return_t cAABBEncapsulates(const cAABB *const restrict container, const cAABB *const restrict containee);
return_t cAABBCollision(const cAABB *const restrict c1, const cAABB *const restrict c2);

#endif
