#ifndef MEMORYARRAY_H
#define MEMORYARRAY_H

#include "memoryShared.h"

/*
** Array allocator.
**
** Effectively a free-list allocator
** but each block has pointers to the
** previous and next elements in the
** array they are a part of, contained
** in a header.
**
** Works best for small arrays with
** frequent insertions and deletions.
**
** It can also be treated as an object
** pool for some reason. Not entirely
** sure what I was thinking when I
** added those functions.
*/

#define MEMORY_ARRAY_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_ARRAY_BLOCK_FLAG_SIZE    sizeof(byte_t)
#define MEMORY_ARRAY_BLOCK_PREV_SIZE    MEMORY_ARRAY_BLOCK_POINTER_SIZE
#define MEMORY_ARRAY_BLOCK_NEXT_SIZE    MEMORY_ARRAY_BLOCK_POINTER_SIZE
#define MEMORY_ARRAY_BLOCK_HEADER_SIZE  MEMORY_ARRAY_BLOCK_PREV_SIZE + MEMORY_ARRAY_BLOCK_NEXT_SIZE
#define MEMORY_ARRAY_BLOCK_SIZE         MEMORY_ARRAY_BLOCK_POINTER_SIZE
#define MEMORY_ARRAY_BLOCK_TOTAL_SIZE   MEMORY_ARRAY_BLOCK_HEADER_SIZE + MEMORY_ARRAY_BLOCK_SIZE

#define MEMORY_ARRAY_PREV_OFFSET_FROM_BLOCK 0
#define MEMORY_ARRAY_NEXT_OFFSET_FROM_BLOCK MEMORY_ARRAY_BLOCK_PREV_SIZE
#define MEMORY_ARRAY_DATA_OFFSET_FROM_BLOCK MEMORY_ARRAY_BLOCK_HEADER_SIZE
#define MEMORY_ARRAY_PREV_OFFSET_FROM_DATA  -MEMORY_ARRAY_BLOCK_NEXT_SIZE - MEMORY_ARRAY_BLOCK_PREV_SIZE
#define MEMORY_ARRAY_NEXT_OFFSET_FROM_DATA  -MEMORY_ARRAY_BLOCK_NEXT_SIZE
#define MEMORY_ARRAY_BLOCK_OFFSET_FROM_DATA -MEMORY_ARRAY_BLOCK_HEADER_SIZE
#define MEMORY_ARRAY_FLAG_OFFSET_FROM_DATA  -MEMORY_ARRAY_BLOCK_FLAG_SIZE

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

size_t memArrayAllocationOverhead(const byte_t *start, const size_t bytes, const size_t length);
byte_t *memArrayInit(memoryArray *array, byte_t *start, const size_t bytes, const size_t length);
byte_t *memArrayAllocate(memoryArray *array);
byte_t *memArrayInsertAfter(memoryArray *array, byte_t *element);
void memArrayFree(memoryArray *array, byte_t *block);
void memArrayClear(memoryArray *array);

byte_t *memArrayStart(const memoryArray *array);
void memArrayPrev(byte_t **i);
void memArrayNext(byte_t **i);
byte_t memArrayBlockStatus(const byte_t *block);
void memArrayBlockNext(const memoryArray *array, byte_t **i);
byte_t *memArrayEnd(const memoryArray *array);

#endif
