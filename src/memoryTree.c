#include "memoryTree.h"
#include <string.h>

#define memTreeGetParent(node) \
	*((byte_t **)(*((uintptr_t *)(node + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOURLESS_MASK))

#define memTreeSetParent(node, parent) \
	*((uintptr_t *)(node + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) = \
		(*((uintptr_t *)(node + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) | MEMORY_TREE_NODE_COLOURLESS_MASK) & \
		(uintptr_t)parent & MEMORY_TREE_NODE_COLOURLESS_MASK;

byte_t *memTreeInit(memoryTree *tree, byte_t *start, const size_t bytes){

	/*
	** Initialize a general purpose memory allocator of size "bytes".
	*/

	if(start){

		tree->start = start;
		tree->end = (byte_t *)MEMORY_TREE_ALIGN((uintptr_t)start) + bytes;

		memTreeClear(tree);

	}

	return start;

}

static inline void memTreeRotateTreeLeft(memoryTree *tree, byte_t *node){

	/*
	** Rotate the red-black tree to the left.
	*/

	byte_t *parent = memTreeGetParent(node);
	byte_t *right = *((byte_t **)(node + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK));
	byte_t *rightleft = *((byte_t **)(right + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));

	*((byte_t **)(node + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK)) = rightleft;

	if(rightleft != NULL){
		*((byte_t **)(rightleft + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) = node;
	}

	memTreeSetParent(right, parent);

	if(parent == NULL){
		tree->root = right;
	}else if(node == *((byte_t **)(parent + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK))){
		*((byte_t **)(parent + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK)) = right;
	}else{
		*((byte_t **)(parent + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK)) = right;
	}

	*((byte_t **)(right + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK)) = node;
	*((byte_t **)(node + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) = right;

}

static inline void memTreeRotateTreeRight(memoryTree *tree, byte_t *node){

	/*
	** Rotate the red-black tree to the right.
	*/

	byte_t *parent = memTreeGetParent(node);
	byte_t *left = *((byte_t **)(node + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));
	byte_t *leftright = *((byte_t **)(left + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK));

	*((byte_t **)(node + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK)) = leftright;

	if(leftright != NULL){
		*((byte_t **)(leftright + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) = node;
	}

	memTreeSetParent(left, parent);

	if(parent == NULL){
		tree->root = left;
	}else if(node == *((byte_t **)(parent + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK))){
		*((byte_t **)(parent + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK)) = left;
	}else{
		*((byte_t **)(parent + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK)) = left;
	}

	*((byte_t **)(left + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK)) = node;
	*((byte_t **)(node + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) = left;

}

static inline void memTreeRepairTree(memoryTree *tree, byte_t *node){

	/*
	** Fix any violations of the red-black
	** tree's rules, starting at "node".
	*/

	byte_t *parent = memTreeGetParent(node);

	// Continue looping while the current node
	// and its parent are both red. If the root
	// node is reached, the loop will terminate.
	while((*((uintptr_t *)(parent + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK) != MEMORY_TREE_NODE_COLOUR_BLACK){

		byte_t *grandparent = memTreeGetParent(parent);
		byte_t *uncle = *((byte_t **)(grandparent + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));

		if(parent == uncle){

			// The uncle is to the right of the grandparent.
			uncle = *((byte_t **)(grandparent + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK));

			if((*((uintptr_t *)(uncle + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK) != MEMORY_TREE_NODE_COLOUR_BLACK){

				// The uncle is red, all we need to
				// do is some recolouring.
				*((uintptr_t *)(parent      + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK;
				*((uintptr_t *)(uncle       + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK;
				*((uintptr_t *)(grandparent + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) |= MEMORY_TREE_NODE_COLOUR_RED_MASK;

				// Continue climbing the tree.
				node = grandparent;

			}else{

				uintptr_t *parentColourBit;
				uintptr_t *grandparentColourBit;
				uintptr_t tempParentColour;

				if(node == *((byte_t **)(parent + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK))){

					// Current node is the right child of its parent.
					// Rotate the tree to the left.
					memTreeRotateTreeLeft(tree, parent);
					node = parent;
					parent = memTreeGetParent(node);

				}

				parentColourBit      = (uintptr_t *)(parent      + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK);
				grandparentColourBit = (uintptr_t *)(grandparent + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK);

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
			if((*((uintptr_t *)(uncle + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK) != MEMORY_TREE_NODE_COLOUR_BLACK){

				// The uncle is red, all we need to
				// do is some recolouring.
				*((uintptr_t *)(parent      + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK;
				*((uintptr_t *)(uncle       + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK;
				*((uintptr_t *)(grandparent + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) |= MEMORY_TREE_NODE_COLOUR_RED_MASK;

				// Continue climbing the tree.
				node = grandparent;

			}else{

				uintptr_t *parentColourBit;
				uintptr_t *grandparentColourBit;
				uintptr_t tempParentColour;

				if(node == *((byte_t **)(parent + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK))){

					// Current node is the left child of its parent.
					// Rotate the tree to the right.
					memTreeRotateTreeRight(tree, parent);
					node = parent;
					parent = memTreeGetParent(node);

				}

				parentColourBit      = (uintptr_t *)(parent      + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK);
				grandparentColourBit = (uintptr_t *)(grandparent + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK);

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
		parent = memTreeGetParent(node);

	}

}

static inline void memTreeInsert(memoryTree *tree, byte_t *block, const size_t bytes){

	/*
	** Add a free block to the red-black tree.
	*/

	byte_t **address = &tree->root;
	uintptr_t current = *((uintptr_t *)address);

	// Reset the active flag.
	*((uintptr_t *)(block + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_BLOCK_INACTIVE_MASK;

	// Set the children to NULL.
	*((byte_t **)(block + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK)) = NULL;
	*((byte_t **)(block + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK)) = NULL;

	// If the root node is free, we can exit early.
	if(current == 0){
		*address = block;
		*((byte_t **)(block + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) = NULL;
		return;
	}

	// Begin by performing a BST insertion.
	do {

		const size_t currentSize = *((size_t *)(current + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK));

		if(bytes <= currentSize){

			// The new block is smaller than
			// the current block. Go to its left.
			address = (byte_t **)(current + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK);

		}else{

			// The new block is larger than
			// the current block. Go to its right.
			address = (byte_t **)(current + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK);

		}

		current = *((uintptr_t *)address);

	} while(current != 0);

	// Insert the free block.
	*address = block;
	*((uintptr_t *)(block + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) = current | MEMORY_TREE_NODE_COLOUR_RED_MASK;

	// Fix red-black tree rule violations.
	memTreeRepairTree(tree, block);

}

static inline void memTreeRemove(memoryTree *tree, byte_t *block){

	/*
	** Remove a free block from the red-black tree.
	*/

	byte_t *child;
	uintptr_t childColour;

	byte_t *parent = memTreeGetParent(block);
	byte_t *left   = *((byte_t **)(block + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));
	byte_t *right  = *((byte_t **)(block + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK));

	// Set the active flag.
	*((uintptr_t *)(block + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK)) |= MEMORY_TREE_BLOCK_ACTIVE_MASK;

	if(left != NULL){
		if(right != NULL){

			// The node has two children.
			// Swap it with its in-order successor.
			byte_t *newParent;
			byte_t *next = *((byte_t **)(right + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));
			byte_t *successor = right;

			// Find the in-order successor.
			while(next != NULL){
				successor = next;
				next = *((byte_t **)(successor + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));
			}
			newParent = memTreeGetParent(successor);
			child = *((byte_t **)(successor + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK));

			// Swap the parents.
			memTreeSetParent(block, successor);
			if(parent != NULL){
				memTreeSetParent(successor, parent);
			}else{
				*((uintptr_t *)(successor + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_MASK;
				tree->root = successor;
			}

			// Swap the left children.
			*((byte_t **)(block + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK)) = NULL;
			*((byte_t **)(successor + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK)) = left;

			// Swap the right children.
			if(child != NULL){
				*((byte_t **)(block + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK)) = child;
				childColour = *((uintptr_t *)(child + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK;
			}else{
				*((byte_t **)(block + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK)) = NULL;
				childColour = 0;
			}
			*((byte_t **)(successor + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK)) = right;

			parent = newParent;
			left = next;
			right = child;

			block = successor;

			/**
			// The node has two children.
			// Swap it with its in-order predecessor.
			byte_t *newParent;
			byte_t *next = *((byte_t **)(left + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK));
			byte_t *predecessor = left;

			// Find the in-order predecessor.
			while(next != NULL){
				predecessor = next;
				next = *((byte_t **)(predecessor + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK));
			}
			newParent = memTreeGetParent(predecessor);
			child = *((byte_t **)(predecessor + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));

			// Swap the parents.
			memTreeSetParent(block, predecessor);
			if(parent != NULL){
				memTreeSetParent(predecessor, parent);
			}else{
				*((uintptr_t *)(predecessor + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_MASK;
				tree->root = predecessor;
			}

			// Swap the left children.
			if(child != NULL){
				*((byte_t **)(block + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK)) = child;
				childColour = *((uintptr_t *)(child + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK;
			}else{
				*((byte_t **)(block + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK)) = NULL;
				childColour = 0;
			}
			*((byte_t **)(predecessor + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK)) = left;

			// Swap the right children.
			*((byte_t **)(block + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK)) = NULL;
			*((byte_t **)(predecessor + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK)) = right;

			parent = newParent;
			left = child;
			right = next;
			**/

		}else{
			child = left;
			childColour = *((uintptr_t *)(left + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK;
		}
	}else if(right != NULL){
		child = right;
		childColour = *((uintptr_t *)(right + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK;
	}else{
		child = NULL;
		childColour = 0;
	}

	// The node should now have one or zero children.
	if((*((uintptr_t *)(block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK) == MEMORY_TREE_NODE_COLOUR_BLACK){

		// If the node is black, have it
		// assume the colour of its child.
		*((uintptr_t *)(block + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) |= childColour;

		while(parent != NULL){

			uintptr_t parentColour;
			uintptr_t siblingColour;
			uintptr_t siblingLeftColour;
			uintptr_t siblingRightColour;

			byte_t *sibling;
			byte_t *siblingLeft;
			byte_t *siblingRight;
			byte_t *parentLeft = *((byte_t **)(parent + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));

			if(parentLeft == block){
				sibling = *((byte_t **)(parent + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK));
			}else{
				sibling = parentLeft;
			}

			// If the node's sibling is red, do
			// some recolours and a rotation.
			siblingColour = (*((uintptr_t *)(sibling + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK);
			if(siblingColour != MEMORY_TREE_NODE_COLOUR_BLACK){

				*((uintptr_t *)(parent  + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) |= MEMORY_TREE_NODE_COLOUR_RED_MASK;
				*((uintptr_t *)(sibling + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK;

				if(block == parentLeft){
					memTreeRotateTreeLeft(tree, parent);
				}else{
					memTreeRotateTreeRight(tree, parent);
				}

				// Update variables.
				sibling = *((byte_t **)(parent + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));
				if(sibling == block){
					sibling = *((byte_t **)(parent + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK));
				}
				parent        = memTreeGetParent(block);
				parentLeft    = *((byte_t **)(parent + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));
				siblingColour = (*((uintptr_t *)(sibling + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK);

			}

			siblingLeft        = *((byte_t **)(sibling + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));
			siblingRight       = *((byte_t **)(sibling + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK));
			parentColour       = (*((uintptr_t *)(parent       + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK);
			siblingLeftColour  = (*((uintptr_t *)(siblingLeft  + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK);
			siblingRightColour = (*((uintptr_t *)(siblingRight + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) & MEMORY_TREE_NODE_COLOUR_MASK);

			if(siblingColour      == MEMORY_TREE_NODE_COLOUR_BLACK &&
			   siblingLeftColour  == MEMORY_TREE_NODE_COLOUR_BLACK &&
			   siblingRightColour == MEMORY_TREE_NODE_COLOUR_BLACK){

				*((uintptr_t *)(sibling + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) |= MEMORY_TREE_NODE_COLOUR_RED_MASK;

				if(parentColour != MEMORY_TREE_NODE_COLOUR_BLACK){
					*((uintptr_t *)(parent + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK;
					break;
				}

			}else{

				if(block == parentLeft &&
				   siblingColour      == MEMORY_TREE_NODE_COLOUR_BLACK &&
				   siblingLeftColour  != MEMORY_TREE_NODE_COLOUR_BLACK &&
				   siblingRightColour == MEMORY_TREE_NODE_COLOUR_BLACK){

					*((uintptr_t *)(sibling     + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) |= MEMORY_TREE_NODE_COLOUR_RED_MASK;
					*((uintptr_t *)(siblingLeft + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK;
					memTreeRotateTreeRight(tree, sibling);

				}else if(block != parentLeft &&
				   siblingColour      == MEMORY_TREE_NODE_COLOUR_BLACK &&
				   siblingLeftColour  == MEMORY_TREE_NODE_COLOUR_BLACK &&
				   siblingRightColour != MEMORY_TREE_NODE_COLOUR_BLACK){

					*((uintptr_t *)(sibling      + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) |= MEMORY_TREE_NODE_COLOUR_RED_MASK;
					*((uintptr_t *)(siblingRight + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK;
					memTreeRotateTreeRight(tree, sibling);

				}else{

					// Set the sibling to the parent's colour.
					*((uintptr_t *)(sibling + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= (uintptr_t)parent | MEMORY_TREE_NODE_COLOURLESS_MASK;
					*((uintptr_t *)(parent  + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK;

					if(block == parentLeft){
						*((uintptr_t *)(siblingRight + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK;
						memTreeRotateTreeRight(tree, parent);
					}else{
						*((uintptr_t *)(siblingLeft + MEMORY_TREE_COLOUR_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_NODE_COLOUR_BLACK_MASK;
						memTreeRotateTreeRight(tree, parent);
					}

				}

				break;

			}

		}

	}

	// Replace the node with its original child.
	parent = memTreeGetParent(block);
	if(parent == NULL){
		tree->root = child;
		if(child != NULL){
			*((byte_t **)(child + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) = NULL;
		}
	}else{
		// Set the parent's child.
		byte_t **parentLeft = (byte_t **)(parent + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK);
		if(block == *parentLeft){
			*parentLeft = child;
		}else{
			*((byte_t **)(parent + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK)) = child;
		}
		if(child != NULL){
			memTreeSetParent(child, parent);
		}
	}

}

/**static inline void memTreeSplit(memoryTree *tree, byte_t *block, size_t *bytes, const size_t data){

	*
	** Attempt to split a block.
	*

	byte_t *next = block + MEMORY_TREE_ALIGN(data);
	const size_t nextSize = block + *bytes - next;

	if(nextSize >= MEMORY_TREE_BLOCK_TOTAL_SIZE){
		// There's enough room for a split.
		// Include the alignment padding in
		// the allocated block.
		*bytes -= nextSize;
		*((size_t *)(next + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK)) = nextSize;
		memTreeInsert(tree, next, nextSize);
	}

}**/

/**static inline void memTreePrepareBlock(byte_t *block, const size_t bytes){

	*
	** Update the current and following block's headers.
	*

	size_t *size = (size_t *)(block + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK);
	if(bytes < *size){
		*((size_t *)(block + bytes + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK)) = bytes;
	}
	*size = bytes;

}**/

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
	/**byte_t *current = tree->root;
	while(current != NULL){

		const size_t currentSize = *((size_t *)(current + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK));

		if(totalBytes < currentSize){

			// Set the new best fit.
			blockSize = currentSize;
			block = current;

			// The block is too big,
			// go to the left.
			current = *((byte_t **)(current + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));

		}else if(totalBytes > currentSize){

			// The block is too small,
			// go to the right.
			current = *((byte_t **)(current + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK));

		}else{
			// We've somehow found a block
			// that fits perfectly.
			blockSize = currentSize;
			block = current;
			break;
		}

	}

	if(block != NULL){

		// Tree manipulation.
		memTreeRemove(block);
		memTreeSplit(tree, block, &blockSize, totalBytes);

		// Set the new block's header information.
		memTreePrepareBlock(block, blockSize);
		block += MEMORY_TREE_DATA_OFFSET_FROM_BLOCK;
	}

	return block;**/

	byte_t *current = tree->root;
	for(;;){

		const size_t currentSize = *((size_t *)(current + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK));

		if(totalBytes < currentSize){

			// The block is too big,
			// go to the left.
			byte_t *next = *((byte_t **)(current + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK));

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
					*((size_t *)(next + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK)) = nextSize;
					*((size_t *)(next + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK)) = blockSize;
					memTreeInsert(tree, next, nextSize);
				}

				break;

			}


		}else if(totalBytes > currentSize){

			// The block is too small,
			// go to the right.
			byte_t *next = *((byte_t **)(current + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK));

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
					*((size_t *)(next + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK)) = nextSize;
					*((size_t *)(next + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK)) = blockSize;
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
	*((size_t *)(block + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK)) = blockSize;

	// Return a pointer to the data.
	return block + MEMORY_TREE_DATA_OFFSET_FROM_BLOCK;

}

void memTreeFree(memoryTree *tree, byte_t *block){

	/*
	** Frees a block of memory from the general
	** purpose allocator.
	**
	** Coalesces the previous and next blocks
	** if they are free.
	*/

	size_t cPrev = *((size_t *)(block + MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA));
	size_t cBytes = *((size_t *)(block + MEMORY_TREE_CURRENT_OFFSET_FROM_DATA));
	byte_t *cBlock = block + MEMORY_TREE_BLOCK_OFFSET_FROM_DATA;

	byte_t *prev;
	byte_t *next;

	// Reset the active flag.
	*((uintptr_t *)(cBlock + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_BLOCK_INACTIVE_MASK;

	prev = cBlock - cPrev;
	next = cBlock + cBytes;

	// Check if there is a preceding
	// block that is inactive.
	if(
		prev >= tree->start &&
		(*((uintptr_t *)(prev + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK)) & MEMORY_TREE_BLOCK_ACTIVE_MASK) == MEMORY_TREE_BLOCK_INACTIVE
	){
		// Perform a merge.
		cBlock = prev;
		cBytes += cPrev;
		cPrev = *((size_t *)(prev + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK));
		// Remove the free block from the tree.
		memTreeRemove(tree, prev);
	}
	// Check if there is a following
	// block that is inactive.
	if(
		next < tree->end &&
		(*((uintptr_t *)(next + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK)) & MEMORY_TREE_BLOCK_ACTIVE_MASK) == MEMORY_TREE_BLOCK_INACTIVE
	){
		// Perform a merge.
		cBytes += *((size_t *)(next + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK));
		// Remove the free block from the tree.
		memTreeRemove(tree, next);
	}

	// Add the new free block to the tree.
	memTreeInsert(tree, cBlock, cBytes);

	// Set the linked list header data.
	*((size_t *)(cBlock + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK)) = cBytes;
	*((size_t *)(cBlock + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK)) = cPrev;

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

	size_t cPrev = *((size_t *)(block + MEMORY_TREE_PREVIOUS_OFFSET_FROM_DATA));
	size_t cBytes = *((size_t *)(block + MEMORY_TREE_CURRENT_OFFSET_FROM_DATA));
	byte_t *cBlock = block + MEMORY_TREE_BLOCK_OFFSET_FROM_DATA;

	byte_t *prev;
	byte_t *next;

	// Reset the active flag.
	*((uintptr_t *)(cBlock + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK)) &= MEMORY_TREE_BLOCK_INACTIVE_MASK;

	prev = cBlock - cPrev;
	next = cBlock + cBytes;

	// Check if there is a preceding
	// block that is inactive.
	if(
		prev >= tree->start &&
		(*((uintptr_t *)(prev + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK)) & MEMORY_TREE_BLOCK_ACTIVE_MASK) == MEMORY_TREE_BLOCK_INACTIVE
	){
		// Perform a merge.
		cBlock = prev;
		cBytes += cPrev;
		cPrev = *((size_t *)(prev + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK));
		// Remove the free block from the tree.
		memTreeRemove(tree, prev);
	}
	// Check if there is a following
	// block that is inactive.
	if(
		next < tree->end &&
		(*((uintptr_t *)(next + MEMORY_TREE_ACTIVE_OFFSET_FROM_BLOCK)) & MEMORY_TREE_BLOCK_ACTIVE_MASK) == MEMORY_TREE_BLOCK_INACTIVE
	){
		// Perform a merge.
		cBytes += *((size_t *)(next + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK));
		// Remove the free block from the tree.
		memTreeRemove(tree, next);
	}

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
			*((size_t *)(cNext + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK)) = nextSize;
			*((size_t *)(cNext + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK)) = cBytes;
			memTreeInsert(tree, cNext, nextSize);
		}

		rBlock = cBlock + MEMORY_TREE_DATA_OFFSET_FROM_BLOCK;

		// Copy the block's data over.
		memcpy((void *)rBlock, (void *)block, bytes);

		// Set the new block's header information.
		// The "previous" header data should already be set.
		*((size_t *)(cBlock + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK)) = cBytes;

	}else{

		// We'll have to look for a new block.
		rBlock = memTreeAllocate(tree, bytes);

		// Copy the block's data over.
		memcpy((void *)rBlock, (void *)block, bytes);

		// Add the new free block to the tree.
		memTreeInsert(tree, cBlock, cBytes);

		// Set the linked list header data.
		*((size_t *)(cBlock + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK)) = cBytes;
		*((size_t *)(cBlock + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK)) = cPrev;

	}

	return rBlock;

}

void memTreeClear(memoryTree *tree){
	tree->root = (byte_t *)MEMORY_TREE_ALIGN((uintptr_t)tree->start);
	*((size_t *)(tree->root + MEMORY_TREE_CURRENT_OFFSET_FROM_BLOCK)) = tree->end - tree->root;
	*((size_t *)(tree->root + MEMORY_TREE_PREVIOUS_OFFSET_FROM_BLOCK)) = 0;
	*((byte_t **)(tree->root + MEMORY_TREE_LEFT_OFFSET_FROM_BLOCK)) = NULL;
	*((byte_t **)(tree->root + MEMORY_TREE_RIGHT_OFFSET_FROM_BLOCK)) = NULL;
	*((byte_t **)(tree->root + MEMORY_TREE_PARENT_OFFSET_FROM_BLOCK)) = NULL;
}
