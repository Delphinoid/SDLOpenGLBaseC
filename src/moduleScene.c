#include "moduleScene.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

memoryList __SceneResourceArray;  // Contains scenes.

return_t moduleSceneResourcesInit(){
	void *memory = memAllocate(
		memListAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SCENE_SIZE,
			RESOURCE_DEFAULT_SCENE_NUM
		)
	);
	if(memListCreate(&__SceneResourceArray, memory, RESOURCE_DEFAULT_SCENE_SIZE, RESOURCE_DEFAULT_SCENE_NUM) == NULL){
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
	return memListAllocate(&__SceneResourceArray);
}
__FORCE_INLINE__ scene *moduleSceneAllocate(){
	scene *r = memListAllocate(&__SceneResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memListAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SCENE_SIZE,
				RESOURCE_DEFAULT_SCENE_NUM
			)
		);
		if(memListExtend(&__SceneResourceArray, memory, RESOURCE_DEFAULT_SCENE_SIZE, RESOURCE_DEFAULT_SCENE_NUM)){
			r = memListAllocate(&__SceneResourceArray);
		}
	}
	return r;
}
__FORCE_INLINE__ void moduleSceneFree(scene *resource){
	scnDelete(resource);
	memListFree(&__SceneResourceArray, (void *)resource);
}
void moduleSceneClear(){

	memoryRegion *region = __SceneResourceArray.region;
	scene *i;
	do {
		i = memListFirst(region);
		while(i < (scene *)memAllocatorEnd(region)){

			moduleSceneFree(i);
			i = memListBlockNext(__SceneResourceArray, i);

		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}

void moduleSceneUpdate(const float elapsedTime, const float dt){

	memoryRegion *region = __SceneResourceArray.region;
	scene *i;
	do {
		i = memListFirst(region);
		while(i < (scene *)memAllocatorEnd(region)){

			scnUpdate(i, elapsedTime, dt);
			i = memListBlockNext(__SceneResourceArray, i);

		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}