#ifndef PARTICLECONSTRAINT_H
#define PARTICLECONSTRAINT_H

typedef struct particleConstraint particleConstraint;
typedef struct particleConstraint {
	union {

	} data;
	void (*func)(const void *const __RESTRICT__ operator, particle *const __RESTRICT__ p, const float dt_ms);
} particleConstraint;

#endif