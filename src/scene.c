#include "scene.h"
#include "memoryManager.h"
#include "inline.h"

return_t scnInit(scene *const restrict scn, size_t objectNum, size_t bodyNum){

	scn->objectNum = 0;
	memPoolInit(&scn->objects);

	if(objectNum == 0){
		objectNum = SCENE_ZONE_DEFAULT_OBJECT_NUM;
	}

	{
		void *const memory = memAllocate(
			memPoolAllocationSize(NULL, sizeof(objInstance *), objectNum)
		);
		if(memPoolCreate(&scn->objects, memory, sizeof(objInstance *), objectNum) == NULL){
			return -1;
		}
	}

	scn->objectNum = objectNum;
	return physSolverInit(&scn->solver, 0);

}

__FORCE_INLINE__ objInstance **scnAllocate(scene *const restrict scn){
	/** scn->objectNum is not correct here. We want fixed-size regions. **/
	objInstance **r = memPoolAllocate(&scn->objects);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
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

__FORCE_INLINE__ void scnFree(scene *const restrict scn, objInstance **const restrict obji){
	memPoolFree(&scn->objects, (void *)obji);
}
#include "moduleObject.h"
void scnUpdate(scene *const restrict scn, const float elapsedTime, const float dt){

	/*
	** Update each object in the scene.
	*/

	physSolverReset(&scn->solver);

	MEMORY_POOL_LOOP_BEGIN(scn->objects, i, objInstance **);

		// Update each object in the scene.
		objiUpdate(*i, &scn->solver, elapsedTime, dt);

	MEMORY_POOL_LOOP_END(scn->objects, i, goto UPDATE_PHYSICS_SOLVER;);

	UPDATE_PHYSICS_SOLVER:
	physSolverUpdate(&scn->solver);

}

void scnReset(scene *const restrict scn){

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

void scnDelete(scene *const restrict scn){

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
