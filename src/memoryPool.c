#include "memoryPool.h"

byte_t *memPoolInit(memoryPool *pool, byte_t *start, const size_t bytes, const size_t length){

	/*
	** Initialize a memory pool with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		pool->block = (bytes > MEMORY_POOL_BLOCK_SIZE ? bytes : MEMORY_POOL_BLOCK_SIZE) + MEMORY_POOL_BLOCK_HEADER_SIZE;
		pool->start = start;
		pool->end = start + pool->block*length;

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
		pool->next = *((byte_t **)r);
		*((byte_t *)(r + MEMORY_POOL_FLAG_OFFSET_FROM_BLOCK)) = MEMORY_POOL_ACTIVE;
		r += MEMORY_POOL_DATA_OFFSET_FROM_BLOCK;
	}
	return r;

}

void memPoolFree(memoryPool *pool, byte_t *block){

	/*
	** Frees a block of memory from the pool.
	*/

	*((byte_t *)(block + MEMORY_POOL_FLAG_OFFSET_FROM_BLOCK)) = MEMORY_POOL_INACTIVE;
	*((byte_t **)block) = pool->next;
	pool->next = block + MEMORY_POOL_FLAG_OFFSET_FROM_DATA;

}

void memPoolClear(memoryPool *pool){

	byte_t *block = pool->start;
	byte_t *next = block + pool->block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < pool->end){
		*((byte_t *)(block + MEMORY_POOL_FLAG_OFFSET_FROM_BLOCK)) = MEMORY_POOL_INACTIVE;
		*((byte_t **)(block + MEMORY_POOL_DATA_OFFSET_FROM_BLOCK)) = next;
		block = next;
		next = block + pool->block;
	}

	// Final block contains a null pointer.
	*((byte_t *)(block + MEMORY_POOL_FLAG_OFFSET_FROM_BLOCK)) = MEMORY_POOL_INACTIVE;
	*((byte_t **)(block + MEMORY_POOL_DATA_OFFSET_FROM_BLOCK)) = NULL;

	pool->next = pool->start;

}

inline byte_t *memPoolStart(const memoryPool *pool){
	return pool->start;
}

inline byte_t memPoolBlockStatus(const byte_t *block){
	return *((byte_t *)(block + MEMORY_POOL_FLAG_OFFSET_FROM_BLOCK));
}

inline void memPoolBlockNext(const memoryPool *pool, byte_t **i){
	*i += pool->block;
}

inline byte_t *memPoolEnd(const memoryPool *pool){
	return pool->end;
}
