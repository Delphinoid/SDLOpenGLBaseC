#include "modulePhysics.h"
#include "inline.h"

void physIslandInit(physIsland *const restrict island){
	aabbTreeInit(&island->tree);
}

__FORCE_INLINE__ return_t physIslandUpdateCollider(physIsland *const restrict island, physCollider *const restrict c){
	if(c->node == NULL){
		// Insert a collider into the island.
		if(aabbTreeInsert(&island->tree, &c->node, (void *)c, &c->aabb, &modulePhysicsAABBNodeAllocate) < 0){
			/** Memory allocation failure. **/
			if(c->node != NULL){
				modulePhysicsAABBNodeFree(c->node);
			}
			return -1;
		}
	}else if(!cAABBEncapsulates(&c->node->aabb, &c->aabb)){
		// Update the collider.
		c->node->aabb = c->aabb;
		cAABBExpandVelocity(&c->node->aabb, &((physRigidBody *)c->body)->linearVelocity, PHYSICS_ISLAND_COLLIDER_AABB_VELOCITY_FACTOR);
		cAABBExpand(&c->node->aabb, PHYSICS_ISLAND_COLLIDER_AABB_ADDEND);
		aabbTreeUpdate(&island->tree, c->node);
	}
	return 1;
}

__FORCE_INLINE__ void physIslandRemoveCollider(physIsland *const restrict island, physCollider *const restrict c){
	if(c->node != NULL){
		aabbTreeRemove(&island->tree, c->node, &modulePhysicsAABBNodeFree);
	}
}

__FORCE_INLINE__ return_t physIslandQuery(const physIsland *const restrict island, const float dt){

	/*
	** Maintain contact and separation pairs for each collider.
	*/

	const float frequency = 1.f/dt;

	aabbNode *node = island->tree.leaves;

	while(node != NULL){

		// Manage any overlaps with this node.
		aabbNode *stack[PHYSICS_ISLAND_QUERY_STACK_SIZE];
		size_t i = 1;

		stack[0] = island->tree.root;

		do {

			aabbNode *test = stack[--i];

			if(AABB_TREE_NODE_IS_LEAF(test)){
				const return_t r = physCollisionQuery(node, test, frequency);
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

		// Remove any outdated contacts and separations.
		physColliderRemoveContacts(node->data.leaf.value);
		physColliderRemoveSeparations(node->data.leaf.value);

		node = node->data.leaf.next;

	}

	return 1;

}

void physIslandDelete(physIsland *const restrict island){
	aabbTreeTraverse(&island->tree, &modulePhysicsAABBNodeFree);
}