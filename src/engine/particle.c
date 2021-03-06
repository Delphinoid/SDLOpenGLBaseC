#include "particle.h"
#include "model.h"
#include <stdio.h>

void particleBaseInit(particleBase *const __RESTRICT__ base){
	base->mdl = &g_mdlDefault;
	base->billboardFlags = BILLBOARD_SPRITE;
	base->lifetime = 0.f;
	base->density = 0.f;
	base->friction = 1.f;
	base->restitution = 1.f;
	base->layers = 0;
}

void particleInit(particle *const __RESTRICT__ p){
	tfInit(&p->configuration);
	#ifdef PARTICLE_ALLOW_INTERPOLATION
	tfInit(&p->previous);
	#endif
	p->currentAnim = 0;
	animInstInit(&p->animator);
	vec3ZeroP(&p->velocity);
	p->lifetime = 0.f;
}

/** TEMPORARY **/
void particleTick(particle *const __RESTRICT__ p, const float dt_s){
	const vec3 delta = vec3VMultS(p->velocity, dt_s);
	#ifdef PARTICLE_ALLOW_INTERPOLATION
	p->previous = p->configuration;
	#endif
	p->configuration.position = vec3VAddV(p->configuration.position, delta);
}

#ifdef PARTICLE_ALLOW_INTERPOLATION
transform particleState(particle *const __RESTRICT__ p, const float interpT){
	return tfInterpolate(p->previous, p->configuration, interpT);
}
#endif

void particleDelete(particle *const __RESTRICT__ p){
	p->lifetime = 0.f;
}