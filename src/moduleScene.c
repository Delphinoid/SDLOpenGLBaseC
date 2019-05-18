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

__HINT_INLINE__ scene *moduleSceneAllocateStatic(){
	return memPoolAllocate(&__SceneResourceArray);
}
__HINT_INLINE__ scene *moduleSceneAllocate(){
	scene *r = moduleSceneAllocateStatic();
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
			r = moduleSceneAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void moduleSceneFree(scene *const restrict resource){
	scnDelete(resource);
	memPoolFree(&__SceneResourceArray, (void *)resource);
}
void moduleSceneClear(){

	MEMORY_POOL_LOOP_BEGIN(__SceneResourceArray, i, scene *);

		moduleSceneFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__SceneResourceArray, i, return;);

}

#if !defined PHYSICS_GAUSS_SEIDEL_SOLVER || defined PHYSICS_FORCE_VELOCITY_BAUMGARTE
void moduleSceneQueryIslands(const float dt){
#else
void moduleSceneQueryIslands(){
#endif

	MEMORY_POOL_LOOP_BEGIN(__SceneResourceArray, i, scene *);

		#if !defined PHYSICS_GAUSS_SEIDEL_SOLVER || defined PHYSICS_FORCE_VELOCITY_BAUMGARTE
		physIslandQuery(&i->island, dt);
		#else
		physIslandQuery(&i->island);
		#endif

	MEMORY_POOL_LOOP_END(__SceneResourceArray, i, return;);

}

void moduleSceneUpdate(const float elapsedTime, const float dt){

	MEMORY_POOL_LOOP_BEGIN(__SceneResourceArray, i, scene *);

		scnUpdate(i, elapsedTime, dt);

	MEMORY_POOL_LOOP_END(__SceneResourceArray, i, return;);

}