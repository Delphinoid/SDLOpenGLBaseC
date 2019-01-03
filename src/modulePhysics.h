#ifndef MODULEPHYSICS_H
#define MODULEPHYSICS_H

#include "physicsRigidBody.h"
#include "skeleton.h"
#include "memoryArray.h"
#include "memorySLink.h"

#define RESOURCE_DEFAULT_RIGID_BODY_SIZE sizeof(physRigidBody)
#define RESOURCE_DEFAULT_RIGID_BODY_NUM 1024*SKELETON_MAX_BONE_NUM

#define RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE sizeof(physRBInstance)
#define RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM SKELETON_MAX_BONE_NUM*4096

#define RESOURCE_DEFAULT_COLLIDER_SIZE sizeof(physCollider)
#define RESOURCE_DEFAULT_COLLIDER_NUM 4096

#define RESOURCE_DEFAULT_CONTACT_SIZE sizeof(physContact)
#define RESOURCE_DEFAULT_CONTACT_NUM RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM

#define RESOURCE_DEFAULT_SEPARATION_SIZE sizeof(physSeparation)
#define RESOURCE_DEFAULT_SEPARATION_NUM RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM

#define RESOURCE_DEFAULT_CONSTRAINT_SIZE sizeof(physConstraint)
#define RESOURCE_DEFAULT_CONSTRAINT_NUM RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM

extern void *__PhysicsContactResourceFreeBlock;
extern memoryRegion *__PhysicsContactResourceFreeRegion;

extern memorySLink __PhysicsRigidBodyResourceArray;          // Contains physRigidBodies.
extern memorySLink __PhysicsRigidBodyInstanceResourceArray;  // Contains physRBInstances.
extern memorySLink __PhysicsColliderResourceArray;           // Contains physColliders.
extern memoryArray __PhysicsContactResourceArray;            // Contains physContacts.
extern memorySLink __PhysicsSeparationResourceArray;         // Contains physSeparations.
extern memorySLink __PhysicsConstraintResourceArray;         // Contains physConstraints.

/** Support locals? Merge all module containers? **/

return_t modulePhysicsResourcesInit();
void modulePhysicsResourcesReset();
void modulePhysicsResourcesDelete();

physRigidBody *modulePhysicsRigidBodyAppendStatic(physRigidBody **const restrict array);
physRigidBody *modulePhysicsRigidBodyAppend(physRigidBody **const restrict array);
physRigidBody *modulePhysicsRigidBodyInsertAfterStatic(physRigidBody **const restrict array, physRigidBody *const restrict resource);
physRigidBody *modulePhysicsRigidBodyInsertAfter(physRigidBody **const restrict array, physRigidBody *const restrict resource);
physRigidBody *modulePhysicsRigidBodyNext(const physRigidBody *const restrict i);
void modulePhysicsRigidBodyFree(physRigidBody **const restrict array, physRigidBody *const restrict resource, const physRigidBody *const restrict previous);
void modulePhysicsRigidBodyFreeArray(physRigidBody **const restrict array);
void modulePhysicsRigidBodyClear();

physRBInstance *modulePhysicsRigidBodyInstanceAppendStatic(physRBInstance **const restrict array);
physRBInstance *modulePhysicsRigidBodyInstanceAppend(physRBInstance **const restrict array);
physRBInstance *modulePhysicsRigidBodyInstanceInsertAfterStatic(physRBInstance **const restrict array, physRBInstance *const restrict resource);
physRBInstance *modulePhysicsRigidBodyInstanceInsertAfter(physRBInstance **const restrict array, physRBInstance *const restrict resource);
physRBInstance *modulePhysicsRigidBodyInstanceNext(const physRBInstance *const restrict i);
void modulePhysicsRigidBodyInstanceFree(physRBInstance **const restrict array, physRBInstance *const restrict resource, const physRBInstance *const restrict previous);
void modulePhysicsRigidBodyInstanceFreeArray(physRBInstance **const restrict array);
void modulePhysicsRigidBodyInstanceClear();

physCollider *modulePhysicsColliderAppendStatic(physCollider **const restrict array);
physCollider *modulePhysicsColliderAppend(physCollider **const restrict array);
physCollider *modulePhysicsColliderInsertAfterStatic(physCollider **const restrict array, physCollider *const restrict resource);
physCollider *modulePhysicsColliderInsertAfter(physCollider **const restrict array, physCollider *const restrict resource);
physCollider *modulePhysicsColliderNext(const physCollider *const restrict i);
void modulePhysicsColliderFree(physCollider **const restrict array, physCollider *const restrict resource, const physCollider *const restrict previous);
void modulePhysicsColliderFreeArray(physCollider **const restrict array);
void modulePhysicsColliderRBIFreeArray(physCollider **const restrict array);
void modulePhysicsColliderClear();

physContact *modulePhysicsContactAllocateStatic();
physContact *modulePhysicsContactAllocate();
void modulePhysicsContactClear();

physSeparation *modulePhysicsSeparationAppendStatic(physSeparation **const restrict array);
physSeparation *modulePhysicsSeparationAppend(physSeparation **const restrict array);
physSeparation *modulePhysicsSeparationInsertAfterStatic(physSeparation **const restrict array, physSeparation *const restrict resource);
physSeparation *modulePhysicsSeparationInsertAfter(physSeparation **const restrict array, physSeparation *const restrict resource);
physSeparation *modulePhysicsSeparationNext(const physSeparation *const restrict i);
void modulePhysicsSeparationFree(physSeparation **const restrict array, physSeparation *const restrict resource, const physSeparation *const restrict previous);
void modulePhysicsSeparationFreeArray(physSeparation **const restrict array);
void modulePhysicsSeparationClear();

physConstraint *modulePhysicsConstraintAppendStatic(physConstraint **const restrict array);
physConstraint *modulePhysicsConstraintAppend(physConstraint **const restrict array);
physConstraint *modulePhysicsConstraintInsertAfterStatic(physConstraint **const restrict array, physConstraint *const restrict resource);
physConstraint *modulePhysicsConstraintInsertAfter(physConstraint **const restrict array, physConstraint *const restrict resource);
physConstraint *modulePhysicsConstraintNext(const physConstraint *const restrict i);
void modulePhysicsConstraintFree(physConstraint **const restrict array, physConstraint *const restrict resource, const physConstraint *const restrict previous);
void modulePhysicsConstraintFreeArray(physConstraint **const restrict array);
void modulePhysicsConstraintClear();

void modulePhysicsIntegrate(const float dt);
void modulePhysicsSolve(const float dt);

#endif