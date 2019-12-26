#ifndef PARTICLEINITIALIZER_H
#define PARTICLEINITIALIZER_H

typedef struct particle particle;

typedef struct particleInitializer particleInitializer;
typedef struct particleInitializer {
	union {

	} data;
	void (*func)(const void *const restrict initializer, particle *const restrict p);
} particleInitializer;

/** TEMPORARY **/
void particleInitializerSphereRandom(const void *const restrict initializer, particle *const restrict p);

#endif