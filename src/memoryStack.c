#include "memoryStack.h"
#include "inline.h"

void memStackInit(memoryStack *const restrict stack){
	stack->free = NULL;
	stack->region = NULL;
}

void *memStackCreate(memoryStack *const restrict stack, void *const start, const size_t bytes, const size_t length){
	if(start != NULL){
		stack->free = start;
		stack->region = (memoryRegion *)((byte_t *)start + memStackAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		stack->region->start = start;
		stack->region->next = NULL;
	}
	return start;
}

void *memStackPush(memoryStack *const restrict stack, const size_t bytes){
	byte_t *const r = stack->free;
	stack->free += bytes;
	if(stack->free > (byte_t *)stack->region){
		stack->free -= bytes;
		return NULL;
	}
	return r;
}

void memStackPop(memoryStack *const restrict stack, const size_t bytes){
	stack->free -= bytes;
	if(stack->free < stack->region->start){
		stack->free = stack->region->start;
	}
}

__FORCE_INLINE__ void memStackClear(memoryStack *const restrict stack){
	stack->free = stack->region->start;
}

void memStackDelete(memoryStack *const restrict stack){
	memRegionFree(stack->region);
}
