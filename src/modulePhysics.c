#include "modulePhysics.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "colliderConvexMesh.h"
#include "inline.h"

memorySLink __PhysicsRigidBodyResourceArray;           // Contains physRigidBodies.
memorySLink __PhysicsRigidBodyInstanceResourceArray;   // Contains physRBInstances.
memorySLink __PhysicsColliderResourceArray;            // Contains physColliders.
memorySLink __PhysicsCollisionInstanceResourceArray;   // Contains physCollisionInfos.
memorySLink __PhysicsConstraintResourceArray;          // Contains physConstraints.

return_t modulePhysicsResourcesInit(){
	void *memory = memAllocate(
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
			RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE,
			RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM
		)
	);
	if(memSLinkCreate(&__PhysicsRigidBodyInstanceResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE, RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM) == NULL){
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
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_COLLISION_SIZE,
			RESOURCE_DEFAULT_COLLISION_NUM
		)
	);
	if(memSLinkCreate(&__PhysicsCollisionInstanceResourceArray, memory, RESOURCE_DEFAULT_COLLISION_SIZE, RESOURCE_DEFAULT_COLLISION_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_CONSTRAINT_SIZE,
			RESOURCE_DEFAULT_CONSTRAINT_NUM
		)
	);
	if(memSLinkCreate(&__PhysicsConstraintResourceArray, memory, RESOURCE_DEFAULT_CONSTRAINT_SIZE, RESOURCE_DEFAULT_CONSTRAINT_NUM) == NULL){
		return -1;
	}
	return 1;
}
void modulePhysicsResourcesReset(){
	memoryRegion *region;
	modulePhysicsRigidBodyClear();
	region = __PhysicsRigidBodyResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsRigidBodyResourceArray.region->next = NULL;
	modulePhysicsRigidBodyInstanceClear();
	region = __PhysicsRigidBodyInstanceResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsRigidBodyInstanceResourceArray.region->next = NULL;
	modulePhysicsColliderClear();
	region = __PhysicsColliderResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsColliderResourceArray.region->next = NULL;
	modulePhysicsCollisionInstanceClear();
	region = __PhysicsCollisionInstanceResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsCollisionInstanceResourceArray.region->next = NULL;
	modulePhysicsConstraintClear();
	region = __PhysicsConstraintResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsConstraintResourceArray.region->next = NULL;
}
void modulePhysicsResourcesDelete(){
	memoryRegion *region;
	modulePhysicsRigidBodyClear();
	region = __PhysicsRigidBodyResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsRigidBodyInstanceClear();
	region = __PhysicsRigidBodyInstanceResourceArray.region;
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
	modulePhysicsCollisionInstanceClear();
	region = __PhysicsCollisionInstanceResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsConstraintClear();
	region = __PhysicsConstraintResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
}

__FORCE_INLINE__ physRigidBody *modulePhysicsRigidBodyAppendStatic(physRigidBody **const restrict array){
	return memSLinkAppend(&__PhysicsRigidBodyResourceArray, (const void **)array);
}
__FORCE_INLINE__ physRigidBody *modulePhysicsRigidBodyAppend(physRigidBody **const restrict array){
	physRigidBody *r = memSLinkAppend(&__PhysicsRigidBodyResourceArray, (const void **)array);
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
			r = memSLinkAppend(&__PhysicsRigidBodyResourceArray, (const void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertAfterStatic(physRigidBody *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsRigidBodyResourceArray, resource);
}
__FORCE_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertAfter(physRigidBody *const restrict resource){
	physRigidBody *r = memSLinkInsertAfter(&__PhysicsRigidBodyResourceArray, resource);
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
			r = memSLinkInsertAfter(&__PhysicsRigidBodyResourceArray, resource);
		}
	}
	return r;
}
__FORCE_INLINE__ physRigidBody *modulePhysicsRigidBodyNext(const physRigidBody *const restrict i){
	return (physRigidBody *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void modulePhysicsRigidBodyFree(physRigidBody **const restrict array, physRigidBody *const restrict resource, physRigidBody *const restrict previous){
	physRigidBodyDelete(resource);
	memSLinkFree(&__PhysicsRigidBodyResourceArray, (const void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsRigidBodyFreeArray(physRigidBody **const restrict array){
	physRigidBody *resource = *array;
	while(resource != NULL){
		physRigidBodyDelete(resource);
		memSLinkFree(&__PhysicsRigidBodyResourceArray, (const void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsRigidBodyClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsRigidBodyResourceArray, i, physRigidBody *);

		modulePhysicsRigidBodyFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__PhysicsRigidBodyResourceArray, i, return;);

}

__FORCE_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceAppendStatic(physRBInstance **const restrict array){
	return memSLinkAppend(&__PhysicsRigidBodyInstanceResourceArray, (const void **)array);
}
__FORCE_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceAppend(physRBInstance **const restrict array){
	physRBInstance *r = memSLinkAppend(&__PhysicsRigidBodyInstanceResourceArray, (const void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE,
				RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsRigidBodyInstanceResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE, RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM)){
			r = memSLinkAppend(&__PhysicsRigidBodyInstanceResourceArray, (const void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceInsertAfterStatic(physRBInstance *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsRigidBodyInstanceResourceArray, resource);
}
__FORCE_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceInsertAfter(physRBInstance *resource){
	physRBInstance *r = memSLinkInsertAfter(&__PhysicsRigidBodyInstanceResourceArray, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE,
				RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsRigidBodyInstanceResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE, RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM)){
			r = memSLinkInsertAfter(&__PhysicsRigidBodyInstanceResourceArray, resource);
		}
	}
	return r;
}
__FORCE_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceNext(const physRBInstance *const restrict i){
	return (physRBInstance *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void modulePhysicsRigidBodyInstanceFree(physRBInstance **const restrict array, physRBInstance *const restrict resource, physRBInstance *const restrict previous){
	physRBIDelete(resource);
	memSLinkFree(&__PhysicsRigidBodyInstanceResourceArray, (const void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsRigidBodyInstanceFreeArray(physRBInstance **const restrict array){
	physRBInstance *resource = *array;
	while(resource != NULL){
		physRBIDelete(resource);
		memSLinkFree(&__PhysicsRigidBodyInstanceResourceArray, (const void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsRigidBodyInstanceClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsRigidBodyInstanceResourceArray, i, physRBInstance *);

		modulePhysicsRigidBodyInstanceFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__PhysicsRigidBodyInstanceResourceArray, i, return;);

}

__FORCE_INLINE__ physCollider *modulePhysicsColliderAppendStatic(physCollider **const restrict array){
	return memSLinkAppend(&__PhysicsColliderResourceArray, (const void **)array);
}
__FORCE_INLINE__ physCollider *modulePhysicsColliderAppend(physCollider **const restrict array){
	physCollider *r = memSLinkAppend(&__PhysicsColliderResourceArray, (const void **)array);
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
			r = memSLinkAppend(&__PhysicsColliderResourceArray, (const void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ physCollider *modulePhysicsColliderInsertAfterStatic(physCollider *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsColliderResourceArray, resource);
}
__FORCE_INLINE__ physCollider *modulePhysicsColliderInsertAfter(physCollider *const restrict resource){
	physCollider *r = memSLinkInsertAfter(&__PhysicsColliderResourceArray, resource);
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
			r = memSLinkInsertAfter(&__PhysicsColliderResourceArray, resource);
		}
	}
	return r;
}
__FORCE_INLINE__ physCollider *modulePhysicsColliderNext(const physCollider *const restrict i){
	return (physCollider *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void modulePhysicsColliderFree(physCollider **const restrict array, physCollider *const restrict resource, physCollider *const restrict previous){
	physColliderDelete(resource);
	memSLinkFree(&__PhysicsColliderResourceArray, (const void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsColliderFreeArray(physCollider **const restrict array){
	physCollider *resource = *array;
	while(resource != NULL){
		physColliderDelete(resource);
		memSLinkFree(&__PhysicsColliderResourceArray, (const void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsColliderRBIFreeArray(physCollider **const restrict array){
	physCollider *resource = *array;
	cMesh *cHull;
	while(resource != NULL){
		cHull = (cMesh *)&resource->c.hull;
		memFree(cHull->vertices);
		memFree(cHull->normals);
		memSLinkFree(&__PhysicsColliderResourceArray, (const void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsColliderClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsColliderResourceArray, i, physCollider *);

		modulePhysicsColliderFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__PhysicsColliderResourceArray, i, return;);

}

__FORCE_INLINE__ physCollisionInfo *modulePhysicsCollisionInstanceAppendStatic(physCollisionInfo **const restrict array){
	return memSLinkAppend(&__PhysicsCollisionInstanceResourceArray, (const void **)array);
}
__FORCE_INLINE__ physCollisionInfo *modulePhysicsCollisionInstanceAppend(physCollisionInfo **const restrict array){
	physCollisionInfo *r = memSLinkAppend(&__PhysicsCollisionInstanceResourceArray, (const void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_COLLISION_SIZE,
				RESOURCE_DEFAULT_COLLISION_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsCollisionInstanceResourceArray, memory, RESOURCE_DEFAULT_COLLISION_SIZE, RESOURCE_DEFAULT_COLLISION_NUM)){
			r = memSLinkAppend(&__PhysicsCollisionInstanceResourceArray, (const void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ physCollisionInfo *modulePhysicsCollisionInstanceInsertAfterStatic(physCollisionInfo *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsCollisionInstanceResourceArray, resource);
}
__FORCE_INLINE__ physCollisionInfo *modulePhysicsCollisionInstanceInsertAfter(physCollisionInfo *const restrict resource){
	physCollisionInfo *r = memSLinkInsertAfter(&__PhysicsCollisionInstanceResourceArray, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_COLLISION_SIZE,
				RESOURCE_DEFAULT_COLLISION_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsCollisionInstanceResourceArray, memory, RESOURCE_DEFAULT_COLLISION_SIZE, RESOURCE_DEFAULT_COLLISION_NUM)){
			r = memSLinkInsertAfter(&__PhysicsCollisionInstanceResourceArray, resource);
		}
	}
	return r;
}
__FORCE_INLINE__ physCollisionInfo *modulePhysicsCollisionInstanceNext(const physCollisionInfo *const restrict i){
	return (physCollisionInfo *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void modulePhysicsCollisionInstanceFree(physCollisionInfo **const restrict array, physCollisionInfo *const restrict resource, physCollisionInfo *const restrict previous){
	memSLinkFree(&__PhysicsCollisionInstanceResourceArray, (const void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsCollisionInstanceFreeArray(physCollisionInfo **const restrict array){
	physCollisionInfo *resource = *array;
	while(resource != NULL){
		memSLinkFree(&__PhysicsCollisionInstanceResourceArray, (const void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsCollisionInstanceClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsCollisionInstanceResourceArray, i, physCollisionInfo *);

		modulePhysicsCollisionInstanceFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__PhysicsCollisionInstanceResourceArray, i, return;);

}

__FORCE_INLINE__ physConstraint *modulePhysicsConstraintAppendStatic(physConstraint **const restrict array){
	return memSLinkAppend(&__PhysicsConstraintResourceArray, (const void **)array);
}
__FORCE_INLINE__ physConstraint *modulePhysicsConstraintAppend(physConstraint **const restrict array){
	physConstraint *r = memSLinkAppend(&__PhysicsConstraintResourceArray, (const void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CONSTRAINT_SIZE,
				RESOURCE_DEFAULT_CONSTRAINT_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsConstraintResourceArray, memory, RESOURCE_DEFAULT_CONSTRAINT_SIZE, RESOURCE_DEFAULT_CONSTRAINT_NUM)){
			r = memSLinkAppend(&__PhysicsConstraintResourceArray, (const void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ physConstraint *modulePhysicsConstraintInsertAfterStatic(physConstraint *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsConstraintResourceArray, resource);
}
__FORCE_INLINE__ physConstraint *modulePhysicsConstraintInsertAfter(physConstraint *const restrict resource){
	physConstraint *r = memSLinkInsertAfter(&__PhysicsConstraintResourceArray, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CONSTRAINT_SIZE,
				RESOURCE_DEFAULT_CONSTRAINT_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsConstraintResourceArray, memory, RESOURCE_DEFAULT_CONSTRAINT_SIZE, RESOURCE_DEFAULT_CONSTRAINT_NUM)){
			r = memSLinkInsertAfter(&__PhysicsConstraintResourceArray, resource);
		}
	}
	return r;
}
__FORCE_INLINE__ physConstraint *modulePhysicsConstraintNext(const physConstraint *const restrict i){
	return (physConstraint *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void modulePhysicsConstraintFree(physConstraint **const restrict array, physConstraint *const restrict resource, physConstraint *const restrict previous){
	memSLinkFree(&__PhysicsConstraintResourceArray, (const void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsConstraintFreeArray(physConstraint **const restrict array){
	physConstraint *resource = *array;
	while(resource != NULL){
		memSLinkFree(&__PhysicsConstraintResourceArray, (const void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsConstraintClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsConstraintResourceArray, i, physConstraint *);

		modulePhysicsConstraintFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__PhysicsConstraintResourceArray, i, return;);

}

void modulePhysicsSolve(){
	///
}