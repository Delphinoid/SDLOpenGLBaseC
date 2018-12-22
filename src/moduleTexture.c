#include "moduleTexture.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

memoryPool __TextureResourceArray;  // Contains textures.

return_t moduleTextureResourcesInit(){
	void *const memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_TEXTURE_SIZE,
			RESOURCE_DEFAULT_TEXTURE_NUM
		)
	);
	if(memPoolCreate(&__TextureResourceArray, memory, RESOURCE_DEFAULT_TEXTURE_SIZE, RESOURCE_DEFAULT_TEXTURE_NUM) == NULL){
		return -1;
	}
	return 1;
}
return_t moduleTextureResourcesInitConstants(){
	texture *tempTex = moduleTextureAllocate();
	if(tempTex == NULL){
		return -1;
	}
	tDefault(tempTex);
	return 1;
}
void moduleTextureResourcesReset(){
	memoryRegion *region;
	moduleTextureClear();
	region = __TextureResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__TextureResourceArray.region->next = NULL;
}
void moduleTextureResourcesDelete(){
	memoryRegion *region;
	tDelete(moduleTextureGetDefault());
	moduleTextureClear();
	region = __TextureResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__HINT_INLINE__ texture *moduleTextureGetDefault(){
	return memPoolFirst(__TextureResourceArray.region);
}
__HINT_INLINE__ texture *moduleTextureAllocateStatic(){
	return memPoolAllocate(&__TextureResourceArray);
}
__HINT_INLINE__ texture *moduleTextureAllocate(){
	texture *r = memPoolAllocate(&__TextureResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_TEXTURE_SIZE,
				RESOURCE_DEFAULT_TEXTURE_NUM
			)
		);
		if(memPoolExtend(&__TextureResourceArray, memory, RESOURCE_DEFAULT_TEXTURE_SIZE, RESOURCE_DEFAULT_TEXTURE_NUM)){
			r = memPoolAllocate(&__TextureResourceArray);
		}
	}
	return r;
}
__HINT_INLINE__ void moduleTextureFree(texture *const restrict resource){
	tDelete(resource);
	memPoolFree(&__TextureResourceArray, (void *)resource);
}
texture *moduleTextureFind(const char *const restrict name){

	MEMORY_POOL_LOOP_BEGIN(__TextureResourceArray, i, texture *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__TextureResourceArray, i, return NULL;);

	return NULL;

}
void moduleTextureClear(){

	MEMORY_POOL_OFFSET_LOOP_BEGIN(
		__TextureResourceArray, i, texture *,
		__TextureResourceArray.region,
		(byte_t *)memPoolFirst(__TextureResourceArray.region) + RESOURCE_TEXTURE_CONSTANTS * RESOURCE_TEXTURE_BLOCK_SIZE
	);

		moduleTextureFree(i);

	MEMORY_POOL_OFFSET_LOOP_END(__TextureResourceArray, i, return;);

}
