#ifndef PARTICLEINITIALIZER_H
#define PARTICLEINITIALIZER_H

#include "qualifiers.h"

typedef struct particle particle;

typedef struct particleInitializer particleInitializer;
typedef struct particleInitializer {
	union {

	} data;
	void (*func)(const void *const __RESTRICT__ initializer, particle *const __RESTRICT__ p);
} particleInitializer;

/** TEMPORARY **/
void particleInitializerSphereRandom(const void *const __RESTRICT__ initializer, particle *const __RESTRICT__ p);

#endif