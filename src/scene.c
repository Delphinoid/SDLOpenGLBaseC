#include "scene.h"
#include "inline.h"

/**#define SCENE_START_CAPACITY 1

return_t scnInit(void *scn){
	((scene *)scn)->objectNum = 0;
	((scene *)scn)->objectCapacity = 0;
	((scene *)scn)->objectIDs = NULL;
	physSolverInit(&((scene *)scn)->solver);
	return 1;
}

return_t scnStateCopy(void *o, void *c){
	if(((scene *)c)->objectCapacity != ((scene *)o)->objectCapacity){
		*
		** We need to allocate more or less memory so that
		** the memory allocated for both scenes match.
		*
		objectIndex_t *tempBuffer = malloc(((scene *)o)->objectCapacity*sizeof(objectIndex_t));
		if(tempBuffer == NULL){
			** Memory allocation failure. **
			return -1;
		}
		free(((scene *)c)->objectIDs);
		((scene *)c)->objectIDs = tempBuffer;
		((scene *)c)->objectCapacity = ((scene *)o)->objectCapacity;
	}
	memcpy(((scene *)c)->objectIDs, ((scene *)o)->objectIDs, ((scene *)o)->objectNum*sizeof(objectIndex_t));
	((scene *)c)->objectNum = ((scene *)o)->objectNum;
	((scene *)c)->objectCapacity = ((scene *)o)->objectCapacity;
	// No point copying the physics solver.
	return 1;
}

void scnResetInterpolation(void *scn){
	//
}

return_t scnLoad(scene *scn){
	return 1;
}

return_t scnObjectAdd(scene *scn, const objectIndex_t objectID){
	if(scn->objectNum >= scn->objectCapacity){
		* Allocate room for more object IDs. *
		**if(scn->objectCapacity == 0){
			scn->objectCapacity = SCENE_START_CAPACITY;
		}**
		objectIndex_t tempCapacity;
		objectIndex_t *tempBuffer;
		if(scn->objectCapacity == 0){
			tempCapacity = 1;
			tempBuffer = malloc(tempCapacity*sizeof(objectIndex_t));
		}else{
			tempCapacity = scn->objectCapacity*2;
			tempBuffer = realloc(scn->objectIDs, tempCapacity*sizeof(objectIndex_t));
		}
		if(tempBuffer == NULL){
			** Memory allocation failure. **
			return -1;
		}
		scn->objectCapacity = tempCapacity;
		scn->objectIDs = tempBuffer;
	}
	scn->objectIDs[scn->objectNum] = objectID;
	++scn->objectNum;
	return 1;
}

void scnObjectRemove(scene *scn, stateManager *sm, const objectIndex_t pos){
	if(pos < scn->objectNum){
		smObjectDelete(sm, SM_TYPE_OBJECT, scn->objectIDs[pos]);
		--scn->objectNum;
		if(pos != scn->objectNum){
			memmove(&scn->objectIDs[pos], &scn->objectIDs[pos+1], scn->objectNum+1-pos);
		}
	}
}**/

return_t scnInit(scene *scn, const size_t objectNum){

	scn->objectNum = 0;
	memListInit(&scn->objects);
	physSolverInit(&scn->solver);

	if(objectNum > 0){

		void *memory = memAllocate(
			memListAllocationSize(NULL, sizeof(objInstance *), objectNum)
		);

		if(memListCreate(&scn->objects, memory, sizeof(objInstance *), objectNum) == NULL){
			return -1;
		}
		scn->objectNum = objectNum;

	}

	return 1;

}

__FORCE_INLINE__ objInstance **scnAllocate(scene *scn){
	/** scn->objectNum is not correct here. We want fixed-size regions. **/
	objInstance **r = memListAllocate(&scn->objects);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memListAllocationSize(NULL, sizeof(objInstance *), scn->objectNum)
		);
		if(memListExtend(&scn->objects, memory, sizeof(objInstance *), scn->objectNum)){
			r = memListAllocate(&scn->objects);
		}
	}
	return r;
}

__FORCE_INLINE__ void scnFree(scene *scn, objInstance **obji){
	memListFree(&scn->objects, (void *)obji);
}

void scnUpdate(scene *scn, const float elapsedTime, const float dt){

	/*
	** Update each object in the scene.
	*/

	if(scn->objectNum > 0){

		size_t objectsLeft = scn->objectNum;
		memoryRegion *region = scn->objects.region;
		objInstance **i;

		physSolverReset(&scn->solver);

		do {
			i = memListFirst(region);
			while(i < (objInstance **)memAllocatorEnd(region)){

				if(objectsLeft > 0){

					// Update each object in the scene.
					objInstance *obji = *i;
					if(obji != NULL){

						objiUpdate(obji, &scn->solver, elapsedTime, dt);
						--objectsLeft;

					}
					memListBlockNext(scn->objects, i);

				}else{
					goto UPDATE_PHYSICS_SOLVER;
				}

			}
			region = memAllocatorNext(region);
		} while(region != NULL);

		UPDATE_PHYSICS_SOLVER:
		physSolverUpdate(&scn->solver);

	}

}

void scnReset(scene *scn){

	/*
	** Free each of the scene's memory regions.
	*/

	memoryRegion *region = scn->objects.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	scn->objects.region->next = NULL;

	physSolverDelete(&scn->solver);

}

void scnDelete(scene *scn){

	/*
	** Free each of the scene's memory regions.
	*/

	memoryRegion *region = scn->objects.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	physSolverDelete(&scn->solver);

}
