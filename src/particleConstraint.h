#ifndef PARTICLECONSTRAINT_H
#define PARTICLECONSTRAINT_H

typedef struct particleConstraint particleConstraint;
typedef struct particleConstraint {
	union {

	} data;
	void (*func)(particleConstraint *const restrict pConstraint, particle *const restrict p);
} particleConstraint;

#endif