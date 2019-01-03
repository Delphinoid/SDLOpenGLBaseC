#include "modulePhysics.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "colliderConvexMesh.h"
#include "inline.h"

void *__PhysicsContactResourceFreeBlock;
memoryRegion *__PhysicsContactResourceFreeRegion;

memorySLink __PhysicsRigidBodyResourceArray;          // Contains physRigidBodies.
memorySLink __PhysicsRigidBodyInstanceResourceArray;  // Contains physRBInstances.
memorySLink __PhysicsColliderResourceArray;           // Contains physColliders.
memoryArray __PhysicsContactResourceArray;            // Contains physContacts.
memorySLink __PhysicsSeparationResourceArray;         // Contains physSeparations.
memorySLink __PhysicsConstraintResourceArray;         // Contains physConstraints.

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
		memArrayAllocationSize(
			NULL,
			RESOURCE_DEFAULT_CONTACT_SIZE,
			RESOURCE_DEFAULT_CONTACT_NUM
		)
	);
	if(memArrayCreate(&__PhysicsContactResourceArray, memory, RESOURCE_DEFAULT_CONTACT_SIZE, RESOURCE_DEFAULT_CONTACT_NUM) == NULL){
		return -1;
	}
	modulePhysicsContactClear();
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SEPARATION_SIZE,
			RESOURCE_DEFAULT_SEPARATION_NUM
		)
	);
	if(memSLinkCreate(&__PhysicsSeparationResourceArray, memory, RESOURCE_DEFAULT_SEPARATION_SIZE, RESOURCE_DEFAULT_SEPARATION_NUM) == NULL){
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
	modulePhysicsSeparationClear();
	region = __PhysicsSeparationResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsSeparationResourceArray.region->next = NULL;
	modulePhysicsContactClear();
	region = __PhysicsContactResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__PhysicsContactResourceArray.region->next = NULL;
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
	modulePhysicsContactClear();
	region = __PhysicsContactResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	modulePhysicsSeparationClear();
	region = __PhysicsSeparationResourceArray.region;
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

__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyAppendStatic(physRigidBody **const restrict array){
	return memSLinkAppend(&__PhysicsRigidBodyResourceArray, (void **)array);
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyAppend(physRigidBody **const restrict array){
	physRigidBody *r = memSLinkAppend(&__PhysicsRigidBodyResourceArray, (void **)array);
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
			r = memSLinkAppend(&__PhysicsRigidBodyResourceArray, (void **)array);
		}
	}
	return r;
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertAfterStatic(physRigidBody **const restrict array, physRigidBody *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsRigidBodyResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physRigidBody *modulePhysicsRigidBodyInsertAfter(physRigidBody **const restrict array, physRigidBody *const restrict resource){
	physRigidBody *r = memSLinkInsertAfter(&__PhysicsRigidBodyResourceArray, (void **)array, (void *)resource);
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
			r = memSLinkInsertAfter(&__PhysicsRigidBodyResourceArray, (void **)array, (void *)resource);
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
		physRigidBodyDelete(resource);
		memSLinkFree(&__PhysicsRigidBodyResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsRigidBodyClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsRigidBodyResourceArray, i, physRigidBody *);

		modulePhysicsRigidBodyFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__PhysicsRigidBodyResourceArray, i, return;);

}

__HINT_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceAppendStatic(physRBInstance **const restrict array){
	return memSLinkAppend(&__PhysicsRigidBodyInstanceResourceArray, (void **)array);
}
__HINT_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceAppend(physRBInstance **const restrict array){
	physRBInstance *r = memSLinkAppend(&__PhysicsRigidBodyInstanceResourceArray, (void **)array);
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
			r = memSLinkAppend(&__PhysicsRigidBodyInstanceResourceArray, (void **)array);
		}
	}
	return r;
}
__HINT_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceInsertAfterStatic(physRBInstance **const restrict array, physRBInstance *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsRigidBodyInstanceResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceInsertAfter(physRBInstance **const restrict array, physRBInstance *const restrict resource){
	physRBInstance *r = memSLinkInsertAfter(&__PhysicsRigidBodyInstanceResourceArray, (void **)array, (void *)resource);
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
			r = memSLinkInsertAfter(&__PhysicsRigidBodyInstanceResourceArray, (void **)array, (void *)resource);
		}
	}
	return r;
}
__HINT_INLINE__ physRBInstance *modulePhysicsRigidBodyInstanceNext(const physRBInstance *const restrict i){
	return (physRBInstance *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void modulePhysicsRigidBodyInstanceFree(physRBInstance **const restrict array, physRBInstance *const restrict resource, const physRBInstance *const restrict previous){
	physRBIDelete(resource);
	memSLinkFree(&__PhysicsRigidBodyInstanceResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsRigidBodyInstanceFreeArray(physRBInstance **const restrict array){
	physRBInstance *resource = *array;
	while(resource != NULL){
		physRBIDelete(resource);
		memSLinkFree(&__PhysicsRigidBodyInstanceResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsRigidBodyInstanceClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsRigidBodyInstanceResourceArray, i, physRBInstance *);

		modulePhysicsRigidBodyInstanceFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__PhysicsRigidBodyInstanceResourceArray, i, return;);

}

__HINT_INLINE__ physCollider *modulePhysicsColliderAppendStatic(physCollider **const restrict array){
	return memSLinkAppend(&__PhysicsColliderResourceArray, (void **)array);
}
__HINT_INLINE__ physCollider *modulePhysicsColliderAppend(physCollider **const restrict array){
	physCollider *r = memSLinkAppend(&__PhysicsColliderResourceArray, (void **)array);
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
			r = memSLinkAppend(&__PhysicsColliderResourceArray, (void **)array);
		}
	}
	return r;
}
__HINT_INLINE__ physCollider *modulePhysicsColliderInsertAfterStatic(physCollider **const restrict array, physCollider *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsColliderResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physCollider *modulePhysicsColliderInsertAfter(physCollider **const restrict array, physCollider *const restrict resource){
	physCollider *r = memSLinkInsertAfter(&__PhysicsColliderResourceArray, (void **)array, (void *)resource);
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
			r = memSLinkInsertAfter(&__PhysicsColliderResourceArray, (void **)array, (void *)resource);
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
		physColliderDelete(resource);
		memSLinkFree(&__PhysicsColliderResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsColliderRBIFreeArray(physCollider **const restrict array){
	physCollider *resource = *array;
	cMesh *cHull;
	while(resource != NULL){
		cHull = (cMesh *)&resource->c.data;
		memFree(cHull->vertices);
		memFree(cHull->normals);
		memSLinkFree(&__PhysicsColliderResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsColliderClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsColliderResourceArray, i, physCollider *);

		modulePhysicsColliderFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__PhysicsColliderResourceArray, i, return;);

}

__HINT_INLINE__ physContact *modulePhysicsContactAllocateStatic(){
	return memArrayPushFast(&__PhysicsContactResourceArray, &__PhysicsContactResourceFreeBlock, &__PhysicsContactResourceFreeRegion);
}
__HINT_INLINE__ physContact *modulePhysicsContactAllocate(){
	physContact *r = memArrayPushFast(&__PhysicsContactResourceArray, &__PhysicsContactResourceFreeBlock, &__PhysicsContactResourceFreeRegion);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memArrayAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CONTACT_SIZE,
				RESOURCE_DEFAULT_CONTACT_NUM
			)
		);
		if(memArrayExtend(&__PhysicsContactResourceArray, memory, RESOURCE_DEFAULT_CONTACT_SIZE, RESOURCE_DEFAULT_CONTACT_NUM)){
			r = memArrayPushFast(&__PhysicsContactResourceArray, &__PhysicsContactResourceFreeBlock, &__PhysicsContactResourceFreeRegion);
		}
	}
	return r;
}
void modulePhysicsContactClear(){
	memArrayClear(&__PhysicsContactResourceArray);
	__PhysicsContactResourceFreeBlock = memArrayFirst(__PhysicsContactResourceArray.region);
	__PhysicsContactResourceFreeRegion = __PhysicsContactResourceArray.region;
}

__HINT_INLINE__ physSeparation *modulePhysicsSeparationAppendStatic(physSeparation **const restrict array){
	return memSLinkAppend(&__PhysicsSeparationResourceArray, (void **)array);
}
__HINT_INLINE__ physSeparation *modulePhysicsSeparationAppend(physSeparation **const restrict array){
	physSeparation *r = memSLinkAppend(&__PhysicsSeparationResourceArray, (void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SEPARATION_SIZE,
				RESOURCE_DEFAULT_SEPARATION_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsSeparationResourceArray, memory, RESOURCE_DEFAULT_SEPARATION_SIZE, RESOURCE_DEFAULT_SEPARATION_NUM)){
			r = memSLinkAppend(&__PhysicsSeparationResourceArray, (void **)array);
		}
	}
	return r;
}
__HINT_INLINE__ physSeparation *modulePhysicsSeparationInsertAfterStatic(physSeparation **const restrict array, physSeparation *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsSeparationResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physSeparation *modulePhysicsSeparationInsertAfter(physSeparation **const restrict array, physSeparation *const restrict resource){
	physSeparation *r = memSLinkInsertAfter(&__PhysicsSeparationResourceArray, (void **)array, (void *)resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SEPARATION_SIZE,
				RESOURCE_DEFAULT_SEPARATION_NUM
			)
		);
		if(memSLinkExtend(&__PhysicsSeparationResourceArray, memory, RESOURCE_DEFAULT_SEPARATION_SIZE, RESOURCE_DEFAULT_SEPARATION_NUM)){
			r = memSLinkInsertAfter(&__PhysicsSeparationResourceArray, (void **)array, (void *)resource);
		}
	}
	return r;
}
__HINT_INLINE__ physSeparation *modulePhysicsSeparationNext(const physSeparation *const restrict i){
	return (physSeparation *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void modulePhysicsSeparationFree(physSeparation **const restrict array, physSeparation *const restrict resource, const physSeparation *const restrict previous){
	memSLinkFree(&__PhysicsSeparationResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsSeparationFreeArray(physSeparation **const restrict array){
	physSeparation *resource = *array;
	while(resource != NULL){
		memSLinkFree(&__PhysicsSeparationResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsSeparationClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsSeparationResourceArray, i, physSeparation *);

		modulePhysicsSeparationFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__PhysicsSeparationResourceArray, i, return;);

}

__HINT_INLINE__ physConstraint *modulePhysicsConstraintAppendStatic(physConstraint **const restrict array){
	return memSLinkAppend(&__PhysicsConstraintResourceArray, (void **)array);
}
__HINT_INLINE__ physConstraint *modulePhysicsConstraintAppend(physConstraint **const restrict array){
	physConstraint *r = memSLinkAppend(&__PhysicsConstraintResourceArray, (void **)array);
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
			r = memSLinkAppend(&__PhysicsConstraintResourceArray, (void **)array);
		}
	}
	return r;
}
__HINT_INLINE__ physConstraint *modulePhysicsConstraintInsertAfterStatic(physConstraint **const restrict array, physConstraint *const restrict resource){
	return memSLinkInsertAfter(&__PhysicsConstraintResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ physConstraint *modulePhysicsConstraintInsertAfter(physConstraint **const restrict array, physConstraint *const restrict resource){
	physConstraint *r = memSLinkInsertAfter(&__PhysicsConstraintResourceArray, (void **)array, (void *)resource);
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
			r = memSLinkInsertAfter(&__PhysicsConstraintResourceArray, (void **)array, (void *)resource);
		}
	}
	return r;
}
__HINT_INLINE__ physConstraint *modulePhysicsConstraintNext(const physConstraint *const restrict i){
	return (physConstraint *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void modulePhysicsConstraintFree(physConstraint **const restrict array, physConstraint *const restrict resource, const physConstraint *const restrict previous){
	memSLinkFree(&__PhysicsConstraintResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void modulePhysicsConstraintFreeArray(physConstraint **const restrict array){
	physConstraint *resource = *array;
	while(resource != NULL){
		memSLinkFree(&__PhysicsConstraintResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void modulePhysicsConstraintClear(){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsConstraintResourceArray, i, physConstraint *);

		modulePhysicsConstraintFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__PhysicsConstraintResourceArray, i, return;);

}

__FORCE_INLINE__ void modulePhysicsIntegrate(const float dt){

	MEMORY_SLINK_LOOP_BEGIN(__PhysicsRigidBodyInstanceResourceArray, i, physRBInstance *);

		physRBIIntegrateConfiguration(i, dt);

	MEMORY_SLINK_LOOP_END(__PhysicsRigidBodyInstanceResourceArray, i, return;);

}
void modulePhysicsSolve(const float dt){

	/*
	** Solves the constraints, followed by the contacts,
	** for all systems being simulated.
	*/

	// Solve constraints.
	///

	// Solve contacts.
	///

	// Integrate positions and orientations.
	modulePhysicsIntegrate(dt);

	// Clear the contact array once we've finished.
	modulePhysicsContactClear();

}