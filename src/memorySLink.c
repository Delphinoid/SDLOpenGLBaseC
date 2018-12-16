#include "memorySLink.h"

void memSLinkInit(memorySLink *const restrict array){
	array->block = 0;
	array->free = NULL;
	array->region = NULL;
}

void *memSLinkCreate(memorySLink *const restrict array, void *const start, const size_t bytes, const size_t length){

	/*
	** Initialize an array allocator with "length"-many
	** elements of "bytes" size.
	*/

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

void *memSLinkAllocate(memorySLink *const restrict array){

	/*
	** Retrieves a new block of memory from the array
	** allocator and updates the "free" pointer.
	** Unspecified behaviour with variable element sizes.
	*/

	byte_t *const r = array->free;
	if(r){
		array->free = memSLinkDataGetNextFreeMasked(r);
		memSLinkDataGetNext(r) = NULL;
	}
	return r;

}

void *memSLinkPrepend(memorySLink *const restrict array, void **const restrict start){

	/*
	** Prepends a new block to the array.
	*/

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

void *memSLinkAppend(memorySLink *const restrict array, const void **const start){

	/*
	** Appends a new block to the array.
	*/

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

void *memSLinkInsertBefore(memorySLink *const restrict array, const void **const restrict start, void *const element, const void *const restrict previous){

	/*
	** Inserts a new item before the specified element.
	*/

	byte_t *const r = array->free;
	if(r){
		array->free = memSLinkDataGetNextFreeMasked(r);
		// Set the new element's next pointer.
		memSLinkDataGetNext(r) = element;
		if(previous != NULL){
			// Set the previous element's next pointer.
			memSLinkDataGetNext(previous) = r;
		}else{
			*start = r;
		}
	}
	return r;

}

void *memSLinkInsertAfter(memorySLink *const restrict array, void *const element){

	/*
	** Inserts a new item after the specified element.
	*/

	byte_t *const r = array->free;
	if(r){
		byte_t **next = memSLinkDataGetNextPointer(element);
		array->free = memSLinkDataGetNextFreeMasked(r);
		// Set the new element's next pointer.
		memSLinkDataGetNext(r) = *next;
		if(*next != NULL){
			// Set the previous element's next pointer.
			*next = r;
		}
	}
	return r;

}

void memSLinkFree(memorySLink *const restrict array, const void **const restrict start, void *const element, const void *const restrict previous){

	/*
	** Removes an element from an array
	** and frees the block.
	*/

	if(previous != NULL){
		// Set the previous element's next pointer.
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

void memSLinkClear(memorySLink *const restrict array){

	byte_t *block = memSLinkAlignStartData(array->region->start);
	byte_t *next = block + array->block;

	array->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (byte_t *)array->region){
		memSLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_SLINK_BLOCK_INVALID;
		block = next;
		next += array->block;
	}

	// Final block contains a null pointer.
	memSLinkDataGetFlags(block) = MEMORY_SLINK_BLOCK_INVALID;

}

void *memSLinkExtend(memorySLink *const restrict array, void *const start, const size_t bytes, const size_t length){

	/*
	** Extends the memory allocator.
	** Its logical function is similar to a
	** realloc, but it creates a new chunk
	** and links it.
	*/

	if(start){

		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memSLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionPrepend(&array->region, newRegion, start);

		memSLinkSetupMemory(start, bytes, length);
		array->free = memSLinkAlignStartData(start);

	}

	return start;

}

void memSLinkDelete(memorySLink *const restrict array){
	memRegionFree(array->region);
}
