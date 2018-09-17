#ifndef MEMORYDLINK_H
#define MEMORYDLINK_H

#include "memoryShared.h"

/*
** Doubly-linked list allocator.
**
** Effectively a free-list allocator
** but each block has pointers to the
** next and previous elements in the
** array it is a part of, contained
** in a header.
**
** Free-list pointers point to the
** data, not the beginning of the block.
**
** It can also be treated as an object
** pool if necessary.
*/

#define MEMORY_DLINK_BLOCK_ACTIVE        (uintptr_t)0x00
#define MEMORY_DLINK_BLOCK_INACTIVE      (uintptr_t)0x01
#define MEMORY_DLINK_BLOCK_INACTIVE_MASK (uintptr_t)0x01
#define MEMORY_DLINK_BLOCK_ACTIVE_MASK   (uintptr_t)~MEMORY_DLINK_BLOCK_INACTIVE_MASK

#define MEMORY_DLINK_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_DLINK_BLOCK_FLAGS_SIZE   sizeof(uintptr_t)
#define MEMORY_DLINK_BLOCK_PREV_SIZE    MEMORY_DLINK_BLOCK_POINTER_SIZE
#define MEMORY_DLINK_BLOCK_NEXT_SIZE    MEMORY_DLINK_BLOCK_POINTER_SIZE
#define MEMORY_DLINK_BLOCK_HEADER_SIZE  (MEMORY_DLINK_BLOCK_NEXT_SIZE + MEMORY_DLINK_BLOCK_PREV_SIZE)
#define MEMORY_DLINK_BLOCK_SIZE         MEMORY_DLINK_BLOCK_POINTER_SIZE
#define MEMORY_DLINK_BLOCK_TOTAL_SIZE   (MEMORY_DLINK_BLOCK_HEADER_SIZE + MEMORY_DLINK_BLOCK_SIZE)

#define MEMORY_DLINK_NEXT_OFFSET_FROM_BLOCK  0
#define MEMORY_DLINK_PREV_OFFSET_FROM_BLOCK  MEMORY_DLINK_BLOCK_PREV_SIZE
#define MEMORY_DLINK_FLAGS_OFFSET_FROM_BLOCK MEMORY_DLINK_NEXT_OFFSET_FROM_BLOCK
#define MEMORY_DLINK_DATA_OFFSET_FROM_BLOCK  MEMORY_DLINK_BLOCK_HEADER_SIZE
#define MEMORY_DLINK_NEXT_OFFSET_FROM_DATA   (-MEMORY_DLINK_BLOCK_NEXT_SIZE - MEMORY_DLINK_BLOCK_PREV_SIZE)
#define MEMORY_DLINK_PREV_OFFSET_FROM_DATA   -MEMORY_DLINK_BLOCK_NEXT_SIZE
#define MEMORY_DLINK_FLAGS_OFFSET_FROM_DATA  MEMORY_DLINK_NEXT_OFFSET_FROM_DATA
#define MEMORY_DLINK_BLOCK_OFFSET_FROM_DATA  -MEMORY_DLINK_BLOCK_HEADER_SIZE

#define memDLinkBlockGetFlags(block)      *((uintptr_t *)((byte_t *)block + MEMORY_DLINK_FLAGS_OFFSET_FROM_BLOCK))
#define memDLinkBlockGetNext(block)       *((byte_t **)((byte_t *)block + MEMORY_DLINK_NEXT_OFFSET_FROM_BLOCK))
#define memDLinkBlockGetNextPointer(block) ((byte_t **)((byte_t *)block + MEMORY_DLINK_NEXT_OFFSET_FROM_BLOCK))
#define memDLinkBlockGetPrev(block)       *((byte_t **)((byte_t *)block + MEMORY_DLINK_PREV_OFFSET_FROM_BLOCK))
#define memDLinkBlockGetPrevPointer(block) ((byte_t **)((byte_t *)block + MEMORY_DLINK_PREV_OFFSET_FROM_BLOCK))
#define memDLinkBlockGetNextFree(block)   *((byte_t **)((byte_t *)block + MEMORY_DLINK_DATA_OFFSET_FROM_BLOCK))
#define memDLinkBlockGetData(block)        ((byte_t *)block + MEMORY_DLINK_DATA_OFFSET_FROM_BLOCK)

#define memDLinkBlockGetActive(block)        *((uintptr_t *)((byte_t *)block + MEMORY_DLINK_FLAGS_OFFSET_FROM_BLOCK))
#define memDLinkBlockGetActiveMasked(block) (*((uintptr_t *)((byte_t *)block + MEMORY_DLINK_FLAGS_OFFSET_FROM_BLOCK)) & MEMORY_DLINK_BLOCK_INACTIVE_MASK)
#define memDLinkBlockGetActivePointer(block)  ((uintptr_t *)((byte_t *)block + MEMORY_DLINK_FLAGS_OFFSET_FROM_BLOCK))
#define memDLinkBlockSetActive(block)        *((uintptr_t *)((byte_t *)block + MEMORY_DLINK_FLAGS_OFFSET_FROM_BLOCK)) &= MEMORY_DLINK_BLOCK_ACTIVE_MASK
#define memDLinkBlockSetInactive(block)      *((uintptr_t *)((byte_t *)block + MEMORY_DLINK_FLAGS_OFFSET_FROM_BLOCK)) |= MEMORY_DLINK_BLOCK_INACTIVE_MASK

#define memDLinkDataGetFlags(data)      *((uintptr_t *)((byte_t *)data + MEMORY_DLINK_FLAG_OFFSET_FROM_DATA))
#define memDLinkDataGetNext(data)       *((byte_t **)((byte_t *)data + MEMORY_DLINK_NEXT_OFFSET_FROM_DATA))
#define memDLinkDataGetNextPointer(data) ((byte_t **)((byte_t *)data + MEMORY_DLINK_NEXT_OFFSET_FROM_DATA))
#define memDLinkDataGetPrev(data)       *((byte_t **)((byte_t *)data + MEMORY_DLINK_PREV_OFFSET_FROM_DATA))
#define memDLinkDataGetPrevPointer(data) ((byte_t **)((byte_t *)data + MEMORY_DLINK_PREV_OFFSET_FROM_DATA))
#define memDLinkDataGetNextFree(data)   *((byte_t **)data)
#define memDLinkDataGetBlock(data)       ((byte_t *)data + MEMORY_DLINK_BLOCK_OFFSET_FROM_DATA)

#define memDLinkDataGetActive(data)        *((uintptr_t *)((byte_t *)data + MEMORY_DLINK_FLAGS_OFFSET_FROM_DATA))
#define memDLinkDataGetActiveMasked(data) (*((uintptr_t *)((byte_t *)data + MEMORY_DLINK_FLAGS_OFFSET_FROM_DATA)) & MEMORY_DLINK_BLOCK_INACTIVE_MASK)
#define memDLinkDataGetActivePointer(data)  ((uintptr_t *)((byte_t *)data + MEMORY_DLINK_FLAGS_OFFSET_FROM_DATA))
#define memDLinkDataSetActive(data)        *((uintptr_t *)((byte_t *)data + MEMORY_DLINK_FLAGS_OFFSET_FROM_DATA)) &= MEMORY_DLINK_BLOCK_ACTIVE_MASK
#define memDLinkDataSetInactive(data)      *((uintptr_t *)((byte_t *)data + MEMORY_DLINK_FLAGS_OFFSET_FROM_DATA)) |= MEMORY_DLINK_BLOCK_INACTIVE_MASK

#ifdef MEMORY_DLINK_LEAN
	#define MEMORY_DLINK_ALIGN(x) x
#else
	#define MEMORY_DLINK_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	size_t block;  // Block size.
	byte_t *free;  // Next free block pointer.
	memoryRegion *region;
} memoryDLink;

#define memDLinkBlockSize(bytes) MEMORY_DLINK_ALIGN((bytes > MEMORY_DLINK_BLOCK_SIZE ? bytes : MEMORY_DLINK_BLOCK_SIZE) + MEMORY_DLINK_BLOCK_HEADER_SIZE)
#define memDLinkBlockSizeUnaligned(bytes)          ((bytes > MEMORY_DLINK_BLOCK_SIZE ? bytes : MEMORY_DLINK_BLOCK_SIZE) + MEMORY_DLINK_BLOCK_HEADER_SIZE)
#define memDLinkAlignStartBlock(start) ((byte_t *)MEMORY_DLINK_ALIGN((uintptr_t)start + MEMORY_DLINK_BLOCK_HEADER_SIZE) - MEMORY_DLINK_BLOCK_HEADER_SIZE)
#define memDLinkAlignStartData(start)  ((byte_t *)MEMORY_DLINK_ALIGN((uintptr_t)start + MEMORY_DLINK_BLOCK_HEADER_SIZE))
#define memDLinkAllocationSize(start, bytes, length) \
	(memDLinkBlockSize(bytes) * length + (uintptr_t)memDLinkAlignStartBlock(start) - (uintptr_t)start)
	// The following can save small amounts of memory but can't be predicted as easily:
	//(memDLinkBlockSize(bytes) * (length - 1) + memDLinkBlockSizeUnaligned(bytes) + (uintptr_t)memDLinkAlignStartBlock(start) - (uintptr_t)start)

#define memDLinkFirst(array)        ((void *)memDLinkAlignStartData(array->start))
#define memDLinkPrev(i)             i = memDLinkDataGetPrev(i)
#define memDLinkNext(i)             i = memDLinkDataGetNext(i)
#define memDLinkBlockStatus(block)  memDLinkDataGetActiveMasked(block)
#define memDLinkBlockNext(array, i) i += array->block
#define memDLinkEnd(array)          ((byte_t *)array->region)
#define memSLinkChunkNext(array)    array->region->next

void memDLinkInit(memoryDLink *array);
void *memDLinkCreate(memoryDLink *array, void *start, const size_t bytes, const size_t length);
void *memDLinkAllocate(memoryDLink *array);
void *memDLinkPrepend(memoryDLink *array, void **start);
void *memDLinkAppend(memoryDLink *array, void **start);
void *memDLinkInsertBefore(memoryDLink *array, void *element);
void *memDLinkInsertAfter(memoryDLink *array, void *element);
void memDLinkFree(memoryDLink *array, void *element);
void *memDLinkSetupMemory(void *start, const size_t bytes, const size_t length);
void memDLinkClear(memoryDLink *array);
void *memDLinkExtend(memoryDLink *array, void *start, const size_t bytes, const size_t length);

#endif
