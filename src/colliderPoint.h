#ifndef COLLIDERPOINT_H
#define COLLIDERPOINT_H

#include "vec3.h"
#include "return.h"

typedef vec3 cPoint;

return_t cPointCollision(const cPoint *const restrict c1, const cPoint *const restrict c2);

#endif
