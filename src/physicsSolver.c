#include "physicsSolver.h"
#include "memoryManager.h"

return_t physSolverInit(physicsSolver *solver, size_t bodyNum){

	void *memory;

	memPoolInit(&solver->bodies);

	if(bodyNum == 0){
		bodyNum = PHYSICS_SOLVER_DEFAULT_BODY_NUM;
	}

	memory = memAllocate(
		memPoolAllocationSize(NULL, sizeof(physRBInstance *), bodyNum)
	);

	if(memPoolCreate(&solver->bodies, memory, sizeof(physRBInstance *), bodyNum) == NULL){
		return -1;
	}

	return 1;

}

void physSolverReset(physicsSolver *solver){
	/** Temporary: This will later be done during solving. **/
	/**physicsBodyIndex_t i;
	for(i = 0; i < solver->bodyCapacity; ++i){
		solver->bodies[i].active = solver->bodies[i].body != NULL;
	}
	solver->bodyNum = 0;**/
}

return_t physSolverAddBody(physicsSolver *solver, physRBInstance *body){
	/*
	** Add a new body to the solver, resizing the
	** bodies array if necessary.
	*/
	/**if(body->id >= solver->bodyCapacity){
		physicsBodyIndex_t i;
		for(i = 0; i < solver->bodyCapacity; ++i){
			if(!solver->bodies[i].active){
				break;
			}
		}
		body->id = i;
		if(body->id == solver->bodyCapacity){
			*
			** The body array is full, double its size.
			** Increasing it by 1 may be better in the
			** long run, but would also probably lead
			** to more fragmentation, so who knows.
			*
			physBodyReference *tempBuffer;
			if(solver->bodyCapacity == 0){
				solver->bodyCapacity = 1;
			}else{
				solver->bodyCapacity *= 2;
			}
			tempBuffer = memReallocate(solver->bodies, solver->bodyCapacity * sizeof(physBodyReference));
			if(tempBuffer == NULL){
				** Memory allocation failure. **
				return -1;
			}
			// Set all the new elements to NULL.
			// Skip the first new element, as it
			// is being set later by the new body.
			++i;
			while(i < solver->bodyCapacity){
				tempBuffer[i].active = 0;
				++i;
			}
			solver->bodies = tempBuffer;
		}
		solver->bodies[body->id].active = 1;
	}
	solver->bodies[body->id].body = body;
	++solver->bodyNum;
	return 1;**/
}

static void physSolverGenerateIslands(physicsSolver *solver){
	//
}

void physSolverUpdate(physicsSolver *solver){

	/*
	** Not 100% sure about this yet. Something
	** something dynamic AABB tree, something
	** something simulation islands.
	*/

	/** TEMPORARY **/

	/**physicsBodyIndex_t i, j;
	cCollisionInfo separationInfo;
	cCollisionContactManifold collisionData;
	for(i = 0; i < solver->bodyNum; i=solver->bodyNum){
		for(j = 2; j < solver->bodyNum; j=solver->bodyNum){
			if(cCollision(&solver->bodies[i].body->colliders[0].c, &solver->bodies[i].body->centroid,
			              &solver->bodies[j].body->colliders[0].c, &solver->bodies[j].body->centroid,
			              &separationInfo, &collisionData)){
			    //
				//physRBIResolveCollision(island->bodies[i], island->bodies[j], &collisionData);
				//if(j==island->bodyNum-1){
					//island->bodies[i]->blah=1;
					//island->bodies[j]->blah=1;
					//exit(0);
				//}
			}else{
				//physRBICacheSeparation()
			}
		}
	}**/

}

void physSolverDelete(physicsSolver *solver){
	//if(solver->bodyArraySizes != NULL){
		//memFree(solver->bodyArraySizes);
	//}
	///if(solver->bodies != NULL){
	///	memFree(solver->bodies);
	///}
	memoryRegion *region = solver->bodies.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}
