#include "physicsSolver.h"
#include "modulePhysics.h"
#include "memoryManager.h"

/**
*** NOTE: Eventually there will be no physics solver
*** class, all constraints will be solved in
*** modulePhysicsSolve() or some similar function.
**/

static void physSolverClear(memoryRegion *const region){

	/** Temporary. **/
	MEMORY_REGION_LOOP_BEGIN(region, i, physRBInstance **);
		*i = NULL;
	MEMORY_REGION_LOOP_END(i);

}

return_t physSolverInit(physicsSolver *const restrict solver, size_t bodyNum){

	solver->bodyNum = 0;

	if(bodyNum == 0){
		bodyNum = PHYSICS_SOLVER_DEFAULT_BODY_NUM;
	}

	{
		const size_t bytes = bodyNum * sizeof(physRBInstance *);
		void *const memory = memAllocate(bytes + sizeof(memoryRegion));
		if(memory == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		solver->bodies = (void *)((byte_t *)memory + bytes);
		solver->bodies->start = memory;
		solver->bodies->next = NULL;
		physSolverClear(solver->bodies);
	}

	solver->bodyNum = bodyNum;
	return 1;

}

void physSolverReset(physicsSolver *const restrict solver){

	/** Temporary: This will later be done during solving. **/
	MEMORY_REGION_LOOP_BEGIN(solver->bodies, i, physRBInstance **);
		*i = NULL;
	MEMORY_REGION_LOOP_END(i);

}

return_t physSolverAllocate(physicsSolver *const restrict solver, physRBInstance *const body){

	/*
	** Add a new body to the solver, resizing the
	** bodies array if necessary.
	*/

	/** Temporary. **/
	physicsBodyIndex_t id = 0;
	MEMORY_REGION_LOOP_BEGIN(solver->bodies, i, physRBInstance **);

		if(id == body->id || (*i == NULL || (*i)->id == (physicsBodyIndex_t)-1)){
			*i = body;
			body->id = id;
			return 1;
		}
		++id;

	MEMORY_REGION_LOOP_END(i);

	{
		const size_t bytes = solver->bodyNum * sizeof(physRBInstance *);
		void *const memory = memAllocate(bytes + sizeof(memoryRegion));
		if(memory == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		memRegionExtend(&solver->bodies, (memoryRegion *)((byte_t *)memory+bytes), memory);
		physSolverClear((memoryRegion *)((byte_t *)memory+bytes));
	}

	return 1;

}

static void physSolverGenerateIslands(physicsSolver *const restrict solver){
	//
}

return_t physSolverUpdate(physicsSolver *const restrict solver){

	/*
	** Not 100% sure about this yet. Something
	** something dynamic AABB tree, something
	** something simulation islands.
	*/

	/** Temporary. **/
	MEMORY_REGION_LOOP_BEGIN(solver->bodies, i, physRBInstance **);
		if(*i != NULL){

			MEMORY_REGION_OFFSET_LOOP_BEGIN(__region_i, j, physRBInstance **, i+1);
			if(*j != NULL){

					/**
					*** Move the below into one function.
					*** Handle bodies with multiple colliders.
					*** Make sure j is not constrained to i
					*** with a PHYSICS_CONSTRAINT_NO_COLLISION
					*** constraint.
					**/
					physContact contactContainer;
					physSeparation separationContainer;

					// The previous separation in the SLink,
					// used for insertions and deletions.
					physSeparation *previous;

					// Find the last frame's separation for
					// this pair, if it exists.
					physSeparation *last = physRBIFindSeparation(*i, (*j)->id, &previous);

					if(last != NULL){

						// Check the last frame's separation.
						if(
							cSeparation(
								&(*i)->colliders[0].c, &(*i)->centroid,
								&(*j)->colliders[0].c, &(*j)->centroid,
								&last->separation
							)
						){
							// The separation still exists, continue.
							continue;
						}

					}else{
						// The first body has no separation data
						// for the current collision in its cache.
						// Set up a new separation cache.
						last = &separationContainer;
						separationContainer.id = (*j)->id;
					}

					if(
						cCollision(
							&(*i)->colliders[0].c, &(*i)->centroid,
							&(*j)->colliders[0].c, &(*j)->centroid,
							&last->separation, &contactContainer.manifold
						)
					){

						// If a separation had been added on the
						// last frame, remove it from the cache.
						if(last != &separationContainer){
							physRBIRemoveSeparation(*i, last, previous);
						}

						// Set up the contact container and
						// add it to the contacts array.
						contactContainer.bodyA = *i;
						contactContainer.bodyB = *j;
						*modulePhysicsContactAllocate() = contactContainer;

					}else{

						// If the separation hasn't been added yet,
						// add it to the first body's cache.
						// If it has been added previously, it will
						// have been modified in cCollision.
						last = physRBICacheSeparation(*i, previous);
						if(last != NULL){
							*last = separationContainer;
						}else{
							/** Memory allocation failure. **/
							return -1;
						}

					}

				}
			MEMORY_REGION_LOOP_END(j);

		}
	MEMORY_REGION_LOOP_END(i);


	/**MEMORY_POOL_LOOP_BEGIN(solver->bodies, i, physRBInstance **);

		MEMORY_POOL_OFFSET_LOOP_BEGIN(
			solver->bodies, j, const physRBInstance **,
			__region_i, memPoolBlockNext(solver->bodies, i)
		);

			cContactManifold collisionData;
			physSeparation separationCache;

			// The previous separation in the SLink,
			// used for insertions and deletions.
			physSeparation *previous;

			// Find the last frame's separation for
			// this pair, if it exists.
			physSeparation *last = physRBIFindSeparation(*i, (*j)->id, &previous);

			cContactManifoldInit(&collisionData);
			if(last == NULL){
				// The first body has no separation data
				// for the current collision in its cache.
				// Set up pointers to a
				cSeparationCacheInit(&separationCache.cache);
				last = &separationCache;
				separationCache.id = (*j)->id;
			}

			if(
				cCollision(
					&(*i)->colliders[0].c, &(*i)->centroid,
					&(*j)->colliders[0].c, &(*j)->centroid,
					&last->cache, &collisionData
				)
			){

				if(last != &separationCache){
					// If a separation was added,
					// remove it from the cache.
					physRBIRemoveSeparation(*i, last, previous);
				}
				//
				//physRBIResolveCollision(island->bodies[i], island->bodies[j], &collisionData);
				//if(j==island->bodyNum-1){
					//island->bodies[i]->blah=1;
					//island->bodies[j]->blah=1;
					//exit(0);
				//}

			}else if(last == &separationCache){
				// If the separation hasn't been added yet,
				// add it to the first body's cache.
				// If it has been added previously, it will
				// have been modified in cCollision.
				last = physRBICacheSeparation(*i, previous);
				if(last != NULL){
					*last = separationCache;
				}else{
					** Memory allocation failure. **
					return -1;
				}
			}

		MEMORY_POOL_OFFSET_LOOP_END(solver->bodies, j, goto PHYSICS_SOLVER_END_LOOP;);

		PHYSICS_SOLVER_END_LOOP: ;

	MEMORY_POOL_LOOP_END(solver->bodies, i, return 1;);**/

	/** TEMPORARY **/

	/**physicsBodyIndex_t i, j;
	cSeparationCache separationInfo;
	cContactManifold collisionData;
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

	return 1;

}

void physSolverDelete(physicsSolver *const restrict solver){
	memoryRegion *region = solver->bodies;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}
