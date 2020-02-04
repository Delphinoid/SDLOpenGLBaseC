#ifndef AABBTREE_H
#define AABBTREE_H

#include "colliderAABB.h"
#include <stddef.h>

#define AABB_TREE_QUERY_STACK_SIZE 256

#define AABB_TREE_NODE_IS_LEAF(node) (node->height == 0)
#define AABB_TREE_NODE_IS_FINAL_BRANCH(node) (node->height == 1)

typedef struct aabbNode aabbNode;

typedef struct {
	aabbNode *left;
	aabbNode *right;
} aabbChildren;

typedef struct {
	void *value;     // The node's value.
	aabbNode *next;  // Next leaf node in the "leaves" linked-list.
} aabbLeaf;

typedef struct aabbNode {
	cAABB aabb;
	aabbNode *parent;
	union {
		aabbChildren children;
		aabbLeaf leaf;
	} data;
	size_t height;
} aabbNode;

typedef struct {
	aabbNode *root;
	aabbNode *leaves;  // Linked-list of leaves for easy traversal.
} aabbTree;

void aabbTreeInit(aabbTree *const __RESTRICT__ tree);
return_t aabbTreeInsert(aabbTree *const __RESTRICT__ tree, aabbNode **node, void *const __RESTRICT__ value, const cAABB *const __RESTRICT__ aabb, aabbNode *(*const allocator)());
void aabbTreeRemove(aabbTree *const __RESTRICT__ tree, aabbNode *const node, void (*deallocator)(aabbNode *const __RESTRICT__));
void aabbTreeUpdate(aabbTree *const __RESTRICT__ tree, aabbNode *const node);
return_t aabbTreeQueryNodeStack(const aabbTree *const __RESTRICT__ tree, aabbNode *const node, return_t (*const func)(aabbNode *const, aabbNode *const));
return_t aabbTreeQueryNode(const aabbTree *const __RESTRICT__ tree, aabbNode *const node, return_t (*const func)(aabbNode *const, aabbNode *const));
return_t aabbTreeQueryStack(const aabbTree *const __RESTRICT__ tree, return_t (*const func)(aabbNode *const, aabbNode *const));
return_t aabbTreeQuery(const aabbTree *const __RESTRICT__ tree, return_t (*const func)(aabbNode *const, aabbNode *const));
void aabbTreeTraverse(aabbTree *const __RESTRICT__ tree, void (*const func)(aabbNode *const __RESTRICT__));

#endif
