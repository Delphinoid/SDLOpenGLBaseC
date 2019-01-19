#include "moduleModel.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

memoryPool __ModelResourceArray;  // Contains models.

return_t moduleModelResourcesInit(){
	void *const memory = memAllocate(
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
return_t moduleModelResourcesInitConstants(){
	model *tempMdl = moduleModelAllocateStatic();
	if(tempMdl == NULL){
		return -1;
	}
	mdlDefault(tempMdl);
	tempMdl = moduleModelAllocateStatic();
	if(tempMdl == NULL){
		return -1;
	}
	mdlCreateSprite(tempMdl);
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
	mdlDelete(moduleModelGetDefault());
	mdlDelete(moduleModelGetSprite());
	moduleModelClear();
	region = __ModelResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__HINT_INLINE__ model *moduleModelGetDefault(){
	return memPoolFirst(__ModelResourceArray.region);
}
__HINT_INLINE__ model *moduleModelGetSprite(){
	return (model *)((byte_t *)memPoolFirst(__ModelResourceArray.region) + RESOURCE_MODEL_BLOCK_SIZE);
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

	MEMORY_POOL_LOOP_BEGIN(__ModelResourceArray, i, model *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__ModelResourceArray, i, return NULL;);

	return NULL;

}
void moduleModelClear(){

	MEMORY_POOL_OFFSET_LOOP_BEGIN(
		__ModelResourceArray, i, model *,
		__ModelResourceArray.region,
		(byte_t *)memPoolFirst(__ModelResourceArray.region) + RESOURCE_MODEL_CONSTANTS * RESOURCE_MODEL_BLOCK_SIZE
	);

		moduleModelFree(i);

	MEMORY_POOL_OFFSET_LOOP_END(__ModelResourceArray, i, return;);

}
