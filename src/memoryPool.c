#include "memoryPool.h"

#define memPoolBlockGetFlags(block)    *((byte_t *)(block + MEMORY_POOL_FLAG_OFFSET_FROM_BLOCK))
#define memPoolBlockGetNextFree(block) *((byte_t **)(block + MEMORY_POOL_DATA_OFFSET_FROM_BLOCK))
#define memPoolBlockGetData(block)       (byte_t *)(block + MEMORY_POOL_DATA_OFFSET_FROM_BLOCK)

#define memPoolDataGetFlags(data)    *((byte_t *)(data + MEMORY_POOL_FLAG_OFFSET_FROM_DATA))
#define memPoolDataGetNextFree(data) *((byte_t **)data)
#define memPoolDataGetBlock(data)      (byte_t *)(block + MEMORY_POOL_BLOCK_OFFSET_FROM_DATA)

size_t memPoolAllocationOverhead(const byte_t *start, const size_t bytes, const size_t length){
	/*
	** Returns the total allocation overhead.
	*/
	const size_t block = MEMORY_POOL_ALIGN((bytes > MEMORY_POOL_BLOCK_SIZE ? bytes : MEMORY_POOL_BLOCK_SIZE) + MEMORY_POOL_BLOCK_HEADER_SIZE) - bytes;
	const uintptr_t offset = (uintptr_t)start;
	return block * length + MEMORY_POOL_ALIGN(offset) - offset;
}

byte_t *memPoolInit(memoryPool *pool, byte_t *start, const size_t bytes, const size_t length){

	/*
	** Initialize a memory pool with "length"-many
	** elements of "bytes" size.
	*/

	if(start){

		// Clamp the block size upwards to
		// match the minimum block size.
		pool->block = MEMORY_POOL_ALIGN((bytes > MEMORY_POOL_BLOCK_SIZE ? bytes : MEMORY_POOL_BLOCK_SIZE) + MEMORY_POOL_BLOCK_HEADER_SIZE);
		pool->start = start;
		pool->end = (byte_t *)MEMORY_POOL_ALIGN((uintptr_t)start) + pool->block*length;

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

void memPoolClear(memoryPool *pool){

	byte_t *start = (byte_t *)MEMORY_POOL_ALIGN((uintptr_t)pool->start);
	byte_t *block = start;
	byte_t *next = memPoolBlockGetData(block) + pool->block;

	// Loop through every block, making it
	// point to the next free block.
	while(next < pool->end){
		memPoolBlockGetFlags(block) = MEMORY_POOL_INACTIVE;
		memPoolBlockGetNextFree(block) = next;
		block = next;
		next = memPoolBlockGetData(block) + pool->block;
	}

	// Final block contains a null pointer.
	memPoolBlockGetFlags(block) = MEMORY_POOL_INACTIVE;
	memPoolBlockGetNextFree(block) = NULL;

	pool->next = memPoolBlockGetData(start);

}

inline byte_t *memPoolStart(const memoryPool *pool){
	return pool->start;
}

inline byte_t memPoolBlockStatus(const byte_t *block){
	return memPoolBlockGetFlags(block);
}

inline void memPoolBlockNext(const memoryPool *pool, byte_t **i){
	*i += pool->block;
}

inline byte_t *memPoolEnd(const memoryPool *pool){
	return pool->end;
}
