#ifndef MEMORYLIST_H
#define MEMORYLIST_H

#include "memoryShared.h"

/*
** Free-list allocator.
*/

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

#define memListFirst(list)        ((void *)memListAlignStartData((region)->start))
#define memListBlockNext(list, i) i = (void *)((byte_t *)i + (list).block)

void memListInit(memoryList *list);
void *memListCreate(memoryList *list, void *start, const size_t bytes, const size_t length);
void *memListAllocate(memoryList *list);
void memListFree(memoryList *list, void *block);
void *memListSetupMemory(void *start, const size_t bytes, const size_t length);
void *memListIndex(memoryList *list, const size_t i);
void memListClear(memoryList *list);
void *memListExtend(memoryList *list, void *start, const size_t bytes, const size_t length);
void memListDelete(memoryList *list);

#endif
