#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include <stddef.h>

/** THESE ARE BOTH TEMPORARY **/
typedef struct particleEmitter {
	// Time until the next trigger.
	///float trigger;

	float lifetime;
	float frequency;
} particleEmitter;

typedef struct particleEmitterBase {
	union {

	} data;
	size_t (*func)(particleEmitter *const restrict emitter);
} particleEmitterBase;

void particleEmitterInit(particleEmitter *const restrict emitter);

size_t particleEmitterTick(particleEmitter *const restrict emitter, const particleEmitterBase *base, const float elapsedTime);

/** TEMPORARY **/
size_t particleEmitterContinuous(particleEmitter *const restrict emitter);

#endif
