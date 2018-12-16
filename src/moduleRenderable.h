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

renderable *moduleRenderableAppendStatic(renderable **const restrict array);
renderable *moduleRenderableAppend(renderable **const restrict array);
renderable *moduleRenderableInsertAfterStatic(renderable *const restrict resource);
renderable *moduleRenderableInsertAfter(renderable *const restrict resource);
renderable *moduleRenderableNext(const renderable *const restrict i);
void moduleRenderableFree(renderable **const restrict array, renderable *const restrict resource, renderable *const restrict previous);
void moduleRenderableFreeArray(renderable **const restrict array);
void moduleRenderableClear();

rndrInstance *moduleRenderableInstanceAppendStatic(rndrInstance **const restrict array);
rndrInstance *moduleRenderableInstanceAppend(rndrInstance **const restrict array);
rndrInstance *moduleRenderableInstanceInsertAfterStatic(rndrInstance *const restrict resource);
rndrInstance *moduleRenderableInstanceInsertAfter(rndrInstance *const restrict resource);
rndrInstance *moduleRenderableInstanceNext(const rndrInstance *const restrict i);
void moduleRenderableInstanceFree(rndrInstance **const restrict array, rndrInstance *resource, rndrInstance *const restrict previous);
void moduleRenderableInstanceFreeArray(rndrInstance **const restrict array);
void moduleRenderableInstanceClear();

#endif
