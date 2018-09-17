#include "memoryDLink.h"

void memDLinkInit(memoryDLink *array){
	array->block = 0;
	array->free = NULL;
	array->region = NULL;
}

void *memDLinkCreate(memoryDLink *array, void *start, const size_t bytes, const size_t length){

	/*
	** Initialize an array allocator with "length"-many
	** elements of "bytes" size.
	*/

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

void *memDLinkAllocate(memoryDLink *array){

	/*
	** Retrieves a new block of memory from the array
	** allocator and updates the "free" pointer.
	** Unspecified behaviour with variable element sizes.
	*/

	byte_t *r = array->free;
	if(r){
		memDLinkDataGetNext(r) = NULL;
		memDLinkDataGetPrev(r) = NULL;
		array->free = memDLinkDataGetNextFree(r);
	}
	return r;

}

void *memDLinkPrepend(memoryDLink *array, void **start){

	/*
	** Prepends a new block to the array.
	*/

	byte_t *r = array->free;
	if(r){
		// Set the new element's pointers.
		memDLinkDataGetNext(r) = *start;
		memDLinkDataGetPrev(r) = NULL;
		// Set the next element's previous pointer.
		if(*start != NULL){
			memDLinkDataGetPrev(*start) = r;
		}
		// Set the beginning of the array.
		*start = r;
		array->free = memDLinkDataGetNextFree(r);
	}
	return r;

}

void *memDLinkAppend(memoryDLink *array, void **start){

	/*
	** Appends a new block to the array.
	*/

	byte_t *r = array->free;
	if(r){
		byte_t *last = NULL;
		byte_t **next = (byte_t **)start;
		while(*next != NULL){
			last = *next;
			next = memDLinkDataGetNextPointer(*next);
		}
		// Set the new element's pointers.
		memDLinkDataGetNext(r) = NULL;
		memDLinkDataGetPrev(r) = last;
		// Set the previous element's next pointer.
		*next = r;
		array->free = memDLinkDataGetNextFree(r);
	}
	return r;

}

void *memDLinkInsertBefore(memoryDLink *array, void *element){

	/*
	** Inserts a new item before the specified element.
	*/

	byte_t *r = array->free;
	if(r){
		byte_t **prev = memDLinkDataGetPrevPointer(element);
		// Set the new element's pointers.
		memDLinkDataGetNext(r) = element;
		memDLinkDataGetPrev(r) = *prev;
		if(*prev != NULL){
			// Set the previous element's next pointer.
			memDLinkDataGetNext(*prev) = r;
			// Set the next element's previous pointer.
			*prev = r;
		}
		array->free = memDLinkDataGetNextFree(r);
	}
	return r;

}

void *memDLinkInsertAfter(memoryDLink *array, void *element){

	/*
	** Inserts a new item after the specified element.
	*/

	byte_t *r = array->free;
	if(r){
		byte_t **next = memDLinkDataGetNextPointer(element);
		// Set the new element's pointers.
		memDLinkDataGetNext(r) = *next;
		memDLinkDataGetPrev(r) = element;
		if(*next != NULL){
			// Set the next element's previous pointer.
			memDLinkDataGetPrev(*next) = r;
			// Set the previous element's next pointer.
			*next = r;
		}
		array->free = memDLinkDataGetNextFree(r);
	}
	return r;

}

void memDLinkFree(memoryDLink *array, void *element){

	/*
	** Removes an element from an array
	** and frees the block.
	*/

	// Set the next element's previous pointer.
	memDLinkDataGetPrev(memDLinkDataGetNext(element)) = memDLinkDataGetPrev(element);
	// Set the previous element's next pointer.
	memDLinkDataGetNext(memDLinkDataGetPrev(element)) = memDLinkDataGetNext(element);

	memDLinkDataGetNext(element) = (byte_t *)MEMORY_DLINK_BLOCK_INACTIVE;
	memDLinkDataGetPrev(element) = (byte_t *)MEMORY_DLINK_BLOCK_INACTIVE;
	memDLinkDataGetNextFree(element) = array->free;
	array->free = element;

}

void *memDLinkSetupMemory(void *start, const size_t bytes, const size_t length){

	const size_t blockSize = memDLinkBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	byte_t *end;

	end = (byte_t *)start + memDLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion);
	start = memDLinkAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		memDLinkDataGetNext(block) = (byte_t *)MEMORY_DLINK_BLOCK_INACTIVE;
		memDLinkDataGetPrev(block) = (byte_t *)MEMORY_DLINK_BLOCK_INACTIVE;
		memDLinkDataGetNextFree(block) = next;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	memDLinkDataGetNext(block) = (byte_t *)MEMORY_DLINK_BLOCK_INACTIVE;
	memDLinkDataGetPrev(block) = (byte_t *)MEMORY_DLINK_BLOCK_INACTIVE;
	memDLinkDataGetNextFree(block) = NULL;

	return start;

}

void memDLinkClear(memoryDLink *array){

	byte_t *block = memDLinkAlignStartData(array->region->start);
	byte_t *next = block + array->block;

	array->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < memDLinkEnd(array)){
		memDLinkDataGetNext(block) = (byte_t *)MEMORY_DLINK_BLOCK_INACTIVE;
		memDLinkDataGetPrev(block) = (byte_t *)MEMORY_DLINK_BLOCK_INACTIVE;
		memDLinkDataGetNextFree(block) = next;
		block = next;
		next += array->block;
	}

	// Final block contains a null pointer.
	memDLinkDataGetNext(block) = (byte_t *)MEMORY_DLINK_BLOCK_INACTIVE;
	memDLinkDataGetPrev(block) = (byte_t *)MEMORY_DLINK_BLOCK_INACTIVE;
	memDLinkDataGetNextFree(block) = NULL;

}

void *memDLinkExtend(memoryDLink *array, void *start, const size_t bytes, const size_t length){

	/*
	** Extends the memory allocator.
	** Its logical function is similar to a
	** realloc, but it creates a new chunk
	** and links it.
	*/

	if(start){

		memoryRegion *newRegion = (memoryRegion *)((byte_t *)start + memDLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionAppend(&array->region, newRegion, start);

		memDLinkSetupMemory(start, bytes, length);
		array->free = memDLinkAlignStartData(start);

	}

	return start;

}
