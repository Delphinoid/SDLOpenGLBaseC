#ifndef MODULETEXTURE_H
#define MODULETEXTURE_H

#include "texture.h"
#include "memoryPool.h"

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
