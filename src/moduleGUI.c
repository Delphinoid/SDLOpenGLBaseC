#include "moduleGUI.h"
#include "moduleSettings.h"
#include "gui.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

#define RESOURCE_DEFAULT_GUI_ELEMENT_SIZE sizeof(guiElement)

memorySLink __g_GUIElementResourceArray;  // Contains guiElements.

return_t moduleGUIResourcesInit(){
	void *const memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_GUI_ELEMENT_SIZE,
			RESOURCE_DEFAULT_GUI_ELEMENT_NUM
		)
	);
	if(memSLinkCreate(&__g_GUIElementResourceArray, memory, RESOURCE_DEFAULT_GUI_ELEMENT_SIZE, RESOURCE_DEFAULT_GUI_ELEMENT_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleGUIResourcesReset(){
	memoryRegion *region;
	moduleGUIElementClear();
	region = __g_GUIElementResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__g_GUIElementResourceArray.region->next = NULL;
}
void moduleGUIResourcesDelete(){
	memoryRegion *region;
	moduleGUIElementClear();
	region = __g_GUIElementResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__HINT_INLINE__ guiElement *moduleGUIElementAppendStatic(guiElement **const restrict array){
	return memSLinkAppend(&__g_GUIElementResourceArray, (void **)array);
}
__HINT_INLINE__ guiElement *moduleGUIElementAppend(guiElement **const restrict array){
	guiElement *r = moduleGUIElementAppendStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_GUI_ELEMENT_SIZE,
				RESOURCE_DEFAULT_GUI_ELEMENT_NUM
			)
		);
		if(memSLinkExtend(&__g_GUIElementResourceArray, memory, RESOURCE_DEFAULT_GUI_ELEMENT_SIZE, RESOURCE_DEFAULT_GUI_ELEMENT_NUM)){
			r = moduleGUIElementAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ guiElement *moduleGUIElementInsertAfterStatic(guiElement **const restrict array, guiElement *const restrict resource){
	return memSLinkInsertAfter(&__g_GUIElementResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ guiElement *moduleGUIElementInsertAfter(guiElement **const restrict array, guiElement *const restrict resource){
	guiElement *r = moduleGUIElementInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_GUI_ELEMENT_SIZE,
				RESOURCE_DEFAULT_GUI_ELEMENT_NUM
			)
		);
		if(memSLinkExtend(&__g_GUIElementResourceArray, memory, RESOURCE_DEFAULT_GUI_ELEMENT_SIZE, RESOURCE_DEFAULT_GUI_ELEMENT_NUM)){
			r = moduleGUIElementInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ guiElement *moduleGUIElementNext(const guiElement *const restrict i){
	return (guiElement *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void moduleGUIElementFree(guiElement **const restrict array, guiElement *const restrict resource, const guiElement *const restrict previous){
	guiElementDelete(resource);
	memSLinkFree(&__g_GUIElementResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void moduleGUIElementFreeArray(guiElement **const restrict array){
	guiElement *resource = *array;
	while(resource != NULL){
		moduleGUIElementFree(array, resource, NULL);
		resource = *array;
	}
}
void moduleGUIElementClear(){

	MEMORY_SLINK_LOOP_BEGIN(__g_GUIElementResourceArray, i, guiElement *);

		moduleGUIElementFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__g_GUIElementResourceArray, i, return;);

}