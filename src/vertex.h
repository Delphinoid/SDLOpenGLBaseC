#ifndef VERTEX_H
#define VERTEX_H

#include "vec3.h"

typedef struct vertex {
	vec3 pos;
	float u, v;
	float nx, ny, nz;
} vertex;

#endif
