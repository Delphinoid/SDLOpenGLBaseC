#ifndef MODULETEXTUREWRAPPER_H
#define MODULETEXTUREWRAPPER_H

#include "moduleSettings.h"
#include "memoryManager.h"
#include "textureWrapper.h"

/** Support locals? Merge all module containers? **/

return_t moduleTextureWrapperResourcesInit();
void moduleTextureWrapperResourcesDelete();

textureWrapper *moduleTextureWrapperGetDefault();
textureWrapper *moduleTextureWrapperAllocate();
void moduleTextureWrapperFree(textureWrapper *resource);
textureWrapper *moduleTextureWrapperFind(const char *name);
void moduleTextureWrapperClear();

#endif
