#include "particleOperator.h"
#include "particle.h"

/** TEMPORARY (TECHNICALLY A FORCE) **/
void particleOperatorAddGravity(const void *const restrict operator, particle *const restrict p, const float elapsedTime){
	const vec3 force = {.x = 0.f, .y = -9.8f, .z = 0.f};
	const vec3 acceleration = vec3VMultS(force, elapsedTime);
	p->velocity = vec3VAddV(p->velocity, acceleration);
}