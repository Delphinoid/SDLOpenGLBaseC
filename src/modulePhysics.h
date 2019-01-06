#ifndef MODULEPHYSICS_H
#define MODULEPHYSICS_H

#include "physicsRigidBody.h"
#include "skeleton.h"
#include "memoryArray.h"
#include "memorySLink.h"

#define RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE sizeof(physRigidBodyLocal)
#define RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM 1024*SKELETON_MAX_BONE_NUM

#define RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE sizeof(physRigidBody)
#define RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM SKELETON_MAX_BONE_NUM*4096

#define RESOURCE_DEFAULT_CONTACT_SIZE sizeof(physContact)
#define RESOURCE_DEFAULT_CONTACT_NUM RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM

#define RESOURCE_DEFAULT_SEPARATION_SIZE sizeof(physSeparation)
#define RESOURCE_DEFAULT_SEPARATION_NUM RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM

#define RESOURCE_DEFAULT_CONSTRAINT_SIZE sizeof(physConstraint)
#define RESOURCE_DEFAULT_CONSTRAINT_NUM RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM

// Forward declarations for inlining.
extern memorySLink __PhysicsRigidBodyLocalResourceArray;  // Contains physRigidBodyLocals.
extern memorySLink __PhysicsRigidBodyResourceArray;       // Contains physRigidBodies.
extern memoryArray __PhysicsContactResourceArray;         // Contains physContacts.
extern memorySLink __PhysicsSeparationResourceArray;      // Contains physSeparations.
extern memorySLink __PhysicsConstraintResourceArray;      // Contains physConstraints.

extern void *__PhysicsContactResourceFreeBlock;
extern memoryRegion *__PhysicsContactResourceFreeRegion;

/** Support locals? Merge all module containers? **/

return_t modulePhysicsResourcesInit();
void modulePhysicsResourcesReset();
void modulePhysicsResourcesDelete();

physRigidBodyLocal *modulePhysicsRigidBodyLocalAppendStatic(physRigidBodyLocal **const restrict array);
physRigidBodyLocal *modulePhysicsRigidBodyLocalAppend(physRigidBodyLocal **const restrict array);
physRigidBodyLocal *modulePhysicsRigidBodyLocalInsertAfterStatic(physRigidBodyLocal **const restrict array, physRigidBodyLocal *const restrict resource);
physRigidBodyLocal *modulePhysicsRigidBodyLocalInsertAfter(physRigidBodyLocal **const restrict array, physRigidBodyLocal *const restrict resource);
physRigidBodyLocal *modulePhysicsRigidBodyLocalNext(const physRigidBodyLocal *const restrict i);
void modulePhysicsRigidBodyLocalFree(physRigidBodyLocal **const restrict array, physRigidBodyLocal *const restrict resource, const physRigidBodyLocal *const restrict previous);
void modulePhysicsRigidBodyLocalFreeArray(physRigidBodyLocal **const restrict array);
void modulePhysicsRigidBodyLocalClear();

physRigidBody *modulePhysicsRigidBodyAppendStatic(physRigidBody **const restrict array);
physRigidBody *modulePhysicsRigidBodyAppend(physRigidBody **const restrict array);
physRigidBody *modulePhysicsRigidBodyInsertAfterStatic(physRigidBody **const restrict array, physRigidBody *const restrict resource);
physRigidBody *modulePhysicsRigidBodyInsertAfter(physRigidBody **const restrict array, physRigidBody *const restrict resource);
physRigidBody *modulePhysicsRigidBodyNext(const physRigidBody *const restrict i);
void modulePhysicsRigidBodyFree(physRigidBody **const restrict array, physRigidBody *const restrict resource, const physRigidBody *const restrict previous);
void modulePhysicsRigidBodyFreeArray(physRigidBody **const restrict array);
void modulePhysicsRigidBodyClear();

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