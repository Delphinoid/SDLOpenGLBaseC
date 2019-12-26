#include "particle.h"
#include "model.h"
#include <stdio.h>

void particleBaseInit(particleBase *const restrict base){
	rndrBaseInit(&base->rndr);
	base->billboardFlags = BILLBOARD_SPRITE;
	base->lifetime = 0.f;
	base->density = 0.f;
	base->friction = 1.f;
	base->restitution = 1.f;
	base->layers = 0;
}

void particleInit(particle *const restrict p){
	boneInit(&p->configuration);
	#ifdef PARTICLE_ALLOW_INTERPOLATION
	boneInit(&p->previous);
	#endif
	p->currentAnim = 0;
	animInstInit(&p->animator);
	vec3ZeroP(&p->velocity);
	p->lifetime = 0.f;
}

return_t particleBaseSetupBufferAttributes(const particleBase *const restrict base, const GLuint stateBufferID){

	GLenum glError;

	// Bind the vertex array object and set up the particle state attributes.
	glBindVertexArray(base->rndr.mdl->buffers.vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, stateBufferID);

	// Set up the vertex attributes.
	// A particle state contains a transformation matrix and some UVs.
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(particleState), (GLvoid *)offsetof(particleState, transformation.m[0]));
	glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(particleState), (GLvoid *)offsetof(particleState, transformation.m[1]));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(particleState), (GLvoid *)offsetof(particleState, transformation.m[2]));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(particleState), (GLvoid *)offsetof(particleState, transformation.m[3]));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(particleState), (GLvoid *)offsetof(particleState, frame));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error creating buffers: %u\n", glError);
		return 0;
	}
	return 1;

}

/** TEMPORARY **/
void particleTick(particle *const restrict p, const float elapsedTime){
	const vec3 delta = vec3VMultS(p->velocity, elapsedTime);
	#ifdef PARTICLE_ALLOW_INTERPOLATION
	p->previous = p->configuration;
	#endif
	p->configuration.position = vec3VAddV(p->configuration.position, delta);
}

#ifdef PARTICLE_ALLOW_INTERPOLATION
bone particleState(particle *const restrict p, const float interpT){
	return boneInterpolate(p->previous, p->configuration, interpT);
}
#endif

void particleDelete(particle *const restrict p){
	p->lifetime = 0.f;
}