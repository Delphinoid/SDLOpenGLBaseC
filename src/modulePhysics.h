#ifndef MODULEPHYSICS_H
#define MODULEPHYSICS_H

#include "physicsRigidBody.h"
#include "physicsCollider.h"
#include "physicsConstraint.h"
#include "physicsCollision.h"
#include "physicsIsland.h"
#include "memoryList.h"
#include "memorySLink.h"

#define RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE sizeof(physRigidBodyBase)
#define RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM 1024*SKELETON_MAX_BONE_NUM

#define RESOURCE_DEFAULT_RIGID_BODY_SIZE sizeof(physRigidBody)
#define RESOURCE_DEFAULT_RIGID_BODY_NUM SKELETON_MAX_BONE_NUM*4096

#define RESOURCE_DEFAULT_COLLIDER_SIZE sizeof(physCollider)
#define RESOURCE_DEFAULT_COLLIDER_NUM RESOURCE_DEFAULT_RIGID_BODY_NUM

#define RESOURCE_DEFAULT_CONSTRAINT_SIZE sizeof(physConstraint)
#define RESOURCE_DEFAULT_CONSTRAINT_NUM RESOURCE_DEFAULT_RIGID_BODY_NUM

#define RESOURCE_DEFAULT_CONTACT_PAIR_SIZE sizeof(physContactPair)
#define RESOURCE_DEFAULT_CONTACT_PAIR_NUM RESOURCE_DEFAULT_RIGID_BODY_NUM

#define RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE sizeof(physSeparationPair)
#define RESOURCE_DEFAULT_SEPARATION_PAIR_NUM RESOURCE_DEFAULT_RIGID_BODY_NUM

#define RESOURCE_DEFAULT_AABB_NODE_SIZE sizeof(aabbNode)
#define RESOURCE_DEFAULT_AABB_NODE_NUM RESOURCE_DEFAULT_RIGID_BODY_NUM

// Forward declarations for inlining.
extern memorySLink __PhysicsRigidBodyLocalResourceArray;  // Contains physRigidBodyBases.
extern memorySLink __PhysicsRigidBodyResourceArray;       // Contains physRigidBodies.
extern memorySLink __PhysicsColliderResourceArray;        // Contains physColliders.
extern memorySLink __PhysicsConstraintResourceArray;      // Contains physConstraints.
extern memoryList  __PhysicsContactPairResourceArray;     // Contains physContactPairs.
extern memoryList  __PhysicsSeparationPairResourceArray;  // Contains physSeparationPairs.
extern memoryList  __PhysicsAABBNodeResourceArray;        // Contains aabbNodes.

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t modulePhysicsResourcesInit();
void modulePhysicsResourcesReset();
void modulePhysicsResourcesDelete();

physRigidBodyBase *modulePhysicsRigidBodyBaseAppendStatic(physRigidBodyBase **const restrict array);
physRigidBodyBase *modulePhysicsRigidBodyBaseAppend(physRigidBodyBase **const restrict array);
physRigidBodyBase *modulePhysicsRigidBodyBaseInsertAfterStatic(physRigidBodyBase **const restrict array, physRigidBodyBase *const restrict resource);
physRigidBodyBase *modulePhysicsRigidBodyBaseInsertAfter(physRigidBodyBase **const restrict array, physRigidBodyBase *const restrict resource);
physRigidBodyBase *modulePhysicsRigidBodyBaseNext(const physRigidBodyBase *const restrict i);
void modulePhysicsRigidBodyBaseFree(physRigidBodyBase **const restrict array, physRigidBodyBase *const restrict resource, const physRigidBodyBase *const restrict previous);
void modulePhysicsRigidBodyBaseFreeArray(physRigidBodyBase **const restrict array);
void modulePhysicsRigidBodyBaseClear();

physRigidBody *modulePhysicsRigidBodyAppendStatic(physRigidBody **const restrict array);
physRigidBody *modulePhysicsRigidBodyAppend(physRigidBody **const restrict array);
physRigidBody *modulePhysicsRigidBodyInsertAfterStatic(physRigidBody **const restrict array, physRigidBody *const restrict resource);
physRigidBody *modulePhysicsRigidBodyInsertAfter(physRigidBody **const restrict array, physRigidBody *const restrict resource);
physRigidBody *modulePhysicsRigidBodyNext(const physRigidBody *const restrict i);
void modulePhysicsRigidBodyFree(physRigidBody **const restrict array, physRigidBody *const restrict resource, const physRigidBody *const restrict previous);
void modulePhysicsRigidBodyFreeArray(physRigidBody **const restrict array);
void modulePhysicsRigidBodyClear();

physCollider *modulePhysicsColliderAppendStatic(physCollider **const restrict array);
physCollider *modulePhysicsColliderAppend(physCollider **const restrict array);
physCollider *modulePhysicsColliderInsertAfterStatic(physCollider **const restrict array, physCollider *const restrict resource);
physCollider *modulePhysicsColliderInsertAfter(physCollider **const restrict array, physCollider *const restrict resource);
physCollider *modulePhysicsColliderNext(const physCollider *const restrict i);
void modulePhysicsColliderFree(physCollider **const restrict array, physCollider *const restrict resource, const physCollider *const restrict previous);
void modulePhysicsColliderFreeArray(physCollider **const restrict array);
void modulePhysicsColliderClear();

physConstraint *modulePhysicsConstraintAppendStatic(physConstraint **const restrict array);
physConstraint *modulePhysicsConstraintAppend(physConstraint **const restrict array);
physConstraint *modulePhysicsConstraintInsertAfterStatic(physConstraint **const restrict array, physConstraint *const restrict resource);
physConstraint *modulePhysicsConstraintInsertAfter(physConstraint **const restrict array, physConstraint *const restrict resource);
physConstraint *modulePhysicsConstraintNext(const physConstraint *const restrict i);
void modulePhysicsConstraintFree(physConstraint **const restrict array, physConstraint *const restrict resource, const physConstraint *const restrict previous);
void modulePhysicsConstraintFreeArray(physConstraint **const restrict array);
void modulePhysicsConstraintClear();

physContactPair *modulePhysicsContactPairAllocateStatic();
physContactPair *modulePhysicsContactPairAllocate();
void modulePhysicsContactPairFree(physContactPair *const restrict resource);
void modulePhysicsContactPairClear();

physSeparationPair *modulePhysicsSeparationPairAllocateStatic();
physSeparationPair *modulePhysicsSeparationPairAllocate();
void modulePhysicsSeparationPairFree(physSeparationPair *const restrict resource);
void modulePhysicsSeparationPairClear();

aabbNode *modulePhysicsAABBNodeAllocateStatic();
aabbNode *modulePhysicsAABBNodeAllocate();
void modulePhysicsAABBNodeFree(aabbNode *const restrict resource);
void modulePhysicsAABBNodeClear();

void modulePhysicsSolve();

#endif