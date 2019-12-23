#ifndef PARTICLEOPERATOR_H
#define PARTICLEOPERATOR_H

typedef struct particleOperator particleOperator;
typedef struct particleOperator {
	union {

	} data;
	void (*func)(particleOperator *const restrict pOperator, particle *const restrict p);
} particleOperator;

#endif
