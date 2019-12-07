#include "moduleModel.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

memoryPool __ModelResourceArray;  // Contains models.

return_t moduleModelResourcesInit(){
	void *memory;
	if(mdlDefaultInit() < 0 || mdlSpriteInit() < 0){
		return -1;
	}
	memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_MODEL_SIZE,
			RESOURCE_DEFAULT_MODEL_NUM
		)
	);
	if(memPoolCreate(&__ModelResourceArray, memory, RESOURCE_DEFAULT_MODEL_SIZE, RESOURCE_DEFAULT_MODEL_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleModelResourcesReset(){
	memoryRegion *region;
	moduleModelClear();
	region = __ModelResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__ModelResourceArray.region->next = NULL;
}
void moduleModelResourcesDelete(){
	memoryRegion *region;
	moduleModelClear();
	region = __ModelResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	mdlDelete(&mdlDefault);
	mdlDelete(&mdlSprite);
}

__HINT_INLINE__ model *moduleModelAllocateStatic(){
	return memPoolAllocate(&__ModelResourceArray);
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
		if(memPoolExtend(&__ModelResourceArray, memory, RESOURCE_DEFAULT_MODEL_SIZE, RESOURCE_DEFAULT_MODEL_NUM)){
			r = moduleModelAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void moduleModelFree(model *const restrict resource){
	mdlDelete(resource);
	memPoolFree(&__ModelResourceArray, (void *)resource);
}
model *moduleModelFind(const char *const restrict name){

	if(strcmp(name, mdlDefault.name) == 0){
		return &mdlDefault;
	}else if(strcmp(name, mdlSprite.name) == 0){
		return &mdlSprite;
	}

	MEMORY_POOL_LOOP_BEGIN(__ModelResourceArray, i, model *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__ModelResourceArray, i, return NULL;);

	return NULL;

}
void moduleModelClear(){

	MEMORY_POOL_LOOP_BEGIN(__ModelResourceArray, i, model *);

		moduleModelFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__ModelResourceArray, i, return;);

}
