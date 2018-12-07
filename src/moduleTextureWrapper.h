#ifndef MODULETEXTUREWRAPPER_H
#define MODULETEXTUREWRAPPER_H

#include "textureWrapper.h"
#include "memoryPool.h"

#define RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE sizeof(textureWrapper)
#define RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM 4096

#define RESOURCE_TEXTURE_WRAPPER_CONSTANTS 1
#define RESOURCE_TEXTURE_WRAPPER_BLOCK_SIZE memPoolBlockSize(sizeof(textureWrapper))

extern memoryPool __TextureWrapperResourceArray;  // Contains textureWrappers.

/** Support locals? Merge all module containers? **/

return_t moduleTextureWrapperResourcesInit();
return_t moduleTextureWrapperResourcesInitConstants();
void moduleTextureWrapperResourcesReset();
void moduleTextureWrapperResourcesDelete();

textureWrapper *moduleTextureWrapperGetDefault();
textureWrapper *moduleTextureWrapperAllocateStatic();
textureWrapper *moduleTextureWrapperAllocate();
void moduleTextureWrapperFree(textureWrapper *resource);
textureWrapper *moduleTextureWrapperFind(const char *name);
void moduleTextureWrapperClear();

#endif
