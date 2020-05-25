#ifndef PARTICLEOPERATOR_H
#define PARTICLEOPERATOR_H

#include "qualifiers.h"

typedef struct particle particle;

typedef struct particleOperator particleOperator;
typedef struct particleOperator {
	union {

	} data;
	void (*func)(const void *const __RESTRICT__ operator, particle *const __RESTRICT__ p, const float dt);
} particleOperator;

/** TEMPORARY (TECHNICALLY A FORCE) **/
void particleOperatorAddGravity(const void *const __RESTRICT__ operator, particle *const __RESTRICT__ p, const float dt);

#endif
