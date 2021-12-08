#ifndef COLLIDERCAPSULE_H
#define COLLIDERCAPSULE_H

#include "transform.h"
#include "return.h"

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
