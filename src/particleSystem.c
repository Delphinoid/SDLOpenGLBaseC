#include "particleSystem.h"

void particleSystemBaseInit(particleSystemBase *const restrict base){
	particleBaseInit(&base->effect);
	base->particleMax = PARTICLE_SYSTEM_MAX_PARTICLES;
	base->initializers = NULL;
	base->initializerLast = NULL;
	base->emitters = NULL;
	base->emitterNum = 0;
	base->operators = NULL;
	base->operatorLast = NULL;
	base->constraints = NULL;
	base->constraintLast = NULL;
	base->lifetime = 0.f;
	base->children = NULL;
	base->childNum = 0;
}

void particleSystemInit(particleSystem *const restrict system){
	system->particles = NULL;
	system->particleNum = 0;
	boneInit(&system->configuration);
	system->lifetime = 0.f;
	system->children = NULL;
	system->base = NULL;
}

void particleSystemInstantiate(particleSystem *const restrict system, const particleSystemBase *const base){

	particleSystemInit(system);

}

void particleSystemTick(particleSystem *const restrict system, const float elapsedTime){

	particleSystem *c = system->children;
	const particleSystem *const cLast = &system->children[system->base->childNum];



	for(; c < cLast; ++c){
		particleSystemTick(c, elapsedTime);
	}

}