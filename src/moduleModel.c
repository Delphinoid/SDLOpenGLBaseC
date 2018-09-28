#include "moduleModel.h"
#include "memoryPool.h"
#include "inline.h"
#include <string.h>

#define RESOURCE_DEFAULT_MODEL_SIZE sizeof(model)
#ifndef RESOURCE_DEFAULT_MODEL_NUM
	#define RESOURCE_DEFAULT_MODEL_NUM 4096
#endif

static memoryPool __ModelResourceArray;  // Contains models.

return_t moduleModelResourcesInit(){
	void *memory = memForceAllocate(
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
void moduleModelResourcesDelete(){
	memoryRegion *region;
	moduleModelClear();
	region = __ModelResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
}

__FORCE_INLINE__ model *moduleModelGetDefault(){
	return memPoolFirst(__ModelResourceArray);
}
__FORCE_INLINE__ model *moduleModelAllocate(){
	return memPoolAllocate(&__ModelResourceArray);
}
__FORCE_INLINE__ void moduleModelFree(model *resource){
	memPoolFree(&__ModelResourceArray, (void *)resource);
}
model *moduleModelFind(const char *name){

	memoryRegion *region = __ModelResourceArray.region;
	do {
		model *i = memPoolFirst(__ModelResourceArray);
		while(i < (model *)memPoolEnd(__ModelResourceArray)){
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
		region = memPoolChunkNext(__ModelResourceArray);
	} while(region != NULL);

	return NULL;

}
void moduleModelClear(){

	memoryRegion *region = __ModelResourceArray.region;
	do {
		model *i = memPoolFirst(__ModelResourceArray);
		while(i < (model *)memPoolEnd(__ModelResourceArray)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				mdlDelete(i);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return NULL;
			}
			memPoolBlockNext(__ModelResourceArray, i);
		}
		region = memPoolChunkNext(__ModelResourceArray);
	} while(region != NULL);

}
