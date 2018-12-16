#include "moduleTextureWrapper.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

memoryPool __TextureWrapperResourceArray;  // Contains textureWrappers.

return_t moduleTextureWrapperResourcesInit(){
	void *const memory = memAllocate(
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
	__TextureWrapperResourceArray.region->next = NULL;
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
		void *const memory = memAllocate(
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
__FORCE_INLINE__ void moduleTextureWrapperFree(textureWrapper *const restrict resource){
	twDelete(resource);
	memPoolFree(&__TextureWrapperResourceArray, (void *)resource);
}
textureWrapper *moduleTextureWrapperFind(const char *const restrict name){

	MEMORY_POOL_LOOP_BEGIN(__TextureWrapperResourceArray, i, textureWrapper *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__TextureWrapperResourceArray, i, return NULL;);

	return NULL;

}
void moduleTextureWrapperClear(){

	MEMORY_POOL_OFFSET_LOOP_BEGIN(
		__TextureWrapperResourceArray, i, textureWrapper *,
		__TextureWrapperResourceArray.region,
		(byte_t *)memPoolFirst(__TextureWrapperResourceArray.region) + RESOURCE_TEXTURE_WRAPPER_CONSTANTS * RESOURCE_TEXTURE_WRAPPER_BLOCK_SIZE
	);

		moduleTextureWrapperFree(i);

	MEMORY_POOL_OFFSET_LOOP_END(__TextureWrapperResourceArray, i, return;);


}
