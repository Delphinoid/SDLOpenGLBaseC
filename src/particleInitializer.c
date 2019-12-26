#include "particleInitializer.h"
#include "particle.h"

/** TEMPORARY **/
#include <stdlib.h>
void particleInitializerSphereRandom(const void *const restrict initializer, particle *const restrict p){
	p->configuration.position.x = 2.f * (((float)rand())/((float)RAND_MAX) - 0.5f);
	p->configuration.position.y = 2.f * (((float)rand())/((float)RAND_MAX) - 0.5f);
	p->configuration.position.z = 2.f * (((float)rand())/((float)RAND_MAX) - 0.5f);
}