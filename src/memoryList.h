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
	byte_t *next;  // Next free block pointer.
	byte_t *start;
	byte_t *end;
} memoryList;

#define memListBlockSize(bytes) MEMORY_LIST_ALIGN((bytes > MEMORY_LIST_BLOCK_SIZE ? bytes : MEMORY_LIST_BLOCK_SIZE))
#define memListBlockSizeUnaligned(bytes)         ((bytes > MEMORY_LIST_BLOCK_SIZE ? bytes : MEMORY_LIST_BLOCK_SIZE))
#define memListAlignStartBlock(start) ((byte_t *)MEMORY_LIST_ALIGN((uintptr_t)start + MEMORY_LIST_BLOCK_HEADER_SIZE) - MEMORY_LIST_BLOCK_HEADER_SIZE)
#define memListAlignStartData(start)  ((byte_t *)MEMORY_LIST_ALIGN((uintptr_t)start + MEMORY_LIST_BLOCK_HEADER_SIZE))
#define memListAllocationSize(start, bytes, length) \
	(memListBlockSize(bytes) * length + (uintptr_t)memListAlignStartBlock(start) - (uintptr_t)start)
	// The following can save small amounts of memory but can't be predicted as easily:
	//(memListBlockSize(bytes) * (length - 1) + memListBlockSizeUnaligned(bytes) + (uintptr_t)memListAlignStartBlock(start) - (uintptr_t)start)

#define memListAppend(list, new) list->next = new->next; new->next = NULL

#define memListFirst(list)        ((void *)memListAlignStartData(list->start))
#define memListBlockNext(list, i) i += list->block
#define memListEnd(list)          list->end

void *memListInit(memoryList *list, void *start, const size_t bytes, const size_t length);
void *memListAllocate(memoryList *list);
void memListFree(memoryList *list, void *block);
void *memListReset(void *start, const size_t bytes, const size_t length);
void memListClear(memoryList *list);

#endif
