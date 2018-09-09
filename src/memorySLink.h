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
** Free-list pointers point to the
** data, not the beginning of the block.
**
** It can also be treated as an object
** pool if necessary.
*/

#define MEMORY_SLINK_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_SLINK_BLOCK_FLAG_SIZE    sizeof(uintptr_t)
#define MEMORY_SLINK_BLOCK_NEXT_SIZE    MEMORY_SLINK_BLOCK_POINTER_SIZE
#define MEMORY_SLINK_BLOCK_HEADER_SIZE  MEMORY_SLINK_BLOCK_NEXT_SIZE
#define MEMORY_SLINK_BLOCK_SIZE         MEMORY_SLINK_BLOCK_POINTER_SIZE
#define MEMORY_SLINK_BLOCK_TOTAL_SIZE   MEMORY_SLINK_BLOCK_HEADER_SIZE + MEMORY_SLINK_BLOCK_SIZE

#define MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK 0
#define MEMORY_SLINK_DATA_OFFSET_FROM_BLOCK MEMORY_SLINK_BLOCK_HEADER_SIZE
#define MEMORY_SLINK_NEXT_OFFSET_FROM_DATA  -MEMORY_SLINK_BLOCK_NEXT_SIZE
#define MEMORY_SLINK_BLOCK_OFFSET_FROM_DATA -MEMORY_SLINK_BLOCK_HEADER_SIZE
#define MEMORY_SLINK_FLAG_OFFSET_FROM_DATA  -MEMORY_SLINK_BLOCK_FLAG_SIZE

#define memSLinkBlockGetFlags(block)      *((uintptr_t *)(block + MEMORY_SLINK_FLAG_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetNext(block)       *((byte_t **)(block + MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetNextPointer(block) ((byte_t **)(block + MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetNextFree(block)   *((byte_t **)(block + MEMORY_SLINK_DATA_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetData(block)        ((byte_t *)(block + MEMORY_SLINK_DATA_OFFSET_FROM_BLOCK))

#define memSLinkDataGetFlags(data)      *((uintptr_t *)(data + MEMORY_SLINK_FLAG_OFFSET_FROM_DATA))
#define memSLinkDataGetNext(data)       *((byte_t **)(data + MEMORY_SLINK_NEXT_OFFSET_FROM_DATA))
#define memSLinkDataGetNextPointer(data) ((byte_t **)(data + MEMORY_SLINK_NEXT_OFFSET_FROM_DATA))
#define memSLinkDataGetNextFree(data)   *((byte_t **)data)
#define memSLinkDataGetBlock(data)       ((byte_t *)(data + MEMORY_SLINK_BLOCK_OFFSET_FROM_DATA))

#ifdef MEMORY_SLINK_LEAN
	#define MEMORY_SLINK_ALIGN(x) x
#else
	#define MEMORY_SLINK_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	size_t block;  // Block size.
	byte_t *next;  // Next free block pointer.
	byte_t *start;
	byte_t *end;
} memorySLink;

#define memSLinkBlockSize(bytes) MEMORY_SLINK_ALIGN((bytes > MEMORY_SLINK_BLOCK_SIZE ? bytes : MEMORY_SLINK_BLOCK_SIZE) + MEMORY_SLINK_BLOCK_HEADER_SIZE)
#define memSLinkBlockSizeUnaligned(bytes)          ((bytes > MEMORY_SLINK_BLOCK_SIZE ? bytes : MEMORY_SLINK_BLOCK_SIZE) + MEMORY_SLINK_BLOCK_HEADER_SIZE)
#define memSLinkAlignStart(start) MEMORY_SLINK_ALIGN((uintptr_t)start + MEMORY_SLINK_BLOCK_HEADER_SIZE)
#define memSLinkAllocationSize(start, bytes, length) \
	(memSLinkBlockSize(bytes) * (length - 1) + memSLinkBlockSizeUnaligned(bytes) + memSLinkAlignStart(start) - (uintptr_t)start)

#define memSLinkAppend(array, new) array->next = new->next; new->next = NULL

#define memSLinkStart(array)        array->start
#define memSLinkNext(i)             i = memSLinkDataGetNext(i)
#define memSLinkBlockStatus(block)  memSLinkDataGetFlags(block)
#define memSLinkBlockNext(array, i) i += array->block
#define memSLinkEnd(array)          array->end

byte_t *memSLinkInit(memorySLink *array, byte_t *start, const size_t bytes, const size_t length);
byte_t *memSLinkAllocate(memorySLink *array);
byte_t *memSLinkInsertBefore(memorySLink *array, byte_t *element, byte_t *previous);
byte_t *memSLinkInsertAfter(memorySLink *array, byte_t *element);
void memSLinkFree(memorySLink *array, byte_t *element, byte_t *previous);
byte_t *memSLinkReset(byte_t *start, const size_t bytes, const size_t length);
void memSLinkClear(memorySLink *array);

#endif
