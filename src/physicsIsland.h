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

void physIslandInit(physIsland *island);
signed char physIslandAddBody(physIsland *island, physRBInstance *prbi);
/*signed char physIslandAddObject(physIsland *island, objInstance *obji);*/
void physIslandUpdate(physIsland *island, const float dt);
void physIslandBroadPhase(physIsland *island, const float dt, physicsBodyIndex_t *pairArraySize, physRigidBody ***pairArray);
signed char physIslandSimulate(physIsland *island, const float dt);
void physIslandDelete(physIsland *island);

#endif
