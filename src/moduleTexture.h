#ifndef MODULETEXTURE_H
#define MODULETEXTURE_H

#include "memoryPool.h"
#include "return.h"

#define RESOURCE_DEFAULT_TEXTURE_NUM 4096
#define RESOURCE_TEXTURE_CONSTANTS 1

// Forward declaration for inlining.
extern memoryPool __g_TextureResourceArray;  // Contains textures.

typedef struct texture texture;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleTextureResourcesInit();
void moduleTextureResourcesReset();
void moduleTextureResourcesDelete();

texture *moduleTextureAllocateStatic();
texture *moduleTextureAllocate();
void moduleTextureFree(texture *const restrict resource);
texture *moduleTextureFind(const char *const restrict name);
void moduleTextureClear();

#endif
