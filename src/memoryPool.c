#include "memoryPool.h"

byte_t *memPoolInit(memoryPool *pool, byte_t *start, const size_t bytes, const size_t length){

	/*
	** Initialize a memory pool with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		pool->block = memPoolBlockSize(bytes);
		pool->start = start;
		pool->end = start + memPoolAllocationSize(start, bytes, length);

		memPoolClear(pool);

	}

	return start;

}

byte_t *memPoolAllocate(memoryPool *pool){

	/*
	** Retrieves a new block of memory from the pool
	** and updates the "next" pointer.
	** Unspecified behaviour with variable element sizes.
	*/

	byte_t *r = pool->next;
	if(r){
		pool->next = memPoolDataGetNextFree(r);
		memPoolDataGetFlags(r) = MEMORY_POOL_ACTIVE;
	}
	return r;

}

void memPoolFree(memoryPool *pool, byte_t *block){

	/*
	** Frees a block of memory from the pool.
	*/

	memPoolDataGetFlags(block) = MEMORY_POOL_INACTIVE;
	memPoolDataGetNextFree(block) = pool->next;
	pool->next = block;

}

byte_t *memPoolReset(byte_t *start, const size_t bytes, const size_t length){

	const size_t blockSize = memPoolBlockSize(bytes);
	byte_t *block;
	byte_t *next;
	byte_t *end;

	end = start + memPoolAllocationSize(start, bytes, length);
	start = memPoolBlockGetData(MEMORY_POOL_ALIGN((uintptr_t)start));

	block = start;
	next = block + blockSize;

	// Loop through every block, making it
	// point to the next free block.
	while(next < end){
		memPoolDataGetFlags(block) = MEMORY_POOL_INACTIVE;
		memPoolDataGetNextFree(block) = next;
		block = next;
		next += blockSize;
	}

	// Final block contains a null pointer.
	memPoolDataGetFlags(block) = MEMORY_POOL_INACTIVE;
	memPoolDataGetNextFree(block) = NULL;

	return start;

}

void memPoolClear(memoryPool *pool){

	byte_t *block = memPoolBlockGetData(MEMORY_POOL_ALIGN((uintptr_t)pool->start));
	byte_t *next = block + pool->block;

	pool->next = block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < pool->end){
		memPoolDataGetFlags(block) = MEMORY_POOL_INACTIVE;
		memPoolDataGetNextFree(block) = next;
		block = next;
		next += pool->block;
	}

	// Final block contains a null pointer.
	memPoolDataGetFlags(block) = MEMORY_POOL_INACTIVE;
	memPoolDataGetNextFree(block) = NULL;

}
