#ifndef MODULERENDERABLE_H
#define MODULERENDERABLE_H

#include "memorySLink.h"
#include "return.h"

#define RESOURCE_DEFAULT_RENDERABLE_BASE_NUM 1024
#define RESOURCE_DEFAULT_RENDERABLE_NUM 4096

// Forward declarations for inlining.
extern memorySLink __g_RenderableBaseResourceArray;  // Contains renderable.
extern memorySLink __g_RenderableResourceArray;      // Contains rndrInstances.

typedef struct renderableBase renderableBase;
typedef struct renderable renderable;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleRenderableResourcesInit();
void moduleRenderableResourcesReset();
void moduleRenderableResourcesDelete();

renderableBase *moduleRenderableBaseAppendStatic(renderableBase **const __RESTRICT__ array);
renderableBase *moduleRenderableBaseAppend(renderableBase **const __RESTRICT__ array);
renderableBase *moduleRenderableBaseInsertAfterStatic(renderableBase **const __RESTRICT__ array, renderableBase *const __RESTRICT__ resource);
renderableBase *moduleRenderableBaseInsertAfter(renderableBase **const __RESTRICT__ array, renderableBase *const __RESTRICT__ resource);
renderableBase *moduleRenderableBaseNext(const renderableBase *const __RESTRICT__ i);
void moduleRenderableBaseFree(renderableBase **const __RESTRICT__ array, renderableBase *const __RESTRICT__ resource, const renderableBase *const __RESTRICT__ previous);
void moduleRenderableBaseFreeArray(renderableBase **const __RESTRICT__ array);
void moduleRenderableBaseClear();

renderable *moduleRenderableAppendStatic(renderable **const __RESTRICT__ array);
renderable *moduleRenderableAppend(renderable **const __RESTRICT__ array);
renderable *moduleRenderableInsertAfterStatic(renderable **const __RESTRICT__ array, renderable *const __RESTRICT__ resource);
renderable *moduleRenderableInsertAfter(renderable **const __RESTRICT__ array, renderable *const __RESTRICT__ resource);
renderable *moduleRenderableNext(const renderable *const __RESTRICT__ i);
void moduleRenderableFree(renderable **const __RESTRICT__ array, renderable *resource, const renderable *const __RESTRICT__ previous);
void moduleRenderableFreeArray(renderable **const __RESTRICT__ array);
void moduleRenderableClear();

#endif
