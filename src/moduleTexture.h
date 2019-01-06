#ifndef MODULETEXTURE_H
#define MODULETEXTURE_H

#include "texture.h"
#include "memoryPool.h"

#define RESOURCE_DEFAULT_TEXTURE_SIZE sizeof(texture)
#define RESOURCE_DEFAULT_TEXTURE_NUM 4096

#define RESOURCE_TEXTURE_CONSTANTS 1
#define RESOURCE_TEXTURE_BLOCK_SIZE memPoolBlockSize(sizeof(texture))

// Forward declaration for inlining.
extern memoryPool __TextureResourceArray;  // Contains textures.

/** Support locals? Merge all module containers? **/

return_t moduleTextureResourcesInit();
return_t moduleTextureResourcesInitConstants();
void moduleTextureResourcesReset();
void moduleTextureResourcesDelete();

texture *moduleTextureGetDefault();
texture *moduleTextureAllocateStatic();
texture *moduleTextureAllocate();
void moduleTextureFree(texture *const restrict resource);
texture *moduleTextureFind(const char *const restrict name);
void moduleTextureClear();

#endif
