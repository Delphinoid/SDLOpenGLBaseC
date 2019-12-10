#ifndef MODULEMODEL_H
#define MODULEMODEL_H

#include "memoryPool.h"
#include "return.h"

#define RESOURCE_DEFAULT_MODEL_NUM 4096
#define RESOURCE_MODEL_CONSTANTS 2

// Forward declaration for inlining.
extern memoryPool __ModelResourceArray;  // Contains models.

typedef struct model model;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleModelResourcesInit();
void moduleModelResourcesReset();
void moduleModelResourcesDelete();

model *moduleModelAllocateStatic();
model *moduleModelAllocate();
void moduleModelFree(model *const restrict resource);
model *moduleModelFind(const char *const restrict name);
void moduleModelClear();

#endif
