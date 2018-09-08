#include "memoryTree.h"
#include <string.h>

byte_t *memTreeInit(memoryTree *tree, byte_t *start, const size_t bytes, const size_t length){

	/*
	** Initialize a general purpose memory allocator.
	**
	** If "length" is specified, it will optimize the size for
	** "length"-many allocations of size "bytes", otherwise
	** the total allocated size will be "bytes".
	*/

	if(start){

		tree->start = start;
		tree->end = start + memTreeAllocationSize(start, bytes, length);

		memTreeClear(tree);

	}

	return start;

}

static inline void memTreeRotateTreeLeft(memoryTree *tree, byte_t *node){

	/*
	** Rotate the red-black tree to the left.
	*/

	byte_t *parent = memTreeNodeGetParentColourless(node);
	byte_t *right = memTreeNodeGetRight(node);
	byte_t *rightleft = memTreeNodeGetLeft(right);

	memTreeNodeGetRight(node) = rightleft;

	if(rightleft != NULL){
		memTreeNodeGetParent(rightleft) = node;
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
	memTreeNodeGetParent(node) = right;

}

static inline void memTreeRotateTreeRight(memoryTree *tree, byte_t *node){

	/*
	** Rotate the red-black tree to the right.
	*/

	byte_t *parent = memTreeNodeGetParentColourless(node);
	byte_t *left = memTreeNodeGetLeft(node);
	byte_t *leftright = memTreeNodeGetRight(left);

	memTreeNodeGetLeft(node) = leftright;

	if(leftright != NULL){
		memTreeNodeGetParent(leftright) = node;
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
	memTreeNodeGetParent(node) = left;

}

static inline void memTreeRepairTree(memoryTree *tree, byte_t *node){

	/*
	** Fix any violations of the red-black
	** tree's rules, starting at "node".
	*/

	byte_t *parent = memTreeNodeGetParentColourless(node);

	// Continue looping while the current node
	// and its parent are both red. If the root
	// node is reached, the loop will terminate.
	while(memTreeNodeGetColourMasked(parent) != MEMORY_TREE_NODE_COLOUR_BLACK){

		byte_t *grandparent = memTreeNodeGetParentColourless(parent);
		byte_t *uncle = memTreeNodeGetLeft(grandparent);

		if(parent == uncle){

			// The uncle is to the right of the grandparent.
			uncle = memTreeNodeGetRight(grandparent);

			if(memTreeNodeGetColourMasked(uncle) != MEMORY_TREE_NODE_COLOUR_BLACK){

				// The uncle is red, all we need to
				// do is some recolouring.
				memTreeNodeSetColourBlack(parent);
				memTreeNodeSetColourBlack(uncle);
				memTreeNodeSetColourRed(grandparent);

				// Continue climbing the tree.
				node = grandparent;

			}else{

				uintptr_t *parentColourBit;
				uintptr_t *grandparentColourBit;
				uintptr_t tempParentColour;

				if(node == memTreeNodeGetRight(parent)){

					// Current node is the right child of its parent.
					// Rotate the tree to the left.
					memTreeRotateTreeLeft(tree, parent);
					node = parent;
					parent = memTreeNodeGetParentColourless(node);

				}

				parentColourBit      = memTreeNodeGetColourPointer(parent);
				grandparentColourBit = memTreeNodeGetColourPointer(grandparent);

				// Rotate the tree to the right.
				memTreeRotateTreeRight(tree, grandparent);

				// Swap the parent and grandparent's colours.
				tempParentColour = *parentColourBit | MEMORY_TREE_NODE_COLOURLESS_MASK;
				*parentColourBit &= *grandparentColourBit | MEMORY_TREE_NODE_COLOURLESS_MASK;
				*grandparentColourBit &= tempParentColour;

				// Continue climbing the tree.
				node = parent;

			}

		}else{

			// The uncle is to the left of the grandparent.
			if(memTreeNodeGetColourMasked(uncle) != MEMORY_TREE_NODE_COLOUR_BLACK){

				// The uncle is red, all we need to
				// do is some recolouring.
				memTreeNodeSetColourBlack(parent);
				memTreeNodeSetColourBlack(uncle);
				memTreeNodeSetColourRed(grandparent);

				// Continue climbing the tree.
				node = grandparent;

			}else{

				uintptr_t *parentColourBit;
				uintptr_t *grandparentColourBit;
				uintptr_t tempParentColour;

				if(node == memTreeNodeGetLeft(parent)){

					// Current node is the left child of its parent.
					// Rotate the tree to the right.
					memTreeRotateTreeRight(tree, parent);
					node = parent;
					parent = memTreeNodeGetParentColourless(node);

				}

				parentColourBit      = memTreeNodeGetColourPointer(parent);
				grandparentColourBit = memTreeNodeGetColourPointer(grandparent);

				// Rotate the tree to the left.
				memTreeRotateTreeLeft(tree, grandparent);

				// Swap the parent and grandparent's colours.
				tempParentColour = *parentColourBit | MEMORY_TREE_NODE_COLOURLESS_MASK;
				*parentColourBit &= *grandparentColourBit | MEMORY_TREE_NODE_COLOURLESS_MASK;
				*grandparentColourBit &= tempParentColour;

				// Continue climbing the tree.
				node = parent;

			}

		}

		// Get the new parent node.
		parent = memTreeNodeGetParentColourless(node);

	}

}

void memTreeInsert(memoryTree *tree, byte_t *block, const size_t bytes){

	/*
	** Add a free block to the red-black tree.
	*/

	byte_t **address = &tree->root;
	uintptr_t current = *((uintptr_t *)address);

	// Reset the active flag.
	memTreeBlockSetInactive(block);

	// Set the children to NULL.
	memTreeBlockGetLeft(block) = NULL;
	memTreeBlockGetRight(block) = NULL;

	// If the root node is free, we can exit early.
	if(current == 0){
		*address = block;
		memTreeBlockGetParent(block) = NULL;
		return;
	}

	// Begin by performing a BST insertion.
	do {

		const size_t currentSize = memTreeBlockGetCurrent(current);

		if(bytes <= currentSize){

			// The new block is smaller than
			// the current block. Go to its left.
			address = memTreeBlockGetLeftPointer(current);

		}else{

			// The new block is larger than
			// the current block. Go to its right.
			address = memTreeBlockGetRightPointer(current);

		}

		current = *((uintptr_t *)address);

	} while(current != 0);

	// Insert the free block.
	*address = block;
	memTreeBlockGetColour(block) = current | MEMORY_TREE_NODE_COLOUR_RED_MASK;

	// Fix red-black tree rule violations.
	memTreeRepairTree(tree, memTreeBlockGetNode(block));

}

void memTreeRemove(memoryTree *tree, byte_t *block){

	/*
	** Remove a free block from the red-black tree.
	*/

	byte_t *child;
	uintptr_t childColour;

	byte_t *parent = memTreeBlockGetParentColourless(block);
	byte_t *left   = memTreeBlockGetLeft(block);
	byte_t *right  = memTreeBlockGetRight(block);

	// Set the active flag.
	memTreeBlockSetActive(block);

	if(left != NULL){
		if(right != NULL){

			// The node has two children.
			// Swap it with its in-order successor.
			byte_t *newParent;
			byte_t *next = memTreeBlockGetLeft(right);
			byte_t *successor = right;

			// Find the in-order successor.
			while(next != NULL){
				successor = next;
				next = memTreeBlockGetLeft(successor);
			}
			newParent = memTreeNodeGetParentColourless(successor);
			child = memTreeBlockGetRight(successor);

			// Swap the parents.
			memTreeNodeSetParentKeepColour(block, newParent);
			// Set the parent's child pointer.
			memTreeBlockGetLeft(newParent) = block;
			if(parent != NULL){
				byte_t **parentLeft = memTreeBlockGetLeftPointer(parent);
				memTreeNodeSetParentKeepColour(successor, parent);
				// Set the parent's child pointer.
				if(*parentLeft == block){
					*parentLeft = successor;
				}else{
					memTreeBlockGetRight(parent) = successor;
				}
			}else{
				// Set the parent to NULL but keep its colour.
				memTreeBlockGetColour(successor) &= MEMORY_TREE_NODE_COLOUR_MASK;
				tree->root = successor;
			}

			// Swap the left children.
			memTreeBlockGetLeft(block) = NULL;
			memTreeBlockGetLeft(successor) = left;

			// Swap the right children.
			if(child != NULL){
				memTreeBlockGetRight(block) = child;
				childColour = memTreeBlockGetColourMasked(child);
			}else{
				memTreeBlockGetRight(block) = NULL;
				childColour = 0;
			}
			memTreeBlockGetRight(successor) = right;

			parent = newParent;
			left = next;
			right = child;

			block = successor;

		}else{
			child = left;
			childColour = memTreeBlockGetColourMasked(left);
		}
	}else if(right != NULL){
		child = right;
		childColour = memTreeBlockGetColourMasked(right);
	}else{
		child = NULL;
		childColour = 0;
	}

	// The node should now have one or zero children.
	if(memTreeBlockGetColourMasked(block) == MEMORY_TREE_NODE_COLOUR_BLACK){

		// If the node is black, have it
		// assume the colour of its child.
		memTreeBlockGetColour(block) |= childColour;

		while(parent != NULL){

			uintptr_t parentColour;
			uintptr_t siblingColour;
			uintptr_t siblingLeftColour;
			uintptr_t siblingRightColour;

			byte_t *sibling;
			byte_t *siblingLeft;
			byte_t *siblingRight;
			byte_t *parentLeft = memTreeBlockGetLeft(parent);

			if(parentLeft == block){
				sibling = memTreeBlockGetRight(parent);
			}else{
				sibling = parentLeft;
			}

			// If the node's sibling is red, do
			// some recolours and a rotation.
			siblingColour = memTreeBlockGetColourMasked(sibling);
			if(siblingColour != MEMORY_TREE_NODE_COLOUR_BLACK){

				memTreeBlockSetColourRed(parent);
				memTreeBlockSetColourBlack(sibling);

				if(block == parentLeft){
					memTreeRotateTreeLeft(tree, parent);
				}else{
					memTreeRotateTreeRight(tree, parent);
				}

				// Update variables.
				sibling = memTreeBlockGetLeft(parent);
				if(sibling == block){
					sibling = memTreeBlockGetRight(parent);
				}
				parent        = memTreeNodeGetParentColourless(block);
				parentLeft    = memTreeBlockGetLeft(parent);
				siblingColour = memTreeBlockGetColourMasked(sibling);

			}

			siblingLeft        = memTreeBlockGetLeft(sibling);
			siblingRight       = memTreeBlockGetRight(sibling);
			parentColour       = memTreeBlockGetColourMasked(parent);
			siblingLeftColour  = memTreeBlockGetColourMasked(siblingLeft);
			siblingRightColour = memTreeBlockGetColourMasked(siblingRight);

			if(siblingColour      == MEMORY_TREE_NODE_COLOUR_BLACK &&
			   siblingLeftColour  == MEMORY_TREE_NODE_COLOUR_BLACK &&
			   siblingRightColour == MEMORY_TREE_NODE_COLOUR_BLACK){

				memTreeBlockSetColourRed(sibling);

				if(parentColour != MEMORY_TREE_NODE_COLOUR_BLACK){
					memTreeBlockSetColourBlack(parent);
					break;
				}

			}else{

				if(block == parentLeft &&
				   siblingColour      == MEMORY_TREE_NODE_COLOUR_BLACK &&
				   siblingLeftColour  != MEMORY_TREE_NODE_COLOUR_BLACK &&
				   siblingRightColour == MEMORY_TREE_NODE_COLOUR_BLACK){

					memTreeBlockSetColourRed(sibling);
					memTreeBlockSetColourBlack(siblingLeft);
					memTreeRotateTreeRight(tree, sibling);

				}else if(block != parentLeft &&
				   siblingColour      == MEMORY_TREE_NODE_COLOUR_BLACK &&
				   siblingLeftColour  == MEMORY_TREE_NODE_COLOUR_BLACK &&
				   siblingRightColour != MEMORY_TREE_NODE_COLOUR_BLACK){

					memTreeBlockSetColourRed(sibling);
					memTreeBlockSetColourBlack(siblingRight);
					memTreeRotateTreeRight(tree, sibling);

				}else{

					// Set the sibling to the parent's colour.
					memTreeBlockGetColour(sibling) &= (uintptr_t)parent | MEMORY_TREE_NODE_COLOURLESS_MASK;
					memTreeBlockSetColourBlack(parent);

					if(block == parentLeft){
						memTreeBlockSetColourBlack(siblingRight);
						memTreeRotateTreeRight(tree, parent);
					}else{
						memTreeBlockSetColourBlack(siblingLeft);
						memTreeRotateTreeRight(tree, parent);
					}

				}

				break;

			}

		}

	}

	// Replace the node with its original child.
	parent = memTreeBlockGetParentColourless(block);
	if(parent == NULL){
		tree->root = child;
		if(child != NULL){
			memTreeBlockGetParent(child) = NULL;
		}
	}else{
		// Set the parent's child.
		byte_t **parentLeft = memTreeBlockGetLeftPointer(parent);
		if(block == *parentLeft){
			*parentLeft = child;
		}else{
			memTreeBlockGetLeft(parent) = child;
		}
		if(child != NULL){
			memTreeBlockSetParentKeepColour(child, parent);
		}
	}

}

byte_t *memTreeAllocate(memoryTree *tree, const size_t bytes){

	/*
	** Retrieves a new block of memory from the general
	** purpose allocator and updates the "next" pointer.
	*/

	const size_t totalBytes = MEMORY_TREE_BLOCK_HEADER_SIZE +
	                          (bytes <= MEMORY_TREE_BLOCK_SIZE ? MEMORY_TREE_BLOCK_SIZE : bytes);

	size_t blockSize = 0;
	byte_t *block = NULL;

	// Loop through the red-black tree and retrieve
	// the best-fitting free block.
	byte_t *current = tree->root;
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
				current = next;
			}else{

				// There is no node to the left.
				// Settle on a bigger block and
				// attempt to split it.
				size_t nextSize;
				next = block + MEMORY_TREE_ALIGN(totalBytes);
				nextSize = block + blockSize - next;

				// Remove the block from the red-black tree.
				memTreeRemove(tree, block);

				if(nextSize >= MEMORY_TREE_BLOCK_TOTAL_SIZE){
					// There's enough room for a split.
					// Include the alignment padding in
					// the allocated block.
					blockSize -= nextSize;
					memTreeBlockGetCurrent(next) = nextSize;
					// blockSize should always have a 0 in the LSB.
					// Also inherit whether or not the block was the last.
					memTreeBlockGetPrevious(next) = blockSize | (memTreeBlockGetFlags(block) & MEMORY_TREE_BLOCK_LAST);
					// The previous block can't be the last node anymore.
					memTreeBlockGetFlags(block) &= MEMORY_TREE_BLOCK_LAST_MASK;
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
				current = next;
			}else if(block != NULL){

				// There is no node to the right,
				// but we do know a node that is
				// big enough to accommodate the
				// data, so we can prepare that.
				size_t nextSize;
				next = block + MEMORY_TREE_ALIGN(totalBytes);
				nextSize = block + blockSize - next;

				// Remove the block from the red-black tree.
				memTreeRemove(tree, block);

				if(nextSize >= MEMORY_TREE_BLOCK_TOTAL_SIZE){
					// There's enough room for a split.
					// Include the alignment padding in
					// the allocated block.
					blockSize -= nextSize;
					memTreeBlockGetCurrent(next) = nextSize;
					// blockSize should always have a 0 in the LSB.
					// Also inherit whether or not the block was the last.
					memTreeBlockGetPrevious(next) = blockSize | (memTreeBlockGetFlags(block) & MEMORY_TREE_BLOCK_LAST);
					// The previous block can't be the last node anymore.
					memTreeBlockGetFlags(block) &= MEMORY_TREE_BLOCK_LAST_MASK;
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
			// Remove the block from the red-black tree.
			memTreeRemove(tree, block);
			break;
		}

	}

	// Set the new block's header information.
	// The "previous" header data should already be set.
	memTreeBlockGetCurrent(block) = blockSize;

	// Return a pointer to the data.
	return memTreeBlockGetData(block);

}

void memTreeFree(memoryTree *tree, byte_t *block){

	/*
	** Frees a block of memory from the general
	** purpose allocator.
	**
	** Coalesces the previous and next blocks
	** if they are free.
	*/

	size_t cPrev = memTreeDataGetPrevious(block);
	size_t cBytes = memTreeDataGetCurrent(block);
	byte_t *cBlock = memTreeDataGetBlock(block);

	byte_t *prev;
	byte_t *next;

	// Reset the active flag.
	memTreeBlockSetInactive(cBlock);

	prev = cBlock - memTreeBlockPreviousFlagless(cPrev);
	next = cBlock + cBytes;

	// Check if there is a preceding
	// block that is inactive.
	if(memTreeBlockIsFirst(cPrev) == 0 && memTreeBlockGetActiveMasked(prev) == MEMORY_TREE_BLOCK_INACTIVE){
		// Perform a merge.
		cBlock = prev;
		cBytes += memTreeBlockPreviousFlagless(cPrev);
		memTreeBlockSetPreviousKeepLast(cPrev, memTreeBlockGetPrevious(prev));
		// Remove the free block from the tree.
		memTreeRemove(tree, prev);
	}
	// Check if there is a following
	// block that is inactive.
	if(memTreeBlockIsLast(cPrev) == 0 && memTreeBlockGetActiveMasked(next) == MEMORY_TREE_BLOCK_INACTIVE){
		// Perform a merge.
		cBytes += memTreeBlockGetCurrent(next);
		memTreeBlockSetPreviousKeepFirst(cPrev, memTreeBlockGetPrevious(next));
		// Remove the free block from the tree.
		memTreeRemove(tree, next);
	}

	// Add the new free block to the tree.
	memTreeInsert(tree, cBlock, cBytes);

	// Set the linked list header data.
	memTreeBlockGetCurrent(cBlock) = cBytes;
	memTreeBlockGetPrevious(cBlock) = cPrev;

}

byte_t *memTreeReallocate(memoryTree *tree, byte_t *block, const size_t bytes){

	/*
	** Reallocates a block of memory. Can potentially
	** be called to try and "defragment" a particular
	** block.
	**
	** If no defragmentation is possible, this function
	** is effectively the same as an alloc + copy + free.
	*/

	const size_t totalBytes = MEMORY_TREE_BLOCK_HEADER_SIZE +
	                          (bytes <= MEMORY_TREE_BLOCK_SIZE ? MEMORY_TREE_BLOCK_SIZE : bytes);

	byte_t *rBlock;

	size_t cPrev = memTreeBlockGetPrevious(block);
	size_t cBytes = memTreeDataGetCurrent(block);
	byte_t *cBlock = memTreeDataGetBlock(block);

	byte_t *prev;
	byte_t *next;

	// Reset the active flag.
	memTreeBlockSetInactive(cBlock);

	prev = cBlock - memTreeBlockPreviousFlagless(cPrev);
	next = cBlock + cBytes;

	// Check if there is a preceding
	// block that is inactive.
	if(memTreeBlockIsFirst(cPrev) == 0 && memTreeBlockGetActiveMasked(prev) == MEMORY_TREE_BLOCK_INACTIVE){
		// Perform a merge.
		cBlock = prev;
		cBytes += memTreeBlockPreviousFlagless(cPrev);
		memTreeBlockSetPreviousKeepLast(cPrev, memTreeBlockGetPrevious(prev));
		// Remove the free block from the tree.
		memTreeRemove(tree, prev);
	}
	// Check if there is a following
	// block that is inactive.
	if(memTreeBlockIsLast(cPrev) == 0 && memTreeBlockGetActiveMasked(next) == MEMORY_TREE_BLOCK_INACTIVE){
		// Perform a merge.
		cBytes += memTreeBlockGetCurrent(next);
		memTreeBlockSetPreviousKeepFirst(cPrev, memTreeBlockGetPrevious(next));
		// Remove the free block from the tree.
		memTreeRemove(tree, next);
	}

	#ifndef MEMORY_TREE_FORCE_MOVE_ON_REALLOC
	// Check if we can fit the new data
	// into this particular fragment.
	if(totalBytes <= cBytes){

		// We can coalesce the previous and / or
		// next blocks to create enough room for
		// the new data.
		byte_t *cNext = cBlock + MEMORY_TREE_ALIGN(totalBytes);
		size_t nextSize = cBlock + cBytes - cNext;

		if(nextSize >= MEMORY_TREE_BLOCK_TOTAL_SIZE){
			// There's enough room for a split.
			// Include the alignment padding in
			// the allocated block.
			cBytes -= nextSize;
			memTreeBlockGetCurrent(cNext) = nextSize;
			memTreeBlockGetPrevious(cNext) = cBytes | (cPrev & MEMORY_TREE_BLOCK_LAST);
			cPrev &= MEMORY_TREE_BLOCK_LAST_MASK;
			memTreeInsert(tree, cNext, nextSize);
		}

		rBlock = memTreeBlockGetData(cBlock);

		// Copy the block's data over.
		memcpy((void *)rBlock, (void *)block, bytes);

		// Set the new block's header information.
		memTreeBlockGetCurrent(cBlock) = cBytes;
		memTreeBlockGetPrevious(cBlock) = cPrev;

	}else{
	#endif

		// We'll have to look for a new block.
		rBlock = memTreeAllocate(tree, bytes);

		if(rBlock != NULL){

			// Copy the block's data over.
			memcpy((void *)rBlock, (void *)block, bytes);

			// Add the new free block to the tree.
			memTreeInsert(tree, cBlock, cBytes);

			// Set the linked list header data.
			memTreeBlockGetCurrent(cBlock) = cBytes;
			memTreeBlockGetPrevious(cBlock) = cPrev;

		}

	#ifndef MEMORY_TREE_FORCE_MOVE_ON_REALLOC
	}
	#endif

	return rBlock;

}

byte_t *memTreeReset(byte_t *start, const size_t bytes, const size_t length){
	byte_t *root = (byte_t *)MEMORY_TREE_ALIGN((uintptr_t)start);
	memTreeBlockGetCurrent(root) = start + memTreeAllocationSize(start, bytes, length) - root;
	memTreeBlockGetPrevious(root) = 0;
	memTreeBlockGetLeft(root) = NULL;
	memTreeBlockGetRight(root) = NULL;
	memTreeBlockGetParent(root) = NULL;
	return root;
}

void memTreeClear(memoryTree *tree){
	tree->root = (byte_t *)MEMORY_TREE_ALIGN((uintptr_t)tree->start);
	memTreeBlockGetCurrent(tree->root) = tree->end - tree->root;
	memTreeBlockGetPrevious(tree->root) = 0;
	memTreeBlockGetLeft(tree->root) = NULL;
	memTreeBlockGetRight(tree->root) = NULL;
	memTreeBlockGetParent(tree->root) = NULL;
}
