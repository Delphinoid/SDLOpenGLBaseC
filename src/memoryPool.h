#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include "memoryShared.h"

/*
** Object pool allocator.
**
** Effectively a free-list allocator
** but each block has an "active"
** flag for iterations, stored in a
** header before the main block data.
**
** Free-list pointers point to the
** data, not the beginning of the block.
*/

/**
*** Instead of using an "active" flag,
*** could we use pointers to the next
*** active member?
**/

#define MEMORY_POOL_INACTIVE 0
#define MEMORY_POOL_ACTIVE   1

#define MEMORY_POOL_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_POOL_BLOCK_FLAGS_SIZE   sizeof(byte_t)
#define MEMORY_POOL_BLOCK_HEADER_SIZE  MEMORY_POOL_BLOCK_FLAGS_SIZE
#define MEMORY_POOL_BLOCK_SIZE         MEMORY_POOL_BLOCK_POINTER_SIZE
#define MEMORY_POOL_BLOCK_TOTAL_SIZE   (MEMORY_POOL_BLOCK_HEADER_SIZE + MEMORY_POOL_BLOCK_SIZE)

#define MEMORY_POOL_FLAGS_OFFSET_FROM_BLOCK 0
#define MEMORY_POOL_DATA_OFFSET_FROM_BLOCK  MEMORY_POOL_BLOCK_FLAGS_SIZE
#define MEMORY_POOL_FLAGS_OFFSET_FROM_DATA  -MEMORY_POOL_BLOCK_FLAGS_SIZE
#define MEMORY_POOL_BLOCK_OFFSET_FROM_DATA  -MEMORY_POOL_BLOCK_HEADER_SIZE

#define memPoolBlockGetFlags(block)    *((byte_t *)block + MEMORY_POOL_FLAGS_OFFSET_FROM_BLOCK)
#define memPoolBlockGetNextFree(block) *((byte_t **)((byte_t *)block + MEMORY_POOL_DATA_OFFSET_FROM_BLOCK))
#define memPoolBlockGetData(block)      ((byte_t *)block + MEMORY_POOL_DATA_OFFSET_FROM_BLOCK)

#define memPoolDataGetFlags(data)    *((byte_t *)data + MEMORY_POOL_FLAGS_OFFSET_FROM_DATA)
#define memPoolDataGetNextFree(data) *((byte_t **)data)
#define memPoolDataGetBlock(data)     ((byte_t *)data + MEMORY_POOL_BLOCK_OFFSET_FROM_DATA)

#ifdef MEMORY_POOL_LEAN
	#define MEMORY_POOL_ALIGN(x) x
#else
	#define MEMORY_POOL_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	size_t block;  // Block size.
	byte_t *free;  // Next free block pointer.
	memoryRegion *region;  // Pointer to the allocator's memory region.
} memoryPool;

#define memPoolBlockSize(bytes) MEMORY_POOL_ALIGN((bytes > MEMORY_POOL_BLOCK_SIZE ? bytes : MEMORY_POOL_BLOCK_SIZE) + MEMORY_POOL_BLOCK_HEADER_SIZE)
#define memPoolBlockSizeUnaligned(bytes)         ((bytes > MEMORY_POOL_BLOCK_SIZE ? bytes : MEMORY_POOL_BLOCK_SIZE) + MEMORY_POOL_BLOCK_HEADER_SIZE)
#define memPoolAlignStartBlock(start) ((byte_t *)MEMORY_POOL_ALIGN((uintptr_t)start + MEMORY_POOL_BLOCK_HEADER_SIZE) - MEMORY_POOL_BLOCK_HEADER_SIZE)
#define memPoolAlignStartData(start)  ((byte_t *)MEMORY_POOL_ALIGN((uintptr_t)start + MEMORY_POOL_BLOCK_HEADER_SIZE))
#define memPoolAllocationSize(start, bytes, length) \
	(memPoolBlockSize(bytes) * length + (uintptr_t)memPoolAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))
	// The following can save small amounts of memory but can't be predicted as easily:
	//(memPoolBlockSize(bytes) * (length - 1) + memPoolBlockSizeUnaligned(bytes) + (uintptr_t)memPoolAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))

#define memPoolFirst(pool)        ((void *)memPoolAlignStartData(pool->region->start))
#define memPoolBlockStatus(block) memPoolBlockGetFlags(block)
#define memPoolBlockNext(pool, i) i += pool->block
#define memPoolEnd(pool)          ((byte_t *)pool->region)
#define memPoolChunkNext(pool)    pool->region->next

void memPoolInit(memoryPool *pool);
void *memPoolCreate(memoryPool *pool, void *start, const size_t bytes, const size_t length);
void *memPoolAllocate(memoryPool *pool);
void memPoolFree(memoryPool *pool, void *block);
void *memPoolSetupMemory(void *start, const size_t bytes, const size_t length);
void memPoolClear(memoryPool *pool);
void *memPoolExtend(memoryPool *pool, void *start, const size_t bytes, const size_t length);

#endif
