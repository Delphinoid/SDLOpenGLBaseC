#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "particle.h"

// Modular particle system inspired by Source.

typedef struct particleInitializer particleInitializer;
typedef struct particleEmitterBase particleEmitterBase;
typedef struct particleEmitter particleEmitter;
typedef struct particleOperator particleOperator;
typedef struct particleConstraint particleConstraint;

typedef struct particleSystemBase particleSystemBase;
typedef struct {

	particleSystemBase *system;

	// How long to delay instantiation
	// of this effect from the parent.
	float delay;

} particleSystemChildBase;

typedef struct particleSystemBase {

	// Particle information.
	particleBase properties;
	size_t particleInitial;
	size_t particleMax;

	// Particle behaviour controls.
	particleInitializer *initializers;
	particleInitializer *initializerLast;  // Last element in initializers + 1.
	particleEmitterBase *emitters;
	size_t emitterNum;
	particleOperator *operators;
	particleOperator *operatorLast;  // Last element in operators + 1.
	particleConstraint *constraints;
	particleConstraint *constraintLast;  // Last element in constraints + 1.

	// System lifetime.
	float lifetime;

	// Child systems.
	/** THIS SHOULD BE particleSystemChildBase **/
	particleSystemBase *children;
	/** THIS SHOULD BE particleSystemChildBase **/
	size_t childNum;

	char *name;

} particleSystemBase;

typedef struct particleSystem particleSystem;
typedef struct particleSystem {

	particle *particles;
	size_t particleNum;

	particleEmitter *emitters;

	/** Control points? **/
	bone configuration;
	/** Control points? **/
	float lifetime;

	particleSystem *children;

	const particleSystemBase *base;

} particleSystem;

void particleSystemBaseInit(particleSystemBase *const __RESTRICT__ base);
void particleSystemInit(particleSystem *const __RESTRICT__ system);

void particleSystemInstantiate(particleSystem *const __RESTRICT__ system, const particleSystemBase *const base);

return_t particleSystemTick(particleSystem *const __RESTRICT__ system, const float elapsedTime);
void particleSystemRender(const particleSystem *const __RESTRICT__ system, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float distance, const float interpT);

void particleSystemBaseDelete(particleSystemBase *const __RESTRICT__ base);
void particleSystemDelete(particleSystem *const __RESTRICT__ system);

#endif
