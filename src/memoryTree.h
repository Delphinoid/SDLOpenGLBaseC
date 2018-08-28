#ifndef MEMORYTREE_H
#define MEMORYTREE_H

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
** Activeness of each block is
** stored in the LSB of the
** "previous" pointer.
**
** Colour information for each
** block is stored in the LSB of
** the "parent" pointer.
**
** In order to guarantee that the
** sizes are always divisible by 8,
** blocks may be assigned extra
** padding of up to 7 bytes.
*/

#define MEMORY_TREE_BLOCK_ACTIVE        (uintptr_t)0x01
#define MEMORY_TREE_BLOCK_INACTIVE      (uintptr_t)0x00
#define MEMORY_TREE_BLOCK_ACTIVE_MASK   (uintptr_t)0x01
#define MEMORY_TREE_BLOCK_INACTIVE_MASK (uintptr_t)~MEMORY_TREE_BLOCK_ACTIVE_MASK

#define MEMORY_TREE_NODE_COLOUR_RED        (uintptr_t)0x01
#define MEMORY_TREE_NODE_COLOUR_BLACK      (uintptr_t)0x00
#define MEMORY_TREE_NODE_COLOUR_RED_MASK   (uintptr_t)0x01
#define MEMORY_TREE_NODE_COLOUR_BLACK_MASK (uintptr_t)~MEMORY_TREE_NODE_COLOUR_RED_MASK
#define MEMORY_TREE_NODE_COLOUR_MASK       (uintptr_t)0x01
#define MEMORY_TREE_NODE_COLOURLESS_MASK   (uintptr_t)~MEMORY_TREE_NODE_COLOUR_MASK

#define MEMORY_TREE_BLOCK_CURRENT_SIZE  sizeof(size_t)
#define MEMORY_TREE_BLOCK_PREVIOUS_SIZE sizeof(size_t)
#define MEMORY_TREE_BLOCK_POINTER_SIZE  sizeof(byte_t *)
#define MEMORY_TREE_BLOCK_HEADER_SIZE   MEMORY_TREE_BLOCK_CURRENT_SIZE + MEMORY_TREE_BLOCK_PREVIOUS_SIZE
#define MEMORY_TREE_BLOCK_SIZE          MEMORY_TREE_BLOCK_POINTER_SIZE + MEMORY_TREE_BLOCK_POINTER_SIZE + MEMORY_TREE_BLOCK_POINTER_SIZE
#define MEMORY_TREE_BLOCK_TOTAL_SIZE    MEMORY_TREE_BLOCK_HEADER_SIZE + MEMORY_TREE_BLOCK_SIZE

#define MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK  0
#define MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK + MEMORY_TREE_BLOCK_CURRENT_SIZE
#define MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK   MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK
#define MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK     MEMORY_TREE_BLOCK_HEADER_SIZE
#define MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK    MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK + MEMORY_TREE_BLOCK_POINTER_SIZE
#define MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK   MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK + MEMORY_TREE_BLOCK_POINTER_SIZE
#define MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK   MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK
#define MEMORY_TREE_DATA_OFFSET_FROM_BLOCK     MEMORY_TREE_BLOCK_HEADER_SIZE
#define MEMORY_TREE_BLOCK_OFFSET_FROM_DATA     -MEMORY_TREE_BLOCK_HEADER_SIZE
#define MEMORY_TREE_CURRENT_OFFSET_FROM_DATA   -MEMORY_TREE_BLOCK_PREVIOUS_SIZE - MEMORY_TREE_BLOCK_CURRENT_SIZE
#define MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA  -MEMORY_TREE_BLOCK_PREVIOUS_SIZE
#define MEMORY_TREE_ACTIVE_OFFSET_FROM_DATA    -MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA
#define MEMORY_TREE_LEFT_OFFSET_FROM_DATA      0
#define MEMORY_TREE_RIGHT_OFFSET_FROM_DATA     MEMORY_TREE_LEFT_OFFSET_FROM_DATA + MEMORY_TREE_BLOCK_POINTER_SIZE
#define MEMORY_TREE_PARENT_OFFSET_FROM_DATA    MEMORY_TREE_RIGHT_OFFSET_FROM_DATA + MEMORY_TREE_BLOCK_POINTER_SIZE
#define MEMORY_TREE_COLOUR_OFFSET_FROM_DATA    MEMORY_TREE_PARENT_OFFSET_FROM_DATA

// Use 8 byte alignment for improved performance.
#define MEMORY_TREE_ALIGNMENT 8
// Rounds x to the nearest address that is
// aligned with MEMORY_TREE_ALIGNMENT.
#define MEMORY_TREE_ALIGN(x) ((x + MEMORY_TREE_ALIGNMENT - 1) & ~(MEMORY_TREE_ALIGNMENT - 1))

typedef struct {
	byte_t *start;
	byte_t *root;   // Pointer to the root node.
	byte_t *end;
	byte_t offset;  // Whether or not a single byte offset is used.
	                // Could alternatively store whether a 1 or a
	                // 0 is used for the colour black, but that would
	                // result in slightly slower colour checks.
} memoryTree;

byte_t *memTreeInit(memoryTree *tree, byte_t *start, const size_t bytes);
byte_t *memTreeAllocate(memoryTree *tree, const size_t bytes);
void memTreeFree(memoryTree *tree, byte_t *block);
byte_t *memTreeReallocate(memoryTree *tree, byte_t *block, const size_t bytes);
void memTreeClear(memoryTree *tree);

#endif
