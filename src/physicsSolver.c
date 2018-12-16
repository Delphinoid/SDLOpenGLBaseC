#include "physicsSolver.h"
#include "memoryManager.h"

return_t physSolverInit(physicsSolver *const restrict solver, size_t bodyNum){

	solver->bodyNum = 0;
	memPoolInit(&solver->bodies);

	if(bodyNum == 0){
		bodyNum = PHYSICS_SOLVER_DEFAULT_BODY_NUM;
	}

	{
		void *const memory = memAllocate(
			memPoolAllocationSize(NULL, sizeof(physRBInstance *), bodyNum)
		);
		if(memPoolCreate(&solver->bodies, memory, sizeof(physRBInstance *), bodyNum) == NULL){
			return -1;
		}
	}

	solver->bodyNum = bodyNum;
	return 1;

}

void physSolverReset(physicsSolver *const restrict solver){

	/** Temporary: This will later be done during solving. **/

	MEMORY_POOL_LOOP_BEGIN(solver->bodies, i, void *);

		memPoolFree(&solver->bodies, i);

	MEMORY_POOL_LOOP_END(solver->bodies, i, return;);

}

physRBInstance **physSolverAllocate(physicsSolver *const restrict solver){
	/*
	** Add a new body to the solver, resizing the
	** bodies array if necessary.
	*/
	physRBInstance **r = memPoolAllocate(&solver->bodies);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
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

static void physSolverGenerateIslands(physicsSolver *const restrict solver){
	//
}

void physSolverUpdate(physicsSolver *const restrict solver){

	/*
	** Not 100% sure about this yet. Something
	** something dynamic AABB tree, something
	** something simulation islands.
	*/

	cCollisionInfo separationInfo;
	cCollisionContactManifold collisionData;

	MEMORY_POOL_LOOP_BEGIN(solver->bodies, i, const physRBInstance **);

		MEMORY_POOL_OFFSET_LOOP_BEGIN(
			solver->bodies, j, const physRBInstance **,
			__region_i, memPoolBlockNext(solver->bodies, i)
		);

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

		MEMORY_POOL_OFFSET_LOOP_END(solver->bodies, j, goto PHYSICS_SOLVER_END_LOOP;);

		PHYSICS_SOLVER_END_LOOP: ;

	MEMORY_POOL_LOOP_END(solver->bodies, i, return;);

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

void physSolverDelete(physicsSolver *const restrict solver){
	memoryRegion *region = solver->bodies.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}
