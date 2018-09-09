#include "memoryList.h"

byte_t *memListInit(memoryList *list, byte_t *start, const size_t bytes, const size_t length){

	/*
	** Initialize a memory list with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		list->block = memListBlockSize(bytes);
		list->start = start;
		list->end = start + memListAllocationSize(start, bytes, length);

		memListClear(list);

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

byte_t *memListReset(byte_t *start, const size_t bytes, const size_t length){

	const size_t blockSize = memListBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	byte_t *end;

	end = start + memListAllocationSize(start, bytes, length);
	start = (byte_t *)memListAlignStart(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		memListDataGetNextFree(block) = next;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	memListDataGetNextFree(block) = NULL;

	return start;

}

void memListClear(memoryList *list){

	byte_t *block = (byte_t *)memListAlignStart(list->start);
	byte_t *next = block + list->block;

	list->next = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < list->end){
		memListDataGetNextFree(block) = next;
		block = next;
		next += list->block;
	}

	// Final block contains a null pointer.
	memListDataGetNextFree(block) = NULL;

}
