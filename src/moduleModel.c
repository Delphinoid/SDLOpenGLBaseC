#include "moduleModel.h"
#include "moduleSettings.h"
#include "model.h"
#include "sprite.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

#define RESOURCE_DEFAULT_MODEL_SIZE sizeof(model)
#define RESOURCE_MODEL_BLOCK_SIZE memPoolBlockSize(sizeof(model))

memoryPool __g_ModelResourceArray;  // Contains models.

return_t moduleModelResourcesInit(){
	void *memory;
	if(sprGenerateStateBuffer() < 0 || meshDefaultInit() < 0 || sprDefaultInit() < 0 || meshBillboardInit() < 0){
		return -1;
	}
	mdlDefaultInit(); mdlSpriteInit(); mdlBillboardInit();
	memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_MODEL_SIZE,
			RESOURCE_DEFAULT_MODEL_NUM
		)
	);
	if(memPoolCreate(&__g_ModelResourceArray, memory, RESOURCE_DEFAULT_MODEL_SIZE, RESOURCE_DEFAULT_MODEL_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleModelResourcesReset(){
	memoryRegion *region;
	moduleModelClear();
	region = __g_ModelResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__g_ModelResourceArray.region->next = NULL;
}
void moduleModelResourcesDelete(){
	memoryRegion *region;
	moduleModelClear();
	region = __g_ModelResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	mdlDelete(&g_mdlDefault);
	mdlDelete(&g_mdlSprite);
	mdlDelete(&g_mdlBillboard);
	sprDeleteStateBuffer();
}

__HINT_INLINE__ model *moduleModelAllocateStatic(){
	return memPoolAllocate(&__g_ModelResourceArray);
}
__HINT_INLINE__ model *moduleModelAllocate(){
	model *r = moduleModelAllocateStatic();
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_MODEL_SIZE,
				RESOURCE_DEFAULT_MODEL_NUM
			)
		);
		if(memPoolExtend(&__g_ModelResourceArray, memory, RESOURCE_DEFAULT_MODEL_SIZE, RESOURCE_DEFAULT_MODEL_NUM)){
			r = moduleModelAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void moduleModelFree(model *const restrict resource){
	mdlDelete(resource);
	memPoolFree(&__g_ModelResourceArray, (void *)resource);
}
model *moduleModelFind(const char *const restrict name){

	if(strcmp(name, g_mdlDefault.name) == 0){
		return &g_mdlDefault;
	}else if(strcmp(name, g_mdlSprite.name) == 0){
		return &g_mdlSprite;
	}

	MEMORY_POOL_LOOP_BEGIN(__g_ModelResourceArray, i, model *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__g_ModelResourceArray, i, return NULL;);

	return NULL;

}
void moduleModelClear(){

	MEMORY_POOL_LOOP_BEGIN(__g_ModelResourceArray, i, model *);

		moduleModelFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__g_ModelResourceArray, i, return;);

}
