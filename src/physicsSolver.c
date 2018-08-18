#include "physicsSolver.h"

void physSolverInit(physicsSolver *solver){
	solver->bodyNum = 0;
	solver->bodyCapacity = 0;
	solver->bodies = NULL;
}

void physSolverReset(physicsSolver *solver){
	/** Temporary: This will later be done during solving. **/
	physicsBodyIndex_t i;
	for(i = 0; i < solver->bodyCapacity; ++i){
		solver->bodies[i].active = solver->bodies[i].body != NULL;
	}
	solver->bodyNum = 0;
}

signed char physSolverAddBody(physicsSolver *solver, physRBInstance *body){
	/*
	** Add a new body to the solver, resizing the
	** bodies array if necessary.
	*/
	if(body->id >= solver->bodyCapacity){
		physicsBodyIndex_t i;
		for(i = 0; i < solver->bodyCapacity; ++i){
			if(!solver->bodies[i].active){
				break;
			}
		}
		body->id = i;
		if(body->id == solver->bodyCapacity){
			/*
			** The body array is full, double its size.
			** Increasing it by 1 may be better in the
			** long run, but would also probably lead
			** to more fragmentation, so who knows.
			*/
			physBodyReference *tempBuffer;
			if(solver->bodyCapacity == 0){
				solver->bodyCapacity = 1;
			}else{
				solver->bodyCapacity *= 2;
			}
			tempBuffer = realloc(solver->bodies, solver->bodyCapacity * sizeof(physBodyReference));
			if(tempBuffer == NULL){
				/** Memory allocation failure. **/
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
	return 1;
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

	physicsBodyIndex_t i, j;
	hbCollisionInfo separationInfo;
	hbCollisionContactManifold collisionData;
	for(i = 0; i < solver->bodyNum; i=solver->bodyNum){
		for(j = 2; j < solver->bodyNum; j=solver->bodyNum){
			if(hbCollision(&solver->bodies[i].body->colliders[0].hb, &solver->bodies[i].body->centroid,
			               &solver->bodies[j].body->colliders[0].hb, &solver->bodies[j].body->centroid,
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
	}

}

void physSolverDelete(physicsSolver *solver){
	//if(solver->bodyArraySizes != NULL){
		//free(solver->bodyArraySizes);
	//}
	if(solver->bodies != NULL){
		free(solver->bodies);
	}
}
