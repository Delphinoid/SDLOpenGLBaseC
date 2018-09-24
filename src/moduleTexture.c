#include "moduleTexture.h"
#include "inline.h"

#define RESOURCE_DEFAULT_TEXTURE_SIZE sizeof(texture)
#ifndef RESOURCE_DEFAULT_TEXTURE_NUM
	#define RESOURCE_DEFAULT_TEXTURE_NUM 4096
#endif

static memoryPool __TextureResourceArray;  // Contains textures.

return_t moduleTextureInit(){
	void *memory = memForceAllocate(
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

__FORCE_INLINE__ texture *moduleTextureGetDefault(){
	return memPoolFirst(__TextureResourceArray);
}
__FORCE_INLINE__ texture *moduleTextureAllocate(){
	return memPoolAllocate(&__TextureResourceArray);
}
__FORCE_INLINE__ void moduleTextureFree(texture *resource){
	memPoolFree(&__TextureResourceArray, (void *)resource);
}
texture *moduleTextureFind(const char *name){

	memoryRegion *region = __TextureResourceArray.region;
	while(region != NULL){
		texture *i = memPoolFirst(__TextureResourceArray);
		while(i < (texture *)memPoolEnd(__TextureResourceArray)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_ACTIVE){

				// Compare the resources' names.
				if(strcmp(name, i->name) == 0){
					return i;
				}

			}else if(flag == MEMORY_POOL_INVALID){
				return NULL;
			}
			memPoolBlockNext(__TextureResourceArray, i);
		}
		region = memPoolChunkNext(__TextureResourceArray);
	}
	return NULL;

}
