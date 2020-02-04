#ifndef COLLIDERPOINT_H
#define COLLIDERPOINT_H

#include "vec3.h"
#include "return.h"

typedef vec3 cPoint;

return_t cPointCollision(const cPoint *const __RESTRICT__ c1, const cPoint *const __RESTRICT__ c2);

#endif
