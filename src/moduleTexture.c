#include "moduleTexture.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

memoryPool __TextureResourceArray;  // Contains textures.

return_t moduleTextureResourcesInit(){
	void *memory = memAllocate(
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

__FORCE_INLINE__ texture *moduleTextureGetDefault(){
	return memPoolFirst(__TextureResourceArray.region);
}
__FORCE_INLINE__ texture *moduleTextureAllocateStatic(){
	return memPoolAllocate(&__TextureResourceArray);
}
__FORCE_INLINE__ texture *moduleTextureAllocate(){
	texture *r = memPoolAllocate(&__TextureResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
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
__FORCE_INLINE__ void moduleTextureFree(texture *resource){
	tDelete(resource);
	memPoolFree(&__TextureResourceArray, (void *)resource);
}
texture *moduleTextureFind(const char *name){

	memoryRegion *region = __TextureResourceArray.region;
	texture *i;
	do {
		i = memPoolFirst(region);
		while(i < (texture *)memAllocatorEnd(region)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				// Compare the resources' names.
				if(strcmp(name, i->name) == 0){
					return i;
				}

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return NULL;
			}
			i = memPoolBlockNext(__TextureResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

	return NULL;

}
void moduleTextureClear(){

	memoryRegion *region = __TextureResourceArray.region;
	// Start after the constant resources.
	texture *i = (texture *)((byte_t *)memPoolFirst(region) + RESOURCE_TEXTURE_CONSTANTS * RESOURCE_TEXTURE_BLOCK_SIZE);
	for(;;){
		while(i < (texture *)memAllocatorEnd(region)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				moduleTextureFree(i);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			i = memPoolBlockNext(__TextureResourceArray, i);
		}
		region = memAllocatorNext(region);
		if(region == NULL){
			return;
		}
		i = memPoolFirst(region);
	}

}
