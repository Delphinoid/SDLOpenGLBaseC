#ifndef MEMORYTREE_H
#define MEMORYTREE_H

#include "memoryShared.h"
#include <stdint.h>

// General purpose allocator.
///
// Implements a red-black tree for
// O(log n) worst-case insertions
// and deletions.
///
// Uses best-fit.
///
// Activeness of each block is
// stored in the LSB of the
// "previous" size.
///
// Whether or not the block is
// the first or last in the tree
// is also stored in the 3rd and
// 2nd bits of the "previous" size.
///
// Colour information for each
// block is stored in the LSB of
// the "parent" pointer.
///
// In order to guarantee that the
// sizes are always divisible by 8,
// blocks may be assigned extra
// padding of up to 7 bytes.
///
// MEMORY_TREE_FORCE_MOVE_ON_REALLOC
// may be specified to force best
// fits on reallocations.
///
// Block format:
// [ Current block size ][Previous block size + active flag ][ Data (or [ Left child pointer ][ Right child pointer ][ Parent pointer + colour flag ]) ]

#define MEMORY_TREE_UNSPECIFIED_LENGTH MEMORY_UNSPECIFIED_LENGTH

#define MEMORY_TREE_BLOCK_ACTIVE        (size_t)0x01
#define MEMORY_TREE_BLOCK_INACTIVE      (size_t)0x00
#define MEMORY_TREE_BLOCK_ACTIVE_MASK   (size_t)0x01
#define MEMORY_TREE_BLOCK_INACTIVE_MASK (size_t)~MEMORY_TREE_BLOCK_ACTIVE_MASK

#define MEMORY_TREE_BLOCK_FIRST      (size_t)0x02
#define MEMORY_TREE_BLOCK_FIRST_MASK (size_t)~0x02
#define MEMORY_TREE_BLOCK_LAST       (size_t)0x04
#define MEMORY_TREE_BLOCK_LAST_MASK  (size_t)~0x04

#define MEMORY_TREE_BLOCK_LAST_AND_ACTIVE_MASK (MEMORY_TREE_BLOCK_LAST | MEMORY_TREE_BLOCK_ACTIVE)

#define MEMORY_TREE_BLOCK_FLAGS_MASK    (size_t)(MEMORY_ALIGNMENT-1)
#define MEMORY_TREE_BLOCK_PREVIOUS_MASK (size_t)~MEMORY_TREE_BLOCK_FLAGS_MASK

#define MEMORY_TREE_NODE_COLOUR_RED        (uintptr_t)0x01
#define MEMORY_TREE_NODE_COLOUR_BLACK      (uintptr_t)0x00
#define MEMORY_TREE_NODE_COLOUR_RED_MASK   (uintptr_t)0x01
#define MEMORY_TREE_NODE_COLOUR_BLACK_MASK (uintptr_t)~MEMORY_TREE_NODE_COLOUR_RED_MASK
#define MEMORY_TREE_NODE_COLOUR_MASK       (uintptr_t)0x01
#define MEMORY_TREE_NODE_COLOURLESS_MASK   (uintptr_t)~MEMORY_TREE_NODE_COLOUR_MASK

#define MEMORY_TREE_BLOCK_CURRENT_SIZE  sizeof(size_t)
#define MEMORY_TREE_BLOCK_PREVIOUS_SIZE sizeof(size_t)
#define MEMORY_TREE_BLOCK_POINTER_SIZE  sizeof(byte_t *)
#define MEMORY_TREE_BLOCK_HEADER_SIZE   (MEMORY_TREE_BLOCK_CURRENT_SIZE + MEMORY_TREE_BLOCK_PREVIOUS_SIZE)
#define MEMORY_TREE_BLOCK_SIZE          (MEMORY_TREE_BLOCK_POINTER_SIZE + MEMORY_TREE_BLOCK_POINTER_SIZE + MEMORY_TREE_BLOCK_POINTER_SIZE)
#define MEMORY_TREE_BLOCK_TOTAL_SIZE    (MEMORY_TREE_BLOCK_HEADER_SIZE + MEMORY_TREE_BLOCK_SIZE)

#define MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK  0
#define MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK (MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK + MEMORY_TREE_BLOCK_CURRENT_SIZE)
#define MEMORY_TREE_FLAGS_OFFSET_FROM_BLOCK    MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK
#define MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK     MEMORY_TREE_BLOCK_HEADER_SIZE
#define MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK    (MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK + MEMORY_TREE_BLOCK_POINTER_SIZE)
#define MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK   (MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK + MEMORY_TREE_BLOCK_POINTER_SIZE)
#define MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK   MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK
#define MEMORY_TREE_DATA_OFFSET_FROM_BLOCK     MEMORY_TREE_BLOCK_HEADER_SIZE
#define MEMORY_TREE_BLOCK_OFFSET_FROM_DATA     -MEMORY_TREE_BLOCK_HEADER_SIZE
#define MEMORY_TREE_CURRENT_OFFSET_FROM_DATA   (-MEMORY_TREE_BLOCK_PREVIOUS_SIZE - MEMORY_TREE_BLOCK_CURRENT_SIZE)
#define MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA  -MEMORY_TREE_BLOCK_PREVIOUS_SIZE
#define MEMORY_TREE_FLAGS_OFFSET_FROM_DATA     MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA
#define MEMORY_TREE_LEFT_OFFSET_FROM_DATA      0
#define MEMORY_TREE_RIGHT_OFFSET_FROM_DATA     (MEMORY_TREE_LEFT_OFFSET_FROM_DATA + MEMORY_TREE_BLOCK_POINTER_SIZE)
#define MEMORY_TREE_PARENT_OFFSET_FROM_DATA    (MEMORY_TREE_RIGHT_OFFSET_FROM_DATA + MEMORY_TREE_BLOCK_POINTER_SIZE)
#define MEMORY_TREE_COLOUR_OFFSET_FROM_DATA    MEMORY_TREE_PARENT_OFFSET_FROM_DATA

#define memTreeDataGetCurrent(data)        *((size_t *)((byte_t *)data + MEMORY_TREE_CURRENT_OFFSET_FROM_DATA))
#define memTreeDataGetCurrentPointer(data)  ((size_t *)((byte_t *)data + MEMORY_TREE_CURRENT_OFFSET_FROM_DATA))
#define memTreeDataGetPrevious(data)       *((size_t *)((byte_t *)data + MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA))
#define memTreeDataGetPreviousPointer(data) ((size_t *)((byte_t *)data + MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA))
#define memTreeDataGetFlags(data)          *((size_t *)((byte_t *)data + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA))
#define memTreeDataGetFlagsPointer(data)    ((size_t *)((byte_t *)data + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA))

#define memTreeDataGetPreviousFlagless(block) (memTreeDataGetFlags(block) & MEMORY_TREE_BLOCK_PREVIOUS_MASK)

#define memTreeDataGetActive(data)        *((byte_t *)data + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA)
#define memTreeDataGetActiveMasked(data) (*((byte_t *)data + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA) & MEMORY_TREE_BLOCK_ACTIVE_MASK)
#define memTreeDataGetActivePointer(data)  ((byte_t *)data + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA)
#define memTreeDataSetActive(data)        *((byte_t *)data + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA) |= MEMORY_TREE_BLOCK_ACTIVE_MASK
#define memTreeDataSetInactive(data)      *((byte_t *)data + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA) &= MEMORY_TREE_BLOCK_INACTIVE_MASK

#define memTreeDataGetBlock(data) ((byte_t *)data + MEMORY_TREE_BLOCK_OFFSET_FROM_DATA)
#define memTreeNodeGetBlock(node) ((byte_t *)node + MEMORY_TREE_BLOCK_OFFSET_FROM_DATA)

#define memTreeBlockGetCurrent(block)        *((size_t *)((byte_t *)block + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetCurrentPointer(block)  ((size_t *)((byte_t *)block + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetPrevious(block)       *((size_t *)((byte_t *)block + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK))
#define memTreeBlockGetPreviousPointer(block) ((size_t *)((byte_t *)block + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK))
#define memTreeBlockGetFlags(block)          *((size_t *)((byte_t *)block + MEMORY_TREE_FLAGS_OFFSET_FROM_BLOCK))
#define memTreeBlockGetFlagsPointer(block)    ((size_t *)((byte_t *)block + MEMORY_TREE_FLAGS_OFFSET_FROM_BLOCK))

#define memTreeBlockGetPreviousFlagless(block) (memTreeBlockGetFlags(block) & MEMORY_TREE_BLOCK_PREVIOUS_MASK)

#define memTreeBlockGetActive(block)        *((size_t *)((byte_t *)block + MEMORY_TREE_FLAGS_OFFSET_FROM_BLOCK))
#define memTreeBlockGetActiveMasked(block) (*((size_t *)((byte_t *)block + MEMORY_TREE_FLAGS_OFFSET_FROM_BLOCK)) & MEMORY_TREE_BLOCK_ACTIVE_MASK)
#define memTreeBlockGetActivePointer(block)  ((size_t *)((byte_t *)block + MEMORY_TREE_FLAGS_OFFSET_FROM_BLOCK))
#define memTreeBlockSetActive(block)        *((size_t *)((byte_t *)block + MEMORY_TREE_FLAGS_OFFSET_FROM_BLOCK)) |= MEMORY_TREE_BLOCK_ACTIVE_MASK
#define memTreeBlockSetInactive(block)      *((size_t *)((byte_t *)block + MEMORY_TREE_FLAGS_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_BLOCK_INACTIVE_MASK

#define memTreeBlockGetColour(block)        *((uintptr_t *)((byte_t *)block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK))
#define memTreeBlockGetColourMasked(block) (*((uintptr_t *)((byte_t *)block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK)
#define memTreeBlockGetColourPointer(block)  ((uintptr_t *)((byte_t *)block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK))
#define memTreeBlockSetColourBlack(block)   *((uintptr_t *)((byte_t *)block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK
#define memTreeBlockSetColourRed(block)     *((uintptr_t *)((byte_t *)block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) |= MEMORY_TREE_NODE_COLOUR_RED_MASK

#define memTreeBlockGetLeft(block)          *((byte_t **)((byte_t *)block + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetLeftPointer(block)    ((byte_t **)((byte_t *)block + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetRight(block)         *((byte_t **)((byte_t *)block + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetRightPointer(block)   ((byte_t **)((byte_t *)block + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetParent(block)        *((byte_t **)((byte_t *)block + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK))
#define memTreeBlockGetParentPointer(block)  ((byte_t **)((byte_t *)block + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK))

#define memTreeBlockGetParentColourless(block) \
	((byte_t *)(memTreeBlockGetColour(block) & MEMORY_TREE_NODE_COLOURLESS_MASK))

#define memTreeBlockSetParentKeepColour(block, parent) \
	*((uintptr_t *)((byte_t *)block + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_MASK; \
	*((uintptr_t *)((byte_t *)block + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) |= (uintptr_t)parent & MEMORY_TREE_NODE_COLOURLESS_MASK;

#define memTreeBlockSetColourKeepParent(block, colour) \
	*((uintptr_t *)((byte_t *)block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOURLESS_MASK; \
	*((uintptr_t *)((byte_t *)block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) |= colour & MEMORY_TREE_NODE_COLOUR_MASK;

#define memTreeBlockSetPreviousKeepFirst(flags, previous) \
	flags = (previous & MEMORY_TREE_BLOCK_FIRST_MASK) | (flags & MEMORY_TREE_BLOCK_FIRST)

#define memTreeBlockSetPreviousKeepLast(flags, previous) \
	flags = (previous & MEMORY_TREE_BLOCK_LAST_MASK) | (flags & MEMORY_TREE_BLOCK_LAST)

#define memTreeBlockSetPreviousKeepLastAndActive(flags, previous) \
	flags = (previous & MEMORY_TREE_BLOCK_LAST_MASK) | (flags & MEMORY_TREE_BLOCK_LAST_AND_ACTIVE_MASK)

#define memTreeBlockSetPreviousKeepFlags(block, previous) \
	memTreeBlockGetFlags(block) = \
		(previous & MEMORY_TREE_BLOCK_PREVIOUS_MASK) | (memTreeBlockGetFlags(block) & MEMORY_TREE_BLOCK_FLAGS_MASK)

#define memTreeBlockPreviousFlagless(flags) (flags & MEMORY_TREE_BLOCK_PREVIOUS_MASK)
#define memTreeBlockIsFirst(flags)          (flags & MEMORY_TREE_BLOCK_FIRST)
#define memTreeBlockIsLast(flags)           (flags & MEMORY_TREE_BLOCK_LAST)

#define memTreeBlockSetLast(flags, newFlags) \
	flags &= MEMORY_TREE_BLOCK_LAST_MASK; \
	flags |= newFlags & MEMORY_TREE_BLOCK_LAST

#define memTreeBlockGetData(block) ((byte_t *)block + MEMORY_TREE_DATA_OFFSET_FROM_BLOCK)
#define memTreeBlockGetNode(block) ((byte_t *)block + MEMORY_TREE_DATA_OFFSET_FROM_BLOCK)

#define memTreeNodeGetCurrent(node)        *((size_t *)((byte_t *)node + MEMORY_TREE_CURRENT_OFFSET_FROM_DATA))
#define memTreeNodeGetCurrentPointer(node)  ((size_t *)((byte_t *)node + MEMORY_TREE_CURRENT_OFFSET_FROM_DATA))
#define memTreeNodeGetPrevious(node)       *((size_t *)((byte_t *)node + MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA))
#define memTreeNodeGetPreviousPointer(node) ((size_t *)((byte_t *)node + MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA))
#define memTreeNodeGetFlags(node)          *((size_t *)((byte_t *)node + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA))
#define memTreeNodeGetFlagsPointer(node)    ((size_t *)((byte_t *)node + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA))

#define memTreeNodeGetPreviousFlagless(node) (memTreeNodeGetFlags(node) & MEMORY_TREE_BLOCK_PREVIOUS_MASK)

#define memTreeNodeGetActive(node)        *((byte_t *)node + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA)
#define memTreeNodeGetActiveMasked(node) (*((byte_t *)node + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA) & MEMORY_TREE_BLOCK_ACTIVE_MASK)
#define memTreeNodeGetActivePointer(node)  ((byte_t *)node + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA)
#define memTreeNodeSetActive(node)        *((byte_t *)node + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA) |= MEMORY_TREE_BLOCK_ACTIVE_MASK
#define memTreeNodeSetInactive(node)      *((byte_t *)node + MEMORY_TREE_FLAGS_OFFSET_FROM_DATA) &= MEMORY_TREE_BLOCK_INACTIVE_MASK

#define memTreeNodeGetColour(node)        *((uintptr_t *)((byte_t *)node + MEMORY_TREE_COLOUR_OFFSET_FROM_DATA))
#define memTreeNodeGetColourMasked(node) (*((uintptr_t *)((byte_t *)node + MEMORY_TREE_COLOUR_OFFSET_FROM_DATA)) & MEMORY_TREE_NODE_COLOUR_MASK)
#define memTreeNodeGetColourPointer(node)  ((uintptr_t *)((byte_t *)node + MEMORY_TREE_COLOUR_OFFSET_FROM_DATA))
#define memTreeNodeSetColourBlack(node)   *((uintptr_t *)((byte_t *)node + MEMORY_TREE_COLOUR_OFFSET_FROM_DATA)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK
#define memTreeNodeSetColourRed(node)     *((uintptr_t *)((byte_t *)node + MEMORY_TREE_COLOUR_OFFSET_FROM_DATA)) |= MEMORY_TREE_NODE_COLOUR_RED_MASK

#define memTreeNodeGetLeft(node)          *((byte_t **)((byte_t *)node + MEMORY_TREE_LEFT_OFFSET_FROM_DATA))
#define memTreeNodeGetLeftPointer(node)    ((byte_t **)((byte_t *)node + MEMORY_TREE_LEFT_OFFSET_FROM_DATA))
#define memTreeNodeGetRight(node)         *((byte_t **)((byte_t *)node + MEMORY_TREE_RIGHT_OFFSET_FROM_DATA))
#define memTreeNodeGetRightPointer(node)   ((byte_t **)((byte_t *)node + MEMORY_TREE_RIGHT_OFFSET_FROM_DATA))
#define memTreeNodeGetParent(node)        *((byte_t **)((byte_t *)node + MEMORY_TREE_PARENT_OFFSET_FROM_DATA))
#define memTreeNodeGetParentPointer(node)  ((byte_t **)((byte_t *)node + MEMORY_TREE_PARENT_OFFSET_FROM_DATA))

#define memTreeNodeIsBlack(node, parent) (node == NULL || memTreeNodeGetParent(node) == parent)
#define memTreeNodeIsRed(node, parent)   (node != NULL && memTreeNodeGetParent(node) != parent)

#define memTreeNodeGetParentColourless(node) \
	((byte_t *)(memTreeNodeGetColour(node) & MEMORY_TREE_NODE_COLOURLESS_MASK))

#define memTreeNodeSetParentKeepColour(node, parent) \
	*((uintptr_t *)((byte_t *)node + MEMORY_TREE_PARENT_OFFSET_FROM_DATA)) &= MEMORY_TREE_NODE_COLOUR_MASK; \
	*((uintptr_t *)((byte_t *)node + MEMORY_TREE_PARENT_OFFSET_FROM_DATA)) |= (uintptr_t)parent & MEMORY_TREE_NODE_COLOURLESS_MASK;

#define memTreeNodeSetColourKeepParent(node, colour) \
	*((uintptr_t *)((byte_t *)node + MEMORY_TREE_COLOUR_OFFSET_FROM_DATA)) &= MEMORY_TREE_NODE_COLOURLESS_MASK; \
	*((uintptr_t *)((byte_t *)node + MEMORY_TREE_COLOUR_OFFSET_FROM_DATA)) |= colour & MEMORY_TREE_NODE_COLOUR_MASK;

#ifdef MEMORY_TREE_LEAN
	#define MEMORY_TREE_ALIGN(x) x
#else
	#define MEMORY_TREE_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	byte_t *root;  // Pointer to the root node.
	memoryRegion *region;  // Pointer to the allocator's memory region.
} memoryTree;

#define memTreeBlockSize(bytes) MEMORY_TREE_ALIGN((bytes > MEMORY_TREE_BLOCK_SIZE ? (bytes + MEMORY_TREE_BLOCK_HEADER_SIZE) : MEMORY_TREE_BLOCK_TOTAL_SIZE))
#define memTreeBlockSizeUnaligned(bytes)          (bytes > MEMORY_TREE_BLOCK_SIZE ? (bytes + MEMORY_TREE_BLOCK_HEADER_SIZE) : MEMORY_TREE_BLOCK_TOTAL_SIZE)
#define memTreeAlignStartBlock(start) ((byte_t *)MEMORY_TREE_ALIGN((uintptr_t)start + MEMORY_TREE_BLOCK_HEADER_SIZE) - MEMORY_TREE_BLOCK_HEADER_SIZE)
#define memTreeAlignStartData(start)  ((byte_t *)MEMORY_TREE_ALIGN((uintptr_t)start + MEMORY_TREE_BLOCK_HEADER_SIZE))
#define memTreeAllocationSize(start, bytes, length) \
	((length > 0 ? memTreeBlockSize(bytes) * length : bytes) + (uintptr_t)memTreeAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))

#define memTreeFirst(region)          ((void *)memTreeAlignStartData((region)->start))
#define memTreeBlockStatus(block)     memTreeBlockGetActiveMasked(block)
#define memTreeBlockNext(tree, i)     (void *)((byte_t *)i + memTreeDataGetCurrent(i));
#define memTreeBlockPrevious(tree, i) (void *)((byte_t *)i - memTreeDataGetCurrent(i));

void memTreeInit(memoryTree *const __RESTRICT__ tree);
void *memTreeCreate(memoryTree *const __RESTRICT__ tree, void *const start, const size_t bytes, const size_t length);
void *memTreeAllocate(memoryTree *const __RESTRICT__ tree, const size_t bytes);
void memTreeFree(memoryTree *const __RESTRICT__ tree, void *const __RESTRICT__ block);
void *memTreeReallocate(memoryTree *const __RESTRICT__ tree, void *const block, const size_t bytes);
void *memTreeSetupMemory(void *start, const size_t bytes, const size_t length);
void memTreeClear(memoryTree *const __RESTRICT__ tree);
void *memTreeExtend(memoryTree *const __RESTRICT__ tree, void *const start, const size_t bytes, const size_t length);
void memTreeDelete(memoryTree *const __RESTRICT__ tree);

#ifdef MEMORY_DEBUG
void memTreePrintFreeBlocks(memoryTree *const __RESTRICT__ tree, const unsigned int recursions);
void memTreePrintAllBlocks(memoryTree *const __RESTRICT__ tree);
#endif

#endif
