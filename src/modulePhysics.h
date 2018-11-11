#ifndef MODULEPHYSICS_H
#define MODULEPHYSICS_H

#include "memoryManager.h"
#include "physicsRigidBody.h"

#define RESOURCE_DEFAULT_COLLIDER_SIZE sizeof(physCollider)
#ifndef RESOURCE_DEFAULT_COLLIDER_NUM
	#define RESOURCE_DEFAULT_COLLIDER_NUM 4096
#endif

#define RESOURCE_DEFAULT_COLLISION_SIZE sizeof(physCollisionInfo)
#ifndef RESOURCE_DEFAULT_COLLISION_NUM
	#define RESOURCE_DEFAULT_COLLISION_NUM RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM
#endif

#define RESOURCE_DEFAULT_CONSTRAINT_SIZE sizeof(physConstraint)
#ifndef RESOURCE_DEFAULT_CONSTRAINT_NUM
	#define RESOURCE_DEFAULT_CONSTRAINT_NUM RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM
#endif

#define RESOURCE_DEFAULT_RIGID_BODY_SIZE sizeof(physRigidBody)
#ifndef RESOURCE_DEFAULT_RIGID_BODY_NUM
	#define RESOURCE_DEFAULT_RIGID_BODY_NUM RESOURCE_DEFAULT_OBJECT_NUM*SKL_MAX_BONE_NUM
#endif

#define RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE sizeof(physRBInstance)
#ifndef RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM
	#define RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM SKL_MAX_BONE_NUM*RESOURCE_DEFAULT_OBJECT_INSTANCE_NUM
#endif

#ifndef MODULE_PHYSICS_USE_LOCAL_DEFINITION

extern memorySLink ColliderResourceArray;            // Contains physColliders.
extern memorySLink CollisionInstanceResourceArray;   // Contains physCollisionInfos.
extern memorySLink ConstraintInstanceResourceArray;  // Contains physConstraints.
extern memoryPool  RigidBodyResourceArray;           // Contains physRigidBodies.
extern memorySLink RigidBodyInstanceResourceArray;   // Contains physRBInstances.

return_t modulePhysicsInit(){
	void *memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_COLLIDER_SIZE,
			RESOURCE_DEFAULT_COLLIDER_NUM
		)
	);
	if(memSLinkCreate(&ColliderResourceArray, memory, RESOURCE_DEFAULT_COLLIDER_SIZE, RESOURCE_DEFAULT_COLLIDER_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_COLLISION_SIZE,
			RESOURCE_DEFAULT_COLLISION_NUM
		)
	);
	if(memSLinkCreate(&CollisionInstanceResourceArray, memory, RESOURCE_DEFAULT_COLLISION_SIZE, RESOURCE_DEFAULT_COLLISION_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_CONSTRAINT_SIZE,
			RESOURCE_DEFAULT_CONSTRAINT_NUM
		)
	);
	if(memSLinkCreate(&ConstraintInstanceResourceArray, memory, RESOURCE_DEFAULT_CONSTRAINT_SIZE, RESOURCE_DEFAULT_CONSTRAINT_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RIGID_BODY_SIZE,
			RESOURCE_DEFAULT_RIGID_BODY_NUM
		)
	);
	if(memPoolCreate(&RigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE,
			RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM
		)
	);
	if(memSLinkCreate(&RigidBodyInstanceResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE, RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM) == NULL){
		return -1;
	}
	return 1;
}

#else

return_t modulePhysicsInit(
	memorySLink *ColliderResourceArray,            // Contains physColliders.
	memorySLink *CollisionInstanceResourceArray,   // Contains physCollisionInfos.
	memorySLink *ConstraintInstanceResourceArray,  // Contains physConstraints.
	memoryPool  *RigidBodyResourceArray,           // Contains physRigidBodies.
	memorySLink *RigidBodyInstanceResourceArray    // Contains physRBInstances.
){
	void *memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_COLLIDER_SIZE,
			RESOURCE_DEFAULT_COLLIDER_NUM
		)
	);
	if(memSLinkCreate(ColliderResourceArray, memory, RESOURCE_DEFAULT_COLLIDER_SIZE, RESOURCE_DEFAULT_COLLIDER_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_COLLISION_SIZE,
			RESOURCE_DEFAULT_COLLISION_NUM
		)
	);
	if(memSLinkCreate(CollisionInstanceResourceArray, memory, RESOURCE_DEFAULT_COLLISION_SIZE, RESOURCE_DEFAULT_COLLISION_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_CONSTRAINT_SIZE,
			RESOURCE_DEFAULT_CONSTRAINT_NUM
		)
	);
	if(memSLinkCreate(ConstraintInstanceResourceArray, memory, RESOURCE_DEFAULT_CONSTRAINT_SIZE, RESOURCE_DEFAULT_CONSTRAINT_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RIGID_BODY_SIZE,
			RESOURCE_DEFAULT_RIGID_BODY_NUM
		)
	);
	if(memPoolCreate(RigidBodyResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_SIZE, RESOURCE_DEFAULT_RIGID_BODY_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE,
			RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM
		)
	);
	if(memSLinkCreate(RigidBodyInstanceResourceArray, memory, RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE, RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM) == NULL){
		return -1;
	}
	return 1;
}

#endif

#endif
