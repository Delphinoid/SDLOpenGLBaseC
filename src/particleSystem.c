#include "graphicsManager.h"
#include "particleSystem.h"
#include "particleInitializer.h"
#include "particleEmitter.h"
#include "particleOperator.h"
#include "particleConstraint.h"
#include "texture.h"
#include "model.h"
#include "inline.h"
#include <stdio.h>

/****/
#include "memoryManager.h"

void particleSystemBaseInit(particleSystemBase *const restrict base){
	particleBaseInit(&base->properties);
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

__FORCE_INLINE__ static void particleSystemParticleInitialize(const particleSystem *const restrict system, particle *const p){
	particleInitializer *i = system->base->initializers;
	const particleInitializer *const iLast = system->base->initializers;
	for(; i < iLast; ++i){
		particleInit(p);
		(*i->func)((void *)(&i->data), p);
	}
	/** Apply system configuration. **/
}

__FORCE_INLINE__ static void particleSystemParticleOperate(const particleSystemBase *const restrict base, particle *const p, const float elapsedTime){
	particleOperator *o = base->operators;
	const particleOperator *const oLast = base->operatorLast;
	for(; o < oLast; ++o){
		(*o->func)((void *)(&o->data), p, elapsedTime);
	}
}

__FORCE_INLINE__ static void particleSystemParticleConstrain(const particleSystemBase *const restrict base, particle *const p, const float elapsedTime){
	particleConstraint *c = base->constraints;
	const particleConstraint *const cLast = base->constraintLast;
	for(; c < cLast; ++c){
		///(*c->func)((void *)(&c->data), p, elapsedTime);
	}
}

__FORCE_INLINE__ static void particleSystemParticleSpawn(particleSystem *const restrict system, const size_t particleNum){
	particle *p = &system->particles[system->particleNum];
	const particle *const pLast = &p[particleNum];
	for(; p < pLast; ++p){
		particleSystemParticleInitialize(system, p);
	}
	system->particleNum += particleNum;
}

/** TEMPORARY PARTICLE EFFECT **/
void particleSystemInstantiate(particleSystem *const restrict system, const particleSystemBase *const base){

	particleSystemInit(system);
	system->base = base;

	if(base->emitterNum != 0){
		system->emitters = memAllocate(base->emitterNum*sizeof(particleEmitter));
		if(system->emitters == NULL){
			/** Memory allocation failure. **/
		}else{
			particleEmitter *e = system->emitters;
			const particleEmitter *const eLast = &e[base->emitterNum];
			for(; e < eLast; ++e){
				particleEmitterInit(e);
			}
		}
	}

	boneInit(&system->configuration);
	system->lifetime = base->lifetime;

	if(base->particleMax != 0){
		system->particles = memAllocate(base->particleMax*sizeof(particle));
		if(system->particles == NULL){
			/** Memory allocation failure. **/
		}else{
			particle *p = system->particles;
			const particle *const pLast = &p[base->particleMax];
			for(; p < pLast; ++p){
				particleInit(p);
				if(system->particleNum < base->particleInitial){
					particleSystemParticleInitialize(system, p);
					++system->particleNum;
				}
			}
		}
	}

	if(base->childNum != 0){
		system->children = memAllocate(base->childNum*sizeof(particleSystem));
		if(system->children == NULL){
			/** Memory allocation failure. **/
		}else{
			particleSystem *c = system->children;
			/** THIS SHOULD BE particleSystemChildBase **/
			particleSystemBase *cBase = base->children;
			/** THIS SHOULD BE particleSystemChildBase **/
			const particleSystem *const cLast = &c[base->childNum];
			for(; c < cLast; ++c, ++cBase){
				particleSystemInstantiate(c, cBase);
			}
		}
	}

}

__FORCE_INLINE__ static void particleSystemEmittersTick(particleSystem *const restrict system, const float elapsedTime){

	size_t remaining = system->base->particleMax - system->particleNum;

	particleEmitter *e = system->emitters;
	particleEmitterBase *eBase = system->base->emitters;
	const particleEmitter *const eLast = &e[system->base->emitterNum];

	for(; e < eLast; ++e, ++eBase){

		const size_t count = particleEmitterTick(e, eBase, elapsedTime);

		if(count < remaining){
			particleSystemParticleSpawn(system, count);
			remaining -= count;
		}else{
			particleSystemParticleSpawn(system, remaining);
			break;
		}

	}

}

__FORCE_INLINE__ static void particleSystemParticlesTick(particleSystem *const restrict system, const float elapsedTime){

	const particleSystemBase *base = system->base;
	particle *p = system->particles;
	const particle *const pLast = &p[system->particleNum];

	for(; p < pLast; ++p){
		p->lifetime -= elapsedTime;
		if(p->lifetime > 0.f){
			particleSystemParticleOperate(base, p, elapsedTime);
			particleSystemParticleConstrain(base, p, elapsedTime);
			particleTick(p, elapsedTime);
		}else{
			particleInit(p);
		}
	}

}

return_t particleSystemTick(particleSystem *const restrict system, const float elapsedTime){

	system->lifetime -= elapsedTime;

	if(system->lifetime > 0.f){

		// The system is dead.
		return 0;

	}else{

		particleSystem *c = system->children;
		const particleSystem *const cLast = &c[system->base->childNum];

		particleSystemEmittersTick(system, elapsedTime);
		particleSystemParticlesTick(system, elapsedTime);

		for(; c < cLast; ++c){
			particleSystemTick(c, elapsedTime);
		}

		return 1;

	}

}

void particleSystemRender(const particleSystem *const restrict system, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT){

	const renderableBase *const rndr = &system->base->properties.rndr;
	vertexIndexNum_t indexNum;
	const void *offset;

	particle *p = system->particles;
	const particle *pLast = &p[system->particleNum];

	particleState *state = &gfxMngr->particleTransformState[0];

	// Bind the texture (if needed).
	gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, rndr->tw->animations[p->currentAnim].frames[p->animator.currentFrame].image->diffuseID);

	// Generate state buffers.
	for(; p < pLast; ++p, ++state){
		state->transformation = boneMatrix(p->configuration);
		state->frame = twState(rndr->tw, &p->animator, p->currentAnim, interpT)->subframe;
	}

	// Upload the state data to the shader.
	glBufferSubData(GL_ARRAY_BUFFER, 0, system->particleNum*sizeof(particleState), &gfxMngr->particleTransformState[0]);

	// Render the model.
	glBindVertexArray(rndr->mdl->buffers.vaoID);
	mdlFindCurrentLOD(rndr->mdl, &indexNum, &offset, distance, gfxMngr->biasLOD);
	if(indexNum){
		glDrawElementsInstanced(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, offset, system->particleNum);
	}

}

void particleSystemBaseDelete(particleSystemBase *const restrict base){
	/** memFree() IS TEMPORARY HERE **/
	if(base->initializers != NULL){
		memFree(base->initializers);
	}
	if(base->emitters != NULL){
		memFree(base->emitters);
	}
	if(base->operators != NULL){
		memFree(base->operators);
	}
	if(base->constraints != NULL){
		memFree(base->constraints);
	}
	if(base->name != NULL){
		memFree(base->name);
	}
}

void particleSystemDelete(particleSystem *const restrict system){
	/** memFree() IS TEMPORARY HERE **/
	if(system->emitters != NULL){
		memFree(system->emitters);
	}
	if(system->particles != NULL){
		memFree(system->particles);
	}
	if(system->children != NULL){
		particleSystem *c = system->children;
		const particleSystem *const cLast = &c[system->base->childNum];
		// Delete the system's children.
		for(; c < cLast; ++c){
			particleSystemDelete(c);
		}
		memFree(system->children);
	}
}