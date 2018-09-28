#ifndef MODULEMODEL_H
#define MODULEMODEL_H

#include "moduleSettings.h"
#include "memoryManager.h"
#include "model.h"

/** Support locals? Merge all module containers? **/

return_t moduleModelResourcesInit();
void moduleModelResourcesDelete();

model *moduleModelGetDefault();
model *moduleModelAllocate();
void moduleModelFree(model *resource);
model *moduleModelFind(const char *name);
void moduleModelClear();

#endif
