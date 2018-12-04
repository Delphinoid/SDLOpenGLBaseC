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

__FORCE_INLINE__ renderable *moduleRenderableAppendStatic(renderable **array){
	return memSLinkAppend(&__RenderableResourceArray, (void **)array);
}
__FORCE_INLINE__ renderable *moduleRenderableAppend(renderable **array){
	renderable *r = memSLinkAppend(&__RenderableResourceArray, (void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
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
__FORCE_INLINE__ renderable *moduleRenderableInsertAfterStatic(renderable *resource){
	return memSLinkInsertAfter(&__RenderableResourceArray, resource);
}
__FORCE_INLINE__ renderable *moduleRenderableInsertAfter(renderable *resource){
	renderable *r = memSLinkInsertAfter(&__RenderableResourceArray, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RENDERABLE_SIZE,
				RESOURCE_DEFAULT_RENDERABLE_NUM
			)
		);
		if(memSLinkExtend(&__RenderableResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_SIZE, RESOURCE_DEFAULT_RENDERABLE_NUM)){
			r = memSLinkInsertAfter(&__RenderableResourceArray, resource);
		}
	}
	return r;
}
__FORCE_INLINE__ renderable *moduleRenderableNext(renderable *i){
	return (renderable *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void moduleRenderableFree(renderable **array, renderable *resource, renderable *previous){
	memSLinkFree(&__RenderableResourceArray, (void **)array, (void *)resource, (void *)previous);
}
void moduleRenderableFreeArray(renderable **array){
	renderable *resource = *array;
	while(resource != NULL){
		memSLinkFree(&__RenderableResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void moduleRenderableClear(){

	memoryRegion *region = __RenderableResourceArray.region;
	renderable *i;
	do {
		i = memSLinkFirst(region);
		while(i < (renderable *)memAllocatorEnd(region)){
			const byte_t flag = memSLinkBlockStatus(i);
			if(flag == MEMORY_SLINK_BLOCK_ACTIVE){

				moduleRenderableFree(NULL, i, NULL);

			}else if(flag == MEMORY_SLINK_BLOCK_INVALID){
				return;
			}
			i = memSLinkBlockNext(__RenderableResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}

__FORCE_INLINE__ rndrInstance *moduleRenderableInstanceAppendStatic(rndrInstance **array){
	return memSLinkAppend(&__RenderableInstanceResourceArray, (void **)array);
}
__FORCE_INLINE__ rndrInstance *moduleRenderableInstanceAppend(rndrInstance **array){
	rndrInstance *r = memSLinkAppend(&__RenderableInstanceResourceArray, (void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
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
__FORCE_INLINE__ rndrInstance *moduleRenderableInstanceInsertAfterStatic(rndrInstance *resource){
	return memSLinkInsertAfter(&__RenderableInstanceResourceArray, resource);
}
__FORCE_INLINE__ rndrInstance *moduleRenderableInstanceInsertAfter(rndrInstance *resource){
	rndrInstance *r = memSLinkInsertAfter(&__RenderableInstanceResourceArray, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE,
				RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM
			)
		);
		if(memSLinkExtend(&__RenderableInstanceResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE, RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM)){
			r = memSLinkInsertAfter(&__RenderableInstanceResourceArray, resource);
		}
	}
	return r;
}
__FORCE_INLINE__ rndrInstance *moduleRenderableInstanceNext(rndrInstance *i){
	return (rndrInstance *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void moduleRenderableInstanceFree(rndrInstance **array, rndrInstance *resource, rndrInstance *previous){
	memSLinkFree(&__RenderableInstanceResourceArray, (void **)array, (void *)resource, (void *)previous);
}
void moduleRenderableInstanceFreeArray(rndrInstance **array){
	rndrInstance *resource = *array;
	while(resource != NULL){
		memSLinkFree(&__RenderableInstanceResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void moduleRenderableInstanceClear(){

	memoryRegion *region = __RenderableInstanceResourceArray.region;
	rndrInstance *i;
	do {
		i = memSLinkFirst(region);
		while(i < (rndrInstance *)memAllocatorEnd(region)){
			const byte_t flag = memSLinkBlockStatus(i);
			if(flag == MEMORY_SLINK_BLOCK_ACTIVE){

				moduleRenderableInstanceFree(NULL, i, NULL);

			}else if(flag == MEMORY_SLINK_BLOCK_INVALID){
				return;
			}
			i = memSLinkBlockNext(__RenderableInstanceResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}