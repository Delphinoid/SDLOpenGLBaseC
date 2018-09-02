#ifndef MEMORYARRAYSINGLE_H
#define MEMORYARRAYSINGLE_H

#include "memoryShared.h"

/*
** Array allocator (singly-linked).
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

#define MEMORY_ARRAY_SINGLE_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_ARRAY_SINGLE_BLOCK_FLAG_SIZE    sizeof(byte_t)
#define MEMORY_ARRAY_SINGLE_BLOCK_NEXT_SIZE    MEMORY_ARRAY_SINGLE_BLOCK_POINTER_SIZE
#define MEMORY_ARRAY_SINGLE_BLOCK_HEADER_SIZE  MEMORY_ARRAY_SINGLE_BLOCK_NEXT_SIZE
#define MEMORY_ARRAY_SINGLE_BLOCK_SIZE         MEMORY_ARRAY_SINGLE_BLOCK_POINTER_SIZE
#define MEMORY_ARRAY_SINGLE_BLOCK_TOTAL_SIZE   MEMORY_ARRAY_SINGLE_BLOCK_HEADER_SIZE + MEMORY_ARRAY_SINGLE_BLOCK_SIZE

#define MEMORY_ARRAY_SINGLE_NEXT_OFFSET_FROM_BLOCK 0
#define MEMORY_ARRAY_SINGLE_DATA_OFFSET_FROM_BLOCK MEMORY_ARRAY_SINGLE_BLOCK_HEADER_SIZE
#define MEMORY_ARRAY_SINGLE_NEXT_OFFSET_FROM_DATA  -MEMORY_ARRAY_SINGLE_BLOCK_NEXT_SIZE
#define MEMORY_ARRAY_SINGLE_BLOCK_OFFSET_FROM_DATA -MEMORY_ARRAY_SINGLE_BLOCK_HEADER_SIZE
#define MEMORY_ARRAY_SINGLE_FLAG_OFFSET_FROM_DATA  -MEMORY_ARRAY_SINGLE_BLOCK_FLAG_SIZE

#define memArraySingleBlockGetFlags(block)      *((byte_t *)(block + MEMORY_ARRAY_SINGLE_FLAG_OFFSET_FROM_BLOCK))
#define memArraySingleBlockGetNext(block)       *((byte_t **)(block + MEMORY_ARRAY_SINGLE_NEXT_OFFSET_FROM_BLOCK))
#define memArraySingleBlockGetNextPointer(block) ((byte_t **)(block + MEMORY_ARRAY_SINGLE_NEXT_OFFSET_FROM_BLOCK))
#define memArraySingleBlockGetNextFree(block)   *((byte_t **)(block + MEMORY_ARRAY_SINGLE_DATA_OFFSET_FROM_BLOCK))
#define memArraySingleBlockGetData(block)        ((byte_t *)(block + MEMORY_ARRAY_SINGLE_DATA_OFFSET_FROM_BLOCK))

#define memArraySingleDataGetFlags(data)      *((byte_t *)(data + MEMORY_ARRAY_SINGLE_FLAG_OFFSET_FROM_DATA))
#define memArraySingleDataGetNext(data)       *((byte_t **)(data + MEMORY_ARRAY_SINGLE_NEXT_OFFSET_FROM_DATA))
#define memArraySingleDataGetNextPointer(data) ((byte_t **)(data + MEMORY_ARRAY_SINGLE_NEXT_OFFSET_FROM_DATA))
#define memArraySingleDataGetNextFree(data)   *((byte_t **)data)
#define memArraySingleDataGetBlock(data)       ((byte_t *)(data + MEMORY_ARRAY_SINGLE_BLOCK_OFFSET_FROM_DATA))

#ifdef MEMORY_ARRAY_SINGLE_LEAN
	#define MEMORY_ARRAY_SINGLE_ALIGN(x) x
#else
	#define MEMORY_ARRAY_SINGLE_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	byte_t *start;
	byte_t *next;  // Next free block pointer.
	byte_t *end;
	size_t block;  // Block size.
} memoryArraySingle;

#define memArraySingleBlockSize(bytes) MEMORY_ARRAY_SINGLE_ALIGN((bytes > MEMORY_ARRAY_SINGLE_BLOCK_SIZE ? bytes : MEMORY_ARRAY_SINGLE_BLOCK_SIZE) + MEMORY_ARRAY_SINGLE_BLOCK_HEADER_SIZE)
#define memArraySingleAllocationSize(start, bytes, length) (memArraySingleBlockSize(bytes) * length + MEMORY_ARRAY_SINGLE_ALIGN((uintptr_t)start) - (uintptr_t)start)

#define memArraySingleStart(array)        array->start
#define memArraySinglePrev(i)             *i = memArraySingleDataGetPrev(*i)
#define memArraySingleNext(i)             *i = memArraySingleDataGetNext(*i)
#define memArraySingleBlockStatus(block)  memArraySingleDataGetFlags(block)
#define memArraySingleBlockNext(array, i) i += array->block
#define memArraySingleEnd(array)          array->end

byte_t *memArraySingleInit(memoryArraySingle *array, byte_t *start, const size_t bytes, const size_t length);
byte_t *memArraySingleAllocate(memoryArraySingle *array);
byte_t *memArraySingleInsertBefore(memoryArraySingle *array, byte_t *element, byte_t *previous);
byte_t *memArraySingleInsertAfter(memoryArraySingle *array, byte_t *element);
void memArraySingleFree(memoryArraySingle *array, byte_t *block);
void memArraySingleClear(memoryArraySingle *array);

#endif
