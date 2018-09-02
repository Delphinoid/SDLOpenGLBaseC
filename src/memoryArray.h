#ifndef MEMORYARRAY_H
#define MEMORYARRAY_H

#include "memoryShared.h"

/*
** Array allocator (doubly-linked).
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

#define MEMORY_ARRAY_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_ARRAY_BLOCK_FLAG_SIZE    sizeof(byte_t)
#define MEMORY_ARRAY_BLOCK_PREV_SIZE    MEMORY_ARRAY_BLOCK_POINTER_SIZE
#define MEMORY_ARRAY_BLOCK_NEXT_SIZE    MEMORY_ARRAY_BLOCK_POINTER_SIZE
#define MEMORY_ARRAY_BLOCK_HEADER_SIZE  MEMORY_ARRAY_BLOCK_NEXT_SIZE + MEMORY_ARRAY_BLOCK_PREV_SIZE
#define MEMORY_ARRAY_BLOCK_SIZE         MEMORY_ARRAY_BLOCK_POINTER_SIZE
#define MEMORY_ARRAY_BLOCK_TOTAL_SIZE   MEMORY_ARRAY_BLOCK_HEADER_SIZE + MEMORY_ARRAY_BLOCK_SIZE

#define MEMORY_ARRAY_NEXT_OFFSET_FROM_BLOCK 0
#define MEMORY_ARRAY_PREV_OFFSET_FROM_BLOCK MEMORY_ARRAY_BLOCK_PREV_SIZE
#define MEMORY_ARRAY_DATA_OFFSET_FROM_BLOCK MEMORY_ARRAY_BLOCK_HEADER_SIZE
#define MEMORY_ARRAY_NEXT_OFFSET_FROM_DATA  -MEMORY_ARRAY_BLOCK_NEXT_SIZE - MEMORY_ARRAY_BLOCK_PREV_SIZE
#define MEMORY_ARRAY_PREV_OFFSET_FROM_DATA  -MEMORY_ARRAY_BLOCK_NEXT_SIZE
#define MEMORY_ARRAY_BLOCK_OFFSET_FROM_DATA -MEMORY_ARRAY_BLOCK_HEADER_SIZE
#define MEMORY_ARRAY_FLAG_OFFSET_FROM_DATA  -MEMORY_ARRAY_BLOCK_FLAG_SIZE

#define memArrayBlockGetFlags(block)      *((byte_t *)(block + MEMORY_ARRAY_FLAG_OFFSET_FROM_BLOCK))
#define memArrayBlockGetNext(block)       *((byte_t **)(block + MEMORY_ARRAY_NEXT_OFFSET_FROM_BLOCK))
#define memArrayBlockGetNextPointer(block) ((byte_t **)(block + MEMORY_ARRAY_NEXT_OFFSET_FROM_BLOCK))
#define memArrayBlockGetPrev(block)       *((byte_t **)(block + MEMORY_ARRAY_PREV_OFFSET_FROM_BLOCK))
#define memArrayBlockGetPrevPointer(block) ((byte_t **)(block + MEMORY_ARRAY_PREV_OFFSET_FROM_BLOCK))
#define memArrayBlockGetNextFree(block)   *((byte_t **)(block + MEMORY_ARRAY_DATA_OFFSET_FROM_BLOCK))
#define memArrayBlockGetData(block)        ((byte_t *)(block + MEMORY_ARRAY_DATA_OFFSET_FROM_BLOCK))

#define memArrayDataGetFlags(data)      *((byte_t *)(data + MEMORY_ARRAY_FLAG_OFFSET_FROM_DATA))
#define memArrayDataGetNext(data)       *((byte_t **)(data + MEMORY_ARRAY_NEXT_OFFSET_FROM_DATA))
#define memArrayDataGetNextPointer(data) ((byte_t **)(data + MEMORY_ARRAY_NEXT_OFFSET_FROM_DATA))
#define memArrayDataGetPrev(data)       *((byte_t **)(data + MEMORY_ARRAY_PREV_OFFSET_FROM_DATA))
#define memArrayDataGetPrevPointer(data) ((byte_t **)(data + MEMORY_ARRAY_PREV_OFFSET_FROM_DATA))
#define memArrayDataGetNextFree(data)   *((byte_t **)data)
#define memArrayDataGetBlock(data)       ((byte_t *)(data + MEMORY_ARRAY_BLOCK_OFFSET_FROM_DATA))

#ifdef MEMORY_ARRAY_LEAN
	#define MEMORY_ARRAY_ALIGN(x) x
#else
	#define MEMORY_ARRAY_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	byte_t *start;
	byte_t *next;  // Next free block pointer.
	byte_t *end;
	size_t block;  // Block size.
} memoryArray;

#define memArrayBlockSize(bytes) MEMORY_ARRAY_ALIGN((bytes > MEMORY_ARRAY_BLOCK_SIZE ? bytes : MEMORY_ARRAY_BLOCK_SIZE) + MEMORY_ARRAY_BLOCK_HEADER_SIZE)
#define memArrayAllocationSize(start, bytes, length) (memArrayBlockSize(bytes) * length + MEMORY_ARRAY_ALIGN((uintptr_t)start) - (uintptr_t)start)

#define memArrayStart(array)        array->start
#define memArrayPrev(i)             *i = memArrayDataGetPrev(*i)
#define memArrayNext(i)             *i = memArrayDataGetNext(*i)
#define memArrayBlockStatus(block)  memArrayDataGetFlags(block)
#define memArrayBlockNext(array, i) i += array->block
#define memArrayEnd(array)          array->end

byte_t *memArrayInit(memoryArray *array, byte_t *start, const size_t bytes, const size_t length);
byte_t *memArrayAllocate(memoryArray *array);
byte_t *memArrayInsertBefore(memoryArray *array, byte_t *element);
byte_t *memArrayInsertAfter(memoryArray *array, byte_t *element);
void memArrayFree(memoryArray *array, byte_t *block);
void memArrayClear(memoryArray *array);

#endif
