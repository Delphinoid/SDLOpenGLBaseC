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
			memPoolAllocationSize(NULL, sizeof(object *), objectNum)
		);
		if(memPoolCreate(&scn->objects, memory, sizeof(object *), objectNum) == NULL){
			return -1;
		}
	}

	scn->objectNum = objectNum;
	physIslandInit(&scn->island);
	return 1;

}

__FORCE_INLINE__ object **scnAllocate(scene *const restrict scn){
	/** scn->objectNum is not correct here. We want fixed-size regions. **/
	object **r = memPoolAllocate(&scn->objects);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memPoolAllocationSize(NULL, sizeof(object *), scn->objectNum)
		);
		if(memPoolExtend(&scn->objects, memory, sizeof(object *), scn->objectNum)){
			r = memPoolAllocate(&scn->objects);
		}
	}
	if(r != NULL){
		++scn->objectNum;
	}
	return r;
}

__FORCE_INLINE__ void scnFree(scene *const restrict scn, object **const restrict obj){
	memPoolFree(&scn->objects, (void *)obj);
}
#include "moduleObject.h"
return_t scnUpdate(scene *const restrict scn, const float elapsedTime/**, const float dt**/){

	/*
	** Update each object in the scene.
	*/

	MEMORY_POOL_LOOP_BEGIN(scn->objects, i, object **);

		// Update each object in the scene.
		objUpdate(*i, &scn->island, elapsedTime);

	MEMORY_POOL_LOOP_END(scn->objects, i, return 1;);

	return 1;

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
	physIslandDelete(&scn->island);

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
	physIslandDelete(&scn->island);

}
