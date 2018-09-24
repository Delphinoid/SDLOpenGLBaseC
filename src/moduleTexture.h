#ifndef MODULETEXTURE_H
#define MODULETEXTURE_H

#include "moduleSettings.h"
#include "memoryManager.h"
#include "texture.h"

return_t moduleTextureInit();

texture *moduleTextureGetDefault();
texture *moduleTextureAllocate();
void moduleTextureFree(texture *resource);
texture *moduleTextureFind(const char *name);

#endif
