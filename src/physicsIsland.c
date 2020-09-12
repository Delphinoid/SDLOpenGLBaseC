#include "physicsIsland.h"
#include "physicsCollision.h"
#include "physicsCollider.h"
#include "physicsRigidBody.h"
#include "physicsConstraint.h"
#include "physicsJoint.h"
#include "modulePhysics.h"

void physIslandInit(physIsland *const __RESTRICT__ island){
	island->bodies = NULL;
	island->joints = NULL;
	island->contacts = NULL;
	island->separations = NULL;
	aabbTreeInit(&island->tree);
}

static aabbNode *physIslandAllocateNode(void *unused){
	return modulePhysicsAABBNodeAllocate();
}
static void physIslandFreeNode(aabbNode *const __RESTRICT__ node, void *island){

	// Only treat the node as a collider if it is a leaf node.
	if(AABB_TREE_NODE_IS_LEAF(node)){

		physCollider *const c = (physCollider *)node->data.leaf.value;
		physContactPair *contact;
		physSeparationPair *separation;

		// Free the collider's contact cache.
		contact = c->contactCache;
		while(contact != NULL){
			modulePhysicsContactPairFree(&((physIsland *)island)->contacts, contact);
			if(contact->colliderA != c){
				contact = contact->nextB;
			}else{
				contact = contact->nextA;
			}
		}

		// Free the collider's separation cache.
		separation = c->separationCache;
		while(separation != NULL){
			modulePhysicsSeparationPairFree(&((physIsland *)island)->separations, separation);
			if(separation->colliderA != c){
				separation = separation->nextB;
			}else{
				separation = separation->nextA;
			}
		}

	}

	// Remove the collider from the AABB tree.
	modulePhysicsAABBNodeFree(node);

}

static __FORCE_INLINE__ return_t physIslandUpdateCollider(physIsland *const __RESTRICT__ island, physCollider *const __RESTRICT__ c){
	if(c->node == NULL){
		// Fatten and insert a collider into the island.
		cAABB expandedAABB = c->aabb;
		cAABBExpandVelocity(&expandedAABB, ((physRigidBody *)c->body)->linearVelocity, PHYSICS_ISLAND_COLLIDER_AABB_VELOCITY_FACTOR);
		cAABBExpand(&expandedAABB, PHYSICS_ISLAND_COLLIDER_AABB_PADDING);
		if(aabbTreeInsert(&island->tree, &c->node, (void *)c, &expandedAABB, &physIslandAllocateNode, NULL) < 0){
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
		cAABBExpand(&c->node->aabb, PHYSICS_ISLAND_COLLIDER_AABB_PADDING);
		aabbTreeUpdate(&island->tree, c->node);
	}
	return 1;
}
static __FORCE_INLINE__ void physIslandRemoveCollider(physIsland *const __RESTRICT__ island, physCollider *const __RESTRICT__ c){
	// Remove the collider and all of its contacts / separations.
	if(c->node != NULL){
		aabbTreeRemove(&island->tree, c->node, &physIslandFreeNode, island);
	}
}

__HINT_INLINE__ void physIslandInsertJoint(physIsland *const __RESTRICT__ island, physJoint *const joint){
	// Prepend the joint to the linked list.
	if(island->joints != NULL){
		memDLinkPrev(island->joints) = (byte_t *)joint;
		memDLinkNext(joint) = (byte_t *)island->joints;
	}
	memDLinkPrev(joint) = NULL;
	island->joints = joint;
}
__HINT_INLINE__ void physIslandRemoveJoint(physIsland *const __RESTRICT__ island, physJoint *const joint){
	// Set the next element's previous pointer.
	if(memDLinkNext(joint) != NULL){
		memDLinkPrev(memDLinkNext(joint)) = memDLinkPrev(joint);
	}
	// Set the previous element's next pointer.
	if(memDLinkPrev(joint) != NULL && island->joints != joint){
		memDLinkNext(memDLinkPrev(joint)) = memDLinkNext(joint);
	}else{
		island->joints = (physJoint *)memDLinkNext(joint);
	}
	// Completely remove the element from the list.
	memDLinkPrev(joint) = NULL;
	memDLinkNext(joint) = NULL;
}

static __FORCE_INLINE__ return_t physIslandUpdateRigidBody(physIsland *const __RESTRICT__ island, physRigidBody *const __RESTRICT__ body){

	// Transform the vertices of each body into global space.
	// If the body is set to not collide, remove the colliders
	// from the island that they are a part of.

	if(physRigidBodyIsCollidable(body)){
		if(flagsAreSet(body->flags, PHYSICS_BODY_TRANSFORMED)){

			// Update each collider.
			physCollider *c = body->hull;
			while(c != NULL){

				// Update the collider's AABB.
				physColliderTransform(c);

				// Add the collider to the island.
				if(physIslandUpdateCollider(island, c) < 0){
					/** Memory allocation failure. **/
					return -1;
				}

				c = (physCollider *)memSLinkNext(c);

			}

			flagsUnset(body->flags, PHYSICS_BODY_COLLISION_MODIFIED);

		}
	}else if(flagsAreSet(body->flags, PHYSICS_BODY_COLLISION_MODIFIED)){

		// Remove each collider.
		physCollider *c = body->hull;
		while(c != NULL){
			physIslandRemoveCollider(island, c);
			c = (physCollider *)memSLinkNext(c);
		}
		flagsUnset(body->flags, PHYSICS_BODY_COLLISION_MODIFIED);

	}

	return 1;

}
__HINT_INLINE__ void physIslandInsertRigidBody(physIsland *const __RESTRICT__ island, physRigidBody *const body){

	physJoint *joint;

	// Prepend the body to the linked list.
	if(island->bodies != NULL){
		memDLinkPrev(island->bodies) = (byte_t *)body;
		memDLinkNext(body) = (byte_t *)island->bodies;
	}
	memDLinkPrev(body) = NULL;
	island->bodies = body;

	// Insert joints.
	joint = body->joints;
	while(joint != NULL){
		physIslandInsertJoint(island, joint);
		if(joint->bodyA != body){
			joint = joint->nextB;
		}else{
			joint = joint->nextA;
		}
	}

}
__HINT_INLINE__ void physIslandRemoveRigidBody(physIsland *const __RESTRICT__ island, physRigidBody *const body){

	physJoint *j;
	physCollider *c;

	// Set the next element's previous pointer.
	if(memDLinkNext(body) != NULL){
		memDLinkPrev(memDLinkNext(body)) = memDLinkPrev(body);
	}
	// Set the previous element's next pointer.
	if(memDLinkPrev(body) != NULL && island->bodies != body){
		memDLinkNext(memDLinkPrev(body)) = memDLinkNext(body);
	}else{
		island->bodies = (physRigidBody *)memDLinkNext(body);
	}
	// Completely remove the element from the list.
	memDLinkPrev(body) = NULL;
	memDLinkNext(body) = NULL;

	// Remove joints.
	j = body->joints;
	while(j != NULL){
		physIslandRemoveJoint(island, j);
		if(j->bodyA != body){
			j = j->nextB;
		}else{
			j = j->nextA;
		}
	}

	// Remove colliders.
	c = body->hull;
	while(c != NULL){
		physIslandRemoveCollider(island, c);
		c = (physCollider *)memSLinkNext(c);
	}

}

__HINT_INLINE__ void physIslandInsertRigidBodies(physIsland *const __RESTRICT__ island, physRigidBody *const bodies, physicsBodyIndex_t bodyNum){

	// Prepend the body to the linked list.
	if(bodies != NULL){
		if(island->bodies != NULL){

			// Get the last body and insert joints as we go.
			physRigidBody *last = bodies;
			for(;;){

				// Insert joints.
				physJoint *j = last->joints;
				while(j != NULL){
					physIslandInsertJoint(island, j);
					if(j->bodyA != last){
						j = j->nextB;
					}else{
						j = j->nextA;
					}
				}

				--bodyNum;
				if(bodyNum <= 0){
					break;
				}
				last = (physRigidBody *)memDLinkNext(last);

			}

			// Insert the body list.
			memDLinkPrev(island->bodies) = (byte_t *)last;
			memDLinkNext(last) = (byte_t *)island->bodies;

		}

		memDLinkPrev(bodies) = NULL;
		island->bodies = bodies;

	}

}
__HINT_INLINE__ void physIslandRemoveRigidBodies(physIsland *const __RESTRICT__ island, physRigidBody *const bodies, physicsBodyIndex_t bodyNum){

	// Remove the bodies from the linked list.
	// Prepend the body to the linked list.
	if(bodies != NULL){

		// Get the last body and remove joints as we go.
		physRigidBody *last = bodies;
		for(;;){

			physJoint *j;
			physCollider *c;

			// Remove joints.
			j = last->joints;
			while(j != NULL){
				physIslandRemoveJoint(island, j);
				if(j->bodyA != last){
					j = j->nextB;
				}else{
					j = j->nextA;
				}
			}

			// Remove colliders.
			c = last->hull;
			while(c != NULL){
				physIslandRemoveCollider(island, c);
				c = (physCollider *)memSLinkNext(c);
			}

			--bodyNum;
			if(bodyNum <= 0){
				break;
			}
			last = (physRigidBody *)memDLinkNext(last);

		}

		// Set the next element's previous pointer.
		if(memDLinkNext(last) != NULL){
			memDLinkPrev(memDLinkNext(last)) = memDLinkPrev(bodies);
		}
		// Set the previous element's next pointer.
		if(memDLinkPrev(bodies) != NULL && island->bodies != bodies){
			memDLinkNext(memDLinkPrev(bodies)) = memDLinkNext(last);
		}else{
			island->bodies = (physRigidBody *)memDLinkNext(last);
		}
		// Completely remove the element from the list.
		memDLinkPrev(bodies) = NULL;
		memDLinkNext(last) = NULL;

	}

}

return_t physIslandCollisionQuery(aabbNode *const n1, aabbNode *const n2, void *island){

	// Manages the broadphase and narrowphase
	// between a potential contact pair.

	physCollider *const c1 = (physCollider *)n1->data.leaf.value;
	physCollider *const c2 = (physCollider *)n2->data.leaf.value;

	// Make sure the colliders and their owning bodies permit collisions.
	if(physColliderPermitCollision(c1, c2) && physRigidBodyPermitCollision((physRigidBody *)c1->body, (physRigidBody *)c2->body)){

		// Broadphase collision check.
		if(cAABBCollision(&c1->aabb, &c2->aabb)){

			cContact manifold;
			physSeparation separation;
			physSeparation *separationPointer;

			// Find a previous separation for the pair, if one exists.
			void *previous, *next;
			void *pair = physColliderFindSeparation(c1, c2, (physSeparationPair **)&previous, (physSeparationPair **)&next);

			// If a separation does exist, check it.
			if(pair != NULL){
				separationPointer = &((physSeparationPair *)pair)->data;
				if(cCheckSeparation(&c1->c, &c2->c, separationPointer)){
					// The separation still exists, refresh it and exit.
					physSeparationPairRefresh((physSeparationPair *)pair);
					return 1;
				}
			}else{
				// Create a new separation.
				separationPointer = &separation;
			}

			// Narrowphase collision check.
			if(cCheckCollision(&c1->c, &c2->c, separationPointer, &manifold)){

				// The two colliders are in contact.
				// If a contact pair already exists, refresh
				// it, otherwise allocate a new pair.
				pair = physColliderFindContact(c1, c2, (physContactPair **)&previous, (physContactPair **)&next);
				if(pair != NULL){
					// The contact already exists, refresh it.
					physContactPairRefresh((physContactPair *)pair);
					physContactPersist(&((physContactPair *)pair)->data, &manifold, c1->body, c2->body, c1, c2);
				}else{
					// Allocate a new contact.
					pair = modulePhysicsContactPairPrepend(&((physIsland *)island)->contacts);
					if(pair == NULL){
						/** Memory allocation failure. **/
						return -1;
					}
					// Initialize the pair and its contact.
					physContactPairInit(pair, c1, c2, previous, next);
					physContactInit(&((physContactPair *)pair)->data, &manifold, c1->body, c2->body, c1, c2);
				}

			}else{

				// The two colliders are not in contact.
				// If a separation pair already exists, refresh
				// it, otherwise allocate a new pair.
				if(pair != NULL){
					// The separation already exists, refresh it.
					physSeparationPairRefresh((physSeparationPair *)pair);
				}else{
					// Allocate a new separation.
					pair = modulePhysicsSeparationPairPrepend(&((physIsland *)island)->separations);
					if(pair == NULL){
						/** Memory allocation failure. **/
						return -1;
					}
					// Initialize the pair and its separation.
					physSeparationPairInit(pair, c1, c2, previous, next);
					((physSeparationPair *)pair)->data = separation;
				}

			}

		}

	}

	return 1;

}

#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
static __FORCE_INLINE__ void physIslandUpdateColliderContacts(physIsland *const __RESTRICT__ island, physCollider *const c, const float frequency){
#else
static __FORCE_INLINE__ void physIslandUpdateColliderContacts(physIsland *const __RESTRICT__ island, physCollider *const c){
#endif

	// Removes any contact that has been inactive for too long.

	physContactPair *i = c->contactCache;

	while(i != NULL && i->colliderA == c){
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		physContactPair *const next = (physContactPair *)memQLinkNextA(i);
		#else
		physContactPair *const next = i->nextA;
		#endif
		if(i->inactive > 0){
			if(i->inactive > PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS){
				// Remove the contact.
				modulePhysicsContactPairFree(&island->contacts, i);
			}else{
				physContactReset(&i->data);
			}
		}else{
			// Update the contact.
			#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
			physContactPresolveConstraints(&i->data, i->colliderA, i->colliderB, frequency);
			#else
			physContactPresolveConstraints(&i->data, i->colliderA, i->colliderB);
			#endif
			++i->inactive;
		}
		i = next;
	}

}
static __FORCE_INLINE__ void physIslandUpdateColliderSeparations(physIsland *const __RESTRICT__ island, physCollider *const c){

	// Removes any separation that has been inactive for too long.

	physSeparationPair *i = c->separationCache;

	while(i != NULL && i->colliderA == c){
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		physSeparationPair *const next = (physSeparationPair *)memQLinkNextA(i);
		#else
		physSeparationPair *const next = i->nextA;
		#endif
		if(i->inactive > PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS){
			// Remove the separation.
			modulePhysicsSeparationPairFree(&island->separations, i);
		}else{
			++i->inactive;
		}
		i = next;
	}

}
#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
static __FORCE_INLINE__ return_t physIslandQuery(physIsland *const __RESTRICT__ island, const float frequency){
#else
static __FORCE_INLINE__ return_t physIslandQuery(physIsland *const __RESTRICT__ island){
#endif

	// Maintain contact and separation pairs for each collider.
	// Presolve the contact constraints while we're at it.

	aabbNode *node = island->tree.leaves;

	while(node != NULL){

		// Check for potential collisions with the current node.
		if(aabbTreeQueryNodeStack(&island->tree, node, &physIslandCollisionQuery, (void *)island) < 0){
			/** Memory allocation failure. **/
			return -1;
		}

		// Remove any outdated contacts and separations and update what's left.
		#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
		physIslandUpdateColliderContacts(island, node->data.leaf.value, frequency);
		#else
		physIslandUpdateColliderContacts(island, node->data.leaf.value);
		#endif
		physIslandUpdateColliderSeparations(island, node->data.leaf.value);

		node = node->data.leaf.next;

	}

	return 1;

}

static __FORCE_INLINE__ return_t physIslandPresolveConstraints(physIsland *const __RESTRICT__ island, const float dt){

	// Presolves all active constraints (asides from contact constraints,
	// which are handled by islands) for all systems being simulated.

	physRigidBody *body;
	physJoint *joint;

	// Integrate velocities.
	body = island->bodies;
	while(body != NULL){

		// Integrate the body's velocities.
		physRigidBodyIntegrateVelocity(body, dt);
		physRigidBodyResetAccumulators(body);

		// Add the body to the physics island
		// and update all of its colliders.
		if(physIslandUpdateRigidBody(island, body) < 0){
			/** Memory allocation failure. **/
			return -1;
		}

		body = (physRigidBody *)memDLinkNext(body);
	}

	// Presolve joint constraints.
	joint = island->joints;
	while(joint != NULL){
		physJointPresolveConstraints(joint, dt);
		joint = (physJoint *)memDLinkNext(joint);
	}

	return 1;

}

static __FORCE_INLINE__ void physIslandSolveConstraints(const physIsland *const __RESTRICT__ island, const float dt){

	// Solves all active constraints
	// for all systems being simulated.

	size_t i;
	physRigidBody *body;
	physJoint *joint;
	physContactPair *contact;


	// Iteratively solve joint and contact velocity constraints.
	i = PHYSICS_VELOCITY_SOLVER_ITERATIONS;
	while(i > 0){

		// Solve joint velocity constraints.
		joint = island->joints;
		while(joint != NULL){
			physJointSolveVelocityConstraints(joint);
			joint = (physJoint *)memDLinkNext(joint);
		}

		// Solve contact velocity constraints.
		contact = island->contacts;
		while(contact != NULL){
			physContactSolveVelocityConstraints(&contact->data, contact->colliderA->body, contact->colliderB->body);
			contact = (physContactPair *)memDLinkNext(contact);
		}

		--i;

	}


	// Integrate configurations.
	body = island->bodies;
	while(body != NULL){
		// Integrate the body's configuration.
		physRigidBodyIntegrateConfiguration(body, dt);
		body = (physRigidBody *)memDLinkNext(body);
	}

	#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL

	// Iteratively solve joint and contact configuration constraints.
	i = PHYSICS_CONFIGURATION_SOLVER_ITERATIONS;
	while(i > 0){

		return_t solved = 1;
		float separation = 0.f;

		// Solve joint configuration constraints.
		joint = island->joints;
		while(joint != NULL){
			solved &= physJointSolveConfigurationConstraints(joint);
			joint = (physJoint *)memDLinkNext(joint);
		}

		// Solve contact configuration constraints.
		contact = island->contacts;
		while(contact != NULL){
			separation = physContactSolveConfigurationConstraints(&contact->data, contact->colliderA->body, contact->colliderB->body, separation);
			contact = (physContactPair *)memDLinkNext(contact);
		}

		// Exit if the errors are small.
		if(solved && separation >= PHYSICS_CONTACT_ERROR_THRESHOLD){
			return;
		}else{
			--i;
		}

	}

	#endif

}

#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
return_t physIslandTick(physIsland *const __RESTRICT__ island, const float dt, const float frequency){
#else
return_t physIslandTick(physIsland *const __RESTRICT__ island, const float dt){
#endif

	// Presolve velocity and joint constraints.
	if(physIslandPresolveConstraints(island, dt) < 0){
		return -1;
	}

	// Query broadphase and presolve contact constraints.
	#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
	physIslandQuery(island, frequency);
	#else
	physIslandQuery(island);
	#endif

	// Solve all constraints.
	physIslandSolveConstraints(island, dt);

	return 1;

}

void physIslandDelete(physIsland *const __RESTRICT__ island){
	aabbTreeTraverse(&island->tree, &physIslandFreeNode, island);
}