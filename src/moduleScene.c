#include "moduleScene.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

memoryPool __SceneResourceArray;  // Contains scenes.

return_t moduleSceneResourcesInit(){
	void *const memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SCENE_SIZE,
			RESOURCE_DEFAULT_SCENE_NUM
		)
	);
	if(memPoolCreate(&__SceneResourceArray, memory, RESOURCE_DEFAULT_SCENE_SIZE, RESOURCE_DEFAULT_SCENE_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleSceneResourcesReset(){
	memoryRegion *region;
	moduleSceneClear();
	region = __SceneResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__SceneResourceArray.region->next = NULL;
}
void moduleSceneResourcesDelete(){
	memoryRegion *region;
	moduleSceneClear();
	region = __SceneResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__FORCE_INLINE__ scene *moduleSceneAllocateStatic(){
	return memPoolAllocate(&__SceneResourceArray);
}
__FORCE_INLINE__ scene *moduleSceneAllocate(){
	scene *r = memPoolAllocate(&__SceneResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SCENE_SIZE,
				RESOURCE_DEFAULT_SCENE_NUM
			)
		);
		if(memPoolExtend(&__SceneResourceArray, memory, RESOURCE_DEFAULT_SCENE_SIZE, RESOURCE_DEFAULT_SCENE_NUM)){
			r = memPoolAllocate(&__SceneResourceArray);
		}
	}
	return r;
}
__FORCE_INLINE__ void moduleSceneFree(scene *const restrict resource){
	scnDelete(resource);
	memPoolFree(&__SceneResourceArray, (void *)resource);
}
void moduleSceneClear(){

	MEMORY_POOL_LOOP_BEGIN(__SceneResourceArray, i, scene *);

		moduleSceneFree(i);

	MEMORY_POOL_LOOP_END(__SceneResourceArray, i, return;);

}

void moduleSceneUpdate(const float elapsedTime, const float dt){

	MEMORY_POOL_LOOP_BEGIN(__SceneResourceArray, i, scene *);

		scnUpdate(i, elapsedTime, dt);

	MEMORY_POOL_LOOP_END(__SceneResourceArray, i, return;);

}