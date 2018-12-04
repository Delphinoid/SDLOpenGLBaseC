#ifndef MODULETEXTURE_H
#define MODULETEXTURE_H

#include "texture.h"
#include "memoryPool.h"

#define RESOURCE_DEFAULT_TEXTURE_SIZE sizeof(texture)
#ifndef RESOURCE_DEFAULT_TEXTURE_NUM
	#define RESOURCE_DEFAULT_TEXTURE_NUM 4096
#endif

#ifndef RESOURCE_TEXTURE_CONSTANTS
	#define RESOURCE_TEXTURE_CONSTANTS 1
#endif
#define RESOURCE_TEXTURE_BLOCK_SIZE memPoolBlockSize(sizeof(texture))

extern memoryPool __TextureResourceArray;  // Contains textures.

/** Support locals? Merge all module containers? **/

return_t moduleTextureResourcesInit();
return_t moduleTextureResourcesInitConstants();
void moduleTextureResourcesReset();
void moduleTextureResourcesDelete();

texture *moduleTextureGetDefault();
texture *moduleTextureAllocateStatic();
texture *moduleTextureAllocate();
void moduleTextureFree(texture *resource);
texture *moduleTextureFind(const char *name);
void moduleTextureClear();

#endif
