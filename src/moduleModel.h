#ifndef MODULEMODEL_H
#define MODULEMODEL_H

#include "model.h"
#include "memoryPool.h"

#define RESOURCE_DEFAULT_MODEL_SIZE sizeof(model)
#ifndef RESOURCE_DEFAULT_MODEL_NUM
	#define RESOURCE_DEFAULT_MODEL_NUM 4096
#endif

#ifndef RESOURCE_MODEL_CONSTANTS
	#define RESOURCE_MODEL_CONSTANTS 2
#endif
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
