#include "memoryTree.h"
#include "inline.h"
#include <string.h>

void memTreeInit(memoryTree *const restrict tree){
	tree->root = NULL;
	tree->region = NULL;
}

void *memTreeCreate(memoryTree *const restrict tree, void *const start, const size_t bytes, const size_t length){

	/*
	** Initialize a general purpose memory allocator.
	**
	** If "length" is specified, it will optimize the size for
	** "length"-many allocations of size "bytes", otherwise
	** the total allocated size will be "bytes".
	*/

	if(start != NULL){

		tree->region = (memoryRegion *)((byte_t *)start + memTreeAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		tree->region->start = start;
		tree->region->next = NULL;

		memTreeClear(tree);

	}

	return start;

}

static __FORCE_INLINE__ void memTreeRotateTreeLeft(memoryTree *const restrict tree, byte_t *const node){

	/*
	** Rotate the red-black tree to the left.
	*/

	byte_t *parent = memTreeNodeGetParentColourless(node);
	byte_t *right = memTreeNodeGetRight(node);
	byte_t *rightleft = memTreeNodeGetLeft(right);

	memTreeNodeGetRight(node) = rightleft;

	if(rightleft != NULL){
		memTreeNodeSetParentKeepColour(rightleft, node);
	}

	memTreeNodeSetParentKeepColour(right, parent);

	if(parent == NULL){
		tree->root = right;
	}else if(node == memTreeNodeGetLeft(parent)){
		memTreeNodeGetLeft(parent) = right;
	}else{
		memTreeNodeGetRight(parent) = right;
	}

	memTreeNodeGetLeft(right) = node;
	memTreeNodeSetParentKeepColour(node, right);

}

static __FORCE_INLINE__ void memTreeRotateTreeRight(memoryTree *const restrict tree, byte_t *const node){

	/*
	** Rotate the red-black tree to the right.
	*/

	byte_t *parent = memTreeNodeGetParentColourless(node);
	byte_t *left = memTreeNodeGetLeft(node);
	byte_t *leftright = memTreeNodeGetRight(left);

	memTreeNodeGetLeft(node) = leftright;

	if(leftright != NULL){
		memTreeNodeSetParentKeepColour(leftright, node);
	}

	memTreeNodeSetParentKeepColour(left, parent);

	if(parent == NULL){
		tree->root = left;
	}else if(node == memTreeNodeGetLeft(parent)){
		memTreeNodeGetLeft(parent) = left;
	}else{
		memTreeNodeGetRight(parent) = left;
	}

	memTreeNodeGetRight(left) = node;
	memTreeNodeSetParentKeepColour(node, left);

}

static __FORCE_INLINE__ void memTreeRepairTree(memoryTree *const restrict tree, const byte_t *node){

	/*
	** Fix any violations of the red-black
	** tree's rules, starting at "node".
	*/

	byte_t *parent = memTreeNodeGetParentColourless(node);

	// Continue looping while the current node
	// and its parent are both red. If the root
	// node is reached, the loop will terminate.
	while(
		node != tree->root &&
		memTreeNodeGetColourMasked(node)   != MEMORY_TREE_NODE_COLOUR_BLACK &&
		memTreeNodeGetColourMasked(parent) != MEMORY_TREE_NODE_COLOUR_BLACK
	){

		byte_t *grandparent = memTreeNodeGetParentColourless(parent);
		byte_t *uncle = memTreeNodeGetLeft(grandparent);

		if(parent == uncle){

			// The uncle is to the right of the grandparent.
			uncle = memTreeNodeGetRight(grandparent);

			if(uncle != NULL && memTreeNodeGetColourMasked(uncle) != MEMORY_TREE_NODE_COLOUR_BLACK){

				// The uncle is red, all we need to
				// do is some recolouring.
				memTreeNodeSetColourBlack(parent);
				memTreeNodeSetColourBlack(uncle);
				memTreeNodeSetColourRed(grandparent);

				// Continue climbing the tree.
				node = grandparent;

			}else{

				uintptr_t tempParentColour;

				if(node == memTreeNodeGetRight(parent)){

					// Current node is the right child of its parent.
					// Rotate the tree to the left.
					memTreeRotateTreeLeft(tree, parent);
					node = parent;
					parent = memTreeNodeGetParentColourless(node);

				}

				// Rotate the tree to the right.
				memTreeRotateTreeRight(tree, grandparent);

				// Swap the parent and grandparent's colours.
				tempParentColour = memTreeNodeGetColour(parent);
				memTreeNodeSetColourKeepParent(parent, memTreeNodeGetColour(grandparent));
				memTreeNodeSetColourKeepParent(grandparent, tempParentColour);

				// Continue climbing the tree.
				node = parent;

			}

		}else{

			// The uncle is to the left of the grandparent.
			if(uncle != NULL && memTreeNodeGetColourMasked(uncle) != MEMORY_TREE_NODE_COLOUR_BLACK){

				// The uncle is red, all we need to
				// do is some recolouring.
				memTreeNodeSetColourBlack(parent);
				memTreeNodeSetColourBlack(uncle);
				memTreeNodeSetColourRed(grandparent);

				// Continue climbing the tree.
				node = grandparent;

			}else{

				uintptr_t tempParentColour;

				if(node == memTreeNodeGetLeft(parent)){

					// Current node is the left child of its parent.
					// Rotate the tree to the right.
					memTreeRotateTreeRight(tree, parent);
					node = parent;
					parent = memTreeNodeGetParentColourless(node);

				}

				// Rotate the tree to the left.
				memTreeRotateTreeLeft(tree, grandparent);

				// Swap the parent and grandparent's colours.
				tempParentColour = memTreeNodeGetColour(parent);
				memTreeNodeSetColourKeepParent(parent, memTreeNodeGetColour(grandparent));
				memTreeNodeSetColourKeepParent(grandparent, tempParentColour);

				// Continue climbing the tree.
				node = parent;

			}

		}

		// Get the new parent node.
		parent = memTreeNodeGetParentColourless(node);

	}

	memTreeNodeSetColourBlack(tree->root);

}

static __FORCE_INLINE__ void memTreeInsert(memoryTree *const restrict tree, void *const block, const size_t bytes){

	/*
	** Add a free block to the red-black tree.
	*/

	byte_t *const node = memTreeBlockGetData(block);

	byte_t **address;
	uintptr_t parent;

	// Reset the active flag.
	memTreeBlockSetInactive(block);

	// Set the children to NULL.
	memTreeNodeGetLeft(node) = NULL;
	memTreeNodeGetRight(node) = NULL;

	// If the root node is free, we can exit early.
	if(tree->root == NULL){
		tree->root = node;
		memTreeNodeGetParent(node) = NULL;
		return;
	}

	address = &tree->root;

	// Begin by performing a BST insertion.
	do {

		parent = *((uintptr_t *)address);

		if(bytes <= memTreeNodeGetCurrent(parent)){

			// The new block is smaller than
			// the current block. Go to its left.
			address = memTreeNodeGetLeftPointer(parent);

		}else{

			// The new block is larger than
			// the current block. Go to its right.
			address = memTreeNodeGetRightPointer(parent);

		}

	} while(*address != NULL);

	// Insert the free block.
	*address = node;
	memTreeNodeGetColour(node) = parent | MEMORY_TREE_NODE_COLOUR_RED_MASK;

	// Fix red-black tree rule violations.
	memTreeRepairTree(tree, node);

}

static __FORCE_INLINE__ void memTreeRemove(memoryTree *const restrict tree, void *const block){

	/*
	** Remove a free block from the red-black tree.
	*/

	byte_t *const node = memTreeBlockGetNode(block);

	byte_t *child;
	uintptr_t childColour;

	byte_t *parent = memTreeNodeGetParentColourless(node);
	byte_t *left   = memTreeNodeGetLeft(node);
	byte_t *right  = memTreeNodeGetRight(node);

	// Set the active flag.
	memTreeBlockSetActive(block);

	if(left != NULL){
		if(right != NULL){

			// The node has two children.
			// Swap it with its in-order successor.
			byte_t *newParent;
			byte_t *next = memTreeNodeGetLeft(right);
			byte_t *successor = right;

			// Find the in-order successor.
			while(next != NULL){
				successor = next;
				next = memTreeNodeGetLeft(successor);
			}
			newParent = memTreeNodeGetParentColourless(successor);
			child = memTreeNodeGetRight(successor);

			// Swap the parents.
			if(newParent == node){
				// If the successor is a direct child
				// of the node, set the new parent to
				// the successor.
				newParent = successor;
				// Also swap the right child.
				right = node;
			}
			memTreeNodeSetParentKeepColour(node, newParent);
			// Set the parent's child pointer.
			memTreeNodeGetLeft(newParent) = node;
			if(parent != NULL){
				byte_t **parentLeft = memTreeNodeGetLeftPointer(parent);
				memTreeNodeSetParentKeepColour(successor, parent);
				// Set the parent's child pointer.
				if(*parentLeft == node){
					*parentLeft = successor;
				}else{
					memTreeNodeGetRight(parent) = successor;
				}
			}else{
				// Set the parent to NULL but keep its colour.
				memTreeNodeGetColour(successor) &= MEMORY_TREE_NODE_COLOUR_MASK;
				tree->root = successor;
			}

			// Swap the left children.
			memTreeNodeGetLeft(node) = NULL;
			memTreeNodeGetLeft(successor) = left;
			if(left != NULL){
				memTreeNodeSetParentKeepColour(left, successor);
			}

			// Swap the right children.
			if(child != NULL){
				memTreeNodeGetRight(node) = child;
				childColour = memTreeNodeGetColourMasked(child);
			}else{
				memTreeNodeGetRight(node) = NULL;
				childColour = MEMORY_TREE_NODE_COLOUR_BLACK;
			}
			memTreeNodeGetRight(successor) = right;
			if(right != NULL){
				memTreeNodeSetParentKeepColour(right, successor);
			}

			parent = newParent;
			left = next;
			right = child;

		}else{
			child = left;
			childColour = memTreeNodeGetColourMasked(left);
		}
	}else if(right != NULL){
		child = right;
		childColour = memTreeNodeGetColourMasked(right);
	}else{
		child = NULL;
		childColour = MEMORY_TREE_NODE_COLOUR_BLACK;
	}

	// The node should now have one or zero children.
	if(memTreeNodeGetColourMasked(node) == MEMORY_TREE_NODE_COLOUR_BLACK){

		const byte_t *current = node;

		// If the node is black, have it
		// assume the colour of its child.
		memTreeNodeGetColour(node) |= childColour;

		while(parent != NULL){

			uintptr_t parentColour;
			uintptr_t siblingColour;
			uintptr_t siblingLeftColour;
			uintptr_t siblingRightColour;

			byte_t *sibling;
			byte_t *siblingLeft;
			byte_t *siblingRight;
			byte_t *parentLeft = memTreeNodeGetLeft(parent);

			if(parentLeft == current){
				sibling = memTreeNodeGetRight(parent);
			}else{
				sibling = parentLeft;
			}

			if(sibling != NULL){

				// If the node's sibling is red, do
				// some recolours and a rotation.
				siblingColour = memTreeNodeGetColourMasked(sibling);
				if(siblingColour != MEMORY_TREE_NODE_COLOUR_BLACK){

					memTreeNodeSetColourRed(parent);
					memTreeNodeSetColourBlack(sibling);

					if(current == parentLeft){
						memTreeRotateTreeLeft(tree, parent);
					}else{
						memTreeRotateTreeRight(tree, parent);
					}

					// Update variables.
					sibling = memTreeNodeGetLeft(parent);
					if(sibling == current){
						sibling = memTreeNodeGetRight(parent);
					}
					if(sibling == NULL){
						goto NULL_SIBLING;
					}
					parent        = memTreeNodeGetParentColourless(current);
					parentLeft    = memTreeNodeGetLeft(parent);
					siblingColour = memTreeNodeGetColourMasked(sibling);

				}

				siblingLeft = memTreeNodeGetLeft(sibling);
				if(siblingLeft == NULL){
					siblingLeftColour = MEMORY_TREE_NODE_COLOUR_BLACK;
				}else{
					siblingLeftColour  = memTreeNodeGetColourMasked(siblingLeft);
				}

				siblingRight = memTreeNodeGetRight(sibling);
				if(siblingRight == NULL){
					siblingRightColour = MEMORY_TREE_NODE_COLOUR_BLACK;
				}else{
					siblingRightColour = memTreeNodeGetColourMasked(siblingRight);
				}

				parentColour = memTreeNodeGetColourMasked(parent);

				if(siblingColour      == MEMORY_TREE_NODE_COLOUR_BLACK &&
				   siblingLeftColour  == MEMORY_TREE_NODE_COLOUR_BLACK &&
				   siblingRightColour == MEMORY_TREE_NODE_COLOUR_BLACK){

					memTreeNodeSetColourRed(sibling);

					if(parentColour != MEMORY_TREE_NODE_COLOUR_BLACK){
						memTreeNodeSetColourBlack(parent);
						break;
					}else{
						current = parent;
						parent = memTreeNodeGetParentColourless(current);
					}

				}else{

					if(current == parentLeft &&
					   siblingColour      == MEMORY_TREE_NODE_COLOUR_BLACK &&
					   siblingLeftColour  != MEMORY_TREE_NODE_COLOUR_BLACK &&
					   siblingRightColour == MEMORY_TREE_NODE_COLOUR_BLACK){

						memTreeNodeSetColourRed(sibling);
						memTreeNodeSetColourBlack(siblingLeft);
						memTreeRotateTreeRight(tree, sibling);

						// Update variables.
						sibling = memTreeNodeGetLeft(parent);
						if(sibling == current){
							sibling = memTreeNodeGetRight(parent);
						}
						parent     = memTreeNodeGetParentColourless(current);
						parentLeft = memTreeNodeGetLeft(parent);
						if(sibling == NULL){
							siblingLeft  = NULL;
							siblingRight = NULL;
						}else{
							siblingLeft  = memTreeNodeGetLeft(sibling);
							siblingRight = memTreeNodeGetRight(sibling);
						}

					}else if(current != parentLeft &&
					         siblingColour      == MEMORY_TREE_NODE_COLOUR_BLACK &&
					         siblingLeftColour  == MEMORY_TREE_NODE_COLOUR_BLACK &&
					         siblingRightColour != MEMORY_TREE_NODE_COLOUR_BLACK){

						memTreeNodeSetColourRed(sibling);
						memTreeNodeSetColourBlack(siblingRight);
						memTreeRotateTreeLeft(tree, sibling);

						// Update variables.
						sibling = memTreeNodeGetLeft(parent);
						if(sibling == current){
							sibling = memTreeNodeGetRight(parent);
						}
						parent     = memTreeNodeGetParentColourless(current);
						parentLeft = memTreeNodeGetLeft(parent);
						if(sibling == NULL){
							siblingLeft  = NULL;
							siblingRight = NULL;
						}else{
							siblingLeft  = memTreeNodeGetLeft(sibling);
							siblingRight = memTreeNodeGetRight(sibling);
						}

						siblingRight = memTreeNodeGetRight(sibling);
						if(siblingRight == NULL){
							siblingRightColour = MEMORY_TREE_NODE_COLOUR_BLACK;
						}else{
							siblingRightColour = memTreeNodeGetColourMasked(siblingRight);
						}

					}

					// Set the sibling to the parent's colour.
					memTreeNodeSetColourKeepParent(sibling, (uintptr_t)parent);
					memTreeNodeSetColourBlack(parent);

					if(current == parentLeft){
						if(siblingRight != NULL){
							memTreeNodeSetColourBlack(siblingRight);
						}
						memTreeRotateTreeLeft(tree, parent);
					}else{
						if(siblingLeft != NULL){
							memTreeNodeSetColourBlack(siblingLeft);
						}
						memTreeRotateTreeRight(tree, parent);
					}

					break;

				}

			}else{
				NULL_SIBLING:
				if(memTreeNodeGetColourMasked(parent) != MEMORY_TREE_NODE_COLOUR_BLACK){
					memTreeNodeSetColourBlack(parent);
					break;
				}else{
					current = parent;
					parent = memTreeNodeGetParentColourless(current);
				}
			}

		}

	}

	// Replace the node with its original child.
	parent = memTreeNodeGetParentColourless(node);
	if(parent == NULL){
		tree->root = child;
		if(child != NULL){
			memTreeNodeGetParent(child) = NULL;
		}
	}else{
		// Set the parent's child.
		byte_t **parentLeft = memTreeNodeGetLeftPointer(parent);
		if(node == *parentLeft){
			*parentLeft = child;
		}else{
			memTreeNodeGetRight(parent) = child;
		}
		if(child != NULL){
			memTreeNodeSetParentKeepColour(child, parent);
		}
	}

}

void *memTreeAllocate(memoryTree *const restrict tree, const size_t bytes){

	/*
	** Retrieves a new block of memory from the general
	** purpose allocator and updates the "next" pointer.
	*/

	if(tree->root == NULL){
		return NULL;
	}else{

		const size_t totalBytes = (bytes <= MEMORY_TREE_BLOCK_SIZE ? MEMORY_TREE_BLOCK_TOTAL_SIZE : (bytes + MEMORY_TREE_BLOCK_HEADER_SIZE));

		size_t blockSize = 0;
		byte_t *block = NULL;

		// Loop through the red-black tree and retrieve
		// the best-fitting free block.
		byte_t *current = memTreeNodeGetBlock(tree->root);
		for(;;){

			const size_t currentSize = memTreeBlockGetCurrent(current);

			if(totalBytes < currentSize){

				// The block is too big,
				// go to the left.
				byte_t *next = memTreeBlockGetLeft(current);

				// Set the new best fit.
				blockSize = currentSize;
				block = current;

				if(next != NULL){
					current = memTreeNodeGetBlock(next);
				}else{

					// There is no node to the left.
					// Settle on a bigger block and
					// attempt to split it.
					size_t nextSize;
					next = memTreeAlignStartBlock((block + totalBytes));
					nextSize = block + blockSize - next;

					// Remove the block from the red-black tree.
					//memTreeRemove(tree, block);

					if(nextSize >= MEMORY_TREE_BLOCK_TOTAL_SIZE){
						// There's enough room for a split.
						// Include the alignment padding in
						// the allocated block.
						blockSize -= nextSize;
						memTreeBlockGetCurrent(next) = nextSize;
						if(memTreeBlockIsLast(memTreeBlockGetFlags(block)) == 0){
							byte_t *nextNext = next+nextSize;
							// blockSize should always have a 0 in the LSB.
							memTreeBlockGetPrevious(next) = blockSize;
							// Set the next-next block's new previous block size.
							memTreeBlockGetFlags(nextNext) = nextSize | (memTreeBlockGetFlags(nextNext) & MEMORY_TREE_BLOCK_FLAGS_MASK);
						}else{
							// blockSize should always have a 0 in the LSB.
							// Also inherit whether or not the block was the last.
							memTreeBlockGetPrevious(next) = blockSize | MEMORY_TREE_BLOCK_LAST;
							// The previous block can't be the last node anymore.
							memTreeBlockGetFlags(block) &= MEMORY_TREE_BLOCK_LAST_MASK;
						}
						// Insert the new free block.
						memTreeInsert(tree, next, nextSize);
					}

					break;

				}


			}else if(totalBytes > currentSize){

				// The block is too small,
				// go to the right.
				byte_t *next = memTreeBlockGetRight(current);

				if(next != NULL){
					current = memTreeNodeGetBlock(next);
				}else if(block != NULL){

					// There is no node to the right,
					// but we do know a node that is
					// big enough to accommodate the
					// data, so we can prepare that.
					size_t nextSize;
					next = memTreeAlignStartBlock((block + totalBytes));
					nextSize = block + blockSize - next;

					// Remove the block from the red-black tree.
					//memTreeRemove(tree, block);

					if(nextSize >= MEMORY_TREE_BLOCK_TOTAL_SIZE){
						// There's enough room for a split.
						// Include the alignment padding in
						// the allocated block.
						blockSize -= nextSize;
						memTreeBlockGetCurrent(next) = nextSize;
						if(memTreeBlockIsLast(memTreeBlockGetFlags(block)) == 0){
							byte_t *nextNext = next+nextSize;
							// blockSize should always have a 0 in the LSB.
							memTreeBlockGetPrevious(next) = blockSize;
							// Set the next-next block's new previous block size.
							memTreeBlockGetFlags(nextNext) = nextSize | (memTreeBlockGetFlags(nextNext) & MEMORY_TREE_BLOCK_FLAGS_MASK);
						}else{
							// blockSize should always have a 0 in the LSB.
							// Also inherit whether or not the block was the last.
							memTreeBlockGetPrevious(next) = blockSize | MEMORY_TREE_BLOCK_LAST;
							// The previous block can't be the last node anymore.
							memTreeBlockGetFlags(block) &= MEMORY_TREE_BLOCK_LAST_MASK;
						}
						// Insert the new free block.
						memTreeInsert(tree, next, nextSize);
					}

					break;

				}else{
					// There is no block big enough
					// to accommodate the data.
					// Return NULL.
					return NULL;
				}

			}else{
				// We've somehow found a block
				// that fits perfectly.
				block = current;
				blockSize = currentSize;
				// Remove the block from the red-black tree.
				//memTreeRemove(tree, block);
				break;
			}

		}

		// Remove the block from the red-black tree.
		memTreeRemove(tree, block);

		// Set the new block's header information.
		// The "previous" header data should already be set.
		memTreeBlockGetCurrent(block) = blockSize;

		// Return a pointer to the data.
		return memTreeBlockGetData(block);

	}

}

void memTreeFree(memoryTree *const restrict tree, void *const restrict block){

	/*
	** Frees a block of memory from the general
	** purpose allocator.
	**
	** Coalesces the previous and next blocks
	** if they are free.
	*/

	byte_t *cBlock = memTreeDataGetBlock(block);
	size_t cBytes = memTreeBlockGetCurrent(cBlock);
	size_t cFlags = memTreeBlockGetPrevious(cBlock);

	byte_t *prev;
	byte_t *next;

	prev = cBlock - memTreeBlockPreviousFlagless(cFlags);
	next = cBlock + cBytes;

	// Check if there is a preceding
	// block that is inactive.
	if(memTreeBlockIsFirst(cFlags) == 0 && memTreeBlockGetActiveMasked(prev) == MEMORY_TREE_BLOCK_INACTIVE){
		// Perform a merge.
		cBlock = prev;
		cBytes += memTreeBlockGetCurrent(prev);
		memTreeBlockSetPreviousKeepLastAndActive(cFlags, memTreeBlockGetPrevious(prev));
		// Remove the free block from the tree.
		memTreeRemove(tree, prev);
	}
	// Check if there is a following
	// block that is inactive.
	if(memTreeBlockIsLast(cFlags) == 0){
		if(memTreeBlockGetActiveMasked(next) == MEMORY_TREE_BLOCK_INACTIVE){
			const size_t nextSize = memTreeBlockGetCurrent(next);
			// Perform a merge.
			cBytes += nextSize;
			// Remove the free block from the tree.
			memTreeRemove(tree, next);
			if(memTreeBlockIsLast(memTreeBlockGetPrevious(next)) == 0){
				next += nextSize;
				// Set the next block's "previous" property.
				memTreeBlockGetPrevious(next) = (memTreeBlockGetPrevious(next) & MEMORY_TREE_BLOCK_FLAGS_MASK) | cBytes;
			}else{
				// Mark the merged block as the last.
				cFlags |= MEMORY_TREE_BLOCK_LAST;
			}
		}else{
			// Set the next block's "previous" property.
			memTreeBlockGetPrevious(next) = (memTreeBlockGetPrevious(next) & MEMORY_TREE_BLOCK_FLAGS_MASK) | cBytes;
		}
	}

	// Set the linked list header data.
	memTreeBlockGetCurrent(cBlock) = cBytes;
	memTreeBlockGetPrevious(cBlock) = cFlags & MEMORY_TREE_BLOCK_INACTIVE_MASK;

	// Add the new free block to the tree.
	memTreeInsert(tree, cBlock, cBytes);

}

void *memTreeReallocate(memoryTree *const restrict tree, void *const block, const size_t bytes){

	/*
	** Reallocates a block of memory. Can potentially
	** be called to try and "defragment" a particular
	** block.
	**
	** If no defragmentation is possible, this function
	** is effectively the same as an alloc + copy + free.
	*/

	byte_t *rBlock = block;

	// If the block is inactive, allocate a new one.
	if(block == NULL || memTreeDataGetActiveMasked(block) == MEMORY_TREE_BLOCK_INACTIVE){

		return memTreeAllocate(tree, bytes);

	}else{

		byte_t *cBlock = memTreeDataGetBlock(rBlock);
		size_t cBytes = memTreeBlockGetCurrent(cBlock);
		size_t cFlags = memTreeBlockGetPrevious(cBlock);

		const size_t totalBytes = (bytes <= MEMORY_TREE_BLOCK_SIZE ? MEMORY_TREE_BLOCK_TOTAL_SIZE : (bytes + MEMORY_TREE_BLOCK_HEADER_SIZE));
		const size_t copyBytes = cBytes < totalBytes ? (cBytes - MEMORY_TREE_BLOCK_HEADER_SIZE) : bytes;

		byte_t *prev = cBlock - memTreeBlockPreviousFlagless(cFlags);
		byte_t *next = cBlock + cBytes;

		// Check if there is a preceding
		// block that is inactive.
		if(memTreeBlockIsFirst(cFlags) == 0 && memTreeBlockGetActiveMasked(prev) == MEMORY_TREE_BLOCK_INACTIVE){
			// Perform a merge.
			cBlock = prev;
			cBytes += memTreeBlockGetCurrent(prev);
			memTreeBlockSetPreviousKeepLastAndActive(cFlags, memTreeBlockGetPrevious(prev));
			// Remove the free block from the tree.
			memTreeRemove(tree, prev);
		}
		// Check if there is a following
		// block that is inactive.
		if(memTreeBlockIsLast(cFlags) == 0){
			if(memTreeBlockGetActiveMasked(next) == MEMORY_TREE_BLOCK_INACTIVE){
				const size_t nextSize = memTreeBlockGetCurrent(next);
				// Perform a merge.
				cBytes += nextSize;
				// Remove the free block from the tree.
				memTreeRemove(tree, next);
				if(memTreeBlockIsLast(memTreeBlockGetFlags(next)) == 0){
					next += nextSize;
					// Set the next block's "previous" property.
					memTreeBlockGetPrevious(next) &= MEMORY_TREE_BLOCK_FLAGS_MASK;
					memTreeBlockGetPrevious(next) |= cBytes;
				}else{
					// Mark the merged block as the last.
					cFlags |= MEMORY_TREE_BLOCK_LAST;
				}
			}else{
				// Set the next block's "previous" property.
				memTreeBlockGetPrevious(next) &= MEMORY_TREE_BLOCK_FLAGS_MASK;
				memTreeBlockGetPrevious(next) |= cBytes;
			}
		}

		// Check if we can fit the new data
		// into this particular fragment.
		if(totalBytes <= cBytes){

			// We can coalesce the previous and / or
			// next blocks to create enough room for
			// the new data.
			byte_t *cNext = memTreeAlignStartBlock((cBlock + totalBytes));
			size_t nextSize = cBlock + cBytes - cNext;

			rBlock = memTreeBlockGetData(cBlock);

			// Copy the block's data over.
			memmove((void *)rBlock, (void *)block, copyBytes);

			if(nextSize >= MEMORY_TREE_BLOCK_TOTAL_SIZE){

				// There's enough room for a split.
				// Include the alignment padding in
				// the allocated block.
				cBytes -= nextSize;
				memTreeBlockGetCurrent(cNext) = nextSize;

				if(memTreeBlockIsLast(cFlags) == 0){
					byte_t *cNextNext = cNext+nextSize;
					// cBytes should always have a 0 in the LSB.
					memTreeBlockGetPrevious(cNext) = cBytes;
					// Set the next-next block's new previous block size.
					memTreeBlockGetFlags(cNextNext) = nextSize | (memTreeBlockGetFlags(cNextNext) & MEMORY_TREE_BLOCK_FLAGS_MASK);
				}else{
					// cBytes should always have a 0 in the LSB.
					// Also inherit whether or not the block was the last.
					memTreeBlockGetPrevious(cNext) = cBytes | MEMORY_TREE_BLOCK_LAST;
					// The previous block can't be the last node anymore.
					cFlags &= MEMORY_TREE_BLOCK_LAST_MASK;
				}

				// Insert the new free block.
				memTreeInsert(tree, cNext, nextSize);

			}

			// Set the new block's header information.
			memTreeBlockGetCurrent(cBlock) = cBytes;
			memTreeBlockGetPrevious(cBlock) = cFlags;

		}else{

			// We'll have to look for a new block.
			rBlock = memTreeAllocate(tree, bytes);

			if(rBlock != NULL){

				// Copy the block's data over.
				memcpy((void *)rBlock, (void *)block, copyBytes);

				// Set the linked list header data.
				memTreeBlockGetCurrent(cBlock) = cBytes;
				memTreeBlockGetPrevious(cBlock) = cFlags;

				// Add the new free block to the tree.
				memTreeInsert(tree, cBlock, cBytes);

			}

		}

		return rBlock;

	}

}

void *memTreeSetupMemory(void *const start, const size_t bytes, const size_t length){
	byte_t *const root = memTreeAlignStartData(start);
	byte_t *const block = memTreeNodeGetBlock(root);
	memTreeBlockGetCurrent(block) = memTreeAllocationSize(start, bytes, length) - sizeof(memoryRegion);
	memTreeBlockGetPrevious(block) = MEMORY_TREE_BLOCK_FIRST | MEMORY_TREE_BLOCK_LAST;
	memTreeNodeGetLeft(root) = NULL;
	memTreeNodeGetRight(root) = NULL;
	memTreeNodeGetParent(root) = NULL;
	return block;
}

void memTreeClear(memoryTree *const restrict tree){
	byte_t *block;
	tree->root = memTreeAlignStartData(tree->region->start);
	block = memTreeNodeGetBlock(tree->root);
	memTreeBlockGetCurrent(block) = (byte_t *)tree->region - block;
	memTreeBlockGetPrevious(block) = MEMORY_TREE_BLOCK_FIRST | MEMORY_TREE_BLOCK_LAST;
	memTreeNodeGetLeft(tree->root) = NULL;
	memTreeNodeGetRight(tree->root) = NULL;
	memTreeNodeGetParent(tree->root) = NULL;
}

void *memTreeExtend(memoryTree *const restrict tree, void *const start, const size_t bytes, const size_t length){

	/*
	** Extends the memory allocator.
	** Its logical function is similar to a
	** realloc, but it creates a new chunk
	** and links it.
	*/

	if(start){

		const size_t totalBytes = memTreeAllocationSize(start, bytes, length) - sizeof(memoryRegion);

		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + totalBytes);
		memRegionPrepend(&tree->region, newRegion, start);

		memTreeSetupMemory(start, bytes, length);
		memTreeInsert(tree, memTreeAlignStartData(start), totalBytes);

	}

	return start;

}

void memTreeDelete(memoryTree *const restrict tree){
	memRegionFree(tree->region);
}

#ifdef MEMORY_DEBUG

#include <stdio.h>

void memTreePrintFreeBlocks(memoryTree *const restrict tree, const unsigned int recursions){

	/*
	** In-order tree traversal where
	** each node's size is printed.
	*/

	const byte_t *node = tree->root;
	const byte_t *nodeLeft;
	const byte_t *nodeRight;
	const byte_t *nodeParent;
	unsigned int i = 0;

	fputs("MEMORY_DEBUG: Free Blocks\n~~~~~~~~~~~~~~~~~~~~~~~~~\n", stdout);

	if(node != NULL){

		// Start on the left-most node.
		while((nodeLeft = memTreeNodeGetLeft(node)) != NULL){
			node = nodeLeft;
		}

		do {

			++i;

			printf(
				   "Address:%u Size:%u Previous:%u First:%u Last:%u State:Inactive\n",
				   (uintptr_t)node,
				   memTreeNodeGetCurrent(node),
				   memTreeNodeGetPreviousFlagless(node),
			       memTreeBlockIsFirst(memTreeNodeGetFlags(node)) > 0,
			       memTreeBlockIsLast(memTreeNodeGetFlags(node)) > 0
			);

			nodeRight = memTreeNodeGetRight(node);
			if(nodeRight != NULL){
				// If we can go right, go right and
				// then try to go left again.
				node = nodeRight;
				while((nodeLeft = memTreeNodeGetLeft(node)) != NULL){
					node = nodeLeft;
				}
			}else{
				// A leaf node was reached.
				// If the leaf node is to the right of its
				// parent, keep going up until this is no
				// longer the case.
				while(
					(nodeParent = memTreeNodeGetParentColourless(node)) != NULL &&
					node == memTreeNodeGetRight(nodeParent)
				){
					node = nodeParent;
				}
				// Go up one final time for the left node.
				node = nodeParent;
			}

			if(i == recursions){
				break;
			}

		} while(node != NULL);

	}

	fputs("\n", stdout);

}

void memTreePrintAllBlocks(memoryTree *const restrict tree){

	size_t regionNum = 0;
	const memoryRegion *region = tree->region;

	fputs("MEMORY_DEBUG: All Blocks\n~~~~~~~~~~~~~~~~~~~~~~~~", stdout);
	while(region != NULL){

		const byte_t *block = memTreeAlignStartBlock(region->start);

		printf("\nRegion #%u:\n", regionNum);
		while(block < (byte_t *)region){

			const size_t size =  memTreeBlockGetCurrent(block);
			if(memTreeBlockGetActiveMasked(block) == MEMORY_TREE_BLOCK_INACTIVE){
				printf(
				       "Address:%u Size:%u Previous:%u First:%u Last:%u State:Inactive\nParent:%u Left:%u Right:%u Colour:%s\n",
				       (uintptr_t)memTreeBlockGetNode(block),
				       size,
				       memTreeBlockGetPreviousFlagless(block),
				       memTreeBlockIsFirst(memTreeBlockGetFlags(block)) > 0,
				       memTreeBlockIsLast(memTreeBlockGetFlags(block)) > 0,
				       (uintptr_t)memTreeBlockGetParentColourless(block),
				       (uintptr_t)memTreeBlockGetLeft(block),
				       (uintptr_t)memTreeBlockGetRight(block),
				       memTreeBlockGetColourMasked(block) == 0 ? "Black" : "Red"
				);
			}else{
				printf(
				       "Address:%u Size:%u Previous:%u First:%u Last:%u State:Active\n",
				       (uintptr_t)memTreeBlockGetNode(block),
				       size,
				       memTreeBlockGetPreviousFlagless(block),
				       memTreeBlockIsFirst(memTreeBlockGetFlags(block)) > 0,
				       memTreeBlockIsLast(memTreeBlockGetFlags(block)) > 0
				);
			}
			block += size;

		}

		region = region->next;
		++regionNum;

		fputs("\n", stdout);

	}

}

#endif
