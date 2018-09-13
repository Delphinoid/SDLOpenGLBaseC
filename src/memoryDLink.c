#include "memoryDLink.h"

void *memDLinkInit(memoryDLink *array, void *start, const size_t bytes, const size_t length){

	/*
	** Initialize an array allocator with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		array->block = memDLinkBlockSize(bytes);
		array->start = start;
		array->end = (byte_t *)start + memDLinkAllocationSize(start, bytes, length);

		memDLinkClear(array);

	}

	return start;

}

void *memDLinkAllocate(memoryDLink *array){

	/*
	** Retrieves a new block of memory from the array
	** allocator and updates the "next" pointer.
	** Unspecified behaviour with variable element sizes.
	*/

	byte_t *r = array->next;
	if(r){
		memDLinkDataGetNext(r) = r;
		memDLinkDataGetPrev(r) = r;
		array->next = memDLinkDataGetNextFree(r);
	}
	return r;

}

void *memDLinkInsertBefore(memoryDLink *array, void *element){

	/*
	** Inserts a new item before the specified element.
	*/

	byte_t *r = array->next;
	if(r){
		byte_t **prev = memDLinkDataGetPrevPointer(element);
		// Set the previous element's next pointer.
		memDLinkDataGetNext(*prev) = r;
		// Set the new element's pointers.
		memDLinkDataGetNext(r) = element;
		memDLinkDataGetPrev(r) = *prev;
		// Set the old element's pointers.
		*prev = r;
		array->next = memDLinkDataGetNextFree(r);
	}
	return r;

}

void *memDLinkInsertAfter(memoryDLink *array, void *element){

	/*
	** Inserts a new item after the specified element.
	*/

	byte_t *r = array->next;
	if(r){
		byte_t **next = memDLinkDataGetNextPointer(element);
		// Set the following element's previous pointer.
		memDLinkDataGetPrev(*next) = r;
		// Set the new element's pointers.
		memDLinkDataGetNext(r) = *next;
		memDLinkDataGetPrev(r) = element;
		// Set the old element's pointers.
		*next = r;
		array->next = memDLinkDataGetNextFree(r);
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

	memDLinkDataGetNext(element) = NULL;
	memDLinkDataGetPrev(element) = NULL;
	memDLinkDataGetNextFree(element) = array->next;
	array->next = element;

}

void *memDLinkReset(void *start, const size_t bytes, const size_t length){

	const size_t blockSize = memDLinkBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	byte_t *end;

	end = (byte_t *)start + memDLinkAllocationSize(start, bytes, length);
	start = memDLinkAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		memDLinkDataGetNext(block) = NULL;
		memDLinkDataGetPrev(block) = NULL;
		memDLinkDataGetNextFree(block) = next;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	memDLinkDataGetNext(block) = NULL;
	memDLinkDataGetPrev(block) = NULL;
	memDLinkDataGetNextFree(block) = NULL;

	return start;

}

void memDLinkClear(memoryDLink *array){

	byte_t *block = memDLinkAlignStartData(array->start);
	byte_t *next = block + array->block;

	array->next = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < array->end){
		memDLinkDataGetNext(block) = NULL;
		memDLinkDataGetPrev(block) = NULL;
		memDLinkDataGetNextFree(block) = next;
		block = next;
		next += array->block;
	}

	// Final block contains a null pointer.
	memDLinkDataGetNext(block) = NULL;
	memDLinkDataGetPrev(block) = NULL;
	memDLinkDataGetNextFree(block) = NULL;

}
