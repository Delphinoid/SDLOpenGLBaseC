#ifndef COLLIDERPOINT_H
#define COLLIDERPOINT_H

#include "return.h"
#include "vec3.h"

typedef vec3 cPoint;

return_t cPointCollision(const cPoint *c1, const cPoint *c2);

#endif
