#include "memoryRList.h"

void memRListInit(memoryRList *const __RESTRICT__ list){
	list->block = 0;
	list->free = NULL;
	list->region = NULL;
}

void *memRListCreate(memoryRList *const __RESTRICT__ list, void *const start, const size_t bytes, const size_t length){

	// Initialize a memory list with "length"-many
	// elements of "bytes" size.

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		list->block = memRListBlockSize(bytes);
		list->region = (memoryRegion *)((byte_t *)start + memRListAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		list->region->start = start;
		list->region->next = NULL;

		memRListClear(list);

	}

	return start;

}

void *memRListCreateInit(memoryRList *const __RESTRICT__ list, void *const start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block)){

	// Initialize a memory list with "length"-many
	// elements of "bytes" size.

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		list->block = memRListBlockSize(bytes);
		list->region = (memoryRegion *)((byte_t *)start + memRListAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		list->region->start = start;
		list->region->next = NULL;

		memRListClearInit(list, func);

	}

	return start;

}

void *memRListAllocate(memoryRList *const __RESTRICT__ list){

	// Retrieves a new block of memory from the list
	// and updates the "free" pointer.
	// Unspecified behaviour with variable element sizes.

	byte_t *const r = list->free;
	if(r){
		list->free = memRListDataGetNextFree(r, list->block);
	}
	return r;

}

void memRListFree(memoryRList *const __RESTRICT__ list, void *const block){

	// Frees a block of memory from the list.

	memRListDataGetNextFree(block, list->block) = list->free;
	list->free = block;

}

void *memRListSetupMemory(void *start, const size_t bytes, const size_t length){

	const size_t blockSize = memRListBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	const byte_t *const end = (byte_t *)start + memRListAllocationSize(start, bytes, length) - sizeof(memoryRegion);

	start = memRListAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		memRListDataGetPrevFree(next) = next;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	memRListDataGetPrevFree(next) = NULL;

	return start;

}

void *memRListSetupMemoryInit(void *start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block)){

	const size_t blockSize = memRListBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	const byte_t *const end = (byte_t *)start + memRListAllocationSize(start, bytes, length) - sizeof(memoryRegion);

	start = memRListAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		(*func)(block);
		memRListDataGetPrevFree(next) = next;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	(*func)(block);
	memRListDataGetPrevFree(next) = NULL;

	return start;

}

void *memRListIndex(memoryRList *const __RESTRICT__ list, const size_t i){

	// Finds the element at index i.

	size_t offset = list->block * i;

	const memoryRegion *region = list->region;
	byte_t *regionStart  = memRListFirst(region);
	size_t  regionSize   = memAllocatorEnd(region) - regionStart;

	while(offset >= regionSize){
		region      = memAllocatorNext(region);
		#ifndef MEMORY_RLIST_INDEX_UNSAFE
			if(region == NULL){
				return NULL;
			}
		#endif
		regionStart = memRListFirst(region);
		regionSize  = memAllocatorEnd(region) - regionStart;
		offset -= regionSize;
	}

	return (void *)(regionStart + offset);

}

void *memRListIndexRegion(memoryRList *const __RESTRICT__ list, const size_t i, memoryRegion **const container){

	// Finds the element at index i.

	size_t offset = list->block * i;

	memoryRegion *region = list->region;
	byte_t *regionStart  = memRListFirst(region);
	size_t  regionSize   = memAllocatorEnd(region) - regionStart;

	while(offset >= regionSize){
		region      = memAllocatorNext(region);
		#ifndef MEMORY_RLIST_INDEX_UNSAFE
			if(region == NULL){
				return NULL;
			}
		#endif
		regionStart = memRListFirst(region);
		regionSize  = memAllocatorEnd(region) - regionStart;
		offset -= regionSize;
	}

	*container = region;
	return (void *)(regionStart + offset);

}

void memRListClear(memoryRList *const __RESTRICT__ list){

	byte_t *block = memRListAlignStartData(list->region->start);
	byte_t *next = block + list->block;

	list->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (byte_t *)list->region){
		memRListDataGetPrevFree(next) = next;
		block = next;
		next += list->block;
	}

	// Final block contains a null pointer.
	memRListDataGetPrevFree(next) = NULL;

}

void memRListClearInit(memoryRList *const __RESTRICT__ list, void (*func)(void *const __RESTRICT__ block)){

	byte_t *block = memRListAlignStartData(list->region->start);
	byte_t *next = block + list->block;

	list->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (byte_t *)list->region){
		(*func)(block);
		memRListDataGetPrevFree(next) = next;
		block = next;
		next += list->block;
	}

	// Final block contains a null pointer.
	(*func)(block);
	memRListDataGetPrevFree(next) = NULL;

}

void *memRListExtend(memoryRList *const __RESTRICT__ list, void *const start, const size_t bytes, const size_t length){

	// Extends the memory allocator.
	// Its logical function is similar to a
	// realloc, but it creates a new chunk
	// and links it.

	if(start){

		// Place the memory region at the end of the chunk.
		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memRListAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionExtend(&list->region, newRegion, start);

		memRListSetupMemory(start, bytes, length);
		list->free = memRListAlignStartData(start);

	}

	return start;

}

void *memRListExtendInit(memoryRList *const __RESTRICT__ list, void *const start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block)){

	// Extends the memory allocator.
	// Its logical function is similar to a
	// realloc, but it creates a new chunk
	// and links it.

	if(start){

		// Place the memory region at the end of the chunk.
		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memRListAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionExtend(&list->region, newRegion, start);

		memRListSetupMemoryInit(start, bytes, length, func);
		list->free = memRListAlignStartData(start);

	}

	return start;

}

void memRListDelete(memoryRList *const __RESTRICT__ list){
	memRegionFree(list->region);
}
