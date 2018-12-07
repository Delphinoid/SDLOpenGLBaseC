#ifndef MODULERENDERABLE_H
#define MODULERENDERABLE_H

#include "renderable.h"
#include "memorySLink.h"

#define RESOURCE_DEFAULT_RENDERABLE_SIZE sizeof(renderable)
#define RESOURCE_DEFAULT_RENDERABLE_NUM 1024

#define RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE sizeof(rndrInstance)
#define RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM 4096

extern memorySLink __RenderableResourceArray;          // Contains renderable.
extern memorySLink __RenderableInstanceResourceArray;  // Contains rndrInstances.

/** Support locals? Merge all module containers? **/

return_t moduleRenderableResourcesInit();
void moduleRenderableResourcesReset();
void moduleRenderableResourcesDelete();

renderable *moduleRenderableAppendStatic(renderable **array);
renderable *moduleRenderableAppend(renderable **array);
renderable *moduleRenderableInsertAfterStatic(renderable *resource);
renderable *moduleRenderableInsertAfter(renderable *resource);
renderable *moduleRenderableNext(renderable *i);
void moduleRenderableFree(renderable **array, renderable *resource, renderable *previous);
void moduleRenderableFreeArray(renderable **array);
void moduleRenderableClear();

rndrInstance *moduleRenderableInstanceAppendStatic(rndrInstance **array);
rndrInstance *moduleRenderableInstanceAppend(rndrInstance **array);
rndrInstance *moduleRenderableInstanceInsertAfterStatic(rndrInstance *resource);
rndrInstance *moduleRenderableInstanceInsertAfter(rndrInstance *resource);
rndrInstance *moduleRenderableInstanceNext(rndrInstance *i);
void moduleRenderableInstanceFree(rndrInstance **array, rndrInstance *resource, rndrInstance *previous);
void moduleRenderableInstanceFreeArray(rndrInstance **array);
void moduleRenderableInstanceClear();

#endif
