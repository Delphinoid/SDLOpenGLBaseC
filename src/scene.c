#include "scene.h"
#include "memoryManager.h"
#include "inline.h"

return_t scnInit(scene *scn, size_t objectNum, size_t bodyNum){

	void *memory;

	scn->objectNum = 0;
	memPoolInit(&scn->objects);

	if(objectNum == 0){
		objectNum = SCENE_ZONE_DEFAULT_OBJECT_NUM;
	}

	memory = memAllocate(
		memPoolAllocationSize(NULL, sizeof(objInstance *), objectNum)
	);

	if(memPoolCreate(&scn->objects, memory, sizeof(objInstance *), objectNum) == NULL){
		return -1;
	}
	scn->objectNum = objectNum;

	return physSolverInit(&scn->solver, 0);

}

__FORCE_INLINE__ objInstance **scnAllocate(scene *scn){
	/** scn->objectNum is not correct here. We want fixed-size regions. **/
	objInstance **r = memPoolAllocate(&scn->objects);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memPoolAllocationSize(NULL, sizeof(objInstance *), scn->objectNum)
		);
		if(memPoolExtend(&scn->objects, memory, sizeof(objInstance *), scn->objectNum)){
			r = memPoolAllocate(&scn->objects);
		}
	}
	if(r != NULL){
		++scn->objectNum;
	}
	return r;
}

__FORCE_INLINE__ void scnFree(scene *scn, objInstance **obji){
	memPoolFree(&scn->objects, (void *)obji);
}
#include "moduleObject.h"
void scnUpdate(scene *scn, const float elapsedTime, const float dt){

	/*
	** Update each object in the scene.
	*/

	memoryRegion *region = scn->objects.region;
	objInstance **i;

	physSolverReset(&scn->solver);

	do {
		i = memPoolFirst(region);
		while(i < (objInstance **)memAllocatorEnd(region)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				// Update each object in the scene.
				objiUpdate(*i, &scn->solver, elapsedTime, dt);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				goto UPDATE_PHYSICS_SOLVER;
			}
			i = memPoolBlockNext(scn->objects, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

	UPDATE_PHYSICS_SOLVER:
	physSolverUpdate(&scn->solver);

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
