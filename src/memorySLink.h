#ifndef MEMORYSLINK_H
#define MEMORYSLINK_H

#include "memoryShared.h"

/*
** Singly-linked list allocator.
**
** Effectively a free-list allocator
** but each block has a pointer to the
** next element in the array it is a
** part of, contained in a header.
**
** If the block is inactive, the pointer
** will contain a 1 in its LSB.
**
** Free-list pointers point to the
** data, not the beginning of the block.
**
** It can also be treated as an object
** pool if necessary.
*/

#define MEMORY_SLINK_BLOCK_ACTIVE        (uintptr_t)0x00
#define MEMORY_SLINK_BLOCK_INACTIVE      (uintptr_t)0x01
#define MEMORY_SLINK_BLOCK_INACTIVE_MASK (uintptr_t)0x01
#define MEMORY_SLINK_BLOCK_ACTIVE_MASK   (uintptr_t)~MEMORY_SLINK_BLOCK_INACTIVE_MASK

#define MEMORY_SLINK_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_SLINK_BLOCK_FLAGS_SIZE   sizeof(uintptr_t)
#define MEMORY_SLINK_BLOCK_NEXT_SIZE    MEMORY_SLINK_BLOCK_POINTER_SIZE
#define MEMORY_SLINK_BLOCK_HEADER_SIZE  MEMORY_SLINK_BLOCK_NEXT_SIZE
#define MEMORY_SLINK_BLOCK_SIZE         MEMORY_SLINK_BLOCK_POINTER_SIZE
#define MEMORY_SLINK_BLOCK_TOTAL_SIZE   (MEMORY_SLINK_BLOCK_HEADER_SIZE + MEMORY_SLINK_BLOCK_SIZE)

#define MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK 0
#define MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK
#define MEMORY_SLINK_DATA_OFFSET_FROM_BLOCK MEMORY_SLINK_BLOCK_HEADER_SIZE
#define MEMORY_SLINK_NEXT_OFFSET_FROM_DATA  -MEMORY_SLINK_BLOCK_NEXT_SIZE
#define MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA  MEMORY_SLINK_NEXT_OFFSET_FROM_DATA
#define MEMORY_SLINK_BLOCK_OFFSET_FROM_DATA -MEMORY_SLINK_BLOCK_HEADER_SIZE

#define memSLinkBlockGetFlags(block)      *((uintptr_t *)((byte_t *)block + MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetNext(block)       *((byte_t **)((byte_t *)block + MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetNextPointer(block) ((byte_t **)((byte_t *)block + MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetNextFree(block)   *((byte_t **)((byte_t *)block + MEMORY_SLINK_DATA_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetData(block)        ((byte_t *)((byte_t *)block + MEMORY_SLINK_DATA_OFFSET_FROM_BLOCK))

#define memSLinkBlockGetActive(block)        *((uintptr_t *)((byte_t *)block + MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetActiveMasked(block) (*((uintptr_t *)((byte_t *)block + MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK)) & MEMORY_SLINK_BLOCK_INACTIVE_MASK)
#define memSLinkBlockGetActivePointer(block)  ((uintptr_t *)((byte_t *)block + MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK))
#define memSLinkBlockSetActive(block)        *((uintptr_t *)((byte_t *)block + MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK)) &= MEMORY_SLINK_BLOCK_ACTIVE_MASK
#define memSLinkBlockSetInactive(block)      *((uintptr_t *)((byte_t *)block + MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK)) |= MEMORY_SLINK_BLOCK_INACTIVE_MASK

#define memSLinkDataGetFlags(data)      *((uintptr_t *)((byte_t *)data + MEMORY_SLINK_FLAG_OFFSET_FROM_DATA))
#define memSLinkDataGetNext(data)       *((byte_t **)((byte_t *)data + MEMORY_SLINK_NEXT_OFFSET_FROM_DATA))
#define memSLinkDataGetNextPointer(data) ((byte_t **)((byte_t *)data + MEMORY_SLINK_NEXT_OFFSET_FROM_DATA))
#define memSLinkDataGetNextFree(data)   *((byte_t **)data)
#define memSLinkDataGetBlock(data)       ((byte_t *)((byte_t *)data + MEMORY_SLINK_BLOCK_OFFSET_FROM_DATA))

#define memSLinkDataGetActive(data)        *((uintptr_t *)((byte_t *)data + MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA))
#define memSLinkDataGetActiveMasked(data) (*((uintptr_t *)((byte_t *)data + MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA)) & MEMORY_SLINK_BLOCK_INACTIVE_MASK)
#define memSLinkDataGetActivePointer(data)  ((uintptr_t *)((byte_t *)data + MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA))
#define memSLinkDataSetActive(data)        *((uintptr_t *)((byte_t *)data + MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA)) &= MEMORY_SLINK_BLOCK_ACTIVE_MASK
#define memSLinkDataSetInactive(data)      *((uintptr_t *)((byte_t *)data + MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA)) |= MEMORY_SLINK_BLOCK_INACTIVE_MASK

#ifdef MEMORY_SLINK_LEAN
	#define MEMORY_SLINK_ALIGN(x) x
#else
	#define MEMORY_SLINK_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	size_t block;  // Block size.
	byte_t *free;  // Next free block pointer.
	memoryRegion *region;  // Pointer to the allocator's memory region.
} memorySLink;

#define memSLinkBlockSize(bytes) MEMORY_SLINK_ALIGN((bytes > MEMORY_SLINK_BLOCK_SIZE ? bytes : MEMORY_SLINK_BLOCK_SIZE) + MEMORY_SLINK_BLOCK_HEADER_SIZE)
#define memSLinkBlockSizeUnaligned(bytes)          ((bytes > MEMORY_SLINK_BLOCK_SIZE ? bytes : MEMORY_SLINK_BLOCK_SIZE) + MEMORY_SLINK_BLOCK_HEADER_SIZE)
#define memSLinkAlignStartBlock(start) ((byte_t *)MEMORY_SLINK_ALIGN((uintptr_t)start + MEMORY_SLINK_BLOCK_HEADER_SIZE) - MEMORY_SLINK_BLOCK_HEADER_SIZE)
#define memSLinkAlignStartData(start)  ((byte_t *)MEMORY_SLINK_ALIGN((uintptr_t)start + MEMORY_SLINK_BLOCK_HEADER_SIZE))
#define memSLinkAllocationSize(start, bytes, length) \
	(memSLinkBlockSize(bytes) * length + (uintptr_t)memSLinkAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))
	// The following can save small amounts of memory but can't be predicted as easily:
	//(memSLinkBlockSize(bytes) * (length - 1) + memSLinkBlockSizeUnaligned(bytes) + (uintptr_t)memSLinkAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))

#define memSLinkFirst(array)        ((void *)memSLinkAlignStartData(array->region->start))
#define memSLinkNext(i)             i = memSLinkDataGetNext(i)
#define memSLinkBlockStatus(block)  memSLinkDataGetActiveMasked(block)
#define memSLinkBlockNext(array, i) i += array->block
#define memSLinkEnd(array)          ((byte_t *)array->region)
#define memSLinkChunkNext(array)    array->region->next

void memSLinkInit(memorySLink *array);
void *memSLinkCreate(memorySLink *array, void *start, const size_t bytes, const size_t length);
void *memSLinkAllocate(memorySLink *array);
void *memSLinkPrepend(memorySLink *array, void **start);
void *memSLinkAppend(memorySLink *array, void **start);
void *memSLinkInsertBefore(memorySLink *array, void *element, void *previous);
void *memSLinkInsertAfter(memorySLink *array, void *element);
void memSLinkFree(memorySLink *array, void *element, void *previous);
void *memSLinkSetupMemory(void *start, const size_t bytes, const size_t length);
void memSLinkClear(memorySLink *array);
void *memSLinkExtend(memorySLink *array, void *start, const size_t bytes, const size_t length);

#endif
