#include "moduleRenderable.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"

memorySLink __RenderableResourceArray;          // Contains renderable.
memorySLink __RenderableInstanceResourceArray;  // Contains rndrInstances.

return_t moduleRenderableResourcesInit(){
	void *memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RENDERABLE_SIZE,
			RESOURCE_DEFAULT_RENDERABLE_NUM
		)
	);
	if(memSLinkCreate(&__RenderableResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_SIZE, RESOURCE_DEFAULT_RENDERABLE_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE,
			RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM
		)
	);
	if(memSLinkCreate(&__RenderableInstanceResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE, RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleRenderableResourcesReset(){
	memoryRegion *region;
	moduleRenderableClear();
	region = __RenderableResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__RenderableResourceArray.region->next = NULL;
	moduleRenderableInstanceClear();
	region = __RenderableInstanceResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__RenderableInstanceResourceArray.region->next = NULL;
}
void moduleRenderableResourcesDelete(){
	memoryRegion *region;
	moduleRenderableClear();
	region = __RenderableResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	moduleRenderableInstanceClear();
	region = __RenderableInstanceResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
}

__HINT_INLINE__ renderable *moduleRenderableAppendStatic(renderable **const restrict array){
	return memSLinkAppend(&__RenderableResourceArray, (void **)array);
}
__HINT_INLINE__ renderable *moduleRenderableAppend(renderable **const restrict array){
	renderable *r = memSLinkAppend(&__RenderableResourceArray, (void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RENDERABLE_SIZE,
				RESOURCE_DEFAULT_RENDERABLE_NUM
			)
		);
		if(memSLinkExtend(&__RenderableResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_SIZE, RESOURCE_DEFAULT_RENDERABLE_NUM)){
			r = memSLinkAppend(&__RenderableResourceArray, (void **)array);
		}
	}
	return r;
}
__HINT_INLINE__ renderable *moduleRenderableInsertAfterStatic(renderable **const restrict array, renderable *const restrict resource){
	return memSLinkInsertAfter(&__RenderableResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ renderable *moduleRenderableInsertAfter(renderable **const restrict array, renderable *const restrict resource){
	renderable *r = memSLinkInsertAfter(&__RenderableResourceArray, (void **)array, (void *)resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RENDERABLE_SIZE,
				RESOURCE_DEFAULT_RENDERABLE_NUM
			)
		);
		if(memSLinkExtend(&__RenderableResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_SIZE, RESOURCE_DEFAULT_RENDERABLE_NUM)){
			r = memSLinkInsertAfter(&__RenderableResourceArray, (void **)array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ renderable *moduleRenderableNext(const renderable *const restrict i){
	return (renderable *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void moduleRenderableFree(renderable **const restrict array, renderable *const restrict resource, const renderable *const restrict previous){
	memSLinkFree(&__RenderableResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void moduleRenderableFreeArray(renderable **const restrict array){
	renderable *resource = *array;
	while(resource != NULL){
		memSLinkFree(&__RenderableResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void moduleRenderableClear(){

	MEMORY_SLINK_LOOP_BEGIN(__RenderableResourceArray, i, renderable *);

		moduleRenderableFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__RenderableResourceArray, i, return;);

}

__HINT_INLINE__ rndrInstance *moduleRenderableInstanceAppendStatic(rndrInstance **const restrict array){
	return memSLinkAppend(&__RenderableInstanceResourceArray, (void **)array);
}
__HINT_INLINE__ rndrInstance *moduleRenderableInstanceAppend(rndrInstance **const restrict array){
	rndrInstance *r = memSLinkAppend(&__RenderableInstanceResourceArray, (void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE,
				RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM
			)
		);
		if(memSLinkExtend(&__RenderableInstanceResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE, RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM)){
			r = memSLinkAppend(&__RenderableInstanceResourceArray, (void **)array);
		}
	}
	return r;
}
__HINT_INLINE__ rndrInstance *moduleRenderableInstanceInsertAfterStatic(rndrInstance **const restrict array, rndrInstance *const restrict resource){
	return memSLinkInsertAfter(&__RenderableInstanceResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ rndrInstance *moduleRenderableInstanceInsertAfter(rndrInstance **const restrict array, rndrInstance *const restrict resource){
	rndrInstance *r = memSLinkInsertAfter(&__RenderableInstanceResourceArray, (void **)array, (void *)resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE,
				RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM
			)
		);
		if(memSLinkExtend(&__RenderableInstanceResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE, RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM)){
			r = memSLinkInsertAfter(&__RenderableInstanceResourceArray, (void **)array, (void *)resource);
		}
	}
	return r;
}
__HINT_INLINE__ rndrInstance *moduleRenderableInstanceNext(const rndrInstance *const restrict i){
	return (rndrInstance *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void moduleRenderableInstanceFree(rndrInstance **const restrict array, rndrInstance *const restrict resource, const rndrInstance *const restrict previous){
	memSLinkFree(&__RenderableInstanceResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void moduleRenderableInstanceFreeArray(rndrInstance **const restrict array){
	rndrInstance *resource = *array;
	while(resource != NULL){
		memSLinkFree(&__RenderableInstanceResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void moduleRenderableInstanceClear(){

	MEMORY_SLINK_LOOP_BEGIN(__RenderableInstanceResourceArray, i, rndrInstance *);

		moduleRenderableInstanceFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__RenderableInstanceResourceArray, i, return;);

}