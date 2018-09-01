#include "memoryArray.h"

size_t memArrayAllocationOverhead(const byte_t *start, const size_t bytes, const size_t length){
	/*
	** Returns the total allocation overhead.
	*/
	const size_t block = MEMORY_ARRAY_ALIGN((bytes > MEMORY_ARRAY_BLOCK_SIZE ? bytes : MEMORY_ARRAY_BLOCK_SIZE) + MEMORY_ARRAY_BLOCK_HEADER_SIZE) - bytes;
	const uintptr_t offset = (uintptr_t)start;
	return block * length + MEMORY_ARRAY_ALIGN(offset) - offset;
}

byte_t *memArrayInit(memoryArray *array, byte_t *start, const size_t bytes, const size_t length){

	/*
	** Initialize an array allocator with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		array->block = MEMORY_ARRAY_ALIGN((bytes > MEMORY_ARRAY_BLOCK_SIZE ? bytes : MEMORY_ARRAY_BLOCK_SIZE) + MEMORY_ARRAY_BLOCK_HEADER_SIZE);
		array->start = start;
		array->end = (byte_t *)MEMORY_ARRAY_ALIGN((uintptr_t)start) + array->block*length;

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
		byte_t **c = (byte_t **)r;
		*(c + MEMORY_ARRAY_PREV_OFFSET_FROM_BLOCK) = r;
		*(c + MEMORY_ARRAY_NEXT_OFFSET_FROM_BLOCK) = r;
		array->next = *c;
		r += MEMORY_ARRAY_DATA_OFFSET_FROM_BLOCK;
	}
	return r;

}

byte_t *memArrayInsertBefore(memoryArray *array, byte_t *element){

	/*
	** Inserts a new item before the specified element.
	*/

	byte_t *r = array->next;
	if(r){
		byte_t **c = (byte_t **)r;
		byte_t **next = (byte_t **)(element + MEMORY_ARRAY_NEXT_OFFSET_FROM_DATA);
		*(c + MEMORY_ARRAY_PREV_OFFSET_FROM_BLOCK) = element;
		*(c + MEMORY_ARRAY_NEXT_OFFSET_FROM_BLOCK) = *next;
		// Set current element's next pointer.
		*next = r;
		// Set next next element's previous pointer.
		*((byte_t **)(*next) + MEMORY_ARRAY_PREV_OFFSET_FROM_BLOCK) = r;
		array->next = *c;
		r += MEMORY_ARRAY_DATA_OFFSET_FROM_BLOCK;
	}
	return r;

}

byte_t *memArrayInsertAfter(memoryArray *array, byte_t *element){

	/*
	** Inserts a new item after the specified element.
	*/

	byte_t *r = array->next;
	if(r){
		byte_t **c = (byte_t **)r;
		byte_t **next = (byte_t **)(element + MEMORY_ARRAY_NEXT_OFFSET_FROM_DATA);
		*(c + MEMORY_ARRAY_PREV_OFFSET_FROM_BLOCK) = element;
		*(c + MEMORY_ARRAY_NEXT_OFFSET_FROM_BLOCK) = *next;
		// Set current element's next pointer.
		*next = r;
		// Set next next element's previous pointer.
		*((byte_t **)(*next) + MEMORY_ARRAY_PREV_OFFSET_FROM_BLOCK) = r;
		array->next = *c;
		r += MEMORY_ARRAY_DATA_OFFSET_FROM_BLOCK;
	}
	return r;

}

void memArrayFree(memoryArray *array, byte_t *block){

	/*
	** Frees an array of elements.
	*/

	byte_t **b = (byte_t **)block;
	*(b + MEMORY_ARRAY_PREV_OFFSET_FROM_DATA) = NULL;
	*(b + MEMORY_ARRAY_NEXT_OFFSET_FROM_DATA) = NULL;
	*b = array->next;
	array->next = block + MEMORY_ARRAY_BLOCK_OFFSET_FROM_DATA;

}

void memArrayClear(memoryArray *array){

	byte_t **block = (byte_t **)array->start;
	byte_t *next = (byte_t *)block + array->block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < array->end){
		*(block + MEMORY_ARRAY_PREV_OFFSET_FROM_BLOCK) = NULL;
		*(block + MEMORY_ARRAY_NEXT_OFFSET_FROM_BLOCK) = NULL;
		*(block + MEMORY_ARRAY_DATA_OFFSET_FROM_BLOCK) = next;
		block = (byte_t **)next;
		next = (byte_t *)block + array->block;
	}

	// Final block contains a null pointer.
	*(block + MEMORY_ARRAY_PREV_OFFSET_FROM_BLOCK) = NULL;
	*(block + MEMORY_ARRAY_NEXT_OFFSET_FROM_BLOCK) = NULL;
	*(block + MEMORY_ARRAY_DATA_OFFSET_FROM_BLOCK) = NULL;

	array->next = array->start;

}

inline byte_t *memArrayStart(const memoryArray *array){
	return array->start;
}

inline void memArrayPrev(byte_t **i){
	*i = *((byte_t **)(*i + MEMORY_ARRAY_PREV_OFFSET_FROM_DATA));
}

inline void memArrayNext(byte_t **i){
	*i = *((byte_t **)(*i + MEMORY_ARRAY_NEXT_OFFSET_FROM_DATA));
}

inline byte_t memArrayBlockStatus(const byte_t *block){
	return *((byte_t *)(block + MEMORY_ARRAY_FLAG_OFFSET_FROM_DATA));
}

inline void memArrayBlockNext(const memoryArray *array, byte_t **i){
	*i += array->block;
}

inline byte_t *memArrayEnd(const memoryArray *array){
	return array->end;
}
