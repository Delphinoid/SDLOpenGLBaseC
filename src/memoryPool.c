#include "memoryPool.h"

void memPoolInit(memoryPool *pool){
	pool->block = 0;
	pool->free = NULL;
	pool->region = NULL;
}

void *memPoolCreate(memoryPool *pool, void *start, const size_t bytes, const size_t length){

	/*
	** Initialize a memory pool with "length"-many
	** elements of "bytes" size.
	*/

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

void *memPoolAllocate(memoryPool *pool){

	/*
	** Retrieves a new block of memory from the pool
	** and updates the "free" pointer.
	** Unspecified behaviour with variable element sizes.
	*/

	byte_t *r = pool->free;
	if(r){
		pool->free = memPoolDataGetNextFree(r);
		memPoolDataGetFlags(r) = MEMORY_POOL_ACTIVE;
	}
	return r;

}

void memPoolFree(memoryPool *pool, void *block){

	/*
	** Frees a block of memory from the pool.
	*/

	memPoolDataGetFlags(block) = MEMORY_POOL_INACTIVE;
	memPoolDataGetNextFree(block) = pool->free;
	pool->free = block;

}

void *memPoolSetupMemory(void *start, const size_t bytes, const size_t length){

	const size_t blockSize = memPoolBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	byte_t *end;

	end = (byte_t *)start + memPoolAllocationSize(start, bytes, length) - sizeof(memoryRegion);
	start = memPoolAlignStartData(start);

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		memPoolDataGetFlags(block) = MEMORY_POOL_INVALID;
		memPoolDataGetNextFree(block) = next;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	memPoolDataGetFlags(block) = MEMORY_POOL_INVALID;
	memPoolDataGetNextFree(block) = NULL;

	return start;

}

void memPoolClear(memoryPool *pool){

	byte_t *block = memPoolAlignStartData(pool->region->start);
	byte_t *next = block + pool->block;

	pool->free = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < (byte_t *)pool->region){
		memPoolDataGetFlags(block) = MEMORY_POOL_INVALID;
		memPoolDataGetNextFree(block) = next;
		block = next;
		next += pool->block;
	}

	// Final block contains a null pointer.
	memPoolDataGetFlags(block) = MEMORY_POOL_INVALID;
	memPoolDataGetNextFree(block) = NULL;

}

void *memPoolExtend(memoryPool *pool, void *start, const size_t bytes, const size_t length){

	/*
	** Extends the memory allocator.
	** Its logical function is similar to a
	** realloc, but it creates a new chunk
	** and links it.
	*/

	if(start){

		memoryRegion *newRegion = (memoryRegion *)((byte_t *)start + memPoolAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		memRegionAppend(&pool->region, newRegion, start);

		memPoolSetupMemory(start, bytes, length);
		pool->free = memPoolAlignStartData(start);

	}

	return start;

}

void memPoolDelete(memoryPool *pool){
	memRegionFree(pool->region);
}
