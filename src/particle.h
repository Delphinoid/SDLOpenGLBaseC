#ifndef PARTICLE_H
#define PARTICLE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "particleSettings.h"
#include "particleState.h"
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

void particleBaseInit(particleBase *const restrict base);
void particleInit(particle *const restrict p);

return_t particleBaseSetupBufferAttributes(const particleBase *const restrict base, const GLuint stateBufferID);

/** TEMPORARY **/
void particleTick(particle *const restrict p, const float elapsedTime);
#ifdef PARTICLE_ALLOW_INTERPOLATION
bone particleState(particle *const restrict p, const float interpT);
#endif

void particleDelete(particle *const restrict p);

#endif
