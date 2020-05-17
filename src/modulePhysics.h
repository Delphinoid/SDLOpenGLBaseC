#ifndef MODULEPHYSICS_H
#define MODULEPHYSICS_H

#include "skeletonSettings.h"
#include "memoryList.h"
#include "memorySLink.h"
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
extern memorySLink __g_PhysicsRigidBodyResourceArray;       // Contains physRigidBodies.
extern memorySLink __g_PhysicsColliderResourceArray;        // Contains physColliders.
extern memoryQLink __g_PhysicsJointResourceArray;           // Contains physJoints.
extern memoryQLink __g_PhysicsContactPairResourceArray;     // Contains physContactPairs.
extern memoryQLink __g_PhysicsSeparationPairResourceArray;  // Contains physSeparationPairs.
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
physRigidBody *modulePhysicsRigidBodyNext(const physRigidBody *const __RESTRICT__ i);
void modulePhysicsRigidBodyFree(physRigidBody **const __RESTRICT__ array, physRigidBody *const __RESTRICT__ resource, const physRigidBody *const __RESTRICT__ previous);
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
void modulePhysicsJointFree(physJoint *const __RESTRICT__ resource);
void modulePhysicsJointClear();

physContactPair *modulePhysicsContactPairAllocateStatic();
physContactPair *modulePhysicsContactPairAllocate();
void modulePhysicsContactPairFree(physContactPair *const __RESTRICT__ resource);
void modulePhysicsContactPairClear();

physSeparationPair *modulePhysicsSeparationPairAllocateStatic();
physSeparationPair *modulePhysicsSeparationPairAllocate();
void modulePhysicsSeparationPairFree(physSeparationPair *const __RESTRICT__ resource);
void modulePhysicsSeparationPairClear();

aabbNode *modulePhysicsAABBNodeAllocateStatic();
aabbNode *modulePhysicsAABBNodeAllocate();
void modulePhysicsAABBNodeFree(aabbNode *const __RESTRICT__ resource);
void modulePhysicsAABBNodeClear();

/** These should be moved to physIsland. **/
void modulePhysicsPresolveConstraints(const float dt);
void modulePhysicsSolveConstraints(const float dt);

#endif