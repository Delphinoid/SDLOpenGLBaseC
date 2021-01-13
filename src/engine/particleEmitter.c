#include "particleEmitter.h"

void particleEmitterInit(particleEmitter *const __RESTRICT__ emitter){
	emitter->lifetime = 0.f;
	emitter->frequency = 0.032f;
}

size_t particleEmitterTick(particleEmitter *const __RESTRICT__ emitter, const particleEmitterBase *base, const float dt_s){
	emitter->lifetime += dt_s;
	return((*base->func)(emitter));
}

/** TEMPORARY **/
size_t particleEmitterContinuous(particleEmitter *const __RESTRICT__ emitter){
	if(emitter->lifetime >= emitter->frequency){
		const size_t count = emitter->lifetime / emitter->frequency;
		emitter->lifetime -= ((float)count) * emitter->frequency;
		return count;
	}
	return 0;
}