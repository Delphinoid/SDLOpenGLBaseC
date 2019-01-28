#include "aabbTree.h"
#include "inline.h"

void aabbTreeInit(aabbTree *const restrict tree){
	tree->root = NULL;
	tree->leaves = NULL;
}

static __FORCE_INLINE__ aabbNode *aabbTreeBalanceNode(aabbTree *const restrict tree, aabbNode *const node){

	/*
	** Rotate the tree to the left or right to
	** restore balance to the specified node.
	*/

	//We only need to perform a rotation of this node
	//is not a leaf and it does not parent any leaves.
	if(node->height > 1){

		aabbNode *const parent = node->parent;
		aabbNode *const left   = node->data.children.left;
		aabbNode *const right  = node->data.children.right;
		const int balance = right->height - left->height;

		// Rotate the node left if its right branch
		// is deeper than its left branch.
		if(balance > 1){

			aabbNode *const grandparentLeft = right->data.children.left;
			aabbNode *const grandparentRight = right->data.children.right;

			// Replace the node with its right child.
			if(parent != NULL){
				if(parent->data.children.left == node){
					parent->data.children.left = right;
				}else{
					parent->data.children.right = right;
				}
			}else{
				tree->root = right;
			}
			right->parent = parent;
			right->data.children.left = node;
			node->parent = right;

			// Make the right node's right child the
			// grandchild with the deeper branch.
			if(grandparentLeft->height >= grandparentRight->height){

				right->data.children.right = grandparentLeft;
				node->data.children.right  = grandparentRight;
				grandparentRight->parent = node;

				cAABBCombine(&left->aabb, &grandparentRight->aabb, &node->aabb);
				cAABBCombine(&node->aabb, &grandparentLeft->aabb,  &right->aabb);

				node->height  = (left->height >= grandparentRight->height ? left->height : grandparentRight->height) + 1;
				right->height = (node->height >= grandparentLeft->height  ? node->height : grandparentLeft->height)  + 1;

			}else{

				right->data.children.right = grandparentRight;
				node->data.children.right  = grandparentLeft;
				grandparentLeft->parent = node;

				cAABBCombine(&left->aabb, &grandparentLeft->aabb,  &node->aabb);
				cAABBCombine(&node->aabb, &grandparentRight->aabb, &right->aabb);

				node->height  = (left->height >= grandparentLeft->height  ? left->height : grandparentLeft->height)  + 1;
				right->height = (node->height >= grandparentRight->height ? node->height : grandparentRight->height) + 1;

			}

			return right;


		// Rotate the node right if its left branch
		// is deeper than its right branch.
		}else if(balance < -1){

			aabbNode *const grandparentLeft = left->data.children.left;
			aabbNode *const grandparentRight = left->data.children.right;

			// Replace the node with its left child.
			if(parent != NULL){
				if(parent->data.children.left == node){
					parent->data.children.left = right;
				}else{
					parent->data.children.right = right;
				}
			}else{
				tree->root = right;
			}
			left->parent = parent;
			left->data.children.right = node;
			node->parent = left;

			// Make the left node's left child the
			// grandchild with the deeper branch.
			if(grandparentLeft->height >= grandparentRight->height){

				left->data.children.left = grandparentLeft;
				node->data.children.left = grandparentRight;
				grandparentRight->parent = node;

				cAABBCombine(&left->aabb, &grandparentRight->aabb, &node->aabb);
				cAABBCombine(&node->aabb, &grandparentLeft->aabb,  &left->aabb);

				node->height = (left->height >= grandparentRight->height ? left->height : grandparentRight->height) + 1;
				left->height = (node->height >= grandparentLeft->height  ? node->height : grandparentLeft->height)  + 1;

			}else{

				left->data.children.left = grandparentRight;
				node->data.children.left = grandparentLeft;
				grandparentLeft->parent = node;

				cAABBCombine(&left->aabb, &grandparentLeft->aabb,  &node->aabb);
				cAABBCombine(&node->aabb, &grandparentRight->aabb, &left->aabb);

				node->height = (left->height >= grandparentLeft->height  ? left->height : grandparentLeft->height)  + 1;
				left->height = (node->height >= grandparentRight->height ? node->height : grandparentRight->height) + 1;

			}


			return left;
		}

	}

	return node;

}

static __FORCE_INLINE__ void aabbTreeBalanceHierarchy(aabbTree *const restrict tree, aabbNode *node){

	/*
	** Iteratively restore balance to the tree
	** by looping through and balancing the
	** specified node and all of its ancestors.
	*/

	do {

		aabbNode *left;
		aabbNode *right;

		node  = aabbTreeBalanceNode(tree, node);
		left  = node->data.children.left;
		right = node->data.children.right;

		// Repair the node's properties to represent its new children.
		cAABBCombine(&left->aabb, &right->aabb, &node->aabb);
		node->height = (left->height >= right->height ? left->height : right->height) + 1;

		// Continue with the node's parent.
		node = node->parent;

	} while(node != NULL);

}

static __FORCE_INLINE__ void aabbTreeInsertLeaf(aabbTree *const restrict tree, aabbNode *const node, aabbNode *const parent){

	/*
	** Insert a leaf node into the tree.
	*/

	aabbNode *siblingParent;
	aabbNode *sibling = tree->root;

	// Find a sibling for the leaf node.
	while(!AABB_TREE_NODE_IS_LEAF(sibling)){

		aabbNode *const left = sibling->data.children.left;
		aabbNode *const right = sibling->data.children.right;

		// Calculate the cost of starting a new branch.
		const float combinedArea  = cAABBSurfaceAreaHalfCombined(&node->aabb, &sibling->aabb);
		const float branchCost    = 2.f * combinedArea;
		const float inheritedCost = 2.f * (combinedArea - cAABBSurfaceAreaHalf(&sibling->aabb));

		float leftCost;
		float rightCost;

		// Calculate the total increase in surface area from combining
		// the node with the current sibling's left child.
		leftCost = cAABBSurfaceAreaHalfCombined(&node->aabb, &left->aabb);
		// If the child is a leaf node, include the
		// cost of creating a new branch node.
		if(AABB_TREE_NODE_IS_LEAF(left)){
			leftCost += inheritedCost - cAABBSurfaceAreaHalf(&left->aabb);
		}

		// Calculate the total increase in surface area from combining
		// the node with the current sibling's right child.
		rightCost = cAABBSurfaceAreaHalfCombined(&node->aabb, &right->aabb);
		//If the child is a leaf node, the cost must
		//include the creation of a new branch node.
		if(AABB_TREE_NODE_IS_LEAF(right)){
			rightCost += inheritedCost - cAABBSurfaceAreaHalf(&right->aabb);
		}

		// If creating a new branch is cheaper, exit the loop.
		if(branchCost < leftCost && branchCost < rightCost){
			break;
		}

		// Continue with the cheaper child.
		if(leftCost <= rightCost){
			sibling = left;
		}else{
			sibling = right;
		}

	}

	// Create a branch node to adopt
	// our node and its sibling.
	siblingParent = sibling->parent;
	cAABBCombine(&node->aabb, &sibling->aabb, &parent->aabb);
	parent->parent = siblingParent;
	parent->data.children.left = sibling;
	parent->data.children.right = node;
	parent->height = sibling->height + 1;

	node->parent = parent;
	sibling->parent = parent;

	if(siblingParent != NULL){
		// Make the sibling's parent point to the parent node.
		if(siblingParent->data.children.left == sibling){
		   siblingParent->data.children.left = parent;
		}else{
			siblingParent->data.children.right = parent;
		}
	}else{
		// The sibling node is the tree's root.
		// Update the root to be the parent node.
		tree->root = parent;
	}

	// Balance the tree.
	aabbTreeBalanceHierarchy(tree, parent);

}

static __FORCE_INLINE__ void aabbTreeRemoveLeaf(aabbTree *const restrict tree, const aabbNode *const node){

	/*
	** Remove a leaf node from the tree.
	*/

	const aabbNode *const parent = node->parent;
	aabbNode *const grandparent = parent->parent;
	aabbNode *const sibling = (parent->data.children.left == node) ? parent->data.children.right : parent->data.children.left;

	// If the node's parent is not the root node,
	// replace its parent with its sibling.
	if(grandparent != NULL){

		if(grandparent->data.children.left == parent){
			grandparent->data.children.left = sibling;
		}else{
			grandparent->data.children.right = sibling;
		}
		sibling->parent = grandparent;

		// Balance the tree.
		aabbTreeBalanceHierarchy(tree, grandparent);

	}else{
		// The node's parent is the tree's root.
		// Make the sibling the new root.
		tree->root = sibling;
		sibling->parent = NULL;
	}

}

return_t aabbTreeInsert(aabbTree *const restrict tree, aabbNode **node, void *const restrict value, const cAABB *const restrict aabb, aabbNode *(*const allocator)()){

	/*
	** Insert a user-specified value into the tree.
	** Takes in a pointer to an allocator.
	** This is for modularity's sake with allocation.
	*/

	*node = allocator();
	if(*node != NULL){

		aabbNode *const leaf = *node;
		leaf->aabb = *aabb;
		leaf->data.leaf.value = value;
		leaf->data.leaf.next = tree->leaves;
		leaf->height = 0;
		tree->leaves = *node;

		if(tree->root != NULL){

			aabbNode *const parent = allocator();
			if(parent != NULL){
				aabbTreeInsertLeaf(tree, *node, parent);
			}else{
				/** Memory allocation failure. **/
				return -1;
			}

		}else{
			// The tree is empty.
			// Add the node as the root node.
			tree->root = leaf;
			leaf->parent = NULL;
		}

	}else{
		/** Memory allocation failure. **/
		return -1;
	}

	return 1;

}

void aabbTreeRemove(aabbTree *const restrict tree, aabbNode *const node, void (*const deallocator)(aabbNode *const restrict)){

	/*
	** Remove a user-specified value from the tree.
	** Takes in a pointer to an deallocator.
	** This is for modularity's sake with deallocation.
	*/

	if(node != tree->root){
		aabbTreeRemoveLeaf(tree, node);
		deallocator(node->parent);
	}else{
		// We're deleting the root node.
		// If it's a leaf, it won't have
		// any children, so we can just
		// update the root pointer and exit.
		tree->root = NULL;
	}

	deallocator(node);

}

void aabbTreeUpdate(aabbTree *const restrict tree, aabbNode *const node){

	/*
	** Update a node in an AABB tree.
	** Tries to re-use nodes in order to do as few
	** allocations and deallocations as possible.
	*/

	if(node != tree->root){
		aabbTreeRemoveLeaf(tree, node);
		aabbTreeInsertLeaf(tree, node, node->parent);
	}

}

__HINT_INLINE__ return_t aabbTreeQueryNodeStack(const aabbTree *const restrict tree, aabbNode *const restrict node, return_t (*const func)(aabbNode *const restrict, aabbNode *const restrict)){

	/*
	** Runs "func()" on each potential leaf that could collide with node.
	*/

	aabbNode *stack[AABB_TREE_QUERY_STACK_SIZE];
	size_t i = 1;

	stack[0] = tree->root;

	do {

		aabbNode *test = stack[--i];

		if(AABB_TREE_NODE_IS_LEAF(test)){
			const return_t r = (*func)(node, test);
			if(r < 0){
				/** Memory allocation failure. **/
				return r;
			}
		}else if(cAABBCollision(&node->aabb, &test->aabb)){
			stack[i] = test->data.children.left;
			++i;
			stack[i] = test->data.children.right;
			++i;
		}

	} while(i);

	return 1;

}

__HINT_INLINE__ return_t aabbTreeQueryNode(const aabbTree *const restrict tree, aabbNode *const restrict node, return_t (*const func)(aabbNode *const restrict, aabbNode *const restrict)){

	/*
	** Runs "func()" on each potential leaf that could collide with node.
	*/

	aabbNode *parent;
	aabbNode *test = tree->root;

	if(test != NULL){

		// If the tree only has one node, handle it separately.
		if(!AABB_TREE_NODE_IS_LEAF(test)){

			for(;;){

				while(cAABBCollision(&node->aabb, &test->aabb)){

					// A node with two leaves has been found.
					// Run the callback function on both children.
					if(AABB_TREE_NODE_IS_FINAL_BRANCH(test)){

						return_t r = (*func)(node, test->data.children.left);
						if(r < 0){
							/** Memory allocation failure. **/
							return r;
						}

						r = (*func)(node, test->data.children.right);
						if(r < 0){
							/** Memory allocation failure. **/
							return r;
						}

						break;

					}else{

						test = test->data.children.left;

					}

				}

				// Climb the tree until a left child is found.
				for(;;){
					parent = test->parent;
					if(parent == NULL){
						return 1;
					}
					if(test == parent->data.children.left){
						break;
					}
					test = parent;
				}

				// We can now start searching for the node's sibling.
				test = parent->data.children.right;

			}

		}else{

			// The tree only has one node.
			const return_t r = (*func)(node, test);
			if(r < 0){
				/** Memory allocation failure. **/
				return r;
			}

		}

	}

	return 1;

}

return_t aabbTreeQueryStack(const aabbTree *const restrict tree, return_t (*const func)(aabbNode *const restrict, aabbNode *const restrict)){

	/*
	** Queries every leaf node in the tree.
	*/

	aabbNode *node = tree->leaves;
	while(node != NULL){
		const return_t r = aabbTreeQueryNodeStack(tree, node, func);
		if(r < 0){
			/** Memory allocation failure. **/
			return r;
		}
		node = node->data.leaf.next;
	}
	return 1;

}

return_t aabbTreeQuery(const aabbTree *const restrict tree, return_t (*const func)(aabbNode *const restrict, aabbNode *const restrict)){

	/*
	** Queries every leaf node in the tree.
	*/

	aabbNode *node = tree->leaves;
	while(node != NULL){
		const return_t r = aabbTreeQueryNode(tree, node, func);
		if(r < 0){
			/** Memory allocation failure. **/
			return r;
		}
		node = node->data.leaf.next;
	}
	return 1;

}

void aabbTreeTraverse(aabbTree *const restrict tree, void (*const func)(aabbNode *const restrict)){

	/*
	** Postorder tree traversal where
	** "func()" is run on each node.
	** Primarily used for tree deallocation.
	*/

	aabbNode *node = tree->root;

	if(node != NULL){

		aabbNode *nodeParent;

		// Start on the left-most node.
		while(!AABB_TREE_NODE_IS_LEAF(node)){
			node = node->data.children.left;
		}

		do {

			if(!AABB_TREE_NODE_IS_LEAF(node)){
				// If we can go right, go right and
				// then try to go left again.
				node = node->data.children.right;
				while(!AABB_TREE_NODE_IS_LEAF(node)){
					node = node->data.children.left;
				}
			}else{
				// A leaf node was reached.
				// If the leaf node is to the right of its
				// parent, keep going up until this is no
				// longer the case.
				while(
					(nodeParent = node->parent) != NULL &&
					node == nodeParent->data.children.right
				){
					(*func)(node);
					node = nodeParent;
				}
				// Go up one final time for the left node.
				(*func)(node);
				node = nodeParent;
			}

		} while(node != NULL);

	}

}