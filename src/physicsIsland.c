#include "physicsIsland.h"

/** Move and rename this when possible. Should implement collision pairing. **/

void physIslandInit(physIsland *island){
	island->bodyNum = 0;
	island->bodyCapacity = 0;
	island->bodies = NULL;
}

signed char physIslandAddBody(physIsland *island, physRBInstance *prbi){
	if(prbi->local != NULL && prbi->local->colliderNum){
		if(island->bodyNum >= island->bodyCapacity){
			/* Allocate room for more bodies. */
			physicsBodyIndex_t i;
			physicsBodyIndex_t tempCapacity;
			physRBInstance **tempBuffer;
			if(island->bodyCapacity == 0){
				tempCapacity = 1;
				tempBuffer = malloc(tempCapacity*sizeof(physRBInstance *));
			}else{
				tempCapacity = island->bodyCapacity*2;
				tempBuffer = realloc(island->bodies, tempCapacity*sizeof(physRBInstance *));
			}
			if(tempBuffer == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			island->bodyCapacity = tempCapacity;
			island->bodies = tempBuffer;
			for(i = island->bodyNum+1; i < island->bodyCapacity; ++i){
				island->bodies[i] = NULL;
			}
		}
		island->bodies[island->bodyNum] = prbi;
		++island->bodyNum;
		return 1;
	}
	return 0;
}

/*signed char physIslandAddObject(physIsland *island, objInstance *obji){
	if(obji->skeletonPhysics != NULL){
		size_t i;
		for(i = 0; i < obji->skl->boneNum; ++i){
			** Memory allocation failure. **
			if(physIslandAddBody(island, &obji->skeletonPhysics[i]) == -1){
				break;
			}
		}
		if(i < obji->skl->boneNum){
			** Memory allocation failure. **
			while(i > 0){
				--i;
				island->bodies[island->bodyNum+i] = NULL;
			}
			return -1;
		}
	}
	return 1;
}*/

void physIslandUpdate(physIsland *island, const float dt){

	physicsBodyIndex_t i;
	physicsBodyIndex_t del = 0;  // Number of bodies that have been deleted.

	for(i = 0; i < island->bodyNum; ++i){

		physicsBodyIndex_t index = i-del;
		island->bodies[index] = island->bodies[i];

		// Check if the body's owner has been deleted.
		if((island->bodies[index]->flags & PHYS_BODY_DELETE) > 0){

			// The body's owner has been deleted, free the body and
			// increase del so future bodies will be shifted over.
			physRBIDelete(island->bodies[index]);
			free(island->bodies[index]);
			++del;

		}else{

			// Integrate the body and solve constraints if desired.
			if((island->bodies[index]->flags & PHYS_BODY_SIMULATE) > 0){
				physRBIIntegrateEuler(island->bodies[index], dt);
			}

			// If the body can collide, update its collision mesh.
			if((island->bodies[index]->flags & PHYS_BODY_COLLIDE) > 0){
				physRBIUpdateCollisionMesh(island->bodies[index]);
			}

		}

	}

	// Decrease the array size by the number of bodies that were deleted.
	island->bodyNum -= del;

}

void physIslandBroadPhase(physIsland *island, const float dt, physicsBodyIndex_t *pairArraySize, physRigidBody ***pairArray){



}

signed char physIslandSimulate(physIsland *island, const float dt){

	//size_t pairArraySize = 0;
	//physRigidBody **pairArray = malloc(2*sizeof(physRigidBody));
	//pairArray[0] = NULL; pairArray[1] = NULL;

	/* Resolve deletions, integrate, solve constraints and update collision meshes. */
	//physIslandUpdate(island, dt);

	/* Broad-phase collision pair generation. */
	//physIslandBroadPhase(island, dt, &pairArraySize, &pairArray);

	/* Narrow-phase collision pair generation. */
	//

	/* Resolve collision pairs. */


	//free(pairArray);

	physicsBodyIndex_t i, j;
	hbCollisionInfo separationInfo;
	hbCollisionContactManifold collisionData;
	for(i = 0; i < island->bodyNum; ++i){
		for(j = i+1; j < island->bodyNum; ++j){
			if(hbCollision(&island->bodies[i]->colliders[0].hb, &island->bodies[i]->colliders[0].centroid,
			               &island->bodies[j]->colliders[0].hb, &island->bodies[j]->colliders[0].centroid,
			               &separationInfo, &collisionData)){
				//physRBIResolveCollision(island->bodies[i], island->bodies[j], &collisionData);
				//if(j==island->bodyNum-1){
					//island->bodies[i]->blah=1;
					//island->bodies[j]->blah=1;
					//exit(0);
				//}
			}
		}
	}

	return 1;

}

void physIslandDelete(physIsland *island){
	if(island->bodies != NULL){
		free(island->bodies);
	}
}
