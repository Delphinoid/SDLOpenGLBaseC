#ifndef MODULEPHYSICS_H
#define MODULEPHYSICS_H

#include "physicsRigidBody.h"
#include "skeleton.h"
#include "memorySLink.h"

#define RESOURCE_DEFAULT_RIGID_BODY_SIZE sizeof(physRigidBody)
#ifndef RESOURCE_DEFAULT_RIGID_BODY_NUM
	#define RESOURCE_DEFAULT_RIGID_BODY_NUM 1024*SKELETON_MAX_BONE_NUM
#endif

#define RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE sizeof(physRBInstance)
#ifndef RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM
	#define RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM SKELETON_MAX_BONE_NUM*4096
#endif

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

extern memorySLink __PhysicsRigidBodyResourceArray;           // Contains physRigidBodies.
extern memorySLink __PhysicsRigidBodyInstanceResourceArray;   // Contains physRBInstances.
extern memorySLink __PhysicsColliderResourceArray;            // Contains physColliders.
extern memorySLink __PhysicsCollisionInstanceResourceArray;   // Contains physCollisionInfos.
extern memorySLink __PhysicsConstraintResourceArray;          // Contains physConstraints.

/** Support locals? Merge all module containers? **/

return_t modulePhysicsResourcesInit();
void modulePhysicsResourcesReset();
void modulePhysicsResourcesDelete();

physRigidBody *modulePhysicsRigidBodyAppendStatic(physRigidBody **array);
physRigidBody *modulePhysicsRigidBodyAppend(physRigidBody **array);
physRigidBody *modulePhysicsRigidBodyInsertAfterStatic(physRigidBody *resource);
physRigidBody *modulePhysicsRigidBodyInsertAfter(physRigidBody *resource);
physRigidBody *modulePhysicsRigidBodyNext(physRigidBody *i);
void modulePhysicsRigidBodyFree(physRigidBody **array, physRigidBody *resource, physRigidBody *previous);
void modulePhysicsRigidBodyFreeArray(physRigidBody **array);
void modulePhysicsRigidBodyClear();

physRBInstance *modulePhysicsRigidBodyInstanceAppendStatic(physRBInstance **array);
physRBInstance *modulePhysicsRigidBodyInstanceAppend(physRBInstance **array);
physRBInstance *modulePhysicsRigidBodyInstanceInsertAfterStatic(physRBInstance *resource);
physRBInstance *modulePhysicsRigidBodyInstanceInsertAfter(physRBInstance *resource);
physRBInstance *modulePhysicsRigidBodyInstanceNext(physRBInstance *i);
void modulePhysicsRigidBodyInstanceFree(physRBInstance **array, physRBInstance *resource, physRBInstance *previous);
void modulePhysicsRigidBodyInstanceFreeArray(physRBInstance **array);
void modulePhysicsRigidBodyInstanceClear();

physCollider *modulePhysicsColliderAppendStatic(physCollider **array);
physCollider *modulePhysicsColliderAppend(physCollider **array);
physCollider *modulePhysicsColliderInsertAfterStatic(physCollider *resource);
physCollider *modulePhysicsColliderInsertAfter(physCollider *resource);
physCollider *modulePhysicsColliderNext(physCollider *i);
void modulePhysicsColliderFree(physCollider **array, physCollider *resource, physCollider *previous);
void modulePhysicsColliderFreeArray(physCollider **array);
void modulePhysicsColliderRBIFreeArray(physCollider **array);
void modulePhysicsColliderClear();

physCollisionInfo *modulePhysicsCollisionInstanceAppendStatic(physCollisionInfo **array);
physCollisionInfo *modulePhysicsCollisionInstanceAppend(physCollisionInfo **array);
physCollisionInfo *modulePhysicsCollisionInstanceInsertAfterStatic(physCollisionInfo *resource);
physCollisionInfo *modulePhysicsCollisionInstanceInsertAfter(physCollisionInfo *resource);
physCollisionInfo *modulePhysicsCollisionInstanceNext(physCollisionInfo *i);
void modulePhysicsCollisionInstanceFree(physCollisionInfo **array, physCollisionInfo *resource, physCollisionInfo *previous);
void modulePhysicsCollisionInstanceFreeArray(physCollisionInfo **array);
void modulePhysicsCollisionInstanceClear();

physConstraint *modulePhysicsConstraintAppendStatic(physConstraint **array);
physConstraint *modulePhysicsConstraintAppend(physConstraint **array);
physConstraint *modulePhysicsConstraintInsertAfterStatic(physConstraint *resource);
physConstraint *modulePhysicsConstraintInsertAfter(physConstraint *resource);
physConstraint *modulePhysicsConstraintNext(physConstraint *i);
void modulePhysicsConstraintFree(physConstraint **array, physConstraint *resource, physConstraint *previous);
void modulePhysicsConstraintFreeArray(physConstraint **array);
void modulePhysicsConstraintClear();

#endif