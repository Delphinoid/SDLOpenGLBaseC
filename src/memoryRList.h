#ifndef MEMORYRLIST_H
#define MEMORYRLIST_H

#include "memoryShared.h"
#include <stdint.h>

// Reverse free-list allocator.
//
// Exactly the same as the regular free-list
// allocator but with the free block pointer
// stored at the end of the block.

#define MEMORY_RLIST_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_RLIST_BLOCK_HEADER_SIZE  0
#define MEMORY_RLIST_BLOCK_SIZE         (MEMORY_RLIST_BLOCK_HEADER_SIZE + MEMORY_RLIST_BLOCK_POINTER_SIZE)

#define memRListBlockGetNextFree(block) *((byte_t **)block)
#define memRListBlockGetData(block)      ((byte_t *)block)

#define memRListDataGetPrevFree(data)       *((byte_t **)((byte_t *)data - MEMORY_RLIST_BLOCK_POINTER_SIZE))
#define memRListDataGetNextFree(data, size) *((byte_t **)((byte_t *)data + size - MEMORY_RLIST_BLOCK_POINTER_SIZE))
#define memRListDataGetBlock(data)           ((byte_t *)data))

#ifdef MEMORY_RLIST_LEAN
	#define MEMORY_RLIST_ALIGN(x) x
#else
	#define MEMORY_RLIST_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	size_t block;  // Block size.
	byte_t *free;  // Next free block pointer.
	memoryRegion *region;  // Pointer to the allocator's memory region.
} memoryRList;

#define memRListBlockSize(bytes) MEMORY_RLIST_ALIGN((bytes > MEMORY_RLIST_BLOCK_SIZE ? bytes : MEMORY_RLIST_BLOCK_SIZE))
#define memRListBlockSizeUnaligned(bytes)          ((bytes > MEMORY_RLIST_BLOCK_SIZE ? bytes : MEMORY_RLIST_BLOCK_SIZE))
#ifndef MEMORY_ALLOCATOR_ALIGNED
	#define memRListAlignStartBlock(start) ((byte_t *)MEMORY_RLIST_ALIGN((uintptr_t)start + MEMORY_RLIST_BLOCK_HEADER_SIZE) - MEMORY_RLIST_BLOCK_HEADER_SIZE)
	#define memRListAlignStartData(start)  ((byte_t *)MEMORY_RLIST_ALIGN((uintptr_t)start + MEMORY_RLIST_BLOCK_HEADER_SIZE))
#else
	#define memRListAlignStartBlock(start) start
	#define memRListAlignStartData(start)  ((byte_t *)start + MEMORY_RLIST_BLOCK_HEADER_SIZE)
#endif
#define memRListAllocationSize(start, bytes, length) \
	(memRListBlockSize(bytes) * length + (uintptr_t)memRListAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))
	// The following can save small amounts of memory but can't be predicted as easily:
	//(memRListBlockSize(bytes) * (length - 1) + memRListBlockSizeUnaligned(bytes) + (uintptr_t)memRListAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))

#define memRListFirst(region)          ((void *)memRListAlignStartData((region)->start))
#define memRListBlockNext(list, i)     (void *)((byte_t *)i + (list).block)
#define memRListBlockPrevious(list, i) (void *)((byte_t *)i - (list).block)

void memRListInit(memoryRList *const __RESTRICT__ list);
void *memRListCreate(memoryRList *const __RESTRICT__ list, void *const start, const size_t bytes, const size_t length);
void *memRListCreateInit(memoryRList *const __RESTRICT__ list, void *const start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block));
void *memRListAllocate(memoryRList *const __RESTRICT__ list);
void memRListFree(memoryRList *const __RESTRICT__ list, void *const block);
void *memRListSetupMemory(void *start, const size_t bytes, const size_t length);
void *memRListSetupMemoryInit(void *start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block));
void *memRListIndex(memoryRList *const __RESTRICT__ list, const size_t i);
void *memRListIndexRegion(memoryRList *const __RESTRICT__ list, const size_t i, memoryRegion **const container);
void memRListClear(memoryRList *const __RESTRICT__ list);
void memRListClearInit(memoryRList *const __RESTRICT__ list, void (*func)(void *const __RESTRICT__ block));
void *memRListExtend(memoryRList *const __RESTRICT__ list, void *const start, const size_t bytes, const size_t length);
void *memRListExtendInit(memoryRList *const __RESTRICT__ list, void *const start, const size_t bytes, const size_t length, void (*func)(void *const __RESTRICT__ block));
void memRListDelete(memoryRList *const __RESTRICT__ list);

#define MEMORY_RLIST_LOOP_BEGIN(allocator, n, type)          \
	{                                                        \
		const memoryRegion *__region_##n = allocator.region; \
		do {                                                 \
			type n = memRListFirst(__region_##n);            \
			while(n < (type)memAllocatorEnd(__region_##n)){  \

#define MEMORY_RLIST_LOOP_END(allocator, n)                  \
				n = memRListBlockNext(allocator, n);         \
			}                                                \
			__region_##n = memAllocatorNext(__region_##n);   \
		} while(__region_##n != NULL);                       \
	}

#define MEMORY_RLIST_OFFSET_LOOP_BEGIN(allocator, n, type, region) \
	{                                                              \
		const memoryRegion *__region_##n = region;                 \
		do {                                                       \
			type n = memRListFirst(__region_##n);                  \
			while(n < (type)memAllocatorEnd(__region_##n)){        \

#define MEMORY_RLIST_OFFSET_LOOP_END(allocator, n)                 \
				n = memRListBlockNext(allocator, n);               \
			}                                                      \
			__region_##n = memAllocatorNext(__region_##n);         \
		} while(__region_##n != NULL);                             \
	}

#endif
