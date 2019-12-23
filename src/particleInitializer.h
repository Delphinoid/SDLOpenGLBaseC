#ifndef PARTICLEINITIALIZER_H
#define PARTICLEINITIALIZER_H

typedef struct particleInitializer particleInitializer;
typedef struct particleInitializer {
	union {

	} data;
	void (*func)(particleInitializer *const restrict pInitializer, particle *const restrict p);
} particleInitializer;

#endif