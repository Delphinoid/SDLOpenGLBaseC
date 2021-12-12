#include "memoryDLink.h"

void memDLinkInit(memoryDLink *const __RESTRICT__ array){
	array->block = 0;
	array->free = NULL;
	array->region = NULL;
}

void *memDLinkCreate(memoryDLink *const __RESTRICT__ array, void *const start, const size_t bytes, const size_t length){

	// Initialize an array allocator with "length"-many
	// elements of "bytes" size.

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		array->block = memDLinkBlockSize(bytes);
		array->region = (memoryRegion *)((byte_t *)start + memDLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		array->region->start = start;
		array->region->next = NULL;

		memDLinkClear(array);

	}

	return start;

}

void *memDLinkCreateInit(memoryDLink *const __RESTRICT__ array, void *const start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block)){

	// Initialize an array allocator with "length"-many
	// elements of "bytes" size.

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		array->block = memDLinkBlockSize(bytes);
		array->region = (memoryRegion *)((byte_t *)start + memDLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		array->region->start = start;
		array->region->next = NULL;

		memDLinkClearInit(array, func);

	}

	return start;

}

void *memDLinkAllocate(memoryDLink *const __RESTRICT__ array){

	// Retrieves a new block of memory from the array
	// allocator and updates the "free" pointer.
	// Unspecified behaviour with variable element sizes.

	byte_t *const r = array->free;
	if(r){
		array->free = memDLinkDataGetNextFreeMasked(r);
		memDLinkDataGetNext(r) = NULL;
		memDLinkDataGetPrev(r) = NULL;
	}
	return r;

}

void *memDLinkPrepend(memoryDLink *const __RESTRICT__ array, void **const start){

	// Prepends a new block to the array.

	byte_t *const r = array->free;
	if(r){
		array->free = memDLinkDataGetNextFreeMasked(r);
		// Set the new element's pointers.
		memDLinkDataGetNext(r) = *start;
		memDLinkDataGetPrev(r) = NULL;
		// Set the next element's previous pointer.
		if(*start != NULL){
			memDLinkDataGetPrev(*start) = r;
		}
		// Set the beginning of the array.
		*start = r;
	}
	return r;

}

void *memDLinkAppend(memoryDLink *const __RESTRICT__ array, void **const start){

	// Appends a new block to the array.

	byte_t *const r = array->free;
	if(r){
		byte_t *last = NULL;
		byte_t **next = (byte_t **)start;
		while(*next != NULL){
			last = *next;
			next = memDLinkDataGetNextPointer(*next);
		}
		array->free = memDLinkDataGetNextFreeMasked(r);
		// Set the new element's pointers.
		memDLinkDataGetNext(r) = NULL;
		memDLinkDataGetPrev(r) = last;
		// Set the previous element's next pointer.
		*next = r;
	}
	return r;

}

void *memDLinkInsertBefore(memoryDLink *const __RESTRICT__ array, void **const start, void *const element){

	// Inserts a new item before the specified element.

	byte_t *const r = array->free;
	if(r){
		byte_t **const prev = memDLinkDataGetPrevPointer(element);
		array->free = memDLinkDataGetNextFreeMasked(r);
		// Set the new element's pointers.
		memDLinkDataGetNext(r) = element;
		memDLinkDataGetPrev(r) = *prev;
		if(*prev != NULL){
			// Set the previous element's next pointer.
			memDLinkDataGetNext(*prev) = r;
			// Set the next element's previous pointer.
			*prev = r;
		}else{
			*start = r;
		}
	}
	return r;

}

void *memDLinkInsertAfter(memoryDLink *const __RESTRICT__ array, void **const start, void *const element){

	// Inserts a new item after the specified element.

	byte_t *const r = array->free;
	if(r){
		if(element == NULL){
			if(*start == NULL){
				// Create a new list.
				array->free = memDLinkDataGetNextFreeMasked(r);
				memDLinkDataGetNext(r) = NULL;
				memDLinkDataGetPrev(r) = NULL;
				*start = r;
			}else{
				// Weird situation - we're inserting after a NULL element.
				// Append it to the end of the list instead, to avoid
				// losing the original list.
				memDLinkAppend(array, start);
			}
		}else{
			byte_t **const next = memDLinkDataGetNextPointer(element);
			array->free = memDLinkDataGetNextFreeMasked(r);
			// Set the new element's pointers.
			memDLinkDataGetNext(r) = *next;
			memDLinkDataGetPrev(r) = element;
			if(*next != NULL){
				// Set the next element's previous pointer.
				memDLinkDataGetPrev(*next) = r;
			}
			// Set the previous element's next pointer.
			*next = r;
		}
	}
	return r;

}

void memDLinkFree(memoryDLink *const __RESTRICT__ array, void **const start, void *const element){

	// Removes an element from an array
	// and frees the block.

	// Set the next element's previous pointer.
	if(memDLinkDataGetNext(element) != NULL){
		memDLinkDataGetPrev(memDLinkDataGetNext(element)) = memDLinkDataGetPrev(element);
	}
	// Set the previous element's next pointer.
	if(memDLinkDataGetPrev(element) != NULL){
		memDLinkDataGetNext(memDLinkDataGetPrev(element)) = memDLinkDataGetNext(element);
	}else if(start != NULL){
		*start = memDLinkDataGetNext(element);
	}

	memDLinkDataGetFlags(element) = (uintptr_t)array->free | MEMORY_DLINK_BLOCK_INACTIVE;
	array->free = element;

}

void *memDLinkSetupMemory(void *start, const size_t bytes, const size_t length){

	const size_t blockSize = memDLinkBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	const byte_t *const end = (const byte_t *)start + memDLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion);

	start = memDLinkAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		memDLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_DLINK_BLOCK_INVALID;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	memDLinkDataGetFlags(block) = MEMORY_DLINK_BLOCK_INVALID;

	return start;

}

void *memDLinkSetupMemoryInit(void *start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block)){

	const size_t blockSize = memDLinkBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	const byte_t *const end = (const byte_t *)start + memDLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion);

	start = memDLinkAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		(*func)(block);
		memDLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_DLINK_BLOCK_INVALID;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	(*func)(block);
	memDLinkDataGetFlags(block) = MEMORY_DLINK_BLOCK_INVALID;

	return start;

}

void memDLinkClear(memoryDLink *const __RESTRICT__ array){

	byte_t *block = memDLinkAlignStartData(array->region->start);
	byte_t *next = block + array->block;

	array->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (const byte_t *)array->region && memDLinkBlockStatus(block) != MEMORY_DLINK_BLOCK_INVALID){
		memDLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_DLINK_BLOCK_INVALID;
		block = next;
		next += array->block;
	}

	// Final block contains a null pointer.
	memDLinkDataGetFlags(block) = MEMORY_DLINK_BLOCK_INVALID;

}

void memDLinkClearInit(memoryDLink *const __RESTRICT__ array, void (*func)(void *const __RESTRICT__ block)){

	byte_t *block = memDLinkAlignStartData(array->region->start);
	byte_t *next = block + array->block;

	array->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (const byte_t *)array->region && memDLinkBlockStatus(block) != MEMORY_DLINK_BLOCK_INVALID){
		(*func)(block);
		memDLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_DLINK_BLOCK_INVALID;
		block = next;
		next += array->block;
	}

	// Final block contains a null pointer.
	(*func)(block);
	memDLinkDataGetFlags(block) = MEMORY_DLINK_BLOCK_INVALID;

}

void *memDLinkExtend(memoryDLink *const __RESTRICT__ array, void *const start, const size_t bytes, const size_t length){

	// Extends the memory allocator.
	// Its logical function is similar to a
	// realloc, but it creates a new chunk
	// and links it.

	if(start){

		// Place the memory region at the end of the chunk.
		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memDLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionPrepend(&array->region, newRegion, start);

		memDLinkSetupMemory(start, bytes, length);
		array->free = memDLinkAlignStartData(start);

	}

	return start;

}

void *memDLinkExtendInit(memoryDLink *const __RESTRICT__ array, void *const start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block)){

	// Extends the memory allocator.
	// Its logical function is similar to a
	// realloc, but it creates a new chunk
	// and links it.

	if(start){

		// Place the memory region at the end of the chunk.
		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memDLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		#if !defined(MEMORY_ALLOCATOR_USE_MALLOC) && !defined(_WIN32)
		newRegion->bytes = memDLinkAllocationSize(start, bytes, length);
		#endif
		memRegionPrepend(&array->region, newRegion, start);

		memDLinkSetupMemoryInit(start, bytes, length, func);
		array->free = memDLinkAlignStartData(start);

	}

	return start;

}

void memDLinkDelete(memoryDLink *const __RESTRICT__ array){
	memRegionFree(array->region);
}
