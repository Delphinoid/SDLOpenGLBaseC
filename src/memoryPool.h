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
** The flag may have three potential
** states:
** MEMORY_POOL_BLOCK_ACTIVE   - Currently in use.
** MEMORY_POOL_BLOCK_INACTIVE - Currently not in use.
** MEMORY_POOL_BLOCK_INVALID  - End of the pool.
**
** Free-list pointers point to the
** data, not the beginning of the block.
*/

/**
*** Instead of using an "active" flag,
*** could we use pointers to the next
*** active member?
**/

#define MEMORY_POOL_BLOCK_ACTIVE   (byte_t)0x00
#define MEMORY_POOL_BLOCK_INACTIVE (byte_t)0x01
#define MEMORY_POOL_BLOCK_INVALID  (byte_t)0x02

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

#define memPoolBlockSize(bytes) MEMORY_POOL_ALIGN((bytes > MEMORY_POOL_BLOCK_SIZE ? (bytes + MEMORY_POOL_BLOCK_HEADER_SIZE) : MEMORY_POOL_BLOCK_TOTAL_SIZE))
#define memPoolBlockSizeUnaligned(bytes)          (bytes > MEMORY_POOL_BLOCK_SIZE ? (bytes + MEMORY_POOL_BLOCK_HEADER_SIZE) : MEMORY_POOL_BLOCK_TOTAL_SIZE)
#ifndef MEMORY_ALLOCATOR_ALIGNED
	#define memPoolAlignStartBlock(start) ((byte_t *)MEMORY_POOL_ALIGN((uintptr_t)start + MEMORY_POOL_BLOCK_HEADER_SIZE) - MEMORY_POOL_BLOCK_HEADER_SIZE)
	#define memPoolAlignStartData(start)  ((byte_t *)MEMORY_POOL_ALIGN((uintptr_t)start + MEMORY_POOL_BLOCK_HEADER_SIZE))
#else
	#define memPoolAlignStartBlock(start) start
	#define memPoolAlignStartData(start)  ((byte_t *)start + MEMORY_POOL_BLOCK_HEADER_SIZE)
#endif
#define memPoolAllocationSize(start, bytes, length) \
	(memPoolBlockSize(bytes) * length + (uintptr_t)memPoolAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))
	// The following can save small amounts of memory but can't be predicted as easily:
	//(memPoolBlockSize(bytes) * (length - 1) + memPoolBlockSizeUnaligned(bytes) + (uintptr_t)memPoolAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))

#define memPoolFirst(region)          ((void *)memPoolAlignStartData((region)->start))
#define memPoolBlockStatus(block)     memPoolDataGetFlags(block)
#define memPoolBlockNext(pool, i)     (void *)((byte_t *)i + (pool).block)
#define memPoolBlockPrevious(pool, i) (void *)((byte_t *)i - (pool).block)

void memPoolInit(memoryPool *const restrict pool);
void *memPoolCreate(memoryPool *const restrict pool, void *start, const size_t bytes, const size_t length);
void *memPoolAllocate(memoryPool *const restrict pool);
void memPoolFree(memoryPool *const restrict pool, void *const block);
void *memPoolSetupMemory(void *start, const size_t bytes, const size_t length);
void *memPoolIndex(memoryPool *const restrict pool, const size_t i);
void memPoolClear(memoryPool *const restrict pool);
void *memPoolExtend(memoryPool *const restrict pool, void *const start, const size_t bytes, const size_t length);
void memPoolDelete(memoryPool *const restrict pool);

#define MEMORY_POOL_LOOP_BEGIN(allocator, n, type)                 \
	{                                                              \
		const memoryRegion *__region_##n = allocator.region;       \
		do {                                                       \
			type n = memPoolFirst(__region_##n);                   \
			while(n < (type)memAllocatorEnd(__region_##n)){        \
				const byte_t __flag_##n = memPoolBlockStatus(n);   \
				if(__flag_##n == MEMORY_POOL_BLOCK_ACTIVE){

#define MEMORY_POOL_LOOP_END(allocator, n, earlyexit)              \
				}else if(__flag_##n == MEMORY_POOL_BLOCK_INVALID){ \
					earlyexit                                      \
				}                                                  \
				n = memPoolBlockNext(allocator, n);                \
			}                                                      \
			__region_##n = memAllocatorNext(__region_##n);         \
		} while(__region_##n != NULL);                             \
	}

#define MEMORY_POOL_OFFSET_LOOP_BEGIN(allocator, n, type, region, block) \
	{                                                                    \
		const memoryRegion *__region_##n = region;                       \
		type n = (type)(block);                                          \
		for(;;){                                                         \
			while(n < (type)memAllocatorEnd(__region_##n)){              \
				const byte_t __flag_##n = memPoolBlockStatus(n);         \
				if(__flag_##n == MEMORY_POOL_BLOCK_ACTIVE){

#define MEMORY_POOL_OFFSET_LOOP_END(allocator, n, earlyexit)             \
				}else if(__flag_##n == MEMORY_POOL_BLOCK_INVALID){       \
					earlyexit                                            \
				}                                                        \
				n = memPoolBlockNext(allocator, n);                      \
			}                                                            \
			__region_##n = memAllocatorNext(__region_##n);               \
			if(__region_##n == NULL){                                    \
				break;                                                   \
			}                                                            \
			n = memPoolFirst(__region_##n);                              \
		}                                                                \
	}

#endif
