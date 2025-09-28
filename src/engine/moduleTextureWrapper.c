#include "settingsModule.h"
#include "moduleTextureWrapper.h"
#include "textureWrapper.h"
#include "memoryManager.h"
#include <string.h>

#define RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE sizeof(textureWrapper)

memoryPool __g_TextureWrapperResourceArray;  // Contains textureWrappers.

return_t moduleTextureWrapperResourcesInit(){
	void *const memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE,
			RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM
		)
	);
	if(memPoolCreate(&__g_TextureWrapperResourceArray, memory, RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE, RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleTextureWrapperResourcesReset(){
	memoryRegion *region;
	moduleTextureWrapperClear();
	region = __g_TextureWrapperResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__g_TextureWrapperResourceArray.region->next = NULL;
}
void moduleTextureWrapperResourcesDelete(){
	memoryRegion *region;
	moduleTextureWrapperClear();
	region = __g_TextureWrapperResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	twDelete(&g_twDefault);
}

__HINT_INLINE__ textureWrapper *moduleTextureWrapperAllocateStatic(){
	return memPoolAllocate(&__g_TextureWrapperResourceArray);
}
__HINT_INLINE__ textureWrapper *moduleTextureWrapperAllocate(){
	textureWrapper *r = memPoolAllocate(&__g_TextureWrapperResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE,
				RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM
			)
		);
		if(memPoolExtend(&__g_TextureWrapperResourceArray, memory, RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE, RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM)){
			r = memPoolAllocate(&__g_TextureWrapperResourceArray);
		}
	}
	return r;
}
__HINT_INLINE__ void moduleTextureWrapperFree(textureWrapper *const __RESTRICT__ resource){
	twDelete(resource);
	memPoolFree(&__g_TextureWrapperResourceArray, (void *)resource);
}
textureWrapper *moduleTextureWrapperFind(const char *const __RESTRICT__ name, const size_t nameLength){

	if(strncmp(name, g_twDefault.name, nameLength) == 0){
		return &g_twDefault;
	}

	MEMORY_POOL_LOOP_BEGIN(__g_TextureWrapperResourceArray, i, textureWrapper *);

		// Compare the resources' names.
		if(strncmp(name, i->name, nameLength) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__g_TextureWrapperResourceArray, i, return NULL;);

	return NULL;

}
void moduleTextureWrapperClear(){

	MEMORY_POOL_LOOP_BEGIN(__g_TextureWrapperResourceArray, i, textureWrapper *);

		moduleTextureWrapperFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__g_TextureWrapperResourceArray, i, return;);


}
