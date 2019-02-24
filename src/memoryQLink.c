#include "memoryQLink.h"

void memQLinkInit(memoryQLink *const restrict array){
	array->block = 0;
	array->free = NULL;
	array->region = NULL;
}

void *memQLinkCreate(memoryQLink *const restrict array, void *const start, const size_t bytes, const size_t length){

	/*
	** Initialize an array allocator with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		array->block = memQLinkBlockSize(bytes);
		array->region = (memoryRegion *)((byte_t *)start + memQLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		array->region->start = start;
		array->region->next = NULL;

		memQLinkClear(array);

	}

	return start;

}

void *memQLinkCreateInit(memoryQLink *const restrict array, void *const start, const size_t bytes, const size_t length, void (*func)(void *const restrict block)){

	/*
	** Initialize an array allocator with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		array->block = memQLinkBlockSize(bytes);
		array->region = (memoryRegion *)((byte_t *)start + memQLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		array->region->start = start;
		array->region->next = NULL;

		memQLinkClearInit(array, func);

	}

	return start;

}

void *memQLinkAllocate(memoryQLink *const restrict array){

	/*
	** Retrieves a new block of memory from the array
	** allocator and updates the "free" pointer.
	** Unspecified behaviour with variable element sizes.
	*/

	byte_t *const r = array->free;
	if(r){
		array->free = memQLinkDataGetNextFreeMasked(r);
		memQLinkDataGetNextA(r) = NULL;
		memQLinkDataGetPrevA(r) = NULL;
		memQLinkDataGetNextB(r) = NULL;
		memQLinkDataGetPrevB(r) = NULL;
	}
	return r;

}

void *memQLinkAllocateSorted(memoryQLink *const restrict array, void **const startA, void **const startB, void *const prevA, void *const nextA, void *const prevB, void *const nextB, unsigned int swapA, unsigned int swapB){

	byte_t *const r = array->free;
	if(r){

		array->free = memQLinkDataGetNextFreeMasked(r);

		if(prevA != NULL){
			// Insert between the previous pair and its next pair.
			memQLinkDataGetNextA(prevA) = r;
		}else{
			// Insert directly before the first pair.
			*startA = r;
		}
		if(nextA != NULL){
			if(swapA){
				memQLinkDataGetPrevB(nextA) = r;
			}else{
				memQLinkDataGetPrevA(nextA) = r;
			}
		}

		if(prevB != NULL){
			// Insert between the previous pair and its next pair.
			memQLinkDataGetNextB(prevB) = r;
		}else{
			// Insert directly before the first pair.
			*startB = r;
		}
		if(nextB != NULL){
			if(swapB){
				memQLinkDataGetPrevA(nextB) = r;
			}else{
				memQLinkDataGetPrevB(nextB) = r;
			}
		}

		memQLinkDataGetPrevA(r) = prevA;
		memQLinkDataGetNextA(r) = nextA;
		memQLinkDataGetPrevB(r) = prevB;
		memQLinkDataGetNextB(r) = nextB;

	}
	return r;

}

void memQLinkFree(memoryQLink *const restrict array, void *const element){

	/*
	** Removes an element from an array
	** and frees the block.
	*/

	memQLinkDataGetFlags(element) = (uintptr_t)array->free | MEMORY_QLINK_BLOCK_INACTIVE;
	array->free = element;

}

void memQLinkFreeSorted(memoryQLink *const restrict array, void **const startA, void **const startB, void *const element){

	/*
	** Removes an element from a sorted
	** array and frees the block.
	*/

	byte_t *temp;

	// Remove references from the previous pairs.
	temp = memQLinkPrevA(element);
	if(temp != NULL){
		memQLinkNextA(temp) = memQLinkNextA(element);
	}else{
		*startA = memQLinkNextA(element);
	}
	temp = memQLinkPrevB(element);
	if(temp != NULL){
		//if(temp->dataA == element->dataB){
		if(memQLinkNextA(temp) == element){
			memQLinkNextA(temp) = memQLinkNextB(element);
		}else{
			memQLinkNextB(temp) = memQLinkNextB(element);
		}
	}else{
		*startB = memQLinkNextB(element);
	}

	// Remove references from the next pairs.
	temp = memQLinkNextA(element);
	if(temp != NULL){
		//if(temp->dataA == element->dataA){
		if(memQLinkPrevA(temp) == element){
			memQLinkPrevA(temp) = memQLinkPrevA(element);
		}else{
			memQLinkPrevB(temp) = memQLinkPrevA(element);
		}
	}
	temp = memQLinkNextB(element);
	if(temp != NULL){
		memQLinkPrevB(temp) = memQLinkPrevB(element);
	}

	memQLinkDataGetFlags(element) = (uintptr_t)array->free | MEMORY_QLINK_BLOCK_INACTIVE;
	array->free = element;

}

void *memQLinkSetupMemory(void *start, const size_t bytes, const size_t length){

	const size_t blockSize = memQLinkBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	const byte_t *const end = (const byte_t *)start + memQLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion);

	start = memQLinkAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		memQLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_QLINK_BLOCK_INVALID;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	memQLinkDataGetFlags(block) = MEMORY_QLINK_BLOCK_INVALID;

	return start;

}

void *memQLinkSetupMemoryInit(void *start, const size_t bytes, const size_t length, void (*func)(void *const restrict block)){

	const size_t blockSize = memQLinkBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	const byte_t *const end = (const byte_t *)start + memQLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion);

	start = memQLinkAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		(*func)(block);
		memQLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_QLINK_BLOCK_INVALID;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	(*func)(block);
	memQLinkDataGetFlags(block) = MEMORY_QLINK_BLOCK_INVALID;

	return start;

}

void memQLinkClear(memoryQLink *const restrict array){

	byte_t *block = memQLinkAlignStartData(array->region->start);
	byte_t *next = block + array->block;

	array->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (const byte_t *)array->region && memQLinkBlockStatus(block) != MEMORY_QLINK_BLOCK_INVALID){
		memQLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_QLINK_BLOCK_INVALID;
		block = next;
		next += array->block;
	}

	// Final block contains a null pointer.
	memQLinkDataGetFlags(block) = MEMORY_QLINK_BLOCK_INVALID;

}

void memQLinkClearInit(memoryQLink *const restrict array, void (*func)(void *const restrict block)){

	byte_t *block = memQLinkAlignStartData(array->region->start);
	byte_t *next = block + array->block;

	array->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (const byte_t *)array->region && memQLinkBlockStatus(block) != MEMORY_QLINK_BLOCK_INVALID){
		(*func)(block);
		memQLinkDataGetFlags(block) = (uintptr_t)next | MEMORY_QLINK_BLOCK_INVALID;
		block = next;
		next += array->block;
	}

	// Final block contains a null pointer.
	(*func)(block);
	memQLinkDataGetFlags(block) = MEMORY_QLINK_BLOCK_INVALID;

}

void *memQLinkExtend(memoryQLink *const restrict array, void *const start, const size_t bytes, const size_t length){

	/*
	** Extends the memory allocator.
	** Its logical function is similar to a
	** realloc, but it creates a new chunk
	** and links it.
	*/

	if(start){

		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memQLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionPrepend(&array->region, newRegion, start);

		memQLinkSetupMemory(start, bytes, length);
		array->free = memQLinkAlignStartData(start);

	}

	return start;

}

void *memQLinkExtendInit(memoryQLink *const restrict array, void *const start, const size_t bytes, const size_t length, void (*func)(void *const restrict block)){

	/*
	** Extends the memory allocator.
	** Its logical function is similar to a
	** realloc, but it creates a new chunk
	** and links it.
	*/

	if(start){

		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memQLinkAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionPrepend(&array->region, newRegion, start);

		memQLinkSetupMemoryInit(start, bytes, length, func);
		array->free = memQLinkAlignStartData(start);

	}

	return start;

}

void memQLinkDelete(memoryQLink *const restrict array){
	memRegionFree(array->region);
}
