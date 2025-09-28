#ifndef MODULEPHYSICS_H
#define MODULEPHYSICS_H

#include "settingsSkeleton.h"
#include "memoryList.h"
#include "memorySLink.h"
#include "memoryDLink.h"
#include "memoryQLink.h"
#include "return.h"

#define RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM 512*SKELETON_MAX_BONE_NUM
#define RESOURCE_DEFAULT_RIGID_BODY_NUM SKELETON_MAX_BONE_NUM*2048
#define RESOURCE_DEFAULT_COLLIDER_NUM RESOURCE_DEFAULT_RIGID_BODY_NUM
#define RESOURCE_DEFAULT_JOINT_NUM RESOURCE_DEFAULT_RIGID_BODY_NUM
#define RESOURCE_DEFAULT_CONTACT_PAIR_NUM RESOURCE_DEFAULT_RIGID_BODY_NUM
#define RESOURCE_DEFAULT_SEPARATION_PAIR_NUM RESOURCE_DEFAULT_RIGID_BODY_NUM
#define RESOURCE_DEFAULT_AABB_NODE_NUM RESOURCE_DEFAULT_RIGID_BODY_NUM

// Forward declarations for inlining.
extern memorySLink __g_PhysicsRigidBodyBaseResourceArray;   // Contains physRigidBodyBases.
extern memoryDLink __g_PhysicsRigidBodyResourceArray;       // Contains physRigidBodies.
extern memorySLink __g_PhysicsColliderResourceArray;        // Contains physColliders.
extern memoryDLink __g_PhysicsJointResourceArray;           // Contains physJoints.
#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
extern memoryQLink __g_PhysicsContactPairResourceArray;     // Contains physContactPairs.
extern memoryQLink __g_PhysicsSeparationPairResourceArray;  // Contains physSeparationPairs.
#else
extern memoryDLink __g_PhysicsContactPairResourceArray;     // Contains physContactPairs.
extern memoryDLink __g_PhysicsSeparationPairResourceArray;  // Contains physSeparationPairs.
#endif
extern memoryList  __g_PhysicsAABBNodeResourceArray;        // Contains aabbNodes.

typedef struct physRigidBodyBase physRigidBodyBase;
typedef struct physRigidBody physRigidBody;
typedef struct physCollider physCollider;
typedef struct physJoint physJoint;
typedef struct physContactPair physContactPair;
typedef struct physSeparationPair physSeparationPair;
typedef struct aabbNode aabbNode;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t modulePhysicsResourcesInit();
void modulePhysicsResourcesReset();
void modulePhysicsResourcesDelete();

physRigidBodyBase *modulePhysicsRigidBodyBaseAppendStatic(physRigidBodyBase **const __RESTRICT__ array);
physRigidBodyBase *modulePhysicsRigidBodyBaseAppend(physRigidBodyBase **const __RESTRICT__ array);
physRigidBodyBase *modulePhysicsRigidBodyBaseInsertAfterStatic(physRigidBodyBase **const __RESTRICT__ array, physRigidBodyBase *const __RESTRICT__ resource);
physRigidBodyBase *modulePhysicsRigidBodyBaseInsertAfter(physRigidBodyBase **const __RESTRICT__ array, physRigidBodyBase *const __RESTRICT__ resource);
physRigidBodyBase *modulePhysicsRigidBodyBaseNext(const physRigidBodyBase *const __RESTRICT__ i);
void modulePhysicsRigidBodyBaseFree(physRigidBodyBase **const __RESTRICT__ array, physRigidBodyBase *const __RESTRICT__ resource, const physRigidBodyBase *const __RESTRICT__ previous);
void modulePhysicsRigidBodyBaseFreeArray(physRigidBodyBase **const __RESTRICT__ array);
void modulePhysicsRigidBodyBaseClear();

physRigidBody *modulePhysicsRigidBodyAppendStatic(physRigidBody **const __RESTRICT__ array);
physRigidBody *modulePhysicsRigidBodyAppend(physRigidBody **const __RESTRICT__ array);
physRigidBody *modulePhysicsRigidBodyInsertAfterStatic(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource);
physRigidBody *modulePhysicsRigidBodyInsertAfter(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource);
physRigidBody *modulePhysicsRigidBodyPrependStatic(physRigidBody **const __RESTRICT__ array);
physRigidBody *modulePhysicsRigidBodyPrepend(physRigidBody **const __RESTRICT__ array);
physRigidBody *modulePhysicsRigidBodyInsertBeforeStatic(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource);
physRigidBody *modulePhysicsRigidBodyInsertBefore(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource);
physRigidBody *modulePhysicsRigidBodyNext(const physRigidBody *const __RESTRICT__ i);
physRigidBody *modulePhysicsRigidBodyPrev(const physRigidBody *const __RESTRICT__ i);
void modulePhysicsRigidBodyFree(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource);
void modulePhysicsRigidBodyFreeArray(physRigidBody **const __RESTRICT__ array);
void modulePhysicsRigidBodyClear();

physCollider *modulePhysicsColliderAppendStatic(physCollider **const __RESTRICT__ array);
physCollider *modulePhysicsColliderAppend(physCollider **const __RESTRICT__ array);
physCollider *modulePhysicsColliderInsertAfterStatic(physCollider **const __RESTRICT__ array, physCollider *const __RESTRICT__ resource);
physCollider *modulePhysicsColliderInsertAfter(physCollider **const __RESTRICT__ array, physCollider *const __RESTRICT__ resource);
physCollider *modulePhysicsColliderNext(const physCollider *const __RESTRICT__ i);
void modulePhysicsColliderFree(physCollider **const __RESTRICT__ array, physCollider *const __RESTRICT__ resource, const physCollider *const __RESTRICT__ previous);
void modulePhysicsColliderFreeArray(physCollider **const __RESTRICT__ array);
void modulePhysicsColliderClear();

physJoint *modulePhysicsJointAllocateStatic();
physJoint *modulePhysicsJointAllocate();
physJoint *modulePhysicsJointAppendStatic(physJoint **const __RESTRICT__ array);
physJoint *modulePhysicsJointAppend(physJoint **const __RESTRICT__ array);
physJoint *modulePhysicsJointInsertAfterStatic(physJoint **const __RESTRICT__ array, physJoint *const __RESTRICT__ resource);
physJoint *modulePhysicsJointInsertAfter(physJoint **const __RESTRICT__ array, physJoint *const __RESTRICT__ resource);
physJoint *modulePhysicsJointPrependStatic(physJoint **const __RESTRICT__ array);
physJoint *modulePhysicsJointPrepend(physJoint **const __RESTRICT__ array);
physJoint *modulePhysicsJointInsertBeforeStatic(physJoint **const __RESTRICT__ array, physJoint *const __RESTRICT__ resource);
physJoint *modulePhysicsJointInsertBefore(physJoint **const __RESTRICT__ array, physJoint *const __RESTRICT__ resource);
physJoint *modulePhysicsJointNext(const physJoint *const __RESTRICT__ i);
physJoint *modulePhysicsJointPrev(const physJoint *const __RESTRICT__ i);
void modulePhysicsJointFree(physJoint **const __RESTRICT__ array, physJoint *const __RESTRICT__ resource);
void modulePhysicsJointFreeArray(physJoint **const __RESTRICT__ array);
void modulePhysicsJointClear();

#ifdef PHYSICS_CONTACT_USE_ALLOCATOR

physContactPair *modulePhysicsContactPairAllocateStatic();
physContactPair *modulePhysicsContactPairAllocate();
void modulePhysicsContactPairFree(physContactPair *const __RESTRICT__ resource);
void modulePhysicsContactPairClear();

physSeparationPair *modulePhysicsSeparationPairAllocateStatic();
physSeparationPair *modulePhysicsSeparationPairAllocate();
void modulePhysicsSeparationPairFree(physSeparationPair *const __RESTRICT__ resource);
void modulePhysicsSeparationPairClear();

#else

physContactPair *modulePhysicsContactPairAppendStatic(physContactPair **const __RESTRICT__ array);
physContactPair *modulePhysicsContactPairAppend(physContactPair **const __RESTRICT__ array);
physContactPair *modulePhysicsContactPairInsertAfterStatic(physContactPair **const __RESTRICT__ array, physContactPair *const __RESTRICT__ resource);
physContactPair *modulePhysicsContactPairInsertAfter(physContactPair **const __RESTRICT__ array, physContactPair *const __RESTRICT__ resource);
physContactPair *modulePhysicsContactPairPrependStatic(physContactPair **const __RESTRICT__ array);
physContactPair *modulePhysicsContactPairPrepend(physContactPair **const __RESTRICT__ array);
physContactPair *modulePhysicsContactPairInsertBeforeStatic(physContactPair **const __RESTRICT__ array, physContactPair *const __RESTRICT__ resource);
physContactPair *modulePhysicsContactPairInsertBefore(physContactPair **const __RESTRICT__ array, physContactPair *const __RESTRICT__ resource);
physContactPair *modulePhysicsContactPairNext(const physContactPair *const __RESTRICT__ i);
physContactPair *modulePhysicsContactPairPrev(const physContactPair *const __RESTRICT__ i);
void modulePhysicsContactPairFree(physContactPair **const __RESTRICT__ array, physContactPair *const __RESTRICT__ resource);
void modulePhysicsContactPairFreeArray(physContactPair **const __RESTRICT__ array);
void modulePhysicsContactPairClear();

physSeparationPair *modulePhysicsSeparationPairAppendStatic(physSeparationPair **const __RESTRICT__ array);
physSeparationPair *modulePhysicsSeparationPairAppend(physSeparationPair **const __RESTRICT__ array);
physSeparationPair *modulePhysicsSeparationPairInsertAfterStatic(physSeparationPair **const __RESTRICT__ array, physSeparationPair *const __RESTRICT__ resource);
physSeparationPair *modulePhysicsSeparationPairInsertAfter(physSeparationPair **const __RESTRICT__ array, physSeparationPair *const __RESTRICT__ resource);
physSeparationPair *modulePhysicsSeparationPairPrependStatic(physSeparationPair **const __RESTRICT__ array);
physSeparationPair *modulePhysicsSeparationPairPrepend(physSeparationPair **const __RESTRICT__ array);
physSeparationPair *modulePhysicsSeparationPairInsertBeforeStatic(physSeparationPair **const __RESTRICT__ array, physSeparationPair *const __RESTRICT__ resource);
physSeparationPair *modulePhysicsSeparationPairInsertBefore(physSeparationPair **const __RESTRICT__ array, physSeparationPair *const __RESTRICT__ resource);
physSeparationPair *modulePhysicsSeparationPairNext(const physSeparationPair *const __RESTRICT__ i);
physSeparationPair *modulePhysicsSeparationPairPrev(const physSeparationPair *const __RESTRICT__ i);
void modulePhysicsSeparationPairFree(physSeparationPair **const __RESTRICT__ array, physSeparationPair *const __RESTRICT__ resource);
void modulePhysicsSeparationPairFreeArray(physSeparationPair **const __RESTRICT__ array);
void modulePhysicsSeparationPairClear();

#endif

aabbNode *modulePhysicsAABBNodeAllocateStatic();
aabbNode *modulePhysicsAABBNodeAllocate();
void modulePhysicsAABBNodeFree(aabbNode *const __RESTRICT__ resource);
void modulePhysicsAABBNodeClear();

#endif
