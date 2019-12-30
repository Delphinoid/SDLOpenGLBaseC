#include "particle.h"
#include "model.h"
#include <stdio.h>

void particleBaseInit(particleBase *const restrict base){
	rndrBaseInit(&base->rndr);
	base->billboardFlags = BILLBOARD_SPRITE;
	base->lifetime = 0.f;
	base->density = 0.f;
	base->friction = 1.f;
	base->restitution = 1.f;
	base->layers = 0;
}

void particleInit(particle *const restrict p){
	boneInit(&p->configuration);
	#ifdef PARTICLE_ALLOW_INTERPOLATION
	boneInit(&p->previous);
	#endif
	p->currentAnim = 0;
	animInstInit(&p->animator);
	vec3ZeroP(&p->velocity);
	p->lifetime = 0.f;
}

/** TEMPORARY **/
void particleTick(particle *const restrict p, const float elapsedTime){
	const vec3 delta = vec3VMultS(p->velocity, elapsedTime);
	#ifdef PARTICLE_ALLOW_INTERPOLATION
	p->previous = p->configuration;
	#endif
	p->configuration.position = vec3VAddV(p->configuration.position, delta);
}

#ifdef PARTICLE_ALLOW_INTERPOLATION
bone particleState(particle *const restrict p, const float interpT){
	return boneInterpolate(p->previous, p->configuration, interpT);
}
#endif

void particleDelete(particle *const restrict p){
	p->lifetime = 0.f;
}