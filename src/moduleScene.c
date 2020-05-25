#include "moduleScene.h"
#include "moduleSettings.h"
#include "scene.h"
#include "memoryManager.h"
#include <string.h>

#define RESOURCE_DEFAULT_SCENE_SIZE sizeof(scene)

memoryPool __g_SceneResourceArray;  // Contains scenes.

return_t moduleSceneResourcesInit(){
	void *const memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SCENE_SIZE,
			RESOURCE_DEFAULT_SCENE_NUM
		)
	);
	if(memPoolCreate(&__g_SceneResourceArray, memory, RESOURCE_DEFAULT_SCENE_SIZE, RESOURCE_DEFAULT_SCENE_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleSceneResourcesReset(){
	memoryRegion *region;
	moduleSceneClear();
	region = __g_SceneResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__g_SceneResourceArray.region->next = NULL;
}
void moduleSceneResourcesDelete(){
	memoryRegion *region;
	moduleSceneClear();
	region = __g_SceneResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__HINT_INLINE__ scene *moduleSceneAllocateStatic(){
	return memPoolAllocate(&__g_SceneResourceArray);
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
		if(memPoolExtend(&__g_SceneResourceArray, memory, RESOURCE_DEFAULT_SCENE_SIZE, RESOURCE_DEFAULT_SCENE_NUM)){
			r = moduleSceneAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void moduleSceneFree(scene *const __RESTRICT__ resource){
	scnDelete(resource);
	memPoolFree(&__g_SceneResourceArray, (void *)resource);
}
void moduleSceneClear(){

	MEMORY_POOL_LOOP_BEGIN(__g_SceneResourceArray, i, scene *);

		moduleSceneFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__g_SceneResourceArray, i, return;);

}

#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
void moduleSceneTick(const float elapsedTime, const float dt, const float frequency){
#else
void moduleSceneTick(const float elapsedTime, const float dt){
#endif

	MEMORY_POOL_LOOP_BEGIN(__g_SceneResourceArray, i, scene *);

		#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
		scnTick(i, elapsedTime, dt, frequency);
		#else
		scnTick(i, elapsedTime, dt);
		#endif

	MEMORY_POOL_LOOP_END(__g_SceneResourceArray, i, return;);

}