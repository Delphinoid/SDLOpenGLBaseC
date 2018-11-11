#include "moduleTextureWrapper.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

#define RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE sizeof(textureWrapper)
#ifndef RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM
	#define RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM 4096
#endif

#define RESOURCE_TEXTURE_WRAPPER_CONSTANTS  1
#define RESOURCE_TEXTURE_WRAPPER_BLOCK_SIZE memPoolBlockSize(sizeof(textureWrapper))

memoryPool __TextureWrapperResourceArray;  // Contains textureWrappers.

return_t moduleTextureWrapperResourcesInit(){
	void *memory = memAllocate(
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
return_t moduleTextureWrapperResourcesInitConstants(){
	textureWrapper *tempTw = moduleTextureWrapperAllocate();
	if(tempTw == NULL){
		return -1;
	}
	twDefault(tempTw);
	return 1;
}
void moduleTextureWrapperResourcesReset(){
	memoryRegion *region;
	moduleTextureWrapperClear();
	region = __TextureWrapperResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}
void moduleTextureWrapperResourcesDelete(){
	memoryRegion *region;
	twDelete(moduleTextureWrapperGetDefault());
	moduleTextureWrapperClear();
	region = __TextureWrapperResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__FORCE_INLINE__ textureWrapper *moduleTextureWrapperGetDefault(){
	return memPoolFirst(__TextureWrapperResourceArray.region);
}
__FORCE_INLINE__ textureWrapper *moduleTextureWrapperAllocateStatic(){
	return memPoolAllocate(&__TextureWrapperResourceArray);
}
__FORCE_INLINE__ textureWrapper *moduleTextureWrapperAllocate(){
	textureWrapper *r = memPoolAllocate(&__TextureWrapperResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE,
				RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM
			)
		);
		if(memPoolExtend(&__TextureWrapperResourceArray, memory, RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE, RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM)){
			r = memPoolAllocate(&__TextureWrapperResourceArray);
		}
	}
	return r;
}
__FORCE_INLINE__ void moduleTextureWrapperFree(textureWrapper *resource){
	memPoolFree(&__TextureWrapperResourceArray, (void *)resource);
}
textureWrapper *moduleTextureWrapperFind(const char *name){

	memoryRegion *region = __TextureWrapperResourceArray.region;
	textureWrapper *i;
	do {
		i = memPoolFirst(region);
		while(i < (textureWrapper *)memAllocatorEnd(region)){
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
		region = memAllocatorNext(region);
	} while(region != NULL);

	return NULL;

}
void moduleTextureWrapperClear(){

	memoryRegion *region = __TextureWrapperResourceArray.region;
	textureWrapper *i = (textureWrapper *)((byte_t *)memPoolFirst(region) + RESOURCE_TEXTURE_WRAPPER_CONSTANTS * RESOURCE_TEXTURE_WRAPPER_BLOCK_SIZE);
	for(;;){
		while(i < (textureWrapper *)memAllocatorEnd(region)){
			byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				twDelete(i);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			memPoolBlockNext(__TextureWrapperResourceArray, i);
		}
		region = memAllocatorNext(region);
		if(region == NULL){
			return;
		}
		i = memPoolFirst(region);
	}

}
