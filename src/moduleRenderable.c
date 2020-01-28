#include "moduleRenderable.h"
#include "moduleSettings.h"
#include "renderable.h"
#include "memoryManager.h"
#include "inline.h"

#define RESOURCE_DEFAULT_RENDERABLE_BASE_SIZE sizeof(renderableBase)
#define RESOURCE_DEFAULT_RENDERABLE_SIZE sizeof(renderable)

memorySLink __g_RenderableBaseResourceArray;  // Contains renderable.
memorySLink __g_RenderableResourceArray;      // Contains rndrInstances.

return_t moduleRenderableResourcesInit(){
	void *memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RENDERABLE_BASE_SIZE,
			RESOURCE_DEFAULT_RENDERABLE_BASE_NUM
		)
	);
	if(memSLinkCreate(&__g_RenderableBaseResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_BASE_SIZE, RESOURCE_DEFAULT_RENDERABLE_BASE_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RENDERABLE_SIZE,
			RESOURCE_DEFAULT_RENDERABLE_NUM
		)
	);
	if(memSLinkCreate(&__g_RenderableResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_SIZE, RESOURCE_DEFAULT_RENDERABLE_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleRenderableResourcesReset(){
	memoryRegion *region;
	moduleRenderableBaseClear();
	region = __g_RenderableBaseResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_RenderableResourceArray.region->next = NULL;
	moduleRenderableClear();
	region = __g_RenderableResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_RenderableResourceArray.region->next = NULL;
}
void moduleRenderableResourcesDelete(){
	memoryRegion *region;
	moduleRenderableBaseClear();
	region = __g_RenderableBaseResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	moduleRenderableClear();
	region = __g_RenderableResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
}

__HINT_INLINE__ renderableBase *moduleRenderableBaseAppendStatic(renderableBase **const restrict array){
	return memSLinkAppend(&__g_RenderableBaseResourceArray, (void **)array);
}
__HINT_INLINE__ renderableBase *moduleRenderableBaseAppend(renderableBase **const restrict array){
	renderableBase *r = moduleRenderableBaseAppendStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RENDERABLE_BASE_SIZE,
				RESOURCE_DEFAULT_RENDERABLE_BASE_NUM
			)
		);
		if(memSLinkExtend(&__g_RenderableBaseResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_BASE_SIZE, RESOURCE_DEFAULT_RENDERABLE_BASE_NUM)){
			r = moduleRenderableBaseAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ renderableBase *moduleRenderableBaseInsertAfterStatic(renderableBase **const restrict array, renderableBase *const restrict resource){
	return memSLinkInsertAfter(&__g_RenderableBaseResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ renderableBase *moduleRenderableBaseInsertAfter(renderableBase **const restrict array, renderableBase *const restrict resource){
	renderableBase *r = moduleRenderableBaseInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RENDERABLE_BASE_SIZE,
				RESOURCE_DEFAULT_RENDERABLE_BASE_NUM
			)
		);
		if(memSLinkExtend(&__g_RenderableBaseResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_BASE_SIZE, RESOURCE_DEFAULT_RENDERABLE_BASE_NUM)){
			r = moduleRenderableBaseInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ renderableBase *moduleRenderableBaseNext(const renderableBase *const restrict i){
	return (renderableBase *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void moduleRenderableBaseFree(renderableBase **const restrict array, renderableBase *const restrict resource, const renderableBase *const restrict previous){
	memSLinkFree(&__g_RenderableBaseResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void moduleRenderableBaseFreeArray(renderableBase **const restrict array){
	renderableBase *resource = *array;
	while(resource != NULL){
		moduleRenderableBaseFree(array, resource, NULL);
		resource = *array;
	}
}
void moduleRenderableBaseClear(){

	MEMORY_SLINK_LOOP_BEGIN(__g_RenderableBaseResourceArray, i, renderableBase *);

		moduleRenderableBaseFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__g_RenderableBaseResourceArray, i, return;);

}

__HINT_INLINE__ renderable *moduleRenderableAppendStatic(renderable **const restrict array){
	return memSLinkAppend(&__g_RenderableResourceArray, (void **)array);
}
__HINT_INLINE__ renderable *moduleRenderableAppend(renderable **const restrict array){
	renderable *r = moduleRenderableAppendStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RENDERABLE_SIZE,
				RESOURCE_DEFAULT_RENDERABLE_NUM
			)
		);
		if(memSLinkExtend(&__g_RenderableResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_SIZE, RESOURCE_DEFAULT_RENDERABLE_NUM)){
			r = moduleRenderableAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ renderable *moduleRenderableInsertAfterStatic(renderable **const restrict array, renderable *const restrict resource){
	return memSLinkInsertAfter(&__g_RenderableResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ renderable *moduleRenderableInsertAfter(renderable **const restrict array, renderable *const restrict resource){
	renderable *r = moduleRenderableInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RENDERABLE_SIZE,
				RESOURCE_DEFAULT_RENDERABLE_NUM
			)
		);
		if(memSLinkExtend(&__g_RenderableResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_SIZE, RESOURCE_DEFAULT_RENDERABLE_NUM)){
			r = moduleRenderableInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ renderable *moduleRenderableNext(const renderable *const restrict i){
	return (renderable *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void moduleRenderableFree(renderable **const restrict array, renderable *const restrict resource, const renderable *const restrict previous){
	memSLinkFree(&__g_RenderableResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void moduleRenderableFreeArray(renderable **const restrict array){
	renderable *resource = *array;
	while(resource != NULL){
		moduleRenderableFree(array, resource, NULL);
		resource = *array;
	}
}
void moduleRenderableClear(){

	MEMORY_SLINK_LOOP_BEGIN(__g_RenderableResourceArray, i, renderable *);

		moduleRenderableFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__g_RenderableResourceArray, i, return;);

}