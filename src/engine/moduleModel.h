#ifndef MODULEMODEL_H
#define MODULEMODEL_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "memoryPool.h"
#include "memorySLink.h"
#include "return.h"

#define RESOURCE_DEFAULT_MODEL_BASE_NUM 1024
#define RESOURCE_DEFAULT_MODEL_NUM 2048

// Forward declaration for inlining.
extern memoryPool __g_ModelBaseResourceArray;  // Contains modelBases.
extern memorySLink __g_ModelResourceArray;     // Contains models.

typedef struct modelBase modelBase;
typedef struct model model;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleModelResourcesInit();
void moduleModelResourcesReset();
void moduleModelResourcesDelete();

modelBase *moduleModelBaseAllocateStatic();
modelBase *moduleModelBaseAllocate();
void moduleModelBaseFree(modelBase *const __RESTRICT__ resource);
modelBase *moduleModelBaseFind(const char *const __RESTRICT__ name, const size_t nameLength);
void moduleModelBaseClear();

model *moduleModelAppendStatic(model **const __RESTRICT__ array);
model *moduleModelAppend(model **const __RESTRICT__ array);
model *moduleModelInsertAfterStatic(model **const __RESTRICT__ array, model *const __RESTRICT__ resource);
model *moduleModelInsertAfter(model **const __RESTRICT__ array, model *const __RESTRICT__ resource);
model *moduleModelNext(const model *const __RESTRICT__ i);
void moduleModelFree(model **const __RESTRICT__ array, model *resource, const model *const __RESTRICT__ previous);
void moduleModelFreeArray(model **const __RESTRICT__ array);
void moduleModelClear();

#endif
