#include "memoryList.h"

void *memListInit(memoryList *list, void *start, const size_t bytes, const size_t length){

	/*
	** Initialize a memory list with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		list->block = memListBlockSize(bytes);
		list->start = start;
		list->end = (byte_t *)start + memListAllocationSize(start, bytes, length);

		memListClear(list);

	}

	return start;

}

void *memListAllocate(memoryList *list){

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

void memListFree(memoryList *list, void *block){

	/*
	** Frees a block of memory from the list.
	*/

	*((byte_t **)block) = list->next;
	list->next = block;

}

void *memListReset(void *start, const size_t bytes, const size_t length){

	const size_t blockSize = memListBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	byte_t *end;

	end = (byte_t *)start + memListAllocationSize(start, bytes, length);
	start = memListBlockGetData(memListAlignStart(start));

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

	byte_t *block = memListBlockGetData(memListAlignStart(list->start));
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
