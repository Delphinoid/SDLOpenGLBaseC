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

physJoint *modulePhysicsJointAllocateStatic();
physJoint *modulePhysicsJointAllocate();
void modulePhysicsJointFree(physJoint *const restrict resource);
void modulePhysicsJointClear();

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

void modulePhysicsSolveConstraints(const float dt);

#endif