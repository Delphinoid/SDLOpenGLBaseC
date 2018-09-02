#include "memoryArray.h"

byte_t *memArrayInit(memoryArray *array, byte_t *start, const size_t bytes, const size_t length){

	/*
	** Initialize an array allocator with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		array->block = memArrayBlockSize(bytes);
		array->start = start;
		array->end = start + memArrayAllocationSize(start, bytes, length);

		memArrayClear(array);

	}

	return start;

}

byte_t *memArrayAllocate(memoryArray *array){

	/*
	** Retrieves a new block of memory from the array
	** allocator and updates the "next" pointer.
	** Unspecified behaviour with variable element sizes.
	*/

	byte_t *r = array->next;
	if(r){
		memArrayDataGetNext(r) = r;
		memArrayDataGetPrev(r) = r;
		array->next = memArrayDataGetNextFree(r);
	}
	return r;

}

byte_t *memArrayInsertBefore(memoryArray *array, byte_t *element){

	/*
	** Inserts a new item before the specified element.
	*/

	byte_t *r = array->next;
	if(r){
		byte_t **prev = memArrayDataGetPrevPointer(element);
		// Set the previous element's next pointer.
		memArrayDataGetNext(*prev) = r;
		// Set the new element's pointers.
		memArrayDataGetNext(r) = element;
		memArrayDataGetPrev(r) = *prev;
		// Set the old element's pointers.
		*prev = r;
		array->next = memArrayDataGetNextFree(r);
	}
	return r;

}

byte_t *memArrayInsertAfter(memoryArray *array, byte_t *element){

	/*
	** Inserts a new item after the specified element.
	*/

	byte_t *r = array->next;
	if(r){
		byte_t **next = memArrayDataGetNextPointer(element);
		// Set the following element's previous pointer.
		memArrayDataGetPrev(*next) = r;
		// Set the new element's pointers.
		memArrayDataGetNext(r) = *next;
		memArrayDataGetPrev(r) = element;
		// Set the old element's pointers.
		*next = r;
		array->next = memArrayDataGetNextFree(r);
	}
	return r;

}

void memArrayFree(memoryArray *array, byte_t *block){

	/*
	** Frees an array of elements.
	*/

	memArrayDataGetNext(block) = NULL;
	memArrayDataGetPrev(block) = NULL;
	memArrayDataGetNextFree(block) = array->next;
	array->next = block;

}

void memArrayClear(memoryArray *array){

	byte_t *start = (byte_t *)MEMORY_ARRAY_ALIGN((uintptr_t)array->start);
	byte_t *block = memArrayBlockGetData(start);
	byte_t *next = block + array->block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < array->end){
		memArrayDataGetNext(block) = NULL;
		memArrayDataGetPrev(block) = NULL;
		memArrayDataGetNextFree(block) = next;
		block = next;
		next += array->block;
	}

	// Final block contains a null pointer.
	memArrayDataGetNext(block) = NULL;
	memArrayDataGetPrev(block) = NULL;
	memArrayDataGetNextFree(block) = NULL;

	array->next = memArrayBlockGetData(start);

}
