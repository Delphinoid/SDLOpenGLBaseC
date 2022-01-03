#include "graphicsManager.h"
#include "particleSystem.h"
#include "particleInitializer.h"
#include "particleEmitter.h"
#include "particleOperator.h"
#include "particleConstraint.h"
#include "spriteSettings.h"
#include "sprite.h"
#include "texture.h"
#include "model.h"
#include <stdio.h>

/****/
#include "memoryManager.h"

void particleSystemBaseInit(particleSystemBase *const __RESTRICT__ base){
	particleBaseInit(&base->properties);
	base->particleInitial = 0;
	base->particleMax = SPRITE_STATE_BUFFER_SIZE;
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
	base->name = NULL;
}

void particleSystemInit(particleSystem *const __RESTRICT__ system){
	system->particles = NULL;
	system->particleNum = 0;
	system->configuration = g_tfIdentity;
	system->lifetime = 0.f;
	system->children = NULL;
	system->base = NULL;
}

__FORCE_INLINE__ static void particleSystemParticleInitialize(const particleSystem *const __RESTRICT__ system, particle *const p){
	particleInitializer *i = system->base->initializers;
	const particleInitializer *const iLast = system->base->initializerLast;
	for(; i < iLast; ++i){
		particleInit(p);
		(*i->func)((void *)(&i->data), p);
	}
	/** Apply system configuration? **/
}

__FORCE_INLINE__ static void particleSystemParticleOperate(const particleSystemBase *const __RESTRICT__ base, particle *const p, const float dt_s){
	particleOperator *o = base->operators;
	const particleOperator *const oLast = base->operatorLast;
	for(; o < oLast; ++o){
		(*o->func)((void *)(&o->data), p, dt_s);
	}
}

__FORCE_INLINE__ static void particleSystemParticleConstrain(const particleSystemBase *const __RESTRICT__ base, particle *const p, const float dt_s){
	particleConstraint *c = base->constraints;
	const particleConstraint *const cLast = base->constraintLast;
	for(; c < cLast; ++c){
		(*c->func)((void *)(&c->data), p, dt_s);
	}
}

__FORCE_INLINE__ static void particleSystemParticleSpawn(particleSystem *const __RESTRICT__ system, const size_t particleNum){
	particle *p = &system->particles[system->particleNum];
	const particle *const pLast = &p[particleNum];
	for(; p < pLast; ++p){
		particleSystemParticleInitialize(system, p);
	}
	system->particleNum += particleNum;
}

/** TEMPORARY PARTICLE EFFECT **/
void particleSystemInstantiate(particleSystem *const __RESTRICT__ system, const particleSystemBase *const base){

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

	system->configuration = g_tfIdentity;
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

__FORCE_INLINE__ static void particleSystemEmittersTick(particleSystem *const __RESTRICT__ system, const float dt_s){

	size_t remaining = system->base->particleMax - system->particleNum;

	if(remaining > 0){

		particleEmitter *e = system->emitters;
		particleEmitterBase *eBase = system->base->emitters;
		const particleEmitter *const eLast = &e[system->base->emitterNum];

		for(; e < eLast; ++e, ++eBase){

			const size_t count = particleEmitterTick(e, eBase, dt_s);

			if(count > 0){
				if(count < remaining){
					particleSystemParticleSpawn(system, count);
					remaining -= count;
				}else{
					particleSystemParticleSpawn(system, remaining);
					break;
				}
			}

		}

	}

}

__FORCE_INLINE__ static void particleSystemParticlesTick(particleSystem *const __RESTRICT__ system, const float dt_s){

	const particleSystemBase *base = system->base;
	particle *p = system->particles;
	const particle *const pLast = &p[system->particleNum];

	for(; p < pLast; ++p){
		p->lifetime -= dt_s;
		///if(p->lifetime > 0.f){
			particleSystemParticleOperate(base, p, dt_s);
			particleSystemParticleConstrain(base, p, dt_s);
			particleTick(p, dt_s);
		///}else{
		///	particleInit(p);
		///}
	}

}

return_t particleSystemTick(particleSystem *const __RESTRICT__ system, const float dt_s){

	system->lifetime -= dt_s;

	if(system->lifetime > 0.f){

		// The system is dead.
		return 0;

	}else{

		particleSystem *c = system->children;
		const particleSystem *const cLast = &c[system->base->childNum];

		particleSystemEmittersTick(system, dt_s);
		particleSystemParticlesTick(system, dt_s);

		// Recursively update the particle systems.
		/// Try to make this function iterative?
		for(; c < cLast; ++c){
			particleSystemTick(c, dt_s);
		}

		return 1;

	}

}

__FORCE_INLINE__ static void particleSystemParticlesRender(const particleSystem *const __RESTRICT__ system, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float distance, const float interpT){

	const modelBase *const mdl = system->base->properties.mdl;
	vertexIndex_t indexNum;
	const void *offset;

	particle *p = system->particles;
	const particle *pLast = &p[system->particleNum];

	spriteState *state = &gfxMngr->shdrData.spriteTransformState[0];

	gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, mdl->tw->animations[p->currentAnim].frames[p->animator.currentFrame].image->diffuseID);

	// Generate state buffers.
	for(; p < pLast; ++p, ++state){
		state->transformation = tfMatrix4(p->configuration);
		state->frame = twState(mdl->tw, &p->animator, p->currentAnim, interpT)->subframe;
	}

	glBindVertexArray(mdl->buffers.vaoID);

	// Upload the state data to the shader.
	glBindBuffer(GL_ARRAY_BUFFER, g_sprStateBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, system->particleNum*sizeof(spriteState), &gfxMngr->shdrData.spriteTransformState[0]);

	// Render the system.
	if(mdl->lods == NULL){
		indexNum = mdl->buffers.indexNum;
		offset = 0;
	}else{
		mdlFindCurrentLOD(mdl->lods, mdl->lodNum, &indexNum, &offset, distance, gfxMngr->shdrData.biasLOD);
	}
	if(indexNum){
		glDrawElementsInstanced(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, offset, system->particleNum);
	}

}

void particleSystemRender(const particleSystem *const __RESTRICT__ system, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float distance, const float interpT){

	particleSystem *c = system->children;
	const particleSystem *const cLast = &c[system->base->childNum];

	particleSystemParticlesRender(system, gfxMngr, cam, distance, interpT);

	// Recursively render the particle systems.
	/// Try to make this function iterative?
	for(; c < cLast; ++c){
		particleSystemRender(c, gfxMngr, cam, distance, interpT);
	}

}

void particleSystemBaseDelete(particleSystemBase *const __RESTRICT__ base){
	/** We should ideally only have one call to memFree. **/
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

void particleSystemDelete(particleSystem *const __RESTRICT__ system){
	/** We should ideally only have one call to memFree. **/
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
