#ifndef COLLIDERCAPSULE_H
#define COLLIDERCAPSULE_H

#include "collision.h"

typedef struct {
	vec3 a;
	vec3 b;
	float radius;
} cCapsule;

typedef struct {
	vec3 *a;
	vec3 *b;
	float radius;
} cCapsuleDynamic;

#endif
