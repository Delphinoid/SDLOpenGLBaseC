#include "memoryStack.h"
#include "qualifiers.h"

void memStackInit(memoryStack *const __RESTRICT__ stack){
	stack->free = NULL;
	stack->region = NULL;
}

void *memStackCreate(memoryStack *const __RESTRICT__ stack, void *const start, const size_t bytes, const size_t length){
	if(start != NULL){
		stack->free = start;
		stack->region = (memoryRegion *)((byte_t *)start + memStackAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		stack->region->start = start;
		stack->region->next = NULL;
	}
	return start;
}

void *memStackPush(memoryStack *const __RESTRICT__ stack, const size_t bytes){
	byte_t *const r = stack->free;
	stack->free += bytes;
	if(stack->free > (byte_t *)stack->region){
		stack->free -= bytes;
		return NULL;
	}
	return r;
}

void memStackPop(memoryStack *const __RESTRICT__ stack, const size_t bytes){
	stack->free -= bytes;
	if(stack->free < stack->region->start){
		stack->free = stack->region->start;
	}
}

__FORCE_INLINE__ void memStackClear(memoryStack *const __RESTRICT__ stack){
	stack->free = stack->region->start;
}

void *memStackExtend(memoryStack *const __RESTRICT__ stack, void *const start, const size_t bytes, const size_t length){

	// Extends the memory allocator.
	// Its logical function is similar to a
	// realloc, but it creates a new chunk
	// and links it.

	if(start){

		// Place the memory region at the end of the chunk.
		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memStackAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionExtend(&stack->region, newRegion, start);

		stack->free = start;

	}

	return start;

}

void memStackDelete(memoryStack *const __RESTRICT__ stack){
	memRegionFree(stack->region);
}
