#include "moduleTexture.h"
#include "moduleSettings.h"
#include "texture.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

#define RESOURCE_DEFAULT_TEXTURE_SIZE sizeof(texture)
#define RESOURCE_TEXTURE_BLOCK_SIZE memPoolBlockSize(sizeof(texture))

memoryPool __g_TextureResourceArray;  // Contains textures.

return_t moduleTextureResourcesInit(){
	void *memory;
	if(tDefaultInit() < 0){
		return -1;
	}
	memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_TEXTURE_SIZE,
			RESOURCE_DEFAULT_TEXTURE_NUM
		)
	);
	if(memPoolCreate(&__g_TextureResourceArray, memory, RESOURCE_DEFAULT_TEXTURE_SIZE, RESOURCE_DEFAULT_TEXTURE_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleTextureResourcesReset(){
	memoryRegion *region;
	moduleTextureClear();
	region = __g_TextureResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__g_TextureResourceArray.region->next = NULL;
}
void moduleTextureResourcesDelete(){
	memoryRegion *region;
	moduleTextureClear();
	region = __g_TextureResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	tDelete(&g_tDefault);
}

__HINT_INLINE__ texture *moduleTextureAllocateStatic(){
	return memPoolAllocate(&__g_TextureResourceArray);
}
__HINT_INLINE__ texture *moduleTextureAllocate(){
	texture *r = memPoolAllocate(&__g_TextureResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_TEXTURE_SIZE,
				RESOURCE_DEFAULT_TEXTURE_NUM
			)
		);
		if(memPoolExtend(&__g_TextureResourceArray, memory, RESOURCE_DEFAULT_TEXTURE_SIZE, RESOURCE_DEFAULT_TEXTURE_NUM)){
			r = memPoolAllocate(&__g_TextureResourceArray);
		}
	}
	return r;
}
__HINT_INLINE__ void moduleTextureFree(texture *const restrict resource){
	tDelete(resource);
	memPoolFree(&__g_TextureResourceArray, (void *)resource);
}
texture *moduleTextureFind(const char *const restrict name){

	if(strcmp(name, g_tDefault.name) == 0){
		return &g_tDefault;
	}

	MEMORY_POOL_LOOP_BEGIN(__g_TextureResourceArray, i, texture *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__g_TextureResourceArray, i, return NULL;);

	return NULL;

}
void moduleTextureClear(){

	MEMORY_POOL_LOOP_BEGIN(__g_TextureResourceArray, i, texture *);

		moduleTextureFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__g_TextureResourceArray, i, return;);

}
