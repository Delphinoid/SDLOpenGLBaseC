#include "modulePhysics.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"

memorySLink __PhysicsRigidBodyBaseResourceArray;   // Contains physRigidBodyBases.
memorySLink __PhysicsRigidBodyResourceArray;       // Contains physRigidBodies.
memorySLink __PhysicsColliderResourceArray;        // Contains physColliders.
memoryQLink __PhysicsJointResourceArray;           // Contains physJoints.
memoryQLink __PhysicsContactPairResourceArray;     // Contains physContactPairs.
memoryQLink __PhysicsSeparationPairResourceArray;  // Contains physSeparationPairs.
memoryList  __PhysicsAABBNodeResourceArray;        // Contains aabbNodes.

return_t modulePhysicsResourcesInit(){
	void *memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE,
			RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM
		)
	);
	if(memSLinkCreate(&__PhysicsRigidBodyBaseResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE, RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RIGID_BODY_SIZE,
			RESOURCE_DEFAULT_RIGID_BODY_NUM
		)
	);
	if(memSLinkCreate(&__PhysicsRigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_COLLIDER_SIZE,
			RESOURCE_DEFAULT_COLLIDER_NUM
		)
	);
	if(memSLinkCreate(&__PhysicsColliderResourceArray, memory, RESOURCE_DEFAULT_COLLIDER_SIZE, RESOURCE_DEFAULT_COLLIDER_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memQLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_JOINT_SIZE,
			RESOURCE_DEFAULT_JOINT_NUM
		)
	);
	if(memQLinkCreate(&__PhysicsJointResourceArray, memory, RESOURCE_DEFAULT_JOINT_SIZE, RESOURCE_DEFAULT_JOINT_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memQLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_CONTACT_PAIR_SIZE,
			RESOURCE_DEFAULT_CONTACT_PAIR_NUM
		)
	);
	if(memQLinkCreate(&__PhysicsContactPairResourceArray, memory, RESOURCE_DEFAULT_CONTACT_PAIR_SIZE, RESOURCE_DEFAULT_CONTACT_PAIR_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memQLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE,
			RESOURCE_DEFAULT_SEPARATION_PAIR_NUM
		)
	);
	if(memQLinkCreate(&__PhysicsSeparationPairResourceArray, memory, RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE, RESOURCE_DEFAULT_SEPARATION_PAIR_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memListAllocationSize(
			NULL,
			RESOURCE_DEFAULT_AABB_NODE_SIZE,
			RESOURCE_DEFAULT_AABB_NODE_NUM
		)
	);
	if(memListCreate(&__PhysicsAABBNodeResourceArray, memory, RESOURCE_DEFAULT_AABB_NODE_SIZE, RESOURCE_DEFAULT_AABB_NODE_NUM) == NULL){
		return -1;
	}
	return 1;
}
void modulePhysicsResourcesReset(){
	memoryRegion *region;
	modulePhysicsRigidBodyBaseClear();
	region = __PhysicsRigidBodyBaseResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsRigidBodyBaseResourceArray.region->next = NULL;
	modulePhysicsRigidBodyClear();
	region = __PhysicsRigidBodyResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsRigidBodyResourceArray.region->next = NULL;
	modulePhysicsColliderClear();
	region = __PhysicsColliderResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsColliderResourceArray.region->next = NULL;
	modulePhysicsJointClear();
	region = __PhysicsJointResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsJointResourceArray.region->next = NULL;
	modulePhysicsContactPairClear();
	region = __PhysicsContactPairResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsContactPairResourceArray.region->next = NULL;
	modulePhysicsSeparationPairClear();
	region = __PhysicsSeparationPairResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsSeparationPairResourceArray.region->next = NULL;
	modulePhysicsAABBNodeClear();
	region = __PhysicsAABBNodeResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsAABBNodeResourceArray.region->next = NULL;
}
void modulePhysicsResourcesDelete(){
	memoryRegion *region;
	modulePhysicsRigidBodyBaseClear();
	region = __PhysicsRigidBodyBaseResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsRigidBodyClear();
	region = __PhysicsRigidBodyResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsColliderClear();
	region = __PhysicsColliderResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsJointClear();
	region = __PhysicsJointResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsContactPairClear();
	region = __PhysicsContactPairResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsSeparationPairClear();
	region = __PhysicsSeparationPairResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsAABBNodeClear();
	region = __PhysicsAABBNodeResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
}

__HINT_INLINE__ physRigidBodyBase *modulePhysicsRigidBodyBaseAppendStatic(physRigidBodyBase **const restrict array){
	return memSLinkAppend(&__PhysicsRigidBodyBaseResourceArray, (void **)array);
}
__HINT_INLINE__ physRigidBodyBase *modulePhysicsRigidBodyBaseAppend(physRigidBodyBase **const restrict array){
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
		if(memSLinkExtend(&__PhysicsRigidBodyBaseResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE, RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM)){
			r = modulePhysicsRigidBodyBaseAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBodyBase *modulePhysicsRigidBodyBaseInsertAfterStatic(physRigidBodyBase **const restrict array, physRigidBodyBase *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsRigidBodyBaseResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physRigidBodyBase *modulePhysicsRigidBodyBaseInsertAfter(physRigidBodyBase **const restrict array, physRigidBodyBase *const restrict resource){
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
		if(memSLinkExtend(&__PhysicsRigidBodyBaseResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE, RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM)){
			r = modulePhysicsRigidBodyBaseInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBodyBase *modulePhysicsRigidBodyBaseNext(const physRigidBodyBase *const restrict i){
	return (physRigidBodyBase *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void modulePhysicsRigidBodyBaseFree(physRigidBodyBase **const restrict array, physRigidBodyBase *const restrict resource, const physRigidBodyBase *const restrict previous){
	physRigidBodyBaseDelete(resource);
	memSLinkFree(&__PhysicsRigidBodyBaseResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsRigidBodyBaseFreeArray(physRigidBodyBase **const restrict array){
	physRigidBodyBase *resource = *array;
	while(resource != NULL){
		modulePhysicsRigidBodyBaseFree(array, resource, NULL);
		resource = *array;
	}
}
void modulePhysicsRigidBodyBaseClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsRigidBodyBaseResourceArray, i, physRigidBodyBase *);

		modulePhysicsRigidBodyBaseFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__PhysicsRigidBodyBaseResourceArray, i, return;);

}

__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyAppendStatic(physRigidBody **const restrict array){
	return memSLinkAppend(&__PhysicsRigidBodyResourceArray, (void **)array);
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyAppend(physRigidBody **const restrict array){
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
		if(memSLinkExtend(&__PhysicsRigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM)){
			r = modulePhysicsRigidBodyAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertAfterStatic(physRigidBody **const restrict array, physRigidBody *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsRigidBodyResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertAfter(physRigidBody **const restrict array, physRigidBody *const restrict resource){
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
		if(memSLinkExtend(&__PhysicsRigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM)){
			r = modulePhysicsRigidBodyInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyNext(const physRigidBody *const restrict i){
	return (physRigidBody *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void modulePhysicsRigidBodyFree(physRigidBody **const restrict array, physRigidBody *const restrict resource, const physRigidBody *const restrict previous){
	physRigidBodyDelete(resource);
	memSLinkFree(&__PhysicsRigidBodyResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsRigidBodyFreeArray(physRigidBody **const restrict array){
	physRigidBody *resource = *array;
	while(resource != NULL){
		modulePhysicsRigidBodyFree(array, resource, NULL);
		resource = *array;
	}
}
void modulePhysicsRigidBodyClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsRigidBodyResourceArray, i, physRigidBody *);

		modulePhysicsRigidBodyFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__PhysicsRigidBodyResourceArray, i, return;);

}

__HINT_INLINE__ physCollider *modulePhysicsColliderAppendStatic(physCollider **const restrict array){
	return memSLinkAppend(&__PhysicsColliderResourceArray, (void **)array);
}
__HINT_INLINE__ physCollider *modulePhysicsColliderAppend(physCollider **const restrict array){
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
		if(memSLinkExtend(&__PhysicsColliderResourceArray, memory, RESOURCE_DEFAULT_COLLIDER_SIZE, RESOURCE_DEFAULT_COLLIDER_NUM)){
			r = modulePhysicsColliderAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physCollider *modulePhysicsColliderInsertAfterStatic(physCollider **const restrict array, physCollider *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsColliderResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physCollider *modulePhysicsColliderInsertAfter(physCollider **const restrict array, physCollider *const restrict resource){
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
		if(memSLinkExtend(&__PhysicsColliderResourceArray, memory, RESOURCE_DEFAULT_COLLIDER_SIZE, RESOURCE_DEFAULT_COLLIDER_NUM)){
			r = modulePhysicsColliderInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physCollider *modulePhysicsColliderNext(const physCollider *const restrict i){
	return (physCollider *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void modulePhysicsColliderFree(physCollider **const restrict array, physCollider *const restrict resource, const physCollider *const restrict previous){
	physColliderDelete(resource);
	memSLinkFree(&__PhysicsColliderResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsColliderFreeArray(physCollider **const restrict array){
	physCollider *resource = *array;
	while(resource != NULL){
		modulePhysicsColliderFree(array, resource, NULL);
		resource = *array;
	}
}
void modulePhysicsColliderClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsColliderResourceArray, i, physCollider *);

		modulePhysicsColliderFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__PhysicsColliderResourceArray, i, return;);

}

__HINT_INLINE__ physJoint *modulePhysicsJointAllocateStatic(){
	return memQLinkAllocate(&__PhysicsJointResourceArray);
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
		if(memQLinkExtend(&__PhysicsJointResourceArray, memory, RESOURCE_DEFAULT_CONTACT_PAIR_SIZE, RESOURCE_DEFAULT_CONTACT_PAIR_NUM)){
			r = modulePhysicsJointAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void modulePhysicsJointFree(physJoint *const restrict resource){
	physJointDelete(resource);
	memQLinkFree(&__PhysicsJointResourceArray, (void *)resource);
}
void modulePhysicsJointClear(){

	MEMORY_QLINK_LOOP_BEGIN(__PhysicsJointResourceArray, i, physJoint *);

		modulePhysicsJointFree(i);
		memQLinkDataSetFlags(i, MEMORY_QLINK_BLOCK_INVALID);

	MEMORY_QLINK_LOOP_INACTIVE_CASE(i);

		memQLinkDataSetFlags(i, MEMORY_QLINK_BLOCK_INVALID);

	MEMORY_QLINK_LOOP_END(__PhysicsJointResourceArray, i, return;);

}

__HINT_INLINE__ physContactPair *modulePhysicsContactPairAllocateStatic(){
	return memQLinkAllocate(&__PhysicsContactPairResourceArray);
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
		if(memQLinkExtend(&__PhysicsContactPairResourceArray, memory, RESOURCE_DEFAULT_CONTACT_PAIR_SIZE, RESOURCE_DEFAULT_CONTACT_PAIR_NUM)){
			r = modulePhysicsContactPairAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void modulePhysicsContactPairFree(physContactPair *const restrict resource){
	physContactPairDelete(resource);
	memQLinkFree(&__PhysicsContactPairResourceArray, (void *)resource);
}
void modulePhysicsContactPairClear(){

	MEMORY_QLINK_LOOP_BEGIN(__PhysicsContactPairResourceArray, i, physContactPair *);

		modulePhysicsContactPairFree(i);
		memQLinkDataSetFlags(i, MEMORY_QLINK_BLOCK_INVALID);

	MEMORY_QLINK_LOOP_INACTIVE_CASE(i);

		memQLinkDataSetFlags(i, MEMORY_QLINK_BLOCK_INVALID);

	MEMORY_QLINK_LOOP_END(__PhysicsContactPairResourceArray, i, return;);

}

__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairAllocateStatic(){
	return memQLinkAllocate(&__PhysicsSeparationPairResourceArray);
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
		if(memQLinkExtend(&__PhysicsSeparationPairResourceArray, memory, RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE, RESOURCE_DEFAULT_SEPARATION_PAIR_NUM)){
			r = modulePhysicsSeparationPairAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void modulePhysicsSeparationPairFree(physSeparationPair *const restrict resource){
	physSeparationPairDelete(resource);
	memQLinkFree(&__PhysicsSeparationPairResourceArray, (void *)resource);
}
void modulePhysicsSeparationPairClear(){

	MEMORY_QLINK_LOOP_BEGIN(__PhysicsSeparationPairResourceArray, i, physSeparationPair *);

		modulePhysicsSeparationPairFree(i);
		memQLinkDataSetFlags(i, MEMORY_QLINK_BLOCK_INVALID);

	MEMORY_QLINK_LOOP_INACTIVE_CASE(i);

		memQLinkDataSetFlags(i, MEMORY_QLINK_BLOCK_INVALID);

	MEMORY_QLINK_LOOP_END(__PhysicsSeparationPairResourceArray, i, return;);

}

__HINT_INLINE__ aabbNode *modulePhysicsAABBNodeAllocateStatic(){
	return memListAllocate(&__PhysicsAABBNodeResourceArray);
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
		if(memListExtend(&__PhysicsAABBNodeResourceArray, memory, RESOURCE_DEFAULT_AABB_NODE_SIZE, RESOURCE_DEFAULT_AABB_NODE_NUM)){
			r = modulePhysicsAABBNodeAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void modulePhysicsAABBNodeFree(aabbNode *const restrict resource){
	///aabbNodeDelete(resource);
	memListFree(&__PhysicsAABBNodeResourceArray, (void *)resource);
}
void modulePhysicsAABBNodeClear(){

	MEMORY_LIST_LOOP_BEGIN(__PhysicsAABBNodeResourceArray, i, aabbNode *);

		modulePhysicsAABBNodeFree(i);

	MEMORY_LIST_LOOP_END(__PhysicsAABBNodeResourceArray, i);

}

void modulePhysicsSolveConstraints(const float dt){


	/*
	** Solves all active constraints
	** for all systems being simulated.
	*/

	size_t i;


	// Integrate velocities.
	MEMORY_SLINK_LOOP_BEGIN(__PhysicsRigidBodyResourceArray, body, physRigidBody *);

		// Integrate the body's velocities.
		physRigidBodyIntegrateVelocity(body, dt);
		physRigidBodyResetAccumulators(body);

	MEMORY_SLINK_LOOP_END(__PhysicsRigidBodyResourceArray, body, goto PHYSICS_JOINT_VELOCITY_SOLVER;);


	// Iteratively solve joint velocity constraints.
	PHYSICS_JOINT_VELOCITY_SOLVER:
	i = PHYSICS_JOINT_VELOCITY_SOLVER_ITERATIONS;
	while(i > 0){

		MEMORY_QLINK_LOOP_BEGIN(__PhysicsJointResourceArray, joint, physJoint *);

			physJointSolveVelocityConstraints(joint);

		MEMORY_QLINK_LOOP_END(__PhysicsJointResourceArray, joint, goto PHYSICS_JOINT_VELOCITY_SOLVER_NEXT_ITERATION;);

		PHYSICS_JOINT_VELOCITY_SOLVER_NEXT_ITERATION:
		--i;

	}

	// Iteratively solve contact velocity constraints.
	i = PHYSICS_CONTACT_VELOCITY_SOLVER_ITERATIONS;
	while(i > 0){

		MEMORY_QLINK_LOOP_BEGIN(__PhysicsContactPairResourceArray, contact, physContactPair *);

			physContactSolveVelocityConstraints(&contact->data, contact->colliderA->body, contact->colliderB->body);

		MEMORY_QLINK_LOOP_END(__PhysicsContactPairResourceArray, contact, goto PHYSICS_CONTACT_VELOCITY_SOLVER_NEXT_ITERATION;);

		PHYSICS_CONTACT_VELOCITY_SOLVER_NEXT_ITERATION:
		--i;

	}


	// Integrate configurations.
	MEMORY_SLINK_LOOP_BEGIN(__PhysicsRigidBodyResourceArray, body, physRigidBody *);

		// Integrate the body's configuration.
		physRigidBodyIntegrateConfiguration(body, dt);

	#ifndef PHYSICS_GAUSS_SEIDEL_SOLVER
	MEMORY_SLINK_LOOP_END(__PhysicsRigidBodyResourceArray, body, return;);
	#else
	MEMORY_SLINK_LOOP_END(__PhysicsRigidBodyResourceArray, body, goto PHYSICS_JOINT_CONFIGURATION_SOLVER;);


	// Iteratively solve joint configuration constraints.
	PHYSICS_JOINT_CONFIGURATION_SOLVER:
	i = PHYSICS_JOINT_CONFIGURATION_SOLVER_ITERATIONS;
	/*while(i > 0){

		float error = 0.f;

		MEMORY_QLINK_LOOP_BEGIN(__PhysicsJointResourceArray, joint, physJoint *);

			error = physJointSolveConfigurationConstraints(joint);

		MEMORY_QLINK_LOOP_END(__PhysicsJointResourceArray, joint, goto PHYSICS_JOINT_CONFIGURATION_SOLVER_NEXT_ITERATION;);

		PHYSICS_JOINT_CONFIGURATION_SOLVER_NEXT_ITERATION:
		// Exit if the error is small.
		if(error >= PHYSICS_ERROR_THRESHOLD){
			goto PHYSICS_CONTACT_CONFIGURATION_SOLVER;
		}else{
			--i;
		}

	}*/

	// Iteratively solve contact configuration constraints.
	PHYSICS_CONTACT_CONFIGURATION_SOLVER:
	i = PHYSICS_CONTACT_CONFIGURATION_SOLVER_ITERATIONS;
	while(i > 0){

		float error = 0.f;

		MEMORY_QLINK_LOOP_BEGIN(__PhysicsContactPairResourceArray, contact, physContactPair *);

			error = physContactSolveConfigurationConstraints(&contact->data, contact->colliderA->body, contact->colliderB->body, error);

		MEMORY_QLINK_LOOP_END(__PhysicsContactPairResourceArray, contact, goto PHYSICS_CONTACT_CONFIGURATION_SOLVER_NEXT_ITERATION;);

		PHYSICS_CONTACT_CONFIGURATION_SOLVER_NEXT_ITERATION:
		// Exit if the error is small.
		if(error >= PHYSICS_ERROR_THRESHOLD){
			return;
		}else{
			--i;
		}

	}

	#endif

}