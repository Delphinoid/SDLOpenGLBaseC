#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

/**#include <stdlib.h>
#include <stdint.h>

#define MEMORY_POOL_INACTIVE 0x00
#define MEMORY_POOL_ACTIVE   0x01

#if INTPTR_MAX == INT64_MAX
	#define MEMORY_POOL_MAX_BYTES 1152921504606846976
	#define MEMORY_POOL_NEXT_BLOCK_NULL
	#define MEMORY_POOL_BLOCK_BITS 64
	#define MEMORY_POOL_BLOCK_TYPE uint64_t
#elif INTPTR_MAX == INT32_MAX
	#define MEMORY_POOL_MAX_BYTES 268435456
	#define MEMORY_POOL_BLOCK_BITS 32
	#define MEMORY_POOL_BLOCK_TYPE uint32_t
#endif

typedef struct {
	MEMORY_POOL_BLOCK_TYPE active : 1;
	MEMORY_POOL_BLOCK_TYPE next   : (MEMORY_POOL_BLOCK_BITS-1);
} memoryListBlock;

typedef struct {
	byte_t *start;
	MEMORY_POOL_BLOCK_TYPE next;
} memoryList;**/

#include "memoryShared.h"

/*
** Object pool allocator.
**
** Effectively a free-list allocator
** but each block has an "active"
** flag for iterations, stored in a
** header before the main block data.
**
** Free-list pointers point to the
** data, not the beginning of the block.
*/

/**
*** Instead of using an "active" flag,
*** could we use pointers to the next
*** active member?
**/

#define MEMORY_POOL_INACTIVE 0
#define MEMORY_POOL_ACTIVE   1

#define MEMORY_POOL_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_POOL_BLOCK_FLAG_SIZE    sizeof(byte_t)
#define MEMORY_POOL_BLOCK_HEADER_SIZE  MEMORY_POOL_BLOCK_FLAG_SIZE
#define MEMORY_POOL_BLOCK_SIZE         MEMORY_POOL_BLOCK_POINTER_SIZE
#define MEMORY_POOL_BLOCK_TOTAL_SIZE   MEMORY_POOL_BLOCK_HEADER_SIZE + MEMORY_POOL_BLOCK_SIZE

#define MEMORY_POOL_FLAG_OFFSET_FROM_BLOCK 0
#define MEMORY_POOL_DATA_OFFSET_FROM_BLOCK MEMORY_POOL_BLOCK_FLAG_SIZE
#define MEMORY_POOL_FLAG_OFFSET_FROM_DATA  -MEMORY_POOL_BLOCK_FLAG_SIZE
#define MEMORY_POOL_BLOCK_OFFSET_FROM_DATA -MEMORY_POOL_BLOCK_HEADER_SIZE

#ifdef MEMORY_POOL_LEAN
	#define MEMORY_POOL_ALIGN(x) x
#else
	#define MEMORY_POOL_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	byte_t *start;
	byte_t *next;  // Next free block pointer.
	byte_t *end;
	size_t block;  // Block size.
} memoryPool;

size_t memPoolAllocationOverhead(const byte_t *start, const size_t bytes, const size_t length);
byte_t *memPoolInit(memoryPool *pool, byte_t *start, const size_t bytes, const size_t length);
byte_t *memPoolAllocate(memoryPool *pool);
void memPoolFree(memoryPool *pool, byte_t *block);
void memPoolClear(memoryPool *pool);

byte_t *memPoolStart(const memoryPool *pool);
byte_t memPoolBlockStatus(const byte_t *block);
void memPoolBlockNext(const memoryPool *pool, byte_t **i);
byte_t *memPoolEnd(const memoryPool *pool);

#endif
