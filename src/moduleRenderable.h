#ifndef MODULERENDERABLE_H
#define MODULERENDERABLE_H

#include "renderable.h"
#include "memorySLink.h"

#define RESOURCE_DEFAULT_RENDERABLE_BASE_SIZE sizeof(renderableBase)
#define RESOURCE_DEFAULT_RENDERABLE_BASE_NUM 1024

#define RESOURCE_DEFAULT_RENDERABLE_SIZE sizeof(renderable)
#define RESOURCE_DEFAULT_RENDERABLE_NUM 4096

// Forward declarations for inlining.
extern memorySLink __RenderableResourceArray;          // Contains renderable.
extern memorySLink __RenderableInstanceResourceArray;  // Contains rndrInstances.

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleRenderableResourcesInit();
void moduleRenderableResourcesReset();
void moduleRenderableResourcesDelete();

renderableBase *moduleRenderableBaseAppendStatic(renderableBase **const restrict array);
renderableBase *moduleRenderableBaseAppend(renderableBase **const restrict array);
renderableBase *moduleRenderableBaseInsertAfterStatic(renderableBase **const restrict array, renderableBase *const restrict resource);
renderableBase *moduleRenderableBaseInsertAfter(renderableBase **const restrict array, renderableBase *const restrict resource);
renderableBase *moduleRenderableBaseNext(const renderableBase *const restrict i);
void moduleRenderableBaseFree(renderableBase **const restrict array, renderableBase *const restrict resource, const renderableBase *const restrict previous);
void moduleRenderableBaseFreeArray(renderableBase **const restrict array);
void moduleRenderableBaseClear();

renderable *moduleRenderableAppendStatic(renderable **const restrict array);
renderable *moduleRenderableAppend(renderable **const restrict array);
renderable *moduleRenderableInsertAfterStatic(renderable **const restrict array, renderable *const restrict resource);
renderable *moduleRenderableInsertAfter(renderable **const restrict array, renderable *const restrict resource);
renderable *moduleRenderableNext(const renderable *const restrict i);
void moduleRenderableFree(renderable **const restrict array, renderable *resource, const renderable *const restrict previous);
void moduleRenderableFreeArray(renderable **const restrict array);
void moduleRenderableClear();

#endif
