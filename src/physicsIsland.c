#include "physicsIsland.h"
#include "physicsCollision.h"
#include "physicsCollider.h"
#include "physicsRigidBody.h"
#include "modulePhysics.h"

void physIslandInit(physIsland *const __RESTRICT__ island){
	aabbTreeInit(&island->tree);
}

__FORCE_INLINE__ return_t physIslandUpdateCollider(physIsland *const __RESTRICT__ island, physCollider *const __RESTRICT__ c){
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
		cAABBExpandVelocity(&c->node->aabb, ((physRigidBody *)c->body)->linearVelocity, PHYSICS_ISLAND_COLLIDER_AABB_VELOCITY_FACTOR);
		cAABBExpand(&c->node->aabb, PHYSICS_ISLAND_COLLIDER_AABB_ADDEND);
		aabbTreeUpdate(&island->tree, c->node);
	}
	return 1;
}

__FORCE_INLINE__ void physIslandRemoveCollider(physIsland *const __RESTRICT__ island, physCollider *const __RESTRICT__ c){
	if(c->node != NULL){
		aabbTreeRemove(&island->tree, c->node, &modulePhysicsAABBNodeFree);
	}
}

#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
__FORCE_INLINE__ return_t physIslandQuery(const physIsland *const __RESTRICT__ island, const float frequency){
#else
__FORCE_INLINE__ return_t physIslandQuery(const physIsland *const __RESTRICT__ island){
#endif

	// Maintain contact and separation pairs for each collider.

	aabbNode *node = island->tree.leaves;

	while(node != NULL){

		// Check for potential collisions with the current node.
		aabbTreeQueryNodeStack(&island->tree, node, &physCollisionQuery);

		// Remove any outdated contacts and separations and update what's left.
		#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
		physColliderUpdateContacts(node->data.leaf.value, frequency);
		#else
		physColliderUpdateContacts(node->data.leaf.value);
		#endif
		physColliderUpdateSeparations(node->data.leaf.value);

		node = node->data.leaf.next;

	}

	return 1;

}

void physIslandDelete(physIsland *const __RESTRICT__ island){
	aabbTreeTraverse(&island->tree, &modulePhysicsAABBNodeFree);
}