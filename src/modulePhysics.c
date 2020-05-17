#include "modulePhysics.h"
#include "moduleSettings.h"
#include "physicsRigidBody.h"
#include "physicsCollider.h"
#include "physicsJoint.h"
#include "physicsCollision.h"
#include "physicsConstraint.h"
#include "aabbTree.h"
#include "memoryManager.h"

#define RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE sizeof(physRigidBodyBase)
#define RESOURCE_DEFAULT_RIGID_BODY_SIZE sizeof(physRigidBody)
#define RESOURCE_DEFAULT_COLLIDER_SIZE sizeof(physCollider)
#define RESOURCE_DEFAULT_JOINT_SIZE sizeof(physJoint)
#define RESOURCE_DEFAULT_CONTACT_PAIR_SIZE sizeof(physContactPair)
#define RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE sizeof(physSeparationPair)
#define RESOURCE_DEFAULT_AABB_NODE_SIZE sizeof(aabbNode)

memorySLink __g_PhysicsRigidBodyBaseResourceArray;   // Contains physRigidBodyBases.
memorySLink __g_PhysicsRigidBodyResourceArray;       // Contains physRigidBodies.
memorySLink __g_PhysicsColliderResourceArray;        // Contains physColliders.
memoryQLink __g_PhysicsJointResourceArray;           // Contains physJoints.
memoryQLink __g_PhysicsContactPairResourceArray;     // Contains physContactPairs.
memoryQLink __g_PhysicsSeparationPairResourceArray;  // Contains physSeparationPairs.
memoryList  __g_PhysicsAABBNodeResourceArray;        // Contains aabbNodes.

return_t modulePhysicsResourcesInit(){
	void *memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE,
			RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM
		)
	);
	if(memSLinkCreate(&__g_PhysicsRigidBodyBaseResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE, RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RIGID_BODY_SIZE,
			RESOURCE_DEFAULT_RIGID_BODY_NUM
		)
	);
	if(memSLinkCreate(&__g_PhysicsRigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_COLLIDER_SIZE,
			RESOURCE_DEFAULT_COLLIDER_NUM
		)
	);
	if(memSLinkCreate(&__g_PhysicsColliderResourceArray, memory, RESOURCE_DEFAULT_COLLIDER_SIZE, RESOURCE_DEFAULT_COLLIDER_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memQLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_JOINT_SIZE,
			RESOURCE_DEFAULT_JOINT_NUM
		)
	);
	if(memQLinkCreate(&__g_PhysicsJointResourceArray, memory, RESOURCE_DEFAULT_JOINT_SIZE, RESOURCE_DEFAULT_JOINT_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memQLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_CONTACT_PAIR_SIZE,
			RESOURCE_DEFAULT_CONTACT_PAIR_NUM
		)
	);
	if(memQLinkCreate(&__g_PhysicsContactPairResourceArray, memory, RESOURCE_DEFAULT_CONTACT_PAIR_SIZE, RESOURCE_DEFAULT_CONTACT_PAIR_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memQLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE,
			RESOURCE_DEFAULT_SEPARATION_PAIR_NUM
		)
	);
	if(memQLinkCreate(&__g_PhysicsSeparationPairResourceArray, memory, RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE, RESOURCE_DEFAULT_SEPARATION_PAIR_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memListAllocationSize(
			NULL,
			RESOURCE_DEFAULT_AABB_NODE_SIZE,
			RESOURCE_DEFAULT_AABB_NODE_NUM
		)
	);
	if(memListCreate(&__g_PhysicsAABBNodeResourceArray, memory, RESOURCE_DEFAULT_AABB_NODE_SIZE, RESOURCE_DEFAULT_AABB_NODE_NUM) == NULL){
		return -1;
	}
	return 1;
}
void modulePhysicsResourcesReset(){
	memoryRegion *region;
	modulePhysicsRigidBodyBaseClear();
	region = __g_PhysicsRigidBodyBaseResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_PhysicsRigidBodyBaseResourceArray.region->next = NULL;
	modulePhysicsRigidBodyClear();
	region = __g_PhysicsRigidBodyResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_PhysicsRigidBodyResourceArray.region->next = NULL;
	modulePhysicsColliderClear();
	region = __g_PhysicsColliderResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_PhysicsColliderResourceArray.region->next = NULL;
	modulePhysicsJointClear();
	region = __g_PhysicsJointResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_PhysicsJointResourceArray.region->next = NULL;
	modulePhysicsContactPairClear();
	region = __g_PhysicsContactPairResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_PhysicsContactPairResourceArray.region->next = NULL;
	modulePhysicsSeparationPairClear();
	region = __g_PhysicsSeparationPairResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_PhysicsSeparationPairResourceArray.region->next = NULL;
	modulePhysicsAABBNodeClear();
	region = __g_PhysicsAABBNodeResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_PhysicsAABBNodeResourceArray.region->next = NULL;
}
void modulePhysicsResourcesDelete(){
	memoryRegion *region;
	modulePhysicsRigidBodyBaseClear();
	region = __g_PhysicsRigidBodyBaseResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsRigidBodyClear();
	region = __g_PhysicsRigidBodyResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsColliderClear();
	region = __g_PhysicsColliderResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsJointClear();
	region = __g_PhysicsJointResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsContactPairClear();
	region = __g_PhysicsContactPairResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsSeparationPairClear();
	region = __g_PhysicsSeparationPairResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsAABBNodeClear();
	region = __g_PhysicsAABBNodeResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
}

__HINT_INLINE__ physRigidBodyBase *modulePhysicsRigidBodyBaseAppendStatic(physRigidBodyBase **const __RESTRICT__ array){
	return memSLinkAppend(&__g_PhysicsRigidBodyBaseResourceArray, (void **)array);
}
__HINT_INLINE__ physRigidBodyBase *modulePhysicsRigidBodyBaseAppend(physRigidBodyBase **const __RESTRICT__ array){
	physRigidBodyBase *r = modulePhysicsRigidBodyBaseAppendStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE,
				RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM
			)
		);
		if(memSLinkExtend(&__g_PhysicsRigidBodyBaseResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE, RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM)){
			r = modulePhysicsRigidBodyBaseAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBodyBase *modulePhysicsRigidBodyBaseInsertAfterStatic(physRigidBodyBase **const __RESTRICT__ array, physRigidBodyBase *const __RESTRICT__ resource){
	return memSLinkInsertAfter(&__g_PhysicsRigidBodyBaseResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physRigidBodyBase *modulePhysicsRigidBodyBaseInsertAfter(physRigidBodyBase **const __RESTRICT__ array, physRigidBodyBase *const __RESTRICT__ resource){
	physRigidBodyBase *r = modulePhysicsRigidBodyBaseInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE,
				RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM
			)
		);
		if(memSLinkExtend(&__g_PhysicsRigidBodyBaseResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE, RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM)){
			r = modulePhysicsRigidBodyBaseInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBodyBase *modulePhysicsRigidBodyBaseNext(const physRigidBodyBase *const __RESTRICT__ i){
	return (physRigidBodyBase *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void modulePhysicsRigidBodyBaseFree(physRigidBodyBase **const __RESTRICT__ array, physRigidBodyBase *const __RESTRICT__ resource, const physRigidBodyBase *const __RESTRICT__ previous){
	physRigidBodyBaseDelete(resource);
	memSLinkFree(&__g_PhysicsRigidBodyBaseResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsRigidBodyBaseFreeArray(physRigidBodyBase **const __RESTRICT__ array){
	physRigidBodyBase *resource = *array;
	while(resource != NULL){
		modulePhysicsRigidBodyBaseFree(array, resource, NULL);
		resource = *array;
	}
}
void modulePhysicsRigidBodyBaseClear(){

	MEMORY_SLINK_LOOP_BEGIN(__g_PhysicsRigidBodyBaseResourceArray, i, physRigidBodyBase *);

		modulePhysicsRigidBodyBaseFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__g_PhysicsRigidBodyBaseResourceArray, i, return;);

}

__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyAppendStatic(physRigidBody **const __RESTRICT__ array){
	return memSLinkAppend(&__g_PhysicsRigidBodyResourceArray, (void **)array);
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyAppend(physRigidBody **const __RESTRICT__ array){
	physRigidBody *r = modulePhysicsRigidBodyAppendStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RIGID_BODY_SIZE,
				RESOURCE_DEFAULT_RIGID_BODY_NUM
			)
		);
		if(memSLinkExtend(&__g_PhysicsRigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM)){
			r = modulePhysicsRigidBodyAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertAfterStatic(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource){
	return memSLinkInsertAfter(&__g_PhysicsRigidBodyResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertAfter(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource){
	physRigidBody *r = modulePhysicsRigidBodyInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RIGID_BODY_SIZE,
				RESOURCE_DEFAULT_RIGID_BODY_NUM
			)
		);
		if(memSLinkExtend(&__g_PhysicsRigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM)){
			r = modulePhysicsRigidBodyInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyNext(const physRigidBody *const __RESTRICT__ i){
	return (physRigidBody *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void modulePhysicsRigidBodyFree(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource, const physRigidBody *const __RESTRICT__ previous){
	physRigidBodyDelete(resource);
	memSLinkFree(&__g_PhysicsRigidBodyResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsRigidBodyFreeArray(physRigidBody **const __RESTRICT__ array){
	physRigidBody *resource = *array;
	while(resource != NULL){
		modulePhysicsRigidBodyFree(array, resource, NULL);
		resource = *array;
	}
}
void modulePhysicsRigidBodyClear(){

	MEMORY_SLINK_LOOP_BEGIN(__g_PhysicsRigidBodyResourceArray, i, physRigidBody *);

		modulePhysicsRigidBodyFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__g_PhysicsRigidBodyResourceArray, i, return;);

}

__HINT_INLINE__ physCollider *modulePhysicsColliderAppendStatic(physCollider **const __RESTRICT__ array){
	return memSLinkAppend(&__g_PhysicsColliderResourceArray, (void **)array);
}
__HINT_INLINE__ physCollider *modulePhysicsColliderAppend(physCollider **const __RESTRICT__ array){
	physCollider *r = modulePhysicsColliderAppendStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_COLLIDER_SIZE,
				RESOURCE_DEFAULT_COLLIDER_NUM
			)
		);
		if(memSLinkExtend(&__g_PhysicsColliderResourceArray, memory, RESOURCE_DEFAULT_COLLIDER_SIZE, RESOURCE_DEFAULT_COLLIDER_NUM)){
			r = modulePhysicsColliderAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physCollider *modulePhysicsColliderInsertAfterStatic(physCollider **const __RESTRICT__ array, physCollider *const __RESTRICT__ resource){
	return memSLinkInsertAfter(&__g_PhysicsColliderResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physCollider *modulePhysicsColliderInsertAfter(physCollider **const __RESTRICT__ array, physCollider *const __RESTRICT__ resource){
	physCollider *r = modulePhysicsColliderInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_COLLIDER_SIZE,
				RESOURCE_DEFAULT_COLLIDER_NUM
			)
		);
		if(memSLinkExtend(&__g_PhysicsColliderResourceArray, memory, RESOURCE_DEFAULT_COLLIDER_SIZE, RESOURCE_DEFAULT_COLLIDER_NUM)){
			r = modulePhysicsColliderInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physCollider *modulePhysicsColliderNext(const physCollider *const __RESTRICT__ i){
	return (physCollider *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void modulePhysicsColliderFree(physCollider **const __RESTRICT__ array, physCollider *const __RESTRICT__ resource, const physCollider *const __RESTRICT__ previous){
	physColliderDelete(resource);
	memSLinkFree(&__g_PhysicsColliderResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsColliderFreeArray(physCollider **const __RESTRICT__ array){
	physCollider *resource = *array;
	while(resource != NULL){
		modulePhysicsColliderFree(array, resource, NULL);
		resource = *array;
	}
}
void modulePhysicsColliderClear(){

	MEMORY_SLINK_LOOP_BEGIN(__g_PhysicsColliderResourceArray, i, physCollider *);

		modulePhysicsColliderFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__g_PhysicsColliderResourceArray, i, return;);

}

__HINT_INLINE__ physJoint *modulePhysicsJointAllocateStatic(){
	return memQLinkAllocate(&__g_PhysicsJointResourceArray);
}
__HINT_INLINE__ physJoint *modulePhysicsJointAllocate(){
	physJoint *r = modulePhysicsJointAllocateStatic();
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memQLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CONTACT_PAIR_SIZE,
				RESOURCE_DEFAULT_CONTACT_PAIR_NUM
			)
		);
		if(memQLinkExtend(&__g_PhysicsJointResourceArray, memory, RESOURCE_DEFAULT_CONTACT_PAIR_SIZE, RESOURCE_DEFAULT_CONTACT_PAIR_NUM)){
			r = modulePhysicsJointAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void modulePhysicsJointFree(physJoint *const __RESTRICT__ resource){
	physJointDelete(resource);
	memQLinkFree(&__g_PhysicsJointResourceArray, (void *)resource);
}
void modulePhysicsJointClear(){

	MEMORY_QLINK_LOOP_BEGIN(__g_PhysicsJointResourceArray, i, physJoint *);

		modulePhysicsJointFree(i);
		memQLinkDataSetFlags(i, MEMORY_QLINK_BLOCK_INVALID);

	MEMORY_QLINK_LOOP_INACTIVE_CASE(i);

		memQLinkDataSetFlags(i, MEMORY_QLINK_BLOCK_INVALID);

	MEMORY_QLINK_LOOP_END(__g_PhysicsJointResourceArray, i, return;);

}

__HINT_INLINE__ physContactPair *modulePhysicsContactPairAllocateStatic(){
	return memQLinkAllocate(&__g_PhysicsContactPairResourceArray);
}
__HINT_INLINE__ physContactPair *modulePhysicsContactPairAllocate(){
	physContactPair *r = modulePhysicsContactPairAllocateStatic();
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memQLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CONTACT_PAIR_SIZE,
				RESOURCE_DEFAULT_CONTACT_PAIR_NUM
			)
		);
		if(memQLinkExtend(&__g_PhysicsContactPairResourceArray, memory, RESOURCE_DEFAULT_CONTACT_PAIR_SIZE, RESOURCE_DEFAULT_CONTACT_PAIR_NUM)){
			r = modulePhysicsContactPairAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void modulePhysicsContactPairFree(physContactPair *const __RESTRICT__ resource){
	physContactPairDelete(resource);
	memQLinkFree(&__g_PhysicsContactPairResourceArray, (void *)resource);
}
void modulePhysicsContactPairClear(){

	MEMORY_QLINK_LOOP_BEGIN(__g_PhysicsContactPairResourceArray, i, physContactPair *);

		modulePhysicsContactPairFree(i);
		memQLinkDataSetFlags(i, MEMORY_QLINK_BLOCK_INVALID);

	MEMORY_QLINK_LOOP_INACTIVE_CASE(i);

		memQLinkDataSetFlags(i, MEMORY_QLINK_BLOCK_INVALID);

	MEMORY_QLINK_LOOP_END(__g_PhysicsContactPairResourceArray, i, return;);

}

__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairAllocateStatic(){
	return memQLinkAllocate(&__g_PhysicsSeparationPairResourceArray);
}
__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairAllocate(){
	physSeparationPair *r = modulePhysicsSeparationPairAllocateStatic();
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memQLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE,
				RESOURCE_DEFAULT_SEPARATION_PAIR_NUM
			)
		);
		if(memQLinkExtend(&__g_PhysicsSeparationPairResourceArray, memory, RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE, RESOURCE_DEFAULT_SEPARATION_PAIR_NUM)){
			r = modulePhysicsSeparationPairAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void modulePhysicsSeparationPairFree(physSeparationPair *const __RESTRICT__ resource){
	physSeparationPairDelete(resource);
	memQLinkFree(&__g_PhysicsSeparationPairResourceArray, (void *)resource);
}
void modulePhysicsSeparationPairClear(){

	MEMORY_QLINK_LOOP_BEGIN(__g_PhysicsSeparationPairResourceArray, i, physSeparationPair *);

		modulePhysicsSeparationPairFree(i);
		memQLinkDataSetFlags(i, MEMORY_QLINK_BLOCK_INVALID);

	MEMORY_QLINK_LOOP_INACTIVE_CASE(i);

		memQLinkDataSetFlags(i, MEMORY_QLINK_BLOCK_INVALID);

	MEMORY_QLINK_LOOP_END(__g_PhysicsSeparationPairResourceArray, i, return;);

}

__HINT_INLINE__ aabbNode *modulePhysicsAABBNodeAllocateStatic(){
	return memListAllocate(&__g_PhysicsAABBNodeResourceArray);
}
__HINT_INLINE__ aabbNode *modulePhysicsAABBNodeAllocate(){
	aabbNode *r = modulePhysicsAABBNodeAllocateStatic();
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memListAllocationSize(
				NULL,
				RESOURCE_DEFAULT_AABB_NODE_SIZE,
				RESOURCE_DEFAULT_AABB_NODE_NUM
			)
		);
		if(memListExtend(&__g_PhysicsAABBNodeResourceArray, memory, RESOURCE_DEFAULT_AABB_NODE_SIZE, RESOURCE_DEFAULT_AABB_NODE_NUM)){
			r = modulePhysicsAABBNodeAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void modulePhysicsAABBNodeFree(aabbNode *const __RESTRICT__ resource){
	///aabbNodeDelete(resource);
	memListFree(&__g_PhysicsAABBNodeResourceArray, (void *)resource);
}
void modulePhysicsAABBNodeClear(){

	MEMORY_LIST_LOOP_BEGIN(__g_PhysicsAABBNodeResourceArray, i, aabbNode *);

		modulePhysicsAABBNodeFree(i);

	MEMORY_LIST_LOOP_END(__g_PhysicsAABBNodeResourceArray, i);

}

/** This should be moved to physIsland. **/
void modulePhysicsPresolveConstraints(const float dt){

	// Presolves all active constraints (asides from contact constraints,
	// which are handled by islands) for all systems being simulated.

	// Integrate velocities.
	MEMORY_SLINK_LOOP_BEGIN(__g_PhysicsRigidBodyResourceArray, body, physRigidBody *);

		// Integrate the body's velocities.
		physRigidBodyIntegrateVelocity(body, dt);
		physRigidBodyResetAccumulators(body);

	MEMORY_SLINK_LOOP_END(__g_PhysicsRigidBodyResourceArray, body, goto PHYSICS_JOINT_PRESOLVER;);

	// Presolve joint constraints.
	PHYSICS_JOINT_PRESOLVER:
	MEMORY_QLINK_LOOP_BEGIN(__g_PhysicsJointResourceArray, joint, physJoint *);

		physJointPresolveConstraints(joint, dt);

	MEMORY_QLINK_LOOP_END(__g_PhysicsJointResourceArray, joint, return;);

}

/** This should be moved to physIsland. **/
void modulePhysicsSolveConstraints(const float dt){

	// Solves all active constraints
	// for all systems being simulated.

	// Iteratively solve joint and contact velocity constraints.
	size_t i = PHYSICS_VELOCITY_SOLVER_ITERATIONS;
	while(i > 0){

		// Solve joint velocity constraints.
		MEMORY_QLINK_LOOP_BEGIN(__g_PhysicsJointResourceArray, joint, physJoint *);

			physJointSolveVelocityConstraints(joint);

		MEMORY_QLINK_LOOP_END(__g_PhysicsJointResourceArray, joint, goto PHYSICS_JOINT_VELOCITY_SOLVER_NEXT_ITERATION;);

		PHYSICS_JOINT_VELOCITY_SOLVER_NEXT_ITERATION:
		// Solve contact velocity constraints.
		MEMORY_QLINK_LOOP_BEGIN(__g_PhysicsContactPairResourceArray, contact, physContactPair *);

			physContactSolveVelocityConstraints(&contact->data, contact->colliderA->body, contact->colliderB->body);

		MEMORY_QLINK_LOOP_END(__g_PhysicsContactPairResourceArray, contact, goto PHYSICS_CONTACT_VELOCITY_SOLVER_NEXT_ITERATION;);

		PHYSICS_CONTACT_VELOCITY_SOLVER_NEXT_ITERATION:
		--i;

	}


	// Integrate configurations.
	MEMORY_SLINK_LOOP_BEGIN(__g_PhysicsRigidBodyResourceArray, body, physRigidBody *);

		// Integrate the body's configuration.
		physRigidBodyIntegrateConfiguration(body, dt);

	#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
	MEMORY_SLINK_LOOP_END(__g_PhysicsRigidBodyResourceArray, body, return;);
	#else
	MEMORY_SLINK_LOOP_END(__g_PhysicsRigidBodyResourceArray, body, goto PHYSICS_CONFIGURATION_SOLVER;);


	// Iteratively solve joint and contact configuration constraints.
	PHYSICS_CONFIGURATION_SOLVER:
	i = PHYSICS_CONFIGURATION_SOLVER_ITERATIONS;
	while(i > 0){

		return_t solved = 1;
		float separation = 0.f;

		// Solve joint configuration constraints.
		MEMORY_QLINK_LOOP_BEGIN(__g_PhysicsJointResourceArray, joint, physJoint *);

			solved &= physJointSolveConfigurationConstraints(joint);

		MEMORY_QLINK_LOOP_END(__g_PhysicsJointResourceArray, joint, goto PHYSICS_JOINT_CONFIGURATION_SOLVER_NEXT_ITERATION;);

		PHYSICS_JOINT_CONFIGURATION_SOLVER_NEXT_ITERATION:
		// Solve contact configuration constraints.
		MEMORY_QLINK_LOOP_BEGIN(__g_PhysicsContactPairResourceArray, contact, physContactPair *);

			separation = physContactSolveConfigurationConstraints(&contact->data, contact->colliderA->body, contact->colliderB->body, separation);

		MEMORY_QLINK_LOOP_END(__g_PhysicsContactPairResourceArray, contact, goto PHYSICS_CONTACT_CONFIGURATION_SOLVER_NEXT_ITERATION;);

		PHYSICS_CONTACT_CONFIGURATION_SOLVER_NEXT_ITERATION:
		// Exit if the errors are small.
		if(solved && separation >= PHYSICS_CONTACT_ERROR_THRESHOLD){
			return;
		}else{
			--i;
		}

	}

	#endif

}