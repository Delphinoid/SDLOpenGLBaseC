#include "physicsIsland.h"
#include "memoryManager.h"

/** Move and rename this when possible. Should implement collision pairing. **/

void physIslandInit(physIsland *const restrict island){
	island->bodyNum = 0;
	island->bodyCapacity = 0;
	island->bodies = NULL;
}

return_t physIslandAddBody(physIsland *const restrict island, physRigidBody *const body){
	if(body->local != NULL){
		if(island->bodyNum >= island->bodyCapacity){
			// Allocate room for more bodies.
			physicsBodyIndex_t i;
			physicsBodyIndex_t tempCapacity;
			physRigidBody **tempBuffer;
			if(island->bodyCapacity == 0){
				tempCapacity = 1;
				tempBuffer = memAllocate(tempCapacity*sizeof(physRigidBody *));
			}else{
				tempCapacity = island->bodyCapacity*2;
				tempBuffer = memReallocate(island->bodies, tempCapacity*sizeof(physRigidBody *));
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
		island->bodies[island->bodyNum] = body;
		++island->bodyNum;
		return 1;
	}
	return 0;
}

/*return_t physIslandAddObject(physIsland *const restrict island, object *const restrict obj){
	if(obj->skeletonBodies != NULL){
		size_t i;
		for(i = 0; i < obj->skl->boneNum; ++i){
			** Memory allocation failure. **
			if(physIslandAddBody(island, &obj->skeletonBodies[i]) < 0){
				break;
			}
		}
		if(i < obj->skl->boneNum){
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

void physIslandUpdate(physIsland *const restrict island, const float dt){

	physicsBodyIndex_t i;
	physicsBodyIndex_t del = 0;  // Number of bodies that have been deleted.

	for(i = 0; i < island->bodyNum; ++i){

		physicsBodyIndex_t index = i-del;
		island->bodies[index] = island->bodies[i];

		// Check if the body's owner has been deleted.
		if(flagsAreSet(island->bodies[index]->flags, PHYSICS_BODY_DELETE)){

			// The body's owner has been deleted, free the body and
			// increase del so future bodies will be shifted over.
			physRigidBodyDelete(island->bodies[index]);
			memFree(island->bodies[index]);
			++del;

		}else{

			// Integrate the body and solve constraints if desired.
			if(flagsAreSet(island->bodies[index]->flags, PHYSICS_BODY_SIMULATE)){
				//physRigidBodyIntegrateEuler(island->bodies[index], dt);
			}

			// If the body can collide, update its collision mesh.
			if(flagsAreSet(island->bodies[index]->flags, PHYSICS_BODY_COLLIDE)){
				physRigidBodyUpdateCollisionMesh(island->bodies[index]);
			}

		}

	}

	// Decrease the array size by the number of bodies that were deleted.
	island->bodyNum -= del;

}

void physIslandBroadPhase(physIsland *const restrict island, const float dt, physicsBodyIndex_t *const restrict pairArraySize, physRigidBodyLocal ***const restrict pairArray){



}

return_t physIslandSimulate(physIsland *const restrict island, const float dt){

	//size_t pairArraySize = 0;
	//physRigidBody **pairArray = memAllocate(2*sizeof(physRigidBody));
	//pairArray[0] = NULL; pairArray[1] = NULL;

	// Resolve deletions, integrate, solve constraints and update collision meshes.
	//physIslandUpdate(island, dt);

	// Broad-phase collision pair generation.
	//physIslandBroadPhase(island, dt, &pairArraySize, &pairArray);

	// Narrow-phase collision pair generation.
	//

	// Resolve collision pairs.


	//memFree(pairArray);

	/*physicsBodyIndex_t i, j;
	cSeparationCache separationInfo;
	cContact collisionData;
	for(i = 0; i < island->bodyNum; ++i){
		for(j = i+1; j < island->bodyNum; ++j){
			if(cCollision(&island->bodies[i]->colliders[0].c, &island->bodies[i]->colliders[0].centroid,
			              &island->bodies[j]->colliders[0].c, &island->bodies[j]->colliders[0].centroid,
			              &separationInfo, &collisionData)){
				//physRigidBodyResolveCollision(island->bodies[i], island->bodies[j], &collisionData);
				//if(j==island->bodyNum-1){
					//island->bodies[i]->blah=1;
					//island->bodies[j]->blah=1;
					//exit(0);
				//}
			}
		}
	}*/

	return 1;

}

void physIslandDelete(physIsland *const restrict island){
	if(island->bodies != NULL){
		memFree(island->bodies);
	}
}
