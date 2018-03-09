#ifndef VERTEX_H
#define VERTEX_H

#include "quat.h"

typedef struct {
	vec3  position;
	float u, v;
	vec3 normal;
	int   bIDs[4];
	float bWeights[4];
} vertex;

void vertInit(vertex *v);
void vertTransform(vertex *v, const vec3 *position, const quat *orientation, const vec3 *scale);

#endif
