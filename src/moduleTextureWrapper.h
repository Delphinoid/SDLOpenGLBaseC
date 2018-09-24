#ifndef MODULETEXTUREWRAPPER_H
#define MODULETEXTUREWRAPPER_H

#include "moduleSettings.h"
#include "memoryManager.h"
#include "textureWrapper.h"

return_t moduleTextureWrapperInit();

textureWrapper *moduleTextureWrapperGetDefault();
textureWrapper *moduleTextureWrapperAllocate();
void moduleTextureWrapperFree(textureWrapper *resource);
textureWrapper *moduleTextureWrapperFind(const char *name);

#endif
