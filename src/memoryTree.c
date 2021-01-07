#include "memoryTree.h"
#include <string.h>

void memTreeInit(memoryTree *const __RESTRICT__ tree){
	tree->root = NULL;
	tree->region = NULL;
}

void *memTreeCreate(memoryTree *const __RESTRICT__ tree, void *const start, const size_t bytes, const size_t length){

	// Initialize a general purpose memory allocator.
	//
	// If "length" is specified, it will optimize the size for
	// "length"-many allocations of size "bytes", otherwise
	// the total allocated size will be "bytes".

	if(start != NULL){

		tree->region = (memoryRegion *)((byte_t *)start + memTreeAllocationSize(start, bytes, length) - sizeof(memoryRegion));
		tree->region->start = start;
		tree->region->next = NULL;

		memTreeClear(tree);

	}

	return start;

}

static __FORCE_INLINE__ void memTreeRotateLeft(memoryTree *const __RESTRICT__ tree, byte_t *const node){

	// Rotate the red-black tree to the left.

	byte_t *const right = memTreeNodeGetRight(node);
	// Temp initially stores the right node's left
	// child, but later stores the parent node.
	byte_t *temp = memTreeNodeGetLeft(right);

	memTreeNodeGetRight(node) = temp;

	if(temp != NULL){
		memTreeNodeSetParentKeepColour(temp, node);
	}

	temp = memTreeNodeGetParentColourless(node);
	memTreeNodeSetParentKeepColour(right, temp);

	if(temp == NULL){
		tree->root = right;
	}else if(node == memTreeNodeGetLeft(temp)){
		memTreeNodeGetLeft(temp) = right;
	}else{
		memTreeNodeGetRight(temp) = right;
	}

	memTreeNodeGetLeft(right) = node;
	memTreeNodeSetParentKeepColour(node, right);

}

static __FORCE_INLINE__ void memTreeRotateRight(memoryTree *const __RESTRICT__ tree, byte_t *const node){

	// Rotate the red-black tree to the right.

	byte_t *const left = memTreeNodeGetLeft(node);
	// Temp initially stores the left node's right
	// child, but later stores the parent node.
	byte_t *temp = memTreeNodeGetRight(left);

	memTreeNodeGetLeft(node) = temp;

	if(temp != NULL){
		memTreeNodeSetParentKeepColour(temp, node);
	}

	temp = memTreeNodeGetParentColourless(node);
	memTreeNodeSetParentKeepColour(left, temp);

	if(temp == NULL){
		tree->root = left;
	}else if(node == memTreeNodeGetLeft(temp)){
		memTreeNodeGetLeft(temp) = left;
	}else{
		memTreeNodeGetRight(temp) = left;
	}

	memTreeNodeGetRight(left) = node;
	memTreeNodeSetParentKeepColour(node, left);

}

static __FORCE_INLINE__ void memTreeRepairTree(memoryTree *const __RESTRICT__ tree, byte_t *node, byte_t *parent){

	// Fix any violations of the red-black
	// tree's rules, starting at "node".

	byte_t *grandparent = memTreeNodeGetParentColourless(parent);

	// Continue looping while the current node
	// and its parent are both red. If the root
	// node is reached, the loop will terminate.
	while(memTreeNodeIsRed(parent, grandparent)){

		byte_t *uncle = memTreeNodeGetLeft(grandparent);

		if(parent == uncle){

			// The uncle is to the right of the grandparent.
			uncle = memTreeNodeGetRight(grandparent);

			if(memTreeNodeIsRed(uncle, grandparent)){

				// The uncle is red, all we need to
				// do is some recolouring.
				memTreeNodeSetColourBlack(parent);
				memTreeNodeSetColourBlack(uncle);
				memTreeNodeSetColourRed(grandparent);

				// Continue climbing the tree.
				node = grandparent;

			}else{

				if(node == memTreeNodeGetRight(parent)){

					// Current node is the right child of its parent.
					// Rotate the tree to the left.
					memTreeRotateLeft(tree, parent);
					parent = node;
					node = memTreeNodeGetLeft(parent);

				}

				// Swap the parent and grandparent's colours.
				memTreeNodeSetColourBlack(parent);
				memTreeNodeSetColourRed(grandparent);

				// Rotate the tree to the right.
				memTreeRotateRight(tree, grandparent);

			}

		}else{

			// The uncle is to the left of the grandparent.
			if(memTreeNodeIsRed(uncle, grandparent)){

				// The uncle is red, all we need to
				// do is some recolouring.
				memTreeNodeSetColourBlack(parent);
				memTreeNodeSetColourBlack(uncle);
				memTreeNodeSetColourRed(grandparent);

				// Continue climbing the tree.
				node = grandparent;

			}else{

				if(node == memTreeNodeGetLeft(parent)){

					// Current node is the left child of its parent.
					// Rotate the tree to the right.
					memTreeRotateRight(tree, parent);
					parent = node;
					node = memTreeNodeGetRight(parent);

				}

				// Swap the parent and grandparent's colours.
				memTreeNodeSetColourBlack(parent);
				memTreeNodeSetColourRed(grandparent);

				// Rotate the tree to the left.
				memTreeRotateLeft(tree, grandparent);

			}

		}

		// If the current node is the root, break from the loop.
		if(node == tree->root){
			break;
		}

		// Get the new parent and grandparent nodes.
		parent = memTreeNodeGetParentColourless(node);
		grandparent = memTreeNodeGetParentColourless(parent);

	}

	memTreeNodeGetParent(tree->root) = NULL;

}

static __FORCE_INLINE__ void memTreeInsert(memoryTree *const __RESTRICT__ tree, void *const block, const size_t bytes){

	// Add a free block to the red-black tree.

	byte_t *node = memTreeBlockGetData(block);

	// Reset the active flag.
	memTreeBlockSetInactive(block);

	// Set the children to NULL.
	memTreeNodeGetLeft(node) = NULL;
	memTreeNodeGetRight(node) = NULL;

	if(tree->root != NULL){

		byte_t **address = &tree->root;
		byte_t *parent;

		// Begin by performing a BST insertion.
		do {

			parent = *address;

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
		memTreeNodeGetColour(node) = (uintptr_t)parent | MEMORY_TREE_NODE_COLOUR_MASK;

		// Fix any red-black tree rule violations.
		memTreeRepairTree(tree, node, parent);

	}else{
		// If the root node is free, we can exit early.
		tree->root = node;
		memTreeNodeGetParent(node) = NULL;
	}

}

static __FORCE_INLINE__ void memTreeRemove(memoryTree *const __RESTRICT__ tree, void *const block){

	// Remove a free block from the red-black tree.

	byte_t *const node = memTreeBlockGetNode(block);

	byte_t *successor;
	byte_t *child;

	byte_t *parent;
	byte_t *left = memTreeNodeGetLeft(node);
	byte_t *right = memTreeNodeGetRight(node);

	// Set the active flag.
	memTreeBlockSetActive(block);

	if(left != NULL){
		if(right != NULL){

			// The node has two children.
			// Swap it with its in-order successor.
			byte_t *next;
			successor = right;
			while((next = memTreeNodeGetLeft(successor)) != NULL){
				successor = next;
			}
			child = memTreeNodeGetRight(successor);

		}else{
			successor = node;
			child = left;
		}
	}else{
		successor = node;
		child = right;
	}

	// Replace the successor's parent's child with the child node.
	parent = memTreeNodeGetParentColourless(successor);
	if(parent == NULL){
		tree->root = child;
	}else if(memTreeNodeGetLeft(parent) == successor){
		memTreeNodeGetLeft(parent) = child;
	}else{
		memTreeNodeGetRight(parent) = child;
	}
	// Replace the child's parent with the successor's.
	if(child != NULL){
		memTreeNodeSetParentKeepColour(child, parent);
	}

	// The node should now have one or zero children.
	if(memTreeNodeIsBlack(successor, parent)){

		while(child != tree->root && memTreeNodeIsBlack(child, parent)){

			byte_t *sibling = memTreeNodeGetLeft(parent);
			byte_t *siblingLeft;
			byte_t *siblingRight;

			if(child == sibling){

				sibling = memTreeNodeGetRight(parent);

				// If the node's sibling is red, swap its colour
				// with its parent and rotate it to the left.
				if(memTreeNodeIsRed(sibling, parent)){
					memTreeNodeSetColourBlack(sibling);
					memTreeNodeSetColourRed(parent);
					memTreeRotateLeft(tree, parent);
					sibling = memTreeNodeGetRight(parent);
				}

				// Keep track of the sibling's child pointers.
				siblingLeft = memTreeNodeGetLeft(sibling);
				siblingRight = memTreeNodeGetRight(sibling);

				// If both of the sibling's children are black,
				// change the sibling's colour to red.
				if(memTreeNodeIsBlack(siblingLeft, sibling) && memTreeNodeIsBlack(siblingRight, sibling)){
					memTreeNodeSetColourRed(sibling);
					child = parent;
					parent = memTreeNodeGetParentColourless(parent);
				}else{

					// Right-right case.
					if(memTreeNodeIsBlack(siblingRight, sibling)){
						memTreeNodeSetColourBlack(siblingLeft);
						memTreeNodeSetColourRed(sibling);
						memTreeRotateRight(tree, sibling);
						siblingRight = sibling;
						sibling = memTreeNodeGetRight(parent);
					}

					// Right-left case.
					memTreeNodeSetColourKeepParent(sibling, memTreeNodeGetColour(parent));
					memTreeNodeSetColourBlack(parent);
					memTreeNodeSetColourBlack(siblingRight);
					memTreeRotateLeft(tree, parent);

					// Break from the loop.
					child = tree->root;

				}

			}else{

				// If the node's sibling is red, swap its colour
				// with its parent and rotate it to the right.
				if(memTreeNodeIsRed(sibling, parent)){
					memTreeNodeSetColourBlack(sibling);
					memTreeNodeSetColourRed(parent);
					memTreeRotateRight(tree, parent);
					sibling = memTreeNodeGetLeft(parent);
				}

				// Keep track of the sibling's child pointers.
				siblingLeft = memTreeNodeGetLeft(sibling);
				siblingRight = memTreeNodeGetRight(sibling);

				// If both of the sibling's children are black,
				// change the sibling's colour to red.
				if(memTreeNodeIsBlack(siblingLeft, sibling) && memTreeNodeIsBlack(siblingRight, sibling)){
					memTreeNodeSetColourRed(sibling);
					child = parent;
					parent = memTreeNodeGetParentColourless(parent);
				}else{

					// Left-left case.
					if(memTreeNodeIsBlack(siblingLeft, sibling)){
						memTreeNodeSetColourBlack(siblingRight);
						memTreeNodeSetColourRed(sibling);
						memTreeRotateLeft(tree, sibling);
						siblingLeft = sibling;
						sibling = memTreeNodeGetLeft(parent);
					}

					// Left-right case.
					memTreeNodeSetColourKeepParent(sibling, memTreeNodeGetColour(parent));
					memTreeNodeSetColourBlack(parent);
					memTreeNodeSetColourBlack(siblingLeft);
					memTreeRotateRight(tree, parent);

					// Break from the loop.
					child = tree->root;

				}

			}

		}

		// Make sure the child is black.
		if(child != NULL){
			memTreeNodeSetColourBlack(child);
		}

	}

	// If the node was not replaced by
	// its successor, do that now.
	if(node != successor){

		parent = memTreeNodeGetParentColourless(node);
		left = memTreeNodeGetLeft(node);
		right = memTreeNodeGetRight(node);

		// Set the left child's parent to the successor.
		if(left != NULL){
			memTreeNodeSetParentKeepColour(left, successor);
		}
		// Set the successor's left child.
		memTreeNodeGetLeft(successor) = left;

		// Set the right child's parent to the successor.
		if(right != NULL){
			memTreeNodeSetParentKeepColour(right, successor);
		}
		// Set the successor's right child.
		memTreeNodeGetRight(successor) = right;

		// Make the node's parent point to the successor.
		if(parent == NULL){
			tree->root = successor;
		}else if(memTreeNodeGetLeft(parent) == node){
			memTreeNodeGetLeft(parent) = successor;
		}else{
			memTreeNodeGetRight(parent) = successor;
		}
		// Set the successor's parent.
		memTreeNodeGetParent(successor) = memTreeNodeGetParent(node);

	}

}

void *memTreeAllocate(memoryTree *const __RESTRICT__ tree, const size_t bytes){

	// Retrieves a new block of memory from the general
	// purpose allocator and updates the "next" pointer.

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

void memTreeFree(memoryTree *const __RESTRICT__ tree, void *const __RESTRICT__ block){

	// Frees a block of memory from the general
	// purpose allocator.
	//
	// Coalesces the previous and next blocks
	// if they are free.

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

void *memTreeReallocate(memoryTree *const __RESTRICT__ tree, void *const block, const size_t bytes){

	// Reallocates a block of memory. Can potentially
	// be called to try and "defragment" a particular
	// block.
	//
	// If no defragmentation is possible, this function
	// is effectively the same as an alloc + copy + free.

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

void memTreeClear(memoryTree *const __RESTRICT__ tree){
	byte_t *block;
	tree->root = memTreeAlignStartData(tree->region->start);
	block = memTreeNodeGetBlock(tree->root);
	memTreeBlockGetCurrent(block) = (byte_t *)tree->region - block;
	memTreeBlockGetPrevious(block) = MEMORY_TREE_BLOCK_FIRST | MEMORY_TREE_BLOCK_LAST;
	memTreeNodeGetLeft(tree->root) = NULL;
	memTreeNodeGetRight(tree->root) = NULL;
	memTreeNodeGetParent(tree->root) = NULL;
}

void *memTreeExtend(memoryTree *const __RESTRICT__ tree, void *const start, const size_t bytes, const size_t length){

	// Extends the memory allocator.
	// Its logical function is similar to a
	// realloc, but it creates a new chunk
	// and links it.

	if(start){

		const size_t totalBytes = memTreeAllocationSize(start, bytes, length) - sizeof(memoryRegion);

		// Place the memory region at the end of the chunk.
		memoryRegion *const newRegion = (memoryRegion *)((byte_t *)start + totalBytes);
		memRegionPrepend(&tree->region, newRegion, start);

		memTreeSetupMemory(start, bytes, length);
		memTreeInsert(tree, memTreeAlignStartBlock(start), totalBytes);

	}

	return start;

}

void memTreeDelete(memoryTree *const __RESTRICT__ tree){
	memRegionFree(tree->region);
}

#ifdef MEMORY_DEBUG

#include <stdio.h>

void memTreePrintFreeBlocks(memoryTree *const __RESTRICT__ tree, const unsigned int recursions){

	// Inorder tree traversal where
	// each node's size is printed.

	const byte_t *node = tree->root;

	fputs("MEMORY_DEBUG: Free Blocks\n~~~~~~~~~~~~~~~~~~~~~~~~~\n", stdout);

	if(node != NULL){

		const byte_t *nodeLeft;
		const byte_t *nodeRight;
		const byte_t *nodeParent;
		unsigned int i = 0;

		// Start on the left-most node.
		while((nodeLeft = memTreeNodeGetLeft(node)) != NULL){
			node = nodeLeft;
		}

		do {

			++i;

			printf(
				   "Address:%llu Size:%llu Previous:%llu First:%u Last:%u State:Inactive\n",
				   (unsigned long long)node,
				   (unsigned long long)memTreeNodeGetCurrent(node),
				   (unsigned long long)memTreeNodeGetPreviousFlagless(node),
			       memTreeBlockIsFirst(memTreeNodeGetFlags(node)) > 0,
			       memTreeBlockIsLast(memTreeNodeGetFlags(node)) > 0
			);

			if((nodeRight = memTreeNodeGetRight(node)) != NULL){
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

void memTreePrintAllBlocks(memoryTree *const __RESTRICT__ tree){

	size_t regionNum = 0;
	const memoryRegion *region = tree->region;

	fputs("MEMORY_DEBUG: All Blocks\n~~~~~~~~~~~~~~~~~~~~~~~~", stdout);
	while(region != NULL){

		const byte_t *block = memTreeAlignStartBlock(region->start);

		printf("\nRegion #%lu:\n", (unsigned long)regionNum);
		while(block < (byte_t *)region){

			const size_t size =  memTreeBlockGetCurrent(block);
			if(memTreeBlockGetActiveMasked(block) == MEMORY_TREE_BLOCK_INACTIVE){
				printf(
				       "Address:%llu Size:%llu Previous:%llu First:%u Last:%u State:Inactive\nParent:%llu Left:%llu Right:%llu Colour:%s\n",
				       (unsigned long long)memTreeBlockGetNode(block),
				       (unsigned long long)size,
				       (unsigned long long)memTreeBlockGetPreviousFlagless(block),
				       memTreeBlockIsFirst(memTreeBlockGetFlags(block)) > 0,
				       memTreeBlockIsLast(memTreeBlockGetFlags(block)) > 0,
				       (unsigned long long)memTreeBlockGetParentColourless(block),
				       (unsigned long long)memTreeBlockGetLeft(block),
				       (unsigned long long)memTreeBlockGetRight(block),
				       memTreeBlockGetColourMasked(block) == 0 ? "Black" : "Red"
				);
			}else{
				printf(
				       "Address:%llu Size:%llu Previous:%llu First:%u Last:%u State:Active\n",
				       (unsigned long long)memTreeBlockGetNode(block),
				       (unsigned long long)size,
				       (unsigned long long)memTreeBlockGetPreviousFlagless(block),
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
