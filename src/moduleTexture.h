#ifndef MODULETEXTURE_H
#define MODULETEXTURE_H

#include "moduleSettings.h"
#include "memoryManager.h"
#include "texture.h"

/** Support locals? Merge all module containers? **/

return_t moduleTextureResourcesInit();
void moduleTextureResourcesDelete();

texture *moduleTextureGetDefault();
texture *moduleTextureAllocate();
void moduleTextureFree(texture *resource);
texture *moduleTextureFind(const char *name);
void moduleTextureClear();

#endif
