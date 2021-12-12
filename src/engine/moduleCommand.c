#include "moduleCommand.h"
#include "moduleSettings.h"
#include "command.h"
#include "memoryManager.h"
#include <string.h>

#define RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE sizeof(cmdTokenized)

memoryDLink __g_CommandTokenizedResourceArray;  // Contains tokenized commands.

return_t moduleCommandResourcesInit(){
	void *const memory = memAllocate(
		memDLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE,
			RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM
		)
	);
	if(memDLinkCreate(&__g_CommandTokenizedResourceArray, memory, RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE, RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleCommandResourcesReset(){
	memoryRegion *region;
	moduleCommandTokenizedClear();
	region = __g_CommandTokenizedResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__g_CommandTokenizedResourceArray.region->next = NULL;
}
void moduleCommandResourcesDelete(){
	memoryRegion *region;
	moduleCommandTokenizedClear();
	region = __g_CommandTokenizedResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__HINT_INLINE__ cmdTokenized *moduleCommandTokenizedPrependStatic(cmdTokenized **const __RESTRICT__ array){
	return memDLinkPrepend(&__g_CommandTokenizedResourceArray, (void **)array);
}
__HINT_INLINE__ cmdTokenized *moduleCommandTokenizedPrepend(cmdTokenized **const __RESTRICT__ array){
	cmdTokenized *r = moduleCommandTokenizedPrependStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE,
				RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM
			)
		);
		if(memDLinkExtend(&__g_CommandTokenizedResourceArray, memory, RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE, RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM)){
			r = moduleCommandTokenizedPrependStatic(array);
		}
	}
	return r;
}

__HINT_INLINE__ cmdTokenized *moduleCommandTokenizedInsertAfterStatic(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource){
	return memDLinkInsertAfter(&__g_CommandTokenizedResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ cmdTokenized *moduleCommandTokenizedInsertAfter(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource){
	cmdTokenized *r = moduleCommandTokenizedInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE,
				RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM
			)
		);
		if(memDLinkExtend(&__g_CommandTokenizedResourceArray, memory, RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE, RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM)){
			r = moduleCommandTokenizedInsertAfterStatic(array, resource);
		}
	}
	return r;
}

__HINT_INLINE__ cmdTokenized *moduleCommandTokenizedInsertBeforeStatic(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource){
	return memDLinkInsertBefore(&__g_CommandTokenizedResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ cmdTokenized *moduleCommandTokenizedInsertBefore(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource){
	cmdTokenized *r = moduleCommandTokenizedInsertBeforeStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE,
				RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM
			)
		);
		if(memDLinkExtend(&__g_CommandTokenizedResourceArray, memory, RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE, RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM)){
			r = moduleCommandTokenizedInsertBeforeStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ cmdTokenized *moduleCommandTokenizedNext(const cmdTokenized *const __RESTRICT__ i){
	return (cmdTokenized *)memDLinkDataGetNext(i);
}
__HINT_INLINE__ cmdTokenized *moduleCommandTokenizedPrev(const cmdTokenized *const __RESTRICT__ i){
	return (cmdTokenized *)memDLinkDataGetPrev(i);
}
__HINT_INLINE__ void moduleCommandTokenizedFree(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource){
	memDLinkFree(&__g_CommandTokenizedResourceArray, (void **)array, (void *)resource);
}
void moduleCommandTokenizedFreeArray(cmdTokenized **const __RESTRICT__ array){
	cmdTokenized *resource = *array;
	while(resource != NULL){
		moduleCommandTokenizedFree(array, resource);
		resource = *array;
	}
}
void moduleCommandTokenizedClear(){

	MEMORY_DLINK_LOOP_BEGIN(__g_CommandTokenizedResourceArray, i, cmdTokenized *);

		moduleCommandTokenizedFree(NULL, i);
		memDLinkDataSetFlags(i, MEMORY_DLINK_BLOCK_INVALID);

	MEMORY_DLINK_LOOP_INACTIVE_CASE(i);

		memDLinkDataSetFlags(i, MEMORY_DLINK_BLOCK_INVALID);

	MEMORY_DLINK_LOOP_END(__g_CommandTokenizedResourceArray, i, return;);

}