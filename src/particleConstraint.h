#ifndef PARTICLECONSTRAINT_H
#define PARTICLECONSTRAINT_H

typedef struct particleConstraint particleConstraint;
typedef struct particleConstraint {
	union {

	} data;
	void (*func)(particleConstraint *const restrict constraint, particle *const restrict p);
} particleConstraint;

#endif