#ifndef PARTICLE_H
#define PARTICLE_H

#include "renderable.h"

typedef uint_least8_t particleIndex_t;

typedef struct {
	renderableBase rndr;
} particleBase;

typedef struct {
	bone configuration;
	rndrState rndr;
	float lifetime;
	const particleBase *base;
} particle;

typedef struct {
	// SLink of particle types.
	size_t particleTypes;
	renderableBase *types;
} particleEmitterBase;

typedef struct {

	// Array of particles.
	size_t particleNum;
	particle *particles;

	// Particle emitters may contain either their
	// own configuration in space or a pointer to
	// another object's configuration.
	//union {
		//bone local;
		bone *configuration;
	//} configuration;

	const particleEmitterBase *base;

} particleEmitter;

#endif
