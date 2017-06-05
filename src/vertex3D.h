#ifndef VERTEX3D_H
#define VERTEX3D_H

#include "vec3.h"

typedef struct vertex3D {
	vec3 pos;
	float u, v;
	float nx, ny, nz;
} vertex3D;

#endif
