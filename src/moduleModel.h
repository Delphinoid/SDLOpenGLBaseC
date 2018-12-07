#ifndef MODULEMODEL_H
#define MODULEMODEL_H

#include "model.h"
#include "memoryPool.h"

#define RESOURCE_DEFAULT_MODEL_SIZE sizeof(model)
#define RESOURCE_DEFAULT_MODEL_NUM 4096

#define RESOURCE_MODEL_CONSTANTS 2
#define RESOURCE_MODEL_BLOCK_SIZE memPoolBlockSize(sizeof(model))

extern memoryPool __ModelResourceArray;  // Contains models.

/** Support locals? Merge all module containers? **/

return_t moduleModelResourcesInit();
return_t moduleModelResourcesInitConstants();
void moduleModelResourcesReset();
void moduleModelResourcesDelete();

model *moduleModelGetDefault();
model *moduleModelGetSprite();
model *moduleModelAllocateStatic();
model *moduleModelAllocate();
void moduleModelFree(model *resource);
model *moduleModelFind(const char *name);
void moduleModelClear();

#endif
