#include "memorySLink.h"

void *memSLinkInit(memorySLink *array, void *start, const size_t bytes, const size_t length){

	/*
	** Initialize an array allocator with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		array->block = memSLinkBlockSize(bytes);
		array->start = start;
		array->end = (byte_t *)start + memSLinkAllocationSize(start, bytes, length);

		memSLinkClear(array);

	}

	return start;

}

void *memSLinkAllocate(memorySLink *array){

	/*
	** Retrieves a new block of memory from the array
	** allocator and updates the "next" pointer.
	** Unspecified behaviour with variable element sizes.
	*/

	byte_t *r = array->next;
	if(r){
		memSLinkDataGetNext(r) = NULL;
		array->next = memSLinkDataGetNextFree(r);
	}
	return r;

}

void *memSLinkInsertBefore(memorySLink *array, void *element, void *previous){

	/*
	** Inserts a new item before the specified element.
	*/

	byte_t *r = array->next;
	if(r){
		if(previous != NULL){
			// Set the previous element's next pointer.
			memSLinkDataGetNext(previous) = r;
		}
		// Set the new element's next pointer.
		memSLinkDataGetNext(r) = element;
		array->next = memSLinkDataGetNextFree(r);
	}
	return r;

}

void *memSLinkInsertAfter(memorySLink *array, void *element){

	/*
	** Inserts a new item after the specified element.
	*/

	byte_t *r = array->next;
	if(r){
		byte_t **next = memSLinkDataGetNextPointer(element);
		// Set the new element's next pointer.
		memSLinkDataGetNext(r) = *next;
		// Set the old element's next pointer.
		*next = r;
		array->next = memSLinkDataGetNextFree(r);
	}
	return r;

}

void memSLinkFree(memorySLink *array, void *element, void *previous){

	/*
	** Removes an element from an array
	** and frees the block.
	*/

	// Set the previous element's next pointer.
	memSLinkDataGetNext(previous) = memSLinkDataGetNext(element);

	memSLinkDataGetNext(element) = NULL;
	memSLinkDataGetNextFree(element) = array->next;
	array->next = element;

}

void *memSLinkReset(void *start, const size_t bytes, const size_t length){

	const size_t blockSize = memSLinkBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	byte_t *end;

	end = (byte_t *)start + memSLinkAllocationSize(start, bytes, length);
	start = memSLinkBlockGetData(memSLinkAlignStart(start));

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		memSLinkDataGetNext(block) = NULL;
		memSLinkDataGetNextFree(block) = next;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	memSLinkDataGetNext(block) = NULL;
	memSLinkDataGetNextFree(block) = NULL;

	return start;

}

void memSLinkClear(memorySLink *array){

	byte_t *block = memSLinkBlockGetData(memSLinkAlignStart(array->start));
	byte_t *next = block + array->block;

	array->next = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < array->end){
		memSLinkDataGetNext(block) = NULL;
		memSLinkDataGetNextFree(block) = next;
		block = next;
		next += array->block;
	}

	// Final block contains a null pointer.
	memSLinkDataGetNext(block) = NULL;
	memSLinkDataGetNextFree(block) = NULL;

}
