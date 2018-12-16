#ifndef MODULEPHYSICS_H
#define MODULEPHYSICS_H

#include "physicsRigidBody.h"
#include "skeleton.h"
#include "memorySLink.h"

#define RESOURCE_DEFAULT_RIGID_BODY_SIZE sizeof(physRigidBody)
#define RESOURCE_DEFAULT_RIGID_BODY_NUM 1024*SKELETON_MAX_BONE_NUM

#define RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE sizeof(physRBInstance)
#define RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM SKELETON_MAX_BONE_NUM*4096

#define RESOURCE_DEFAULT_COLLIDER_SIZE sizeof(physCollider)
#define RESOURCE_DEFAULT_COLLIDER_NUM 4096

#define RESOURCE_DEFAULT_COLLISION_SIZE sizeof(physCollisionInfo)
#define RESOURCE_DEFAULT_COLLISION_NUM RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM

#define RESOURCE_DEFAULT_CONSTRAINT_SIZE sizeof(physConstraint)
#define RESOURCE_DEFAULT_CONSTRAINT_NUM RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM

extern memorySLink __PhysicsRigidBodyResourceArray;           // Contains physRigidBodies.
extern memorySLink __PhysicsRigidBodyInstanceResourceArray;   // Contains physRBInstances.
extern memorySLink __PhysicsColliderResourceArray;            // Contains physColliders.
extern memorySLink __PhysicsCollisionInstanceResourceArray;   // Contains physCollisionInfos.
extern memorySLink __PhysicsConstraintResourceArray;          // Contains physConstraints.

/** Support locals? Merge all module containers? **/

return_t modulePhysicsResourcesInit();
void modulePhysicsResourcesReset();
void modulePhysicsResourcesDelete();

physRigidBody *modulePhysicsRigidBodyAppendStatic(physRigidBody **const restrict array);
physRigidBody *modulePhysicsRigidBodyAppend(physRigidBody **const restrict array);
physRigidBody *modulePhysicsRigidBodyInsertAfterStatic(physRigidBody *const restrict resource);
physRigidBody *modulePhysicsRigidBodyInsertAfter(physRigidBody *const restrict resource);
physRigidBody *modulePhysicsRigidBodyNext(const physRigidBody *const restrict i);
void modulePhysicsRigidBodyFree(physRigidBody **const restrict array, physRigidBody *const restrict resource, physRigidBody *const restrict previous);
void modulePhysicsRigidBodyFreeArray(physRigidBody **const restrict array);
void modulePhysicsRigidBodyClear();

physRBInstance *modulePhysicsRigidBodyInstanceAppendStatic(physRBInstance **const restrict array);
physRBInstance *modulePhysicsRigidBodyInstanceAppend(physRBInstance **const restrict array);
physRBInstance *modulePhysicsRigidBodyInstanceInsertAfterStatic(physRBInstance *const restrict resource);
physRBInstance *modulePhysicsRigidBodyInstanceInsertAfter(physRBInstance *const restrict resource);
physRBInstance *modulePhysicsRigidBodyInstanceNext(const physRBInstance *const restrict i);
void modulePhysicsRigidBodyInstanceFree(physRBInstance **const restrict array, physRBInstance *const restrict resource, physRBInstance *const restrict previous);
void modulePhysicsRigidBodyInstanceFreeArray(physRBInstance **const restrict array);
void modulePhysicsRigidBodyInstanceClear();

physCollider *modulePhysicsColliderAppendStatic(physCollider **const restrict array);
physCollider *modulePhysicsColliderAppend(physCollider **const restrict array);
physCollider *modulePhysicsColliderInsertAfterStatic(physCollider *const restrict resource);
physCollider *modulePhysicsColliderInsertAfter(physCollider *const restrict resource);
physCollider *modulePhysicsColliderNext(const physCollider *const restrict i);
void modulePhysicsColliderFree(physCollider **const restrict array, physCollider *const restrict resource, physCollider *const restrict previous);
void modulePhysicsColliderFreeArray(physCollider **const restrict array);
void modulePhysicsColliderRBIFreeArray(physCollider **const restrict array);
void modulePhysicsColliderClear();

physCollisionInfo *modulePhysicsCollisionInstanceAppendStatic(physCollisionInfo **const restrict array);
physCollisionInfo *modulePhysicsCollisionInstanceAppend(physCollisionInfo **const restrict array);
physCollisionInfo *modulePhysicsCollisionInstanceInsertAfterStatic(physCollisionInfo *const restrict resource);
physCollisionInfo *modulePhysicsCollisionInstanceInsertAfter(physCollisionInfo *const restrict resource);
physCollisionInfo *modulePhysicsCollisionInstanceNext(const physCollisionInfo *const restrict i);
void modulePhysicsCollisionInstanceFree(physCollisionInfo **const restrict array, physCollisionInfo *const restrict resource, physCollisionInfo *const restrict previous);
void modulePhysicsCollisionInstanceFreeArray(physCollisionInfo **const restrict array);
void modulePhysicsCollisionInstanceClear();

physConstraint *modulePhysicsConstraintAppendStatic(physConstraint **const restrict array);
physConstraint *modulePhysicsConstraintAppend(physConstraint **const restrict array);
physConstraint *modulePhysicsConstraintInsertAfterStatic(physConstraint *const restrict resource);
physConstraint *modulePhysicsConstraintInsertAfter(physConstraint *const restrict resource);
physConstraint *modulePhysicsConstraintNext(const physConstraint *const restrict i);
void modulePhysicsConstraintFree(physConstraint **const restrict array, physConstraint *const restrict resource, physConstraint *const restrict previous);
void modulePhysicsConstraintFreeArray(physConstraint **const restrict array);
void modulePhysicsConstraintClear();

void modulePhysicsSolve();

#endif