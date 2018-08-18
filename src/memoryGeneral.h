#ifndef MEMORYGENERAL_H
#define MEMORYGENERAL_H

#include "memoryShared.h"

/*
** General purpose allocator.
**
** Implements a red-black tree for
** O(log n) worst-case insertions
** and deletions.
**
** Uses best-fit.
**
** Colour information is stored in
** the LSB of the "prev" pointer.
** In order to guarantee that the
** pointers are always even, blocks
** may have an extra single-byte
** overhead. The start of the tree
** may also be offset by a single
** byte.
*/

#define MEMORY_GENERAL_NODE_BLACK 0
#define MEMORY_GENERAL_NODE_RED   1

#define MEMORY_GENERAL_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_GENERAL_BLOCK_HEADER_SIZE  MEMORY_GENERAL_BLOCK_POINTER_SIZE + MEMORY_GENERAL_BLOCK_POINTER_SIZE
#define MEMORY_GENERAL_BLOCK_SIZE         MEMORY_GENERAL_BLOCK_POINTER_SIZE + MEMORY_GENERAL_BLOCK_POINTER_SIZE + MEMORY_GENERAL_BLOCK_POINTER_SIZE
#define MEMORY_GENERAL_BLOCK_TOTAL_SIZE   MEMORY_GENERAL_BLOCK_HEADER_SIZE + MEMORY_GENERAL_BLOCK_SIZE

#define MEMORY_GENERAL_NEXT_OFFSET_FROM_BLOCK   0
#define MEMORY_GENERAL_PREV_OFFSET_FROM_BLOCK   MEMORY_GENERAL_NEXT_OFFSET_FROM_BLOCK + MEMORY_GENERAL_BLOCK_POINTER_SIZE
#define MEMORY_GENERAL_LEFT_OFFSET_FROM_BLOCK   MEMORY_GENERAL_BLOCK_HEADER_SIZE
#define MEMORY_GENERAL_RIGHT_OFFSET_FROM_BLOCK  MEMORY_GENERAL_LEFT_OFFSET_FROM_BLOCK + MEMORY_GENERAL_BLOCK_POINTER_SIZE
#define MEMORY_GENERAL_PARENT_OFFSET_FROM_BLOCK MEMORY_GENERAL_RIGHT_OFFSET_FROM_BLOCK + MEMORY_GENERAL_BLOCK_POINTER_SIZE
#define MEMORY_GENERAL_DATA_OFFSET_FROM_BLOCK   MEMORY_GENERAL_BLOCK_HEADER_SIZE
#define MEMORY_GENERAL_BLOCK_OFFSET_FROM_DATA   -MEMORY_GENERAL_BLOCK_HEADER_SIZE
#define MEMORY_GENERAL_NEXT_OFFSET_FROM_DATA    -MEMORY_GENERAL_BLOCK_POINTER_SIZE - MEMORY_GENERAL_BLOCK_POINTER_SIZE
#define MEMORY_GENERAL_PREV_OFFSET_FROM_DATA    -MEMORY_GENERAL_BLOCK_POINTER_SIZE
#define MEMORY_GENERAL_LEFT_OFFSET_FROM_DATA    0
#define MEMORY_GENERAL_RIGHT_OFFSET_FROM_DATA   MEMORY_GENERAL_LEFT_OFFSET_FROM_DATA + MEMORY_GENERAL_BLOCK_POINTER_SIZE
#define MEMORY_GENERAL_PARENT_OFFSET_FROM_DATA  MEMORY_GENERAL_RIGHT_OFFSET_FROM_DATA + MEMORY_GENERAL_BLOCK_POINTER_SIZE

typedef struct {
	byte_t *start;
	byte_t *next;   // Pointer to the root node.
	byte_t *end;
	byte_t offset;  // Whether or not a single byte offset is used.
	                // Could alternatively store whether a 1 or a
	                // 0 is used for the colour black, but that would
	                // result in slightly slower colour checks.
} memoryGeneral;

byte_t *memGeneralInit(memoryGeneral *gen, byte_t *start, const size_t bytes);
byte_t *memGeneralAllocate(memoryGeneral *gen, const size_t bytes);
void memGeneralFree(memoryGeneral *gen, byte_t *block);
byte_t *memGeneralReallocate(memoryGeneral *gen, byte_t *block, const size_t bytes);
void memGeneralClear(memoryGeneral *gen);

#endif
