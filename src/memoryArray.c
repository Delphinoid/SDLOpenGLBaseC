#include "memoryArray.h"

void memArrayInit(memoryArray *const restrict array){
	array->block = 0;
	array->size = 0;
	array->region = NULL;
}

void *memArrayCreate(memoryArray *const restrict array, void *const start, const size_t bytes, const size_t length){

	/*
	** Initialize a memory array with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		array->block = memArrayBlockSize(bytes);
		array->size = length;
		array->region = (memoryRegion *)((byte_t *)start + memArrayAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		array->region->start = start;
		array->region->next = NULL;

	}

	return start;

}

void *memArrayPush(memoryArray *const restrict array){
	void *const r = memArrayIndex(array, array->size);
	if(r){
		++array->size;
	}
	return r;
}

void *memArrayPushFast(memoryArray *const restrict array, void **const block, memoryRegion **const container){
	void *const r = *block;
	if(r){
		*block = memArrayBlockNext(*array, r);
		if(*block >= (void *)memAllocatorEnd(*container)){
			*container = (*container)->next;
			if(*container){
				*block = memArrayFirst(*container);
			}else{
				*block = NULL;
			}
		}
		++array->size;
	}
	return r;
}

void memArrayPop(memoryArray *const restrict array){
	--array->size;
}

void *memArrayIndex(memoryArray *const restrict array, const size_t i){

	/*
	** Finds the element at index i.
	*/

	size_t offset = array->block * i;

	const memoryRegion *region = array->region;
	byte_t *regionStart  = memArrayFirst(region);
	size_t  regionSize   = memAllocatorEnd(region) - regionStart;

	while(offset >= regionSize){
		region      = memAllocatorNext(region);
		#ifndef MEMORY_ARRAY_INDEX_UNSAFE
			if(region == NULL){
				return NULL;
			}
		#endif
		regionStart = memArrayFirst(region);
		regionSize  = memAllocatorEnd(region) - regionStart;
		offset -= regionSize;
	}

	return (void *)(regionStart + offset);

}

void *memArrayIndexRegion(memoryArray *const restrict array, const size_t i, memoryRegion **const container){

	/*
	** Finds the element at index i.
	*/

	size_t offset = array->block * i;

	memoryRegion *region = array->region;
	byte_t *regionStart  = memArrayFirst(region);
	size_t  regionSize   = memAllocatorEnd(region) - regionStart;

	while(offset >= regionSize){
		region      = memAllocatorNext(region);
		#ifndef MEMORY_ARRAY_INDEX_UNSAFE
			if(region == NULL){
				return NULL;
			}
		#endif
		regionStart = memArrayFirst(region);
		regionSize  = memAllocatorEnd(region) - regionStart;
		offset -= regionSize;
	}

	*container = region;
	return (void *)(regionStart + offset);

}

void memArrayClear(memoryArray *const restrict array){
	array->size = 0;
}

void *memArrayExtend(memoryArray *const restrict array, void *const start, const size_t bytes, const size_t length){

	/*
	** Extends the memory allocator.
	** Its logical function is similar to a
	** realloc, but it creates a new chunk
	** and links it.
	*/

	if(start){

		// Place the memory region at the end of the chunk.
		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memArrayAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionExtend(&array->region, newRegion, start);

	}

	return start;

}

void memArrayDelete(memoryArray *const restrict array){
	memRegionFree(array->region);
}