#include "physicsSolver.h"
#include "memoryManager.h"

return_t physSolverInit(physicsSolver *solver, size_t bodyNum){

	void *memory;

	solver->bodyNum = 0;
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
	solver->bodyNum = bodyNum;

	return 1;

}

void physSolverReset(physicsSolver *solver){

	/** Temporary: This will later be done during solving. **/

	memoryRegion *region = solver->bodies.region;
	physRBInstance **i;

	do {
		i = memPoolFirst(region);
		while(i < (physRBInstance **)memAllocatorEnd(region)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				memPoolFree(&solver->bodies, (void *)i);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			i = memPoolBlockNext(solver->bodies, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}

physRBInstance **physSolverAllocate(physicsSolver *solver){
	/*
	** Add a new body to the solver, resizing the
	** bodies array if necessary.
	*/
	physRBInstance **r = memPoolAllocate(&solver->bodies);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memPoolAllocationSize(NULL, sizeof(physRBInstance *), solver->bodyNum)
		);
		if(memPoolExtend(&solver->bodies, memory, sizeof(physRBInstance *), solver->bodyNum)){
			r = memPoolAllocate(&solver->bodies);
		}
	}
	if(r != NULL){
		++solver->bodyNum;
	}
	return r;
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

	memoryRegion *region1 = solver->bodies.region;
	memoryRegion *region2;
	physRBInstance **i;
	physRBInstance **j;

	cCollisionInfo separationInfo;
	cCollisionContactManifold collisionData;

	do {
		i = memPoolFirst(region1);
		while(i < (physRBInstance **)memAllocatorEnd(region1)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				region2 = region1;
				j = memPoolBlockNext(solver->bodies, i);
				for(;;){
					while(j < (physRBInstance **)memAllocatorEnd(region2)){
						const byte_t flag = memPoolBlockStatus(j);
						if(flag == MEMORY_POOL_BLOCK_ACTIVE){

							if(
								cCollision(
									&(*i)->colliders[0].c, &(*i)->centroid,
									&(*j)->colliders[0].c, &(*j)->centroid,
									&separationInfo, &collisionData
								)
							){
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

						}else if(flag == MEMORY_POOL_BLOCK_INVALID){
							goto PHYSICS_SOLVER_END_LOOP;
						}
						j = memPoolBlockNext(solver->bodies, j);
					}
					region2 = memAllocatorNext(region2);
					if(region2 == NULL){
						break;
					}
					j = memPoolFirst(region2);
				}


			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			PHYSICS_SOLVER_END_LOOP:
			i = memPoolBlockNext(solver->bodies, i);
		}
		region1 = memAllocatorNext(region1);
	} while(region1 != NULL);

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
	memoryRegion *region = solver->bodies.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}
