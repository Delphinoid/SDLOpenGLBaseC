#include "memoryList.h"

byte_t *memListInit(memoryList *list, byte_t *start, const size_t bytes, const size_t length){

	/*
	** Initialize a memory list with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		const size_t clampedBytes = bytes > sizeof(memoryListBlock) ? bytes : sizeof(memoryListBlock);

		size_t i;
		memoryListBlock *block = (memoryListBlock *)start;

		// Loop through every block, making it
		// point to the next free block.
		for(i = 1; i < length; ++i){
			memoryListBlock *next = block + clampedBytes;
			block->next = (byte_t *)next;
			block = next;
		}

		// Final block contains a null pointer.
		block->next = NULL;

		list->next = start;

	}

	return start;

}

byte_t *memListAllocate(memoryList *list){

	/*
	** Retrieves a new block of memory from the list
	** and updates the "next" pointer.
	** Unspecified behaviour with variable element sizes.
	*/

	byte_t *r = list->next;
	if(r){
		list->next = *((byte_t **)r);
	}
	return r;

}

void memListFree(memoryList *list, byte_t *block){

	/*
	** Frees a block of memory from the list.
	*/

	*((byte_t **)block) = list->next;
	list->next = block;

}
