#include "memoryArraySingle.h"

byte_t *memArraySingleInit(memoryArraySingle *array, byte_t *start, const size_t bytes, const size_t length){

	/*
	** Initialize an array allocator with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		array->block = memArraySingleBlockSize(bytes);
		array->start = start;
		array->end = start + memArraySingleAllocationSize(start, bytes, length);

		memArraySingleClear(array);

	}

	return start;

}

byte_t *memArraySingleAllocate(memoryArraySingle *array){

	/*
	** Retrieves a new block of memory from the array
	** allocator and updates the "next" pointer.
	** Unspecified behaviour with variable element sizes.
	*/

	byte_t *r = array->next;
	if(r){
		memArraySingleDataGetNext(r) = NULL;
		array->next = memArraySingleDataGetNextFree(r);
	}
	return r;

}

byte_t *memArraySingleInsertBefore(memoryArraySingle *array, byte_t *element, byte_t *previous){

	/*
	** Inserts a new item before the specified element.
	*/

	byte_t *r = array->next;
	if(r){
		if(previous != NULL){
			// Set the previous element's next pointer.
			memArraySingleDataGetNext(previous) = r;
		}
		// Set the new element's next pointer.
		memArraySingleDataGetNext(r) = element;
		array->next = memArraySingleDataGetNextFree(r);
	}
	return r;

}

byte_t *memArraySingleInsertAfter(memoryArraySingle *array, byte_t *element){

	/*
	** Inserts a new item after the specified element.
	*/

	byte_t *r = array->next;
	if(r){
		byte_t **next = memArraySingleDataGetNextPointer(element);
		// Set the new element's next pointer.
		memArraySingleDataGetNext(r) = *next;
		// Set the old element's next pointer.
		*next = r;
		array->next = memArraySingleDataGetNextFree(r);
	}
	return r;

}

void memArraySingleFree(memoryArraySingle *array, byte_t *block){

	/*
	** Frees an array of elements.
	*/

	memArraySingleDataGetNext(block) = NULL;
	memArraySingleDataGetNextFree(block) = array->next;
	array->next = block;

}

void memArraySingleClear(memoryArraySingle *array){

	byte_t *start = (byte_t *)MEMORY_ARRAY_SINGLE_ALIGN((uintptr_t)array->start);
	byte_t *block = memArraySingleBlockGetData(start);
	byte_t *next = block + array->block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < array->end){
		memArraySingleDataGetNext(block) = NULL;
		memArraySingleDataGetNextFree(block) = next;
		block = next;
		next += array->block;
	}

	// Final block contains a null pointer.
	memArraySingleDataGetNext(block) = NULL;
	memArraySingleDataGetNextFree(block) = NULL;

	array->next = memArraySingleBlockGetData(start);

}
