#ifndef PARTICLEOPERATOR_H
#define PARTICLEOPERATOR_H

typedef struct particle particle;

typedef struct particleOperator particleOperator;
typedef struct particleOperator {
	union {

	} data;
	void (*func)(const void *const restrict operator, particle *const restrict p, const float elapsedTime);
} particleOperator;

/** TEMPORARY (TECHNICALLY A FORCE) **/
void particleOperatorAddGravity(const void *const restrict operator, particle *const restrict p, const float elapsedTime);

#endif
