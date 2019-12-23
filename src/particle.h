#ifndef PARTICLE_H
#define PARTICLE_H

#include "colliderShared.h"
#include "renderable.h"
#include "bone.h"

typedef struct {

	// Render data.
	renderableBase rndr;
	flags_t billboardFlags;

	// Particle lifetime.
	float lifetime;

	// Physical data.
	float density;
	float friction;
	float restitution;
	colliderMask_t layers;

} particleBase;

typedef struct {

	// Particle space configuration.
	bone configuration;

	// Texture wrapper animation information.
	animIndex_t currentAnim;
	animationInstance animator;

	// Particle properties.
	vec3 velocity;
	float lifetime;

} particle;

void particleBaseInit(particleBase *const restrict base);
void particleInit(particle *const restrict p);

#endif
