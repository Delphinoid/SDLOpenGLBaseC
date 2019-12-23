#include "particle.h"

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
	p->currentAnim = 0;
	animInstInit(&p->animator);
	vec3ZeroP(&p->velocity);
	p->lifetime = 0.f;
}

/**void particleEmitterTick(particleEmitter *const restrict emitter, const float elapsedTime){

	const particle *p = NULL;
	particle *i = emitter->particles;

	while(i != NULL){
		i->lifetime += elapsedTime;
		if(i->base->lifetime > 0.f && i->lifetime > i->base->lifetime){
			///moduleParticleFree(&emitter->particles, i, p);
		}else{
			/// Check for collision. Use (cPoint *)&i->configuration.position.
			rndrTick(&i->rndr, elapsedTime);
			p = i;
		}
		///i = (particle *)memSLinkNext(p);
	}

}**/