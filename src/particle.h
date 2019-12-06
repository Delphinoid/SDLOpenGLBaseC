#ifndef PARTICLE_H
#define PARTICLE_H

#include "renderable.h"

typedef uint_least8_t particleIndex_t;

typedef struct {
	bone configuration;
	rndrState rndr;
	float lifetime;
	particleIndex_t type;
} particle;

typedef struct {
	// Particle types.
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
