#include "moduleTextureWrapper.h"
#include "memoryPool.h"
#include "inline.h"
#include <string.h>

#define RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE sizeof(textureWrapper)
#ifndef RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM
	#define RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM 4096
#endif

static memoryPool __TextureWrapperResourceArray;  // Contains textureWrappers.

return_t moduleTextureWrapperResourcesInit(){
	void *memory = memForceAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE,
			RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM
		)
	);
	if(memPoolCreate(&__TextureWrapperResourceArray, memory, RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE, RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleTextureWrapperResourcesDelete(){
	memoryRegion *region;
	moduleTextureWrapperClear();
	region = __TextureWrapperResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
}

__FORCE_INLINE__ textureWrapper *moduleTextureWrapperGetDefault(){
	return memPoolFirst(__TextureWrapperResourceArray);
}
__FORCE_INLINE__ textureWrapper *moduleTextureWrapperAllocate(){
	return memPoolAllocate(&__TextureWrapperResourceArray);
}
__FORCE_INLINE__ void moduleTextureWrapperFree(textureWrapper *resource){
	memPoolFree(&__TextureWrapperResourceArray, (void *)resource);
}
textureWrapper *moduleTextureWrapperFind(const char *name){

	memoryRegion *region = __TextureWrapperResourceArray.region;
	do {
		textureWrapper *i = memPoolFirst(__TextureWrapperResourceArray);
		while(i < (textureWrapper *)memPoolEnd(__TextureWrapperResourceArray)){
			byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				// Compare the resources' names.
				if(strcmp(name, i->name) == 0){
					return i;
				}

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return NULL;
			}
			memPoolBlockNext(__TextureWrapperResourceArray, i);
		}
		region = memPoolChunkNext(__TextureWrapperResourceArray);
	} while(region != NULL);

	return NULL;

}
void moduleTextureWrapperClear(){

	memoryRegion *region = __TextureWrapperResourceArray.region;
	do {
		textureWrapper *i = memPoolFirst(__TextureWrapperResourceArray);
		while(i < (textureWrapper *)memPoolEnd(__TextureWrapperResourceArray)){
			byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				twDelete(i);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			memPoolBlockNext(__TextureWrapperResourceArray, i);
		}
		region = memPoolChunkNext(__TextureWrapperResourceArray);
	} while(region != NULL);

}
