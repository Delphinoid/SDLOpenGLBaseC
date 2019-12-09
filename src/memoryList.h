#ifndef MEMORYLIST_H
#define MEMORYLIST_H

#include "memoryShared.h"

// Free-list allocator.
//
// Also defines list pool functions,
// which differ only in that they
// store the free-list pointer at the
// end of the block, allowing active
// flags to be stored at the beginning.
//
// Block format:
// [ Data (or free-list pointer) ]

#define MEMORY_LIST_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_LIST_BLOCK_HEADER_SIZE  0
#define MEMORY_LIST_BLOCK_SIZE         (MEMORY_LIST_BLOCK_HEADER_SIZE + MEMORY_LIST_BLOCK_POINTER_SIZE)

#define memListBlockGetNextFree(block) *((byte_t **)block)
#define memListBlockGetData(block)      ((byte_t *)block)

#define memListDataGetNextFree(data) *((byte_t **)data)
#define memListDataGetBlock(data)     ((byte_t *)data))

#ifdef MEMORY_LIST_LEAN
	#define MEMORY_LIST_ALIGN(x) x
#else
	#define MEMORY_LIST_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	size_t block;  // Block size.
	byte_t *free;  // Next free block pointer.
	memoryRegion *region;  // Pointer to the allocator's memory region.
} memoryList;

#define memListBlockSize(bytes) MEMORY_LIST_ALIGN((bytes > MEMORY_LIST_BLOCK_SIZE ? bytes : MEMORY_LIST_BLOCK_SIZE))
#define memListBlockSizeUnaligned(bytes)         ((bytes > MEMORY_LIST_BLOCK_SIZE ? bytes : MEMORY_LIST_BLOCK_SIZE))
#ifndef MEMORY_ALLOCATOR_ALIGNED
	#define memListAlignStartBlock(start) ((byte_t *)MEMORY_LIST_ALIGN((uintptr_t)start + MEMORY_LIST_BLOCK_HEADER_SIZE) - MEMORY_LIST_BLOCK_HEADER_SIZE)
	#define memListAlignStartData(start)  ((byte_t *)MEMORY_LIST_ALIGN((uintptr_t)start + MEMORY_LIST_BLOCK_HEADER_SIZE))
#else
	#define memListAlignStartBlock(start) start
	#define memListAlignStartData(start)  ((byte_t *)start + MEMORY_LIST_BLOCK_HEADER_SIZE)
#endif
#define memListAllocationSize(start, bytes, length) \
	(memListBlockSize(bytes) * length + (uintptr_t)memListAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))
	// The following can save small amounts of memory but can't be predicted as easily:
	//(memListBlockSize(bytes) * (length - 1) + memListBlockSizeUnaligned(bytes) + (uintptr_t)memListAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))

#define memListFirst(region)          ((void *)memListAlignStartData((region)->start))
#define memListBlockNext(list, i)     (void *)((byte_t *)i + (list).block)
#define memListBlockPrevious(list, i) (void *)((byte_t *)i - (list).block)

void memListInit(memoryList *const restrict list);
void *memListCreate(memoryList *const restrict list, void *const start, const size_t bytes, const size_t length);
void *memListCreateInit(memoryList *const restrict list, void *const start, const size_t bytes, const size_t length, void (*func)(void *const restrict block));
void *memListAllocate(memoryList *const restrict list);
void memListFree(memoryList *const restrict list, void *const block);
void *memListSetupMemory(void *start, const size_t bytes, const size_t length);
void *memListSetupMemoryInit(void *start, const size_t bytes, const size_t length, void (*func)(void *const restrict block));
void *memListIndex(memoryList *const restrict list, const size_t i);
void *memListIndexRegion(memoryList *const restrict list, const size_t i, memoryRegion **const container);
void memListClear(memoryList *const restrict list);
void memListClearInit(memoryList *const restrict list, void (*func)(void *const restrict block));
void *memListExtend(memoryList *const restrict list, void *const start, const size_t bytes, const size_t length);
void *memListExtendInit(memoryList *const restrict list, void *const start, const size_t bytes, const size_t length, void (*func)(void *const restrict block));
void memListDelete(memoryList *const restrict list);

#define MEMORY_LIST_LOOP_BEGIN(allocator, n, type)           \
	{                                                        \
		const memoryRegion *__region_##n = allocator.region; \
		do {                                                 \
			type n = memListFirst(__region_##n);             \
			while(n < (type)memAllocatorEnd(__region_##n)){  \

#define MEMORY_LIST_LOOP_END(allocator, n)                   \
				n = memListBlockNext(allocator, n);          \
			}                                                \
			__region_##n = memAllocatorNext(__region_##n);   \
		} while(__region_##n != NULL);                       \
	}

#define MEMORY_LIST_OFFSET_LOOP_BEGIN(allocator, n, type, region) \
	{                                                             \
		const memoryRegion *__region_##n = region;                \
		do {                                                      \
			type n = memListFirst(__region_##n);                  \
			while(n < (type)memAllocatorEnd(__region_##n)){       \

#define MEMORY_LIST_OFFSET_LOOP_END(allocator, n)                 \
				n = memListBlockNext(allocator, n);               \
			}                                                     \
			__region_##n = memAllocatorNext(__region_##n);        \
		} while(__region_##n != NULL);                            \
	}

#endif
