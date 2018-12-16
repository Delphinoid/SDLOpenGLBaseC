#ifndef PHYSICSISLAND_H
#define PHYSICSISLAND_H

#include "physicsRigidBody.h"

typedef struct {

	/* Physics body storage. */
	physicsBodyIndex_t bodyNum;
	physicsBodyIndex_t bodyCapacity;
	physRBInstance **bodies;  /* An array of pointers to bodies. */

	/* Collision pair storage. */
	/*size_t pairNum;
	size_t pairCapacity;
	physCollisionPair *pairs;*/

} physIsland;

void physIslandInit(physIsland *const restrict island);
return_t physIslandAddBody(physIsland *const restrict island, physRBInstance *const prbi);
/*return_t physIslandAddObject(physIsland *const restrict island, objInstance *const restrict obji);*/
void physIslandUpdate(physIsland *const restrict island, const float dt);
void physIslandBroadPhase(physIsland *const restrict island, const float dt, physicsBodyIndex_t *const restrict pairArraySize, physRigidBody ***const restrict pairArray);
return_t physIslandSimulate(physIsland *const restrict island, const float dt);
void physIslandDelete(physIsland *const restrict island);

#endif
