#include "moduleInput.h"
#include "moduleSettings.h"
#include "inputManager.h"
#include "memoryManager.h"
#include <string.h>

#define RESOURCE_DEFAULT_INPUT_KEY_BINDING_SIZE sizeof(inputKeyBinding)

memorySLink __g_InputKeyBindingResourceArray;  // Contains cameras.

return_t moduleInputResourcesInit(){
	void *const memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_INPUT_KEY_BINDING_SIZE,
			RESOURCE_DEFAULT_INPUT_KEY_BINDING_NUM
		)
	);
	if(memSLinkCreate(&__g_InputKeyBindingResourceArray, memory, RESOURCE_DEFAULT_INPUT_KEY_BINDING_SIZE, RESOURCE_DEFAULT_INPUT_KEY_BINDING_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleInputResourcesReset(){
	memoryRegion *region;
	moduleInputKeyBindingClear();
	region = __g_InputKeyBindingResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__g_InputKeyBindingResourceArray.region->next = NULL;
}
void moduleInputResourcesDelete(){
	memoryRegion *region;
	moduleInputKeyBindingClear();
	region = __g_InputKeyBindingResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__HINT_INLINE__ inputKeyBinding *moduleInputKeyBindingPrependStatic(inputKeyBinding **const __RESTRICT__ array){
	return memSLinkPrepend(&__g_InputKeyBindingResourceArray, (void **)array);
}
__HINT_INLINE__ inputKeyBinding *moduleInputKeyBindingPrepend(inputKeyBinding **const __RESTRICT__ array){
	inputKeyBinding *r = moduleInputKeyBindingPrependStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_INPUT_KEY_BINDING_SIZE,
				RESOURCE_DEFAULT_INPUT_KEY_BINDING_NUM
			)
		);
		if(memSLinkExtend(&__g_InputKeyBindingResourceArray, memory, RESOURCE_DEFAULT_INPUT_KEY_BINDING_SIZE, RESOURCE_DEFAULT_INPUT_KEY_BINDING_NUM)){
			r = moduleInputKeyBindingPrependStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ inputKeyBinding *moduleInputKeyBindingNext(const inputKeyBinding *const __RESTRICT__ i){
	return (inputKeyBinding *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void moduleInputKeyBindingFree(inputKeyBinding **const __RESTRICT__ array, inputKeyBinding *const __RESTRICT__ resource, const inputKeyBinding *const __RESTRICT__ previous){
	memSLinkFree(&__g_InputKeyBindingResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void moduleInputKeyBindingFreeArray(inputKeyBinding **const __RESTRICT__ array){
	inputKeyBinding *resource = *array;
	while(resource != NULL){
		moduleInputKeyBindingFree(array, resource, NULL);
		resource = *array;
	}
}
void moduleInputKeyBindingClear(){

	MEMORY_SLINK_LOOP_BEGIN(__g_InputKeyBindingResourceArray, i, inputKeyBinding *);

		moduleInputKeyBindingFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__g_InputKeyBindingResourceArray, i, return;);

}