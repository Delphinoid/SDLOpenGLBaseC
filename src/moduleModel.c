#include "moduleModel.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

#define RESOURCE_DEFAULT_MODEL_SIZE sizeof(model)
#ifndef RESOURCE_DEFAULT_MODEL_NUM
	#define RESOURCE_DEFAULT_MODEL_NUM 4096
#endif

#define RESOURCE_MODEL_CONSTANTS  2
#define RESOURCE_MODEL_BLOCK_SIZE memPoolBlockSize(sizeof(model))

memoryPool __ModelResourceArray;  // Contains models.

return_t moduleModelResourcesInit(){
	void *memory = memAllocate(
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

__FORCE_INLINE__ model *moduleModelGetDefault(){
	return memPoolFirst(__ModelResourceArray.region);
}
__FORCE_INLINE__ model *moduleModelGetSprite(){
	return (model *)((byte_t *)memPoolFirst(__ModelResourceArray.region) + RESOURCE_MODEL_BLOCK_SIZE);
}
__FORCE_INLINE__ model *moduleModelAllocateStatic(){
	return memPoolAllocate(&__ModelResourceArray);
}
__FORCE_INLINE__ model *moduleModelAllocate(){
	model *r = memPoolAllocate(&__ModelResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_MODEL_SIZE,
				RESOURCE_DEFAULT_MODEL_NUM
			)
		);
		if(memPoolExtend(&__ModelResourceArray, memory, RESOURCE_DEFAULT_MODEL_SIZE, RESOURCE_DEFAULT_MODEL_NUM)){
			r = memPoolAllocate(&__ModelResourceArray);
		}
	}
	return r;
}
__FORCE_INLINE__ void moduleModelFree(model *resource){
	memPoolFree(&__ModelResourceArray, (void *)resource);
}
model *moduleModelFind(const char *name){

	memoryRegion *region = __ModelResourceArray.region;
	model *i;
	do {
		i = memPoolFirst(region);
		while(i < (model *)memAllocatorEnd(region)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				// Compare the resources' names.
				if(strcmp(name, i->name) == 0){
					return i;
				}

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return NULL;
			}
			memPoolBlockNext(__ModelResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

	return NULL;

}
void moduleModelClear(){

	memoryRegion *region = __ModelResourceArray.region;
	// Start after the constant resources.
	model *i = (model *)((byte_t *)memPoolFirst(region) + RESOURCE_MODEL_CONSTANTS * RESOURCE_MODEL_BLOCK_SIZE);
	for(;;){
		while(i < (model *)memAllocatorEnd(region)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				mdlDelete(i);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			memPoolBlockNext(__ModelResourceArray, i);
		}
		region = memAllocatorNext(region);
		if(region == NULL){
			return;
		}
		i = memPoolFirst(region);
	}

}
