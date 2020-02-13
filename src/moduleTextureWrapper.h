#ifndef MODULETEXTUREWRAPPER_H
#define MODULETEXTUREWRAPPER_H

#include "memoryPool.h"
#include "return.h"

#define RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM 4096
#define RESOURCE_TEXTURE_WRAPPER_CONSTANTS 1

// Forward declaration for inlining.
extern memoryPool __g_TextureWrapperResourceArray;  // Contains textureWrappers.

typedef struct textureWrapper textureWrapper;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleTextureWrapperResourcesInit();
void moduleTextureWrapperResourcesReset();
void moduleTextureWrapperResourcesDelete();

textureWrapper *moduleTextureWrapperAllocateStatic();
textureWrapper *moduleTextureWrapperAllocate();
void moduleTextureWrapperFree(textureWrapper *const __RESTRICT__ resource);
textureWrapper *moduleTextureWrapperFind(const char *const __RESTRICT__ name, const size_t nameLength);
void moduleTextureWrapperClear();

#endif
