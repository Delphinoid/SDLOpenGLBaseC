#ifndef PARTICLE_H
#define PARTICLE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "settingsParticle.h"
#include "colliderShared.h"
#include "transform.h"
#include "model.h"

typedef struct {

	// Render data.
	const modelBase *mdl;
	flags_t billboardFlags;

	// Particle lifetime.
	float lifetime;

	// Physical data.
	float density;
	float friction;
	float restitution;
	colliderMask_t layer;
	colliderMask_t mask;

} particleBase;

typedef struct particle {

	// Particle space configuration.
	transform configuration;
	#ifdef PARTICLE_ALLOW_INTERPOLATION
	transform previous;
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
void particleTick(particle *const __RESTRICT__ p, const float dt_s);
#ifdef PARTICLE_ALLOW_INTERPOLATION
transform particleState(particle *const __RESTRICT__ p, const float interpT);
#endif

void particleDelete(particle *const __RESTRICT__ p);

#endif
