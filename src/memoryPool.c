#include "memoryPool.h"

void memPoolInit(memoryPool *const __RESTRICT__ pool){
	pool->block = 0;
	pool->free = NULL;
	pool->region = NULL;
}

void *memPoolCreate(memoryPool *const __RESTRICT__ pool, void *start, const size_t bytes, const size_t length){

	// Initialize a memory pool with "length"-many
	// elements of "bytes" size.

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		pool->block = memPoolBlockSize(bytes);
		pool->region = (memoryRegion *)((byte_t *)start + memPoolAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		pool->region->start = start;
		pool->region->next = NULL;

		memPoolClear(pool);

	}

	return start;

}

void *memPoolCreateInit(memoryPool *const __RESTRICT__ pool, void *start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block)){

	// Initialize a memory pool with "length"-many
	// elements of "bytes" size.

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		pool->block = memPoolBlockSize(bytes);
		pool->region = (memoryRegion *)((byte_t *)start + memPoolAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		pool->region->start = start;
		pool->region->next = NULL;

		memPoolClearInit(pool, func);

	}

	return start;

}

void *memPoolAllocate(memoryPool *const __RESTRICT__ pool){

	// Retrieves a new block of memory from the pool
	// and updates the "free" pointer.
	// Unspecified behaviour with variable element sizes.
	byte_t *const r = pool->free;
	if(r){
		pool->free = memPoolDataGetNextFree(r);
		memPoolDataGetFlags(r) = MEMORY_POOL_BLOCK_ACTIVE;
	}
	return r;

}

void memPoolFree(memoryPool *const __RESTRICT__ pool, void *const block){

	// Frees a block of memory from the pool.

	memPoolDataGetFlags(block) = MEMORY_POOL_BLOCK_INACTIVE;
	memPoolDataGetNextFree(block) = pool->free;
	pool->free = block;

}

void *memPoolSetupMemory(void *start, const size_t bytes, const size_t length){

	const size_t blockSize = memPoolBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	const byte_t *const end = (byte_t *)start + memPoolAllocationSize(start, bytes, length) - sizeof(memoryRegion);

	start = memPoolAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		memPoolDataGetFlags(block) = MEMORY_POOL_BLOCK_INVALID;
		memPoolDataGetNextFree(block) = next;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	memPoolDataGetFlags(block) = MEMORY_POOL_BLOCK_INVALID;
	memPoolDataGetNextFree(block) = NULL;

	return start;

}

void *memPoolSetupMemoryInit(void *start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block)){

	const size_t blockSize = memPoolBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	const byte_t *const end = (byte_t *)start + memPoolAllocationSize(start, bytes, length) - sizeof(memoryRegion);

	start = memPoolAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		(*func)(block);
		memPoolDataGetFlags(block) = MEMORY_POOL_BLOCK_INVALID;
		memPoolDataGetNextFree(block) = next;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	(*func)(block);
	memPoolDataGetFlags(block) = MEMORY_POOL_BLOCK_INVALID;
	memPoolDataGetNextFree(block) = NULL;

	return start;

}

void *memPoolIndex(memoryPool *const __RESTRICT__ pool, const size_t i){

	// Finds the element at index i.

	size_t offset = pool->block * i;

	const memoryRegion *region = pool->region;
	byte_t *regionStart  = memPoolFirst(region);
	size_t  regionSize   = memAllocatorEnd(region) - regionStart;

	while(offset >= regionSize){
		region      = memAllocatorNext(region);
		#ifndef MEMORY_POOL_INDEX_UNSAFE
			if(region == NULL){
				return NULL;
			}
		#endif
		regionStart = memPoolFirst(region);
		regionSize  = memAllocatorEnd(region) - regionStart;
		offset -= regionSize;
	}

	return (void *)(regionStart + offset);

}

void *memPoolIndexRegion(memoryPool *const __RESTRICT__ pool, const size_t i, memoryRegion **const container){

	// Finds the element at index i.

	size_t offset = pool->block * i;

	memoryRegion *region = pool->region;
	byte_t *regionStart  = memPoolFirst(region);
	size_t  regionSize   = memAllocatorEnd(region) - regionStart;

	while(offset >= regionSize){
		region      = memAllocatorNext(region);
		#ifndef MEMORY_POOL_INDEX_UNSAFE
			if(region == NULL){
				return NULL;
			}
		#endif
		regionStart = memPoolFirst(region);
		regionSize  = memAllocatorEnd(region) - regionStart;
		offset -= regionSize;
	}

	*container = region;
	return (void *)(regionStart + offset);

}

void memPoolClear(memoryPool *const __RESTRICT__ pool){

	byte_t *block = memPoolAlignStartData(pool->region->start);
	byte_t *next = block + pool->block;

	pool->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (byte_t *)pool->region && memPoolDataGetFlags(block) != MEMORY_POOL_BLOCK_INVALID){
		memPoolDataGetFlags(block) = MEMORY_POOL_BLOCK_INVALID;
		memPoolDataGetNextFree(block) = next;
		block = next;
		next += pool->block;
	}

	// Final block contains a null pointer.
	memPoolDataGetFlags(block) = MEMORY_POOL_BLOCK_INVALID;
	memPoolDataGetNextFree(block) = NULL;

}

void memPoolClearInit(memoryPool *const __RESTRICT__ pool, void (*func)(void *const __RESTRICT__ block)){

	byte_t *block = memPoolAlignStartData(pool->region->start);
	byte_t *next = block + pool->block;

	pool->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (byte_t *)pool->region && memPoolDataGetFlags(block) != MEMORY_POOL_BLOCK_INVALID){
		(*func)(block);
		memPoolDataGetFlags(block) = MEMORY_POOL_BLOCK_INVALID;
		memPoolDataGetNextFree(block) = next;
		block = next;
		next += pool->block;
	}

	// Final block contains a null pointer.
	(*func)(block);
	memPoolDataGetFlags(block) = MEMORY_POOL_BLOCK_INVALID;
	memPoolDataGetNextFree(block) = NULL;

}

void *memPoolExtend(memoryPool *const __RESTRICT__ pool, void *const start, const size_t bytes, const size_t length){

	// Extends the memory allocator.
	// Its logical function is similar to a
	// realloc, but it creates a new chunk
	// and links it.

	if(start){

		// Place the memory region at the end of the chunk.
		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memPoolAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionExtend(&pool->region, newRegion, start);

		memPoolSetupMemory(start, bytes, length);
		pool->free = memPoolAlignStartData(start);

	}

	return start;

}

void *memPoolExtendInit(memoryPool *const __RESTRICT__ pool, void *const start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block)){

	// Extends the memory allocator.
	// Its logical function is similar to a
	// realloc, but it creates a new chunk
	// and links it.

	if(start){

		// Place the memory region at the end of the chunk.
		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + memPoolAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionExtend(&pool->region, newRegion, start);

		memPoolSetupMemoryInit(start, bytes, length, func);
		pool->free = memPoolAlignStartData(start);

	}

	return start;

}

void memPoolDelete(memoryPool *const __RESTRICT__ pool){
	memRegionFree(pool->region);
}
