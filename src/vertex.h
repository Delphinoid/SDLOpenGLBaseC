#ifndef VERTEX_H
#define VERTEX_H

#include "quat.h"

// WARNING: Changing this alone will not work,
//          as the shader still expects 4 values.
#define VERTEX_MAX_BONES 4

typedef struct {
	vec3  position;
	float u, v;
	vec3 normal;
	int   bIDs[VERTEX_MAX_BONES];
	float bWeights[VERTEX_MAX_BONES];
} vertex;

void vertInit(vertex *const restrict v);
void vertTransform(vertex *const restrict v, const vec3 position, const quat orientation, const vec3 scale);

#endif
