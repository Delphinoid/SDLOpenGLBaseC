#include "memoryList.h"

void memListInit(memoryList *list){
	list->block = 0;
	list->free = NULL;
	list->region = NULL;
}

void *memListCreate(memoryList *list, void *start, const size_t bytes, const size_t length){

	/*
	** Initialize a memory list with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		list->block = memListBlockSize(bytes);
		list->region = (memoryRegion *)((byte_t *)start + memListAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		list->region->start = start;
		list->region->next = NULL;

		memListClear(list);

	}

	return start;

}

void *memListAllocate(memoryList *list){

	/*
	** Retrieves a new block of memory from the list
	** and updates the "free" pointer.
	** Unspecified behaviour with variable element sizes.
	*/

	byte_t *r = list->free;
	if(r){
		list->free = *((byte_t **)r);
	}
	return r;

}

void memListFree(memoryList *list, void *block){

	/*
	** Frees a block of memory from the list.
	*/

	*((byte_t **)block) = list->free;
	list->free = block;

}

void *memListSetupMemory(void *start, const size_t bytes, const size_t length){

	const size_t blockSize = memListBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	byte_t *end;

	end = (byte_t *)start + memListAllocationSize(start, bytes, length) - sizeof(memoryRegion);
	start = memListAlignStartData(start);

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

void *memListIndex(memoryList *list, const size_t i){

	/*
	** Finds the element at index i.
	*/

	size_t offset = list->block * i;

	memoryRegion *region = list->region;
	byte_t *regionStart  = memListFirst(region);
	size_t  regionSize   = memAllocatorEnd(region) - regionStart;

	while(offset >= regionSize){
		region      = memAllocatorNext(region);
		regionStart = memListFirst(region);
		regionSize  = memAllocatorEnd(region) - regionStart;
		offset -= regionSize;
	}

	return (void *)(regionStart + offset);

}

void memListClear(memoryList *list){

	byte_t *block = memListAlignStartData(list->region->start);
	byte_t *next = block + list->block;

	list->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (byte_t *)list->region){
		memListDataGetNextFree(block) = next;
		block = next;
		next += list->block;
	}

	// Final block contains a null pointer.
	memListDataGetNextFree(block) = NULL;

}

void *memListExtend(memoryList *list, void *start, const size_t bytes, const size_t length){

	/*
	** Extends the memory allocator.
	** Its logical function is similar to a
	** realloc, but it creates a new chunk
	** and links it.
	*/

	if(start){

		memoryRegion *newRegion = (memoryRegion *)((byte_t *)start + memListAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionExtend(&list->region, newRegion, start);

		memListSetupMemory(start, bytes, length);
		list->free = memListAlignStartData(start);

	}

	return start;

}

void memListDelete(memoryList *list){
	memRegionFree(list->region);
}
