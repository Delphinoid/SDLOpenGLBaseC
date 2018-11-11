#ifndef MODULETEXTUREWRAPPER_H
#define MODULETEXTUREWRAPPER_H

#include "textureWrapper.h"
#include "memoryPool.h"

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
