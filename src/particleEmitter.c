#include "particleEmitter.h"

void particleEmitterInit(particleEmitter *const restrict emitter){
	emitter->lifetime = 0.f;
	emitter->frequency = 0.032f;
}

size_t particleEmitterTick(particleEmitter *const restrict emitter, const particleEmitterBase *base, const float elapsedTime){
	emitter->lifetime += elapsedTime;
	return((*base->func)(emitter));
}

/** TEMPORARY **/
size_t particleEmitterContinuous(particleEmitter *const restrict emitter){
	if(emitter->lifetime >= emitter->frequency){
		const size_t count = emitter->lifetime / emitter->frequency;
		emitter->lifetime -= ((float)count) * emitter->frequency;
		return count;
	}
	return 0;
}