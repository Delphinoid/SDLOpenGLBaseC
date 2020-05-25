#include "memorySLink.h"

void memSLinkInit(memorySLink *const __RESTRICT__ array){
	array->block = 0;
	array->free = NULL;
	array->region = NULL;
}

void *memSLinkCreate(memorySLink *const __RESTRICT__ array, void *const start, const size_t bytes, const size_t length){

	// Initialize an array allocator with "length"-many
	// elements of "bytes" size.

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		array->block = memSLinkBlockSize(bytes);
		array->region = (memoryRegion *)((byte_t *)start + memSLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		array->region->start = start;
		array->region->next = NULL;

		memSLinkClear(array);

	}

	return start;

}

void *memSLinkCreateInit(memorySLink *const __RESTRICT__ array, void *const start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block)){

	// Initialize an array allocator with "length"-many
	// elements of "bytes" size.

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		array->block = memSLinkBlockSize(bytes);
		array->region = (memoryRegion *)((byte_t *)start + memSLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		array->region->start = start;
		array->region->next = NULL;

		memSLinkClearInit(array, func);

	}

	return start;

}

void *memSLinkAllocate(memorySLink *const __RESTRICT__ array){

	// Retrieves a new block of memory from the array
	// allocator and updates the "free" pointer.
	// Unspecified behaviour with variable element sizes.

	byte_t *const r = array->free;
	if(r){
		array->free = memSLinkDataGetNextFreeMasked(r);
		memSLinkDataGetNext(r) = NULL;
	}
	return r;

}

void *memSLinkPrepend(memorySLink *const __RESTRICT__ array, void **const start){

	// Prepends a new block to the array.

	byte_t *const r = array->free;
	if(r){
		array->free = memSLinkDataGetNextFreeMasked(r);
		// Set the new element's pointers.
		memSLinkDataGetNext(r) = *start;
		// Set the beginning of the array.
		*start = r;
	}
	return r;

}

void *memSLinkAppend(memorySLink *const __RESTRICT__ array, void **const start){

	// Appends a new block to the array.

	byte_t *const r = array->free;
	if(r){
		byte_t **next = (byte_t **)start;
		while(*next != NULL){
			next = memSLinkDataGetNextPointer(*next);
		}
		array->free = memSLinkDataGetNextFreeMasked(r);
		// Set the new element's pointers.
		memSLinkDataGetNext(r) = NULL;
		// Set the previous element's next pointer.
		*next = r;
	}
	return r;

}

void *memSLinkInsertBefore(memorySLink *const __RESTRICT__ array, void **const start, void *const element, const void *const previous){

	// Inserts a new item before the specified element.

	byte_t *const r = array->free;
	if(r){
		array->free = memSLinkDataGetNextFreeMasked(r);
		// Set the new element's next pointer.
		memSLinkDataGetNext(r) = element;
		if(previous == NULL){
			*start = r;
		}else{
			// Set the previous element's next pointer.
			memSLinkDataGetNext(previous) = r;
		}
	}
	return r;

}

void *memSLinkInsertAfter(memorySLink *const __RESTRICT__ array, void **const start, void *const element){

	// Inserts a new item after the specified element.

	byte_t *const r = array->free;
	if(r){
		array->free = memSLinkDataGetNextFreeMasked(r);
		if(element == NULL){
			memSLinkDataGetNext(r) = NULL;
			*start = r;
		}else{
			byte_t **next = memSLinkDataGetNextPointer(element);
			// Set the new element's next pointer.
			memSLinkDataGetNext(r) = *next;
			// Set the previous element's next pointer.
			*next = r;
		}
	}
	return r;

}

void memSLinkFree(memorySLink *const __RESTRICT__ array, void **const start, void *const element, const void *const previous){

	// Removes an element from an array
	// and frees the block.

	// Set the previous element's next pointer.
	if(previous != NULL){
		memSLinkDataGetNext(previous) = memSLinkDataGetNext(element);
	}else if(start != NULL){
		*start = memSLinkDataGetNext(element);
	}

	memSLinkDataGetFlags(element) = (uintptr_t)array->free | MEMORY_SLINK_BLOCK_INACTIVE;
	array->free = element;

}

void *memSLinkSetupMemory(void *start, const size_t bytes, const size_t length){

	const size_t blockSize = memSLinkBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	const byte_t *const end = (byte_t *)start + memSLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion);

	start = memSLinkAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		memSLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_SLINK_BLOCK_INVALID;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	memSLinkDataGetFlags(block) = MEMORY_SLINK_BLOCK_INVALID;

	return start;

}

void *memSLinkSetupMemoryInit(void *start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block)){

	const size_t blockSize = memSLinkBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	const byte_t *const end = (byte_t *)start + memSLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion);

	start = memSLinkAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		(*func)(block);
		memSLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_SLINK_BLOCK_INVALID;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	(*func)(block);
	memSLinkDataGetFlags(block) = MEMORY_SLINK_BLOCK_INVALID;

	return start;

}

void memSLinkClear(memorySLink *const __RESTRICT__ array){

	byte_t *block = memSLinkAlignStartData(array->region->start);
	byte_t *next = block + array->block;

	array->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (byte_t *)array->region && memSLinkBlockStatus(block) != MEMORY_SLINK_BLOCK_INVALID){
		memSLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_SLINK_BLOCK_INVALID;
		block = next;
		next += array->block;
	}

	// Final block contains a null pointer.
	memSLinkDataGetFlags(block) = MEMORY_SLINK_BLOCK_INVALID;

}

void memSLinkClearInit(memorySLink *const __RESTRICT__ array, void (*func)(void *const __RESTRICT__ block)){

	byte_t *block = memSLinkAlignStartData(array->region->start);
	byte_t *next = block + array->block;

	array->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (byte_t *)array->region && memSLinkBlockStatus(block) != MEMORY_SLINK_BLOCK_INVALID){
		(*func)(block);
		memSLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_SLINK_BLOCK_INVALID;
		block = next;
		next += array->block;
	}

	// Final block contains a null pointer.
	(*func)(block);
	memSLinkDataGetFlags(block) = MEMORY_SLINK_BLOCK_INVALID;

}

void *memSLinkExtend(memorySLink *const __RESTRICT__ array, void *const start, const size_t bytes, const size_t length){

	// Extends the memory allocator.
	// Its logical function is similar to a
	// realloc, but it creates a new chunk
	// and links it.

	if(start){

		// Place the memory region at the end of the chunk.
		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memSLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionPrepend(&array->region, newRegion, start);

		memSLinkSetupMemory(start, bytes, length);
		array->free = memSLinkAlignStartData(start);

	}

	return start;

}

void *memSLinkExtendInit(memorySLink *const __RESTRICT__ array, void *const start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block)){

	// Extends the memory allocator.
	// Its logical function is similar to a
	// realloc, but it creates a new chunk
	// and links it.

	if(start){

		// Place the memory region at the end of the chunk.
		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memSLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionPrepend(&array->region, newRegion, start);

		memSLinkSetupMemoryInit(start, bytes, length, func);
		array->free = memSLinkAlignStartData(start);

	}

	return start;

}

void memSLinkDelete(memorySLink *const __RESTRICT__ array){
	memRegionFree(array->region);
}
