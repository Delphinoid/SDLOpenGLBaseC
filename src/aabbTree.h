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

void aabbTreeInit(aabbTree *const restrict tree);
return_t aabbTreeInsert(aabbTree *const restrict tree, aabbNode **node, void *const restrict value, const cAABB *const restrict aabb, aabbNode *(*const allocator)());
void aabbTreeRemove(aabbTree *const restrict tree, aabbNode *const node, void (*deallocator)(aabbNode *const restrict));
void aabbTreeUpdate(aabbTree *const restrict tree, aabbNode *const node);
return_t aabbTreeQueryNodeStack(const aabbTree *const restrict tree, aabbNode *const node, return_t (*const func)(aabbNode *const, aabbNode *const));
return_t aabbTreeQueryNode(const aabbTree *const restrict tree, aabbNode *const node, return_t (*const func)(aabbNode *const, aabbNode *const));
return_t aabbTreeQueryStack(const aabbTree *const restrict tree, return_t (*const func)(aabbNode *const, aabbNode *const));
return_t aabbTreeQuery(const aabbTree *const restrict tree, return_t (*const func)(aabbNode *const, aabbNode *const));
void aabbTreeTraverse(aabbTree *const restrict tree, void (*const func)(aabbNode *const restrict));

#endif
