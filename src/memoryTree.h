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
** "previous" size.
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

#define MEMORY_TREE_UNSPECIFIED_LENGTH MEMORY_UNSPECIFIED_LENGTH

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

#define memTreeDataGetCurrent(data)        *((byte_t *)(data + MEMORY_TREE_CURRENT_OFFSET_FROM_DATA))
#define memTreeDataGetCurrentPointer(data)  ((byte_t *)(data + MEMORY_TREE_CURRENT_OFFSET_FROM_DATA))
#define memTreeDataGetPrevious(data)       *((byte_t *)(data + MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA))
#define memTreeDataGetPreviousPointer(data) ((byte_t *)(data + MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA))

#define memTreeDataGetPreviousActiveless(data) (byte_t *)(*((uintptr_t *)(data + MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA)) & MEMORY_TREE_BLOCK_INACTIVE_MASK)

#define memTreeDataGetActive(data)        *((byte_t *)(data + MEMORY_TREE_ACTIVE_OFFSET_FROM_DATA))
#define memTreeDataGetActiveMasked(data) (*((byte_t *)(data + MEMORY_TREE_ACTIVE_OFFSET_FROM_DATA)) & MEMORY_TREE_BLOCK_ACTIVE_MASK)
#define memTreeDataGetActivePointer(data)  ((byte_t *)(data + MEMORY_TREE_ACTIVE_OFFSET_FROM_DATA))
#define memTreeDataSetActive(data)        *((byte_t *)(data + MEMORY_TREE_ACTIVE_OFFSET_FROM_DATA)) |= MEMORY_TREE_BLOCK_ACTIVE_MASK
#define memTreeDataSetInactive(data)      *((byte_t *)(data + MEMORY_TREE_ACTIVE_OFFSET_FROM_DATA)) &= MEMORY_TREE_BLOCK_INACTIVE_MASK

#define memTreeDataGetBlock(data) (data + MEMORY_TREE_BLOCK_OFFSET_FROM_DATA)

#define memTreeBlockGetCurrent(block)        *((byte_t *)(block + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetCurrentPointer(block)  ((byte_t *)(block + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetPrevious(block)       *((byte_t *)(block + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK))
#define memTreeBlockGetPreviousPointer(block) ((byte_t *)(block + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK))

#define memTreeBlockGetPreviousActiveless(block) (byte_t *)(*((uintptr_t *)(block + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK)) & MEMORY_TREE_BLOCK_INACTIVE_MASK)

#define memTreeBlockGetActive(block)        *((byte_t *)(block + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK))
#define memTreeBlockGetActiveMasked(block) (*((byte_t *)(block + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK)) & MEMORY_TREE_BLOCK_ACTIVE_MASK)
#define memTreeBlockGetActivePointer(block)  ((byte_t *)(block + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK))
#define memTreeBlockSetActive(block)        *((byte_t *)(block + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK)) |= MEMORY_TREE_BLOCK_ACTIVE_MASK
#define memTreeBlockSetInactive(block)      *((byte_t *)(block + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_BLOCK_INACTIVE_MASK

#define memTreeBlockGetColour(block)        *((uintptr_t *)(block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK))
#define memTreeBlockGetColourMasked(block) (*((uintptr_t *)(block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK)
#define memTreeBlockGetColourPointer(block)  ((uintptr_t *)(block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK))
#define memTreeBlockSetColourBlack(block)   *((uintptr_t *)(block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK
#define memTreeBlockSetColourRed(block)     *((uintptr_t *)(block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) |= MEMORY_TREE_NODE_COLOUR_RED_MASK

#define memTreeBlockGetLeft(block)          *((byte_t **)(block + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetLeftPointer(block)    ((byte_t **)(block + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetRight(block)         *((byte_t **)(block + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetRightPointer(block)   ((byte_t **)(block + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetParent(block)        *((byte_t **)(block + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetParentPointer(block)  ((byte_t **)(block + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK))

#define memTreeBlockGetParentColourless(block) \
	(byte_t *)(*((uintptr_t *)(block + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOURLESS_MASK)

#define memTreeBlockSetParentColourless(block, parent) \
	*((uintptr_t *)(block + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) = \
		(*((uintptr_t *)(block + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) | MEMORY_TREE_NODE_COLOURLESS_MASK) & \
		(uintptr_t)parent & MEMORY_TREE_NODE_COLOURLESS_MASK

#define memTreeBlockGetData(block) (block + MEMORY_TREE_DATA_OFFSET_FROM_BLOCK)
#define memTreeBlockGetNode(block) (block + MEMORY_TREE_DATA_OFFSET_FROM_BLOCK)

#define memTreeNodeGetColour(node)        *((uintptr_t *)(node + MEMORY_TREE_COLOUR_OFFSET_FROM_DATA))
#define memTreeNodeGetColourMasked(node) (*((uintptr_t *)(node + MEMORY_TREE_COLOUR_OFFSET_FROM_DATA)) & MEMORY_TREE_NODE_COLOUR_MASK)
#define memTreeNodeGetColourPointer(node)  ((uintptr_t *)(node + MEMORY_TREE_COLOUR_OFFSET_FROM_DATA))
#define memTreeNodeSetColourBlack(node)   *((uintptr_t *)(node + MEMORY_TREE_COLOUR_OFFSET_FROM_DATA)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK
#define memTreeNodeSetColourRed(node)     *((uintptr_t *)(node + MEMORY_TREE_COLOUR_OFFSET_FROM_DATA)) |= MEMORY_TREE_NODE_COLOUR_RED_MASK

#define memTreeNodeGetLeft(node)          *((byte_t **)(node + MEMORY_TREE_LEFT_OFFSET_FROM_DATA))
#define memTreeNodeGetLeftPointer(node)    ((byte_t **)(node + MEMORY_TREE_LEFT_OFFSET_FROM_DATA))
#define memTreeNodeGetRight(node)         *((byte_t **)(node + MEMORY_TREE_RIGHT_OFFSET_FROM_DATA))
#define memTreeNodeGetRightPointer(node)   ((byte_t **)(node + MEMORY_TREE_RIGHT_OFFSET_FROM_DATA))
#define memTreeNodeGetParent(node)        *((byte_t **)(node + MEMORY_TREE_PARENT_OFFSET_FROM_DATA))
#define memTreeNodeGetParentPointer(node)  ((byte_t **)(node + MEMORY_TREE_PARENT_OFFSET_FROM_DATA))

#define memTreeNodeGetParentColourless(node) \
	*((byte_t **)(*((uintptr_t *)(node + MEMORY_TREE_PARENT_OFFSET_FROM_DATA)) & MEMORY_TREE_NODE_COLOURLESS_MASK))

#define memTreeNodeSetParentColourless(node, parent) \
	*((uintptr_t *)(node + MEMORY_TREE_PARENT_OFFSET_FROM_DATA)) = \
		(*((uintptr_t *)(node + MEMORY_TREE_PARENT_OFFSET_FROM_DATA)) | MEMORY_TREE_NODE_COLOURLESS_MASK) & \
		(uintptr_t)parent & MEMORY_TREE_NODE_COLOURLESS_MASK

#ifdef MEMORY_TREE_LEAN
	#define MEMORY_TREE_ALIGN(x) x
#else
	#define MEMORY_TREE_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	byte_t *start;
	byte_t *root;   // Pointer to the root node.
	byte_t *end;
	byte_t offset;  // Whether or not a single byte offset is used.
	                // Could alternatively store whether a 1 or a
	                // 0 is used for the colour black, but that would
	                // result in slightly slower colour checks.
} memoryTree;

#define memTreeBlockSize(bytes) MEMORY_TREE_ALIGN(((bytes > MEMORY_TREE_BLOCK_SIZE ? bytes : MEMORY_TREE_BLOCK_SIZE) + MEMORY_TREE_BLOCK_HEADER_SIZE))
#define memTreeAllocationSize(start, bytes, length) ((length > 0 ? memTreeBlockSize(bytes) * length : bytes) + MEMORY_TREE_ALIGN((uintptr_t)start) - (uintptr_t)start)

byte_t *memTreeInit(memoryTree *tree, byte_t *start, const size_t bytes, const size_t length);
byte_t *memTreeAllocate(memoryTree *tree, const size_t bytes);
void memTreeFree(memoryTree *tree, byte_t *block);
byte_t *memTreeReallocate(memoryTree *tree, byte_t *block, const size_t bytes);
void memTreeClear(memoryTree *tree);

#endif
