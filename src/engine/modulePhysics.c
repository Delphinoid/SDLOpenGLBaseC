#include "settingsModule.h"
#include "modulePhysics.h"
#include "physicsRigidBody.h"
#include "physicsCollider.h"
#include "physicsJoint.h"
#include "physicsContact.h"
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
memoryDLink __g_PhysicsRigidBodyResourceArray;       // Contains physRigidBodies.
memorySLink __g_PhysicsColliderResourceArray;        // Contains physColliders.
memoryDLink __g_PhysicsJointResourceArray;           // Contains physJoints.
#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
memoryQLink __g_PhysicsContactPairResourceArray;     // Contains physContactPairs.
memoryQLink __g_PhysicsSeparationPairResourceArray;  // Contains physSeparationPairs.
#else
memoryDLink __g_PhysicsContactPairResourceArray;     // Contains physContactPairs.
memoryDLink __g_PhysicsSeparationPairResourceArray;  // Contains physSeparationPairs.
#endif
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
		memDLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RIGID_BODY_SIZE,
			RESOURCE_DEFAULT_RIGID_BODY_NUM
		)
	);
	if(memDLinkCreate(&__g_PhysicsRigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM) == NULL){
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
		memDLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_JOINT_SIZE,
			RESOURCE_DEFAULT_JOINT_NUM
		)
	);
	if(memDLinkCreate(&__g_PhysicsJointResourceArray, memory, RESOURCE_DEFAULT_JOINT_SIZE, RESOURCE_DEFAULT_JOINT_NUM) == NULL){
		return -1;
	}
	#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
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
	#else
	memory = memAllocate(
		memDLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_CONTACT_PAIR_SIZE,
			RESOURCE_DEFAULT_CONTACT_PAIR_NUM
		)
	);
	if(memDLinkCreate(&__g_PhysicsContactPairResourceArray, memory, RESOURCE_DEFAULT_CONTACT_PAIR_SIZE, RESOURCE_DEFAULT_CONTACT_PAIR_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memDLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE,
			RESOURCE_DEFAULT_SEPARATION_PAIR_NUM
		)
	);
	if(memDLinkCreate(&__g_PhysicsSeparationPairResourceArray, memory, RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE, RESOURCE_DEFAULT_SEPARATION_PAIR_NUM) == NULL){
		return -1;
	}
	#endif
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
	return memDLinkAppend(&__g_PhysicsRigidBodyResourceArray, (void **)array);
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyAppend(physRigidBody **const __RESTRICT__ array){
	physRigidBody *r = modulePhysicsRigidBodyAppendStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RIGID_BODY_SIZE,
				RESOURCE_DEFAULT_RIGID_BODY_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsRigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM)){
			r = modulePhysicsRigidBodyAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertAfterStatic(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource){
	return memDLinkInsertAfter(&__g_PhysicsRigidBodyResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertAfter(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource){
	physRigidBody *r = modulePhysicsRigidBodyInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RIGID_BODY_SIZE,
				RESOURCE_DEFAULT_RIGID_BODY_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsRigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM)){
			r = modulePhysicsRigidBodyInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyPrependStatic(physRigidBody **const __RESTRICT__ array){
	return memDLinkPrepend(&__g_PhysicsRigidBodyResourceArray, (void **)array);
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyPrepend(physRigidBody **const __RESTRICT__ array){
	physRigidBody *r = modulePhysicsRigidBodyPrependStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RIGID_BODY_SIZE,
				RESOURCE_DEFAULT_RIGID_BODY_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsRigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM)){
			r = modulePhysicsRigidBodyPrependStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertBeforeStatic(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource){
	return memDLinkInsertBefore(&__g_PhysicsRigidBodyResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertBefore(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource){
	physRigidBody *r = modulePhysicsRigidBodyInsertBeforeStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RIGID_BODY_SIZE,
				RESOURCE_DEFAULT_RIGID_BODY_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsRigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM)){
			r = modulePhysicsRigidBodyInsertBeforeStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyNext(const physRigidBody *const __RESTRICT__ i){
	return (physRigidBody *)memDLinkDataGetNext(i);
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyPrev(const physRigidBody *const __RESTRICT__ i){
	return (physRigidBody *)memDLinkDataGetPrev(i);
}
__HINT_INLINE__ void modulePhysicsRigidBodyFree(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource){
	physRigidBodyDelete(resource);
	memDLinkFree(&__g_PhysicsRigidBodyResourceArray, (void **)array, (void *)resource);
}
void modulePhysicsRigidBodyFreeArray(physRigidBody **const __RESTRICT__ array){
	physRigidBody *resource = *array;
	while(resource != NULL){
		modulePhysicsRigidBodyFree(array, resource);
		resource = *array;
	}
}
void modulePhysicsRigidBodyClear(){

	MEMORY_DLINK_LOOP_BEGIN(__g_PhysicsRigidBodyResourceArray, i, physRigidBody *);

		modulePhysicsRigidBodyFree(NULL, i);
		memDLinkDataSetFlags(i, MEMORY_DLINK_BLOCK_INVALID);

	MEMORY_DLINK_LOOP_INACTIVE_CASE(i);

		memDLinkDataSetFlags(i, MEMORY_DLINK_BLOCK_INVALID);

	MEMORY_DLINK_LOOP_END(__g_PhysicsRigidBodyResourceArray, i, return;);

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
	return memDLinkAllocate(&__g_PhysicsJointResourceArray);
}
__HINT_INLINE__ physJoint *modulePhysicsJointAllocate(){
	physJoint *r = modulePhysicsJointAllocateStatic();
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_JOINT_SIZE,
				RESOURCE_DEFAULT_JOINT_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsJointResourceArray, memory, RESOURCE_DEFAULT_JOINT_SIZE, RESOURCE_DEFAULT_JOINT_NUM)){
			r = modulePhysicsJointAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ physJoint *modulePhysicsJointAppendStatic(physJoint **const __RESTRICT__ array){
	return memDLinkAppend(&__g_PhysicsJointResourceArray, (void **)array);
}
__HINT_INLINE__ physJoint *modulePhysicsJointAppend(physJoint **const __RESTRICT__ array){
	physJoint *r = modulePhysicsJointAppendStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_JOINT_SIZE,
				RESOURCE_DEFAULT_JOINT_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsJointResourceArray, memory, RESOURCE_DEFAULT_JOINT_SIZE, RESOURCE_DEFAULT_JOINT_NUM)){
			r = modulePhysicsJointAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physJoint *modulePhysicsJointInsertAfterStatic(physJoint **const __RESTRICT__ array, physJoint *const __RESTRICT__ resource){
	return memDLinkInsertAfter(&__g_PhysicsJointResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physJoint *modulePhysicsJointInsertAfter(physJoint **const __RESTRICT__ array, physJoint *const __RESTRICT__ resource){
	physJoint *r = modulePhysicsJointInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_JOINT_SIZE,
				RESOURCE_DEFAULT_JOINT_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsJointResourceArray, memory, RESOURCE_DEFAULT_JOINT_SIZE, RESOURCE_DEFAULT_JOINT_NUM)){
			r = modulePhysicsJointInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physJoint *modulePhysicsJointPrependStatic(physJoint **const __RESTRICT__ array){
	return memDLinkPrepend(&__g_PhysicsJointResourceArray, (void **)array);
}
__HINT_INLINE__ physJoint *modulePhysicsJointPrepend(physJoint **const __RESTRICT__ array){
	physJoint *r = modulePhysicsJointPrependStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_JOINT_SIZE,
				RESOURCE_DEFAULT_JOINT_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsJointResourceArray, memory, RESOURCE_DEFAULT_JOINT_SIZE, RESOURCE_DEFAULT_JOINT_NUM)){
			r = modulePhysicsJointPrependStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physJoint *modulePhysicsJointInsertBeforeStatic(physJoint **const __RESTRICT__ array, physJoint *const __RESTRICT__ resource){
	return memDLinkInsertBefore(&__g_PhysicsJointResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physJoint *modulePhysicsJointInsertBefore(physJoint **const __RESTRICT__ array, physJoint *const __RESTRICT__ resource){
	physJoint *r = modulePhysicsJointInsertBeforeStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_JOINT_SIZE,
				RESOURCE_DEFAULT_JOINT_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsJointResourceArray, memory, RESOURCE_DEFAULT_JOINT_SIZE, RESOURCE_DEFAULT_JOINT_NUM)){
			r = modulePhysicsJointInsertBeforeStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physJoint *modulePhysicsJointNext(const physJoint *const __RESTRICT__ i){
	return (physJoint *)memDLinkDataGetNext(i);
}
__HINT_INLINE__ physJoint *modulePhysicsJointPrev(const physJoint *const __RESTRICT__ i){
	return (physJoint *)memDLinkDataGetPrev(i);
}
__HINT_INLINE__ void modulePhysicsJointFree(physJoint **const __RESTRICT__ array, physJoint *const __RESTRICT__ resource){
	physJointDelete(resource);
	memDLinkFree(&__g_PhysicsJointResourceArray, (void **)array, (void *)resource);
}
void modulePhysicsJointFreeArray(physJoint **const __RESTRICT__ array){
	physJoint *resource = *array;
	while(resource != NULL){
		modulePhysicsJointFree(array, resource);
		resource = *array;
	}
}
void modulePhysicsJointClear(){

	MEMORY_DLINK_LOOP_BEGIN(__g_PhysicsJointResourceArray, i, physJoint *);

		modulePhysicsJointFree(NULL, i);
		memDLinkDataSetFlags(i, MEMORY_DLINK_BLOCK_INVALID);

	MEMORY_DLINK_LOOP_INACTIVE_CASE(i);

		memDLinkDataSetFlags(i, MEMORY_DLINK_BLOCK_INVALID);

	MEMORY_DLINK_LOOP_END(__g_PhysicsJointResourceArray, i, return;);

}

#ifdef PHYSICS_CONTACT_USE_ALLOCATOR

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

#else

__HINT_INLINE__ physContactPair *modulePhysicsContactPairAppendStatic(physContactPair **const __RESTRICT__ array){
	return memDLinkAppend(&__g_PhysicsContactPairResourceArray, (void **)array);
}
__HINT_INLINE__ physContactPair *modulePhysicsContactPairAppend(physContactPair **const __RESTRICT__ array){
	physContactPair *r = modulePhysicsContactPairAppendStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CONTACT_PAIR_SIZE,
				RESOURCE_DEFAULT_CONTACT_PAIR_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsContactPairResourceArray, memory, RESOURCE_DEFAULT_CONTACT_PAIR_SIZE, RESOURCE_DEFAULT_CONTACT_PAIR_NUM)){
			r = modulePhysicsContactPairAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physContactPair *modulePhysicsContactPairInsertAfterStatic(physContactPair **const __RESTRICT__ array, physContactPair *const __RESTRICT__ resource){
	return memDLinkInsertAfter(&__g_PhysicsContactPairResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physContactPair *modulePhysicsContactPairInsertAfter(physContactPair **const __RESTRICT__ array, physContactPair *const __RESTRICT__ resource){
	physContactPair *r = modulePhysicsContactPairInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CONTACT_PAIR_SIZE,
				RESOURCE_DEFAULT_CONTACT_PAIR_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsContactPairResourceArray, memory, RESOURCE_DEFAULT_CONTACT_PAIR_SIZE, RESOURCE_DEFAULT_CONTACT_PAIR_NUM)){
			r = modulePhysicsContactPairInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physContactPair *modulePhysicsContactPairPrependStatic(physContactPair **const __RESTRICT__ array){
	return memDLinkPrepend(&__g_PhysicsContactPairResourceArray, (void **)array);
}
__HINT_INLINE__ physContactPair *modulePhysicsContactPairPrepend(physContactPair **const __RESTRICT__ array){
	physContactPair *r = modulePhysicsContactPairPrependStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CONTACT_PAIR_SIZE,
				RESOURCE_DEFAULT_CONTACT_PAIR_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsContactPairResourceArray, memory, RESOURCE_DEFAULT_CONTACT_PAIR_SIZE, RESOURCE_DEFAULT_CONTACT_PAIR_NUM)){
			r = modulePhysicsContactPairPrependStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physContactPair *modulePhysicsContactPairInsertBeforeStatic(physContactPair **const __RESTRICT__ array, physContactPair *const __RESTRICT__ resource){
	return memDLinkInsertBefore(&__g_PhysicsContactPairResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physContactPair *modulePhysicsContactPairInsertBefore(physContactPair **const __RESTRICT__ array, physContactPair *const __RESTRICT__ resource){
	physContactPair *r = modulePhysicsContactPairInsertBeforeStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CONTACT_PAIR_SIZE,
				RESOURCE_DEFAULT_CONTACT_PAIR_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsContactPairResourceArray, memory, RESOURCE_DEFAULT_CONTACT_PAIR_SIZE, RESOURCE_DEFAULT_CONTACT_PAIR_NUM)){
			r = modulePhysicsContactPairInsertBeforeStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physContactPair *modulePhysicsContactPairNext(const physContactPair *const __RESTRICT__ i){
	return (physContactPair *)memDLinkDataGetNext(i);
}
__HINT_INLINE__ physContactPair *modulePhysicsContactPairPrev(const physContactPair *const __RESTRICT__ i){
	return (physContactPair *)memDLinkDataGetPrev(i);
}
__HINT_INLINE__ void modulePhysicsContactPairFree(physContactPair **const __RESTRICT__ array, physContactPair *const __RESTRICT__ resource){
	physContactPairDelete(resource);
	memDLinkFree(&__g_PhysicsContactPairResourceArray, (void **)array, (void *)resource);
}
void modulePhysicsContactPairFreeArray(physContactPair **const __RESTRICT__ array){
	physContactPair *resource = *array;
	while(resource != NULL){
		modulePhysicsContactPairFree(array, resource);
		resource = *array;
	}
}
void modulePhysicsContactPairClear(){

	MEMORY_DLINK_LOOP_BEGIN(__g_PhysicsContactPairResourceArray, i, physContactPair *);

		modulePhysicsContactPairFree(NULL, i);
		memDLinkDataSetFlags(i, MEMORY_DLINK_BLOCK_INVALID);

	MEMORY_DLINK_LOOP_INACTIVE_CASE(i);

		memDLinkDataSetFlags(i, MEMORY_DLINK_BLOCK_INVALID);

	MEMORY_DLINK_LOOP_END(__g_PhysicsContactPairResourceArray, i, return;);

}

__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairAppendStatic(physSeparationPair **const __RESTRICT__ array){
	return memDLinkAppend(&__g_PhysicsSeparationPairResourceArray, (void **)array);
}
__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairAppend(physSeparationPair **const __RESTRICT__ array){
	physSeparationPair *r = modulePhysicsSeparationPairAppendStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE,
				RESOURCE_DEFAULT_SEPARATION_PAIR_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsSeparationPairResourceArray, memory, RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE, RESOURCE_DEFAULT_SEPARATION_PAIR_NUM)){
			r = modulePhysicsSeparationPairAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairInsertAfterStatic(physSeparationPair **const __RESTRICT__ array, physSeparationPair *const __RESTRICT__ resource){
	return memDLinkInsertAfter(&__g_PhysicsSeparationPairResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairInsertAfter(physSeparationPair **const __RESTRICT__ array, physSeparationPair *const __RESTRICT__ resource){
	physSeparationPair *r = modulePhysicsSeparationPairInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE,
				RESOURCE_DEFAULT_SEPARATION_PAIR_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsSeparationPairResourceArray, memory, RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE, RESOURCE_DEFAULT_SEPARATION_PAIR_NUM)){
			r = modulePhysicsSeparationPairInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairPrependStatic(physSeparationPair **const __RESTRICT__ array){
	return memDLinkPrepend(&__g_PhysicsSeparationPairResourceArray, (void **)array);
}
__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairPrepend(physSeparationPair **const __RESTRICT__ array){
	physSeparationPair *r = modulePhysicsSeparationPairPrependStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE,
				RESOURCE_DEFAULT_SEPARATION_PAIR_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsSeparationPairResourceArray, memory, RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE, RESOURCE_DEFAULT_SEPARATION_PAIR_NUM)){
			r = modulePhysicsSeparationPairPrependStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairInsertBeforeStatic(physSeparationPair **const __RESTRICT__ array, physSeparationPair *const __RESTRICT__ resource){
	return memDLinkInsertBefore(&__g_PhysicsSeparationPairResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairInsertBefore(physSeparationPair **const __RESTRICT__ array, physSeparationPair *const __RESTRICT__ resource){
	physSeparationPair *r = modulePhysicsSeparationPairInsertBeforeStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memDLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE,
				RESOURCE_DEFAULT_SEPARATION_PAIR_NUM
			)
		);
		if(memDLinkExtend(&__g_PhysicsSeparationPairResourceArray, memory, RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE, RESOURCE_DEFAULT_SEPARATION_PAIR_NUM)){
			r = modulePhysicsSeparationPairInsertBeforeStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairNext(const physSeparationPair *const __RESTRICT__ i){
	return (physSeparationPair *)memDLinkDataGetNext(i);
}
__HINT_INLINE__ physSeparationPair *modulePhysicsSeparationPairPrev(const physSeparationPair *const __RESTRICT__ i){
	return (physSeparationPair *)memDLinkDataGetPrev(i);
}
__HINT_INLINE__ void modulePhysicsSeparationPairFree(physSeparationPair **const __RESTRICT__ array, physSeparationPair *const __RESTRICT__ resource){
	physSeparationPairDelete(resource);
	memDLinkFree(&__g_PhysicsSeparationPairResourceArray, (void **)array, (void *)resource);
}
void modulePhysicsSeparationPairFreeArray(physSeparationPair **const __RESTRICT__ array){
	physSeparationPair *resource = *array;
	while(resource != NULL){
		modulePhysicsSeparationPairFree(array, resource);
		resource = *array;
	}
}
void modulePhysicsSeparationPairClear(){

	MEMORY_DLINK_LOOP_BEGIN(__g_PhysicsSeparationPairResourceArray, i, physSeparationPair *);

		modulePhysicsSeparationPairFree(NULL, i);
		memDLinkDataSetFlags(i, MEMORY_DLINK_BLOCK_INVALID);

	MEMORY_DLINK_LOOP_INACTIVE_CASE(i);

		memDLinkDataSetFlags(i, MEMORY_DLINK_BLOCK_INVALID);

	MEMORY_DLINK_LOOP_END(__g_PhysicsSeparationPairResourceArray, i, return;);

}

#endif

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
