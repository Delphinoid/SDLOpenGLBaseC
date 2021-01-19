#include "moduleCommand.h"
#include "moduleSettings.h"
#include "command.h"
#include "memoryManager.h"
#include <string.h>

#define RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE sizeof(cmdTokenized)

memorySLink __g_CommandTokenizedResourceArray;  // Contains tokenized commands.

return_t moduleCommandResourcesInit(){
	void *const memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE,
			RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM
		)
	);
	if(memSLinkCreate(&__g_CommandTokenizedResourceArray, memory, RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE, RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM) == NULL){
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

__HINT_INLINE__ cmdTokenized *moduleCommandTokenizedInsertAfterStatic(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource){
	return memSLinkInsertAfter(&__g_CommandTokenizedResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ cmdTokenized *moduleCommandTokenizedInsertAfter(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource){
	cmdTokenized *r = moduleCommandTokenizedInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE,
				RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM
			)
		);
		if(memSLinkExtend(&__g_CommandTokenizedResourceArray, memory, RESOURCE_DEFAULT_COMMAND_TOKENIZED_SIZE, RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM)){
			r = moduleCommandTokenizedInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ cmdTokenized *moduleCommandTokenizedNext(const cmdTokenized *const __RESTRICT__ i){
	return (cmdTokenized *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void moduleCommandTokenizedFree(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource, const cmdTokenized *const __RESTRICT__ previous){
	memSLinkFree(&__g_CommandTokenizedResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void moduleCommandTokenizedFreeArray(cmdTokenized **const __RESTRICT__ array){
	cmdTokenized *resource = *array;
	while(resource != NULL){
		moduleCommandTokenizedFree(array, resource, NULL);
		resource = *array;
	}
}
void moduleCommandTokenizedClear(){

	MEMORY_SLINK_LOOP_BEGIN(__g_CommandTokenizedResourceArray, i, cmdTokenized *);

		moduleCommandTokenizedFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__g_CommandTokenizedResourceArray, i, return;);

}