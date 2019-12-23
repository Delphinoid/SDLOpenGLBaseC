#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "particle.h"

// Modular particle system inspired by Source.

#define PARTICLE_SYSTEM_MAX_PARTICLES 1024

typedef struct particleInitializer particleInitializer;
typedef struct particleEmitterBase particleEmitterBase;
typedef struct particleEmitter particleEmitter;
typedef struct particleOperator particleOperator;
typedef struct particleConstraint particleConstraint;

typedef struct particleSystemChildBase particleSystemChildBase;
typedef struct {

	// Particle information.
	particleBase effect;
	size_t particleInitial;
	size_t particleMax;

	// Particle behaviour controls.
	particleInitializer *initializers;
	particleInitializer *initializerLast;
	particleEmitterBase *emitters;
	size_t emitterNum;
	particleOperator *operators;
	particleOperator *operatorLast;
	particleConstraint *constraints;
	particleConstraint *constraintLast;

	// System lifetime.
	float lifetime;

	// Child systems.
	particleSystemChildBase *children;
	size_t childNum;

} particleSystemBase;

typedef struct particleSystemChildBase {

	particleSystemBase system;

	// How long to delay instantiation
	// of this effect from the parent.
	float delay;

} particleSystemChildBase;

typedef struct particleSystem particleSystem;
typedef struct particleSystem {

	particle *particles;
	size_t particleNum;

	particleEmitter *emitters;

	/// Control points?
	bone configuration;
	/// Control points?
	float lifetime;

	particleSystem *children;

	const particleSystemBase *base;

} particleSystem;

void particleSystemBaseInit(particleSystemBase *const restrict base);
void particleSystemInit(particleSystem *const restrict system);
void particleSystemInstantiate(particleSystem *const restrict system, const particleSystemBase *const base);

#endif
