#ifndef PARTICLECONSTRAINT_H
#define PARTICLECONSTRAINT_H

typedef struct particleConstraint particleConstraint;
typedef struct particleConstraint {
	union {

	} data;
	void (*func)(const void *const restrict operator, particle *const restrict p, const float elapsedTime);
} particleConstraint;

#endif