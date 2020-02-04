#ifndef PARTICLE_H
#define PARTICLE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "particleSettings.h"
#include "colliderShared.h"
#include "renderable.h"
#include "bone.h"
#include "return.h"

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

typedef struct particle {

	// Particle space configuration.
	bone configuration;
	#ifdef PARTICLE_ALLOW_INTERPOLATION
	bone previous;
	#endif

	// Texture wrapper animation information.
	animIndex_t currentAnim;
	animationInstance animator;

	// Particle properties.
	vec3 velocity;
	float lifetime;

} particle;

void particleBaseInit(particleBase *const __RESTRICT__ base);
void particleInit(particle *const __RESTRICT__ p);

/** TEMPORARY **/
void particleTick(particle *const __RESTRICT__ p, const float elapsedTime);
#ifdef PARTICLE_ALLOW_INTERPOLATION
bone particleState(particle *const __RESTRICT__ p, const float interpT);
#endif

void particleDelete(particle *const __RESTRICT__ p);

#endif
