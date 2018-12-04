#include "modulePhysics.h"
#include "moduleSettings.h"
#include "memoryManager.h"
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

__FORCE_INLINE__ physRigidBody *modulePhysicsRigidBodyAppendStatic(physRigidBody **array){
	return memSLinkAppend(&__PhysicsRigidBodyResourceArray, (void **)array);
}
__FORCE_INLINE__ physRigidBody *modulePhysicsRigidBodyAppend(physRigidBody **array){
	physRigidBody *r = memSLinkAppend(&__PhysicsRigidBodyResourceArray, (void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RIGID_BODY_SIZE,
				RESOURCE_DEFAULT_RIGID_BODY_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsRigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM)){
			r = memSLinkAppend(&__PhysicsRigidBodyResourceArray, (void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertAfterStatic(physRigidBody *resource){
	return memSLinkInsertAfter(&__PhysicsRigidBodyResourceArray, resource);
}
__FORCE_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertAfter(physRigidBody *resource){
	physRigidBody *r = memSLinkInsertAfter(&__PhysicsRigidBodyResourceArray, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
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
__FORCE_INLINE__ physRigidBody *modulePhysicsRigidBodyNext(physRigidBody *i){
	return (physRigidBody *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void modulePhysicsRigidBodyFree(physRigidBody **array, physRigidBody *resource, physRigidBody *previous){
	physRigidBodyDelete(resource);
	memSLinkFree(&__PhysicsRigidBodyResourceArray, (void **)array, (void *)resource, (void *)previous);
}
void modulePhysicsRigidBodyFreeArray(physRigidBody **array){
	physRigidBody *resource = *array;
	while(resource != NULL){
		physRigidBodyDelete(resource);
		memSLinkFree(&__PhysicsRigidBodyResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsRigidBodyClear(){

	memoryRegion *region = __PhysicsRigidBodyResourceArray.region;
	physRigidBody *i;
	do {
		i = memSLinkFirst(region);
		while(i < (physRigidBody *)memAllocatorEnd(region)){
			const byte_t flag = memSLinkBlockStatus(i);
			if(flag == MEMORY_SLINK_BLOCK_ACTIVE){

				modulePhysicsRigidBodyFree(NULL, i, NULL);

			}else if(flag == MEMORY_SLINK_BLOCK_INVALID){
				return;
			}
			i = memSLinkBlockNext(__PhysicsRigidBodyResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}

__FORCE_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceAppendStatic(physRBInstance **array){
	return memSLinkAppend(&__PhysicsRigidBodyInstanceResourceArray, (void **)array);
}
__FORCE_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceAppend(physRBInstance **array){
	physRBInstance *r = memSLinkAppend(&__PhysicsRigidBodyInstanceResourceArray, (void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE,
				RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsRigidBodyInstanceResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE, RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM)){
			r = memSLinkAppend(&__PhysicsRigidBodyInstanceResourceArray, (void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceInsertAfterStatic(physRBInstance *resource){
	return memSLinkInsertAfter(&__PhysicsRigidBodyInstanceResourceArray, resource);
}
__FORCE_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceInsertAfter(physRBInstance *resource){
	physRBInstance *r = memSLinkInsertAfter(&__PhysicsRigidBodyInstanceResourceArray, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
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
__FORCE_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceNext(physRBInstance *i){
	return (physRBInstance *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void modulePhysicsRigidBodyInstanceFree(physRBInstance **array, physRBInstance *resource, physRBInstance *previous){
	physRBIDelete(resource);
	memSLinkFree(&__PhysicsRigidBodyInstanceResourceArray, (void **)array, (void *)resource, (void *)previous);
}
void modulePhysicsRigidBodyInstanceFreeArray(physRBInstance **array){
	physRBInstance *resource = *array;
	while(resource != NULL){
		physRBIDelete(resource);
		memSLinkFree(&__PhysicsRigidBodyInstanceResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsRigidBodyInstanceClear(){

	memoryRegion *region = __PhysicsRigidBodyInstanceResourceArray.region;
	physRBInstance *i;
	do {
		i = memSLinkFirst(region);
		while(i < (physRBInstance *)memAllocatorEnd(region)){
			const byte_t flag = memSLinkBlockStatus(i);
			if(flag == MEMORY_SLINK_BLOCK_ACTIVE){

				modulePhysicsRigidBodyInstanceFree(NULL, i, NULL);

			}else if(flag == MEMORY_SLINK_BLOCK_INVALID){
				return;
			}
			i = memSLinkBlockNext(__PhysicsRigidBodyInstanceResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}

__FORCE_INLINE__ physCollider *modulePhysicsColliderAppendStatic(physCollider **array){
	return memSLinkAppend(&__PhysicsColliderResourceArray, (void **)array);
}
__FORCE_INLINE__ physCollider *modulePhysicsColliderAppend(physCollider **array){
	physCollider *r = memSLinkAppend(&__PhysicsColliderResourceArray, (void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_COLLIDER_SIZE,
				RESOURCE_DEFAULT_COLLIDER_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsColliderResourceArray, memory, RESOURCE_DEFAULT_COLLIDER_SIZE, RESOURCE_DEFAULT_COLLIDER_NUM)){
			r = memSLinkAppend(&__PhysicsColliderResourceArray, (void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ physCollider *modulePhysicsColliderInsertAfterStatic(physCollider *resource){
	return memSLinkInsertAfter(&__PhysicsColliderResourceArray, resource);
}
__FORCE_INLINE__ physCollider *modulePhysicsColliderInsertAfter(physCollider *resource){
	physCollider *r = memSLinkInsertAfter(&__PhysicsColliderResourceArray, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
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
__FORCE_INLINE__ physCollider *modulePhysicsColliderNext(physCollider *i){
	return (physCollider *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void modulePhysicsColliderFree(physCollider **array, physCollider *resource, physCollider *previous){
	physColliderDelete(resource);
	memSLinkFree(&__PhysicsColliderResourceArray, (void **)array, (void *)resource, (void *)previous);
}
void modulePhysicsColliderFreeArray(physCollider **array){
	physCollider *resource = *array;
	while(resource != NULL){
		physColliderDelete(resource);
		memSLinkFree(&__PhysicsColliderResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsColliderRBIFreeArray(physCollider **array){
	physCollider *resource = *array;
	cMesh *cHull;
	while(resource != NULL){
		cHull = (cMesh *)&resource->c.hull;
		memFree(cHull->vertices);
		memFree(cHull->normals);
		memSLinkFree(&__PhysicsColliderResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsColliderClear(){

	memoryRegion *region = __PhysicsColliderResourceArray.region;
	physCollider *i;
	do {
		i = memSLinkFirst(region);
		while(i < (physCollider *)memAllocatorEnd(region)){
			const byte_t flag = memSLinkBlockStatus(i);
			if(flag == MEMORY_SLINK_BLOCK_ACTIVE){

				modulePhysicsColliderFree(NULL, i, NULL);

			}else if(flag == MEMORY_SLINK_BLOCK_INVALID){
				return;
			}
			i = memSLinkBlockNext(__PhysicsColliderResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}

__FORCE_INLINE__ physCollisionInfo *modulePhysicsCollisionInstanceAppendStatic(physCollisionInfo **array){
	return memSLinkAppend(&__PhysicsCollisionInstanceResourceArray, (void **)array);
}
__FORCE_INLINE__ physCollisionInfo *modulePhysicsCollisionInstanceAppend(physCollisionInfo **array){
	physCollisionInfo *r = memSLinkAppend(&__PhysicsCollisionInstanceResourceArray, (void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_COLLISION_SIZE,
				RESOURCE_DEFAULT_COLLISION_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsCollisionInstanceResourceArray, memory, RESOURCE_DEFAULT_COLLISION_SIZE, RESOURCE_DEFAULT_COLLISION_NUM)){
			r = memSLinkAppend(&__PhysicsCollisionInstanceResourceArray, (void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ physCollisionInfo *modulePhysicsCollisionInstanceInsertAfterStatic(physCollisionInfo *resource){
	return memSLinkInsertAfter(&__PhysicsCollisionInstanceResourceArray, resource);
}
__FORCE_INLINE__ physCollisionInfo *modulePhysicsCollisionInstanceInsertAfter(physCollisionInfo *resource){
	physCollisionInfo *r = memSLinkInsertAfter(&__PhysicsCollisionInstanceResourceArray, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
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
__FORCE_INLINE__ physCollisionInfo *modulePhysicsCollisionInstanceNext(physCollisionInfo *i){
	return (physCollisionInfo *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void modulePhysicsCollisionInstanceFree(physCollisionInfo **array, physCollisionInfo *resource, physCollisionInfo *previous){
	memSLinkFree(&__PhysicsCollisionInstanceResourceArray, (void **)array, (void *)resource, (void *)previous);
}
void modulePhysicsCollisionInstanceFreeArray(physCollisionInfo **array){
	physCollisionInfo *resource = *array;
	while(resource != NULL){
		memSLinkFree(&__PhysicsCollisionInstanceResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsCollisionInstanceClear(){

	memoryRegion *region = __PhysicsCollisionInstanceResourceArray.region;
	physCollisionInfo *i;
	do {
		i = memSLinkFirst(region);
		while(i < (physCollisionInfo *)memAllocatorEnd(region)){
			const byte_t flag = memSLinkBlockStatus(i);
			if(flag == MEMORY_SLINK_BLOCK_ACTIVE){

				modulePhysicsCollisionInstanceFree(NULL, i, NULL);

			}else if(flag == MEMORY_SLINK_BLOCK_INVALID){
				return;
			}
			i = memSLinkBlockNext(__PhysicsCollisionInstanceResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}

__FORCE_INLINE__ physConstraint *modulePhysicsConstraintAppendStatic(physConstraint **array){
	return memSLinkAppend(&__PhysicsConstraintResourceArray, (void **)array);
}
__FORCE_INLINE__ physConstraint *modulePhysicsConstraintAppend(physConstraint **array){
	physConstraint *r = memSLinkAppend(&__PhysicsConstraintResourceArray, (void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CONSTRAINT_SIZE,
				RESOURCE_DEFAULT_CONSTRAINT_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsConstraintResourceArray, memory, RESOURCE_DEFAULT_CONSTRAINT_SIZE, RESOURCE_DEFAULT_CONSTRAINT_NUM)){
			r = memSLinkAppend(&__PhysicsConstraintResourceArray, (void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ physConstraint *modulePhysicsConstraintInsertAfterStatic(physConstraint *resource){
	return memSLinkInsertAfter(&__PhysicsConstraintResourceArray, resource);
}
__FORCE_INLINE__ physConstraint *modulePhysicsConstraintInsertAfter(physConstraint *resource){
	physConstraint *r = memSLinkInsertAfter(&__PhysicsConstraintResourceArray, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
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
__FORCE_INLINE__ physConstraint *modulePhysicsConstraintNext(physConstraint *i){
	return (physConstraint *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void modulePhysicsConstraintFree(physConstraint **array, physConstraint *resource, physConstraint *previous){
	memSLinkFree(&__PhysicsConstraintResourceArray, (void **)array, (void *)resource, (void *)previous);
}
void modulePhysicsConstraintFreeArray(physConstraint **array){
	physConstraint *resource = *array;
	while(resource != NULL){
		memSLinkFree(&__PhysicsConstraintResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsConstraintClear(){

	memoryRegion *region = __PhysicsConstraintResourceArray.region;
	physConstraint *i;
	do {
		i = memSLinkFirst(region);
		while(i < (physConstraint *)memAllocatorEnd(region)){
			const byte_t flag = memSLinkBlockStatus(i);
			if(flag == MEMORY_SLINK_BLOCK_ACTIVE){

				modulePhysicsConstraintFree(NULL, i, NULL);

			}else if(flag == MEMORY_SLINK_BLOCK_INVALID){
				return;
			}
			i = memSLinkBlockNext(__PhysicsConstraintResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}