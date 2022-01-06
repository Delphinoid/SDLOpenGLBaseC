#ifndef VERTEX_H
#define VERTEX_H

#include "transform.h"

// WARNING: Changing this alone will not work,
//          as the shader still expects 4 values.
#define VERTEX_MAX_BONES 4

typedef struct vertex {
	vec3  position;
	float u, v;
	vec3 normal;
	int   bIDs[VERTEX_MAX_BONES];
	float bWeights[VERTEX_MAX_BONES];
} vertex;

void vInit(vertex *const __RESTRICT__ v);

#endif
