#ifndef MODULESKELETON_H
#define MODULESKELETON_H

#include "skeleton.h"
#include "memoryPool.h"
#include "memorySLink.h"

#define RESOURCE_DEFAULT_SKELETON_SIZE sizeof(skeleton)
#define RESOURCE_DEFAULT_SKELETON_NUM 4096

#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE sizeof(sklAnim)
#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM 4096

#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE sizeof(sklAnimFragment)
#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM 4096

#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE sizeof(sklAnimInstance)
#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM 4096

#define RESOURCE_SKELETON_CONSTANTS 1
#define RESOURCE_SKELETON_BLOCK_SIZE memPoolBlockSize(sizeof(skeleton))

// Forward declarations for inlining.
extern memoryPool  __SkeletonResourceArray;                   // Contains skeletons.
extern memoryPool  __SkeletonAnimationResourceArray;          // Contains sklAnims.
extern memorySLink __SkeletonAnimationFragmentResourceArray;  // Contains sklAnimFragments.
extern memorySLink __SkeletonAnimationInstanceResourceArray;  // Contains sklAnimInstances.

/** Support locals? Merge all module containers? **/

return_t moduleSkeletonResourcesInit();
return_t moduleSkeletonResourcesInitConstants();
void moduleSkeletonResourcesReset();
void moduleSkeletonResourcesDelete();

skeleton *moduleSkeletonGetDefault();
skeleton *moduleSkeletonAllocateStatic();
skeleton *moduleSkeletonAllocate();
void moduleSkeletonFree(skeleton *const restrict resource);
skeleton *moduleSkeletonFind(const char *const restrict name);
void moduleSkeletonClear();

sklAnim *moduleSkeletonAnimationAllocateStatic();
sklAnim *moduleSkeletonAnimationAllocate();
void moduleSkeletonAnimationFree(sklAnim *const restrict resource);
sklAnim *moduleSkeletonAnimationFind(const char *const restrict name);
void moduleSkeletonAnimationClear();

sklAnimInstance *moduleSkeletonAnimationInstanceAppendStatic(sklAnimInstance **const restrict array);
sklAnimInstance *moduleSkeletonAnimationInstanceAppend(sklAnimInstance **const restrict array);
sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfterStatic(sklAnimInstance **const restrict array, sklAnimInstance *const restrict resource);
sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfter(sklAnimInstance **const restrict array, sklAnimInstance *const restrict resource);
sklAnimInstance *moduleSkeletonAnimationInstanceNext(const sklAnimInstance *const restrict i);
void moduleSkeletonAnimationInstanceFree(sklAnimInstance **const restrict array, sklAnimInstance *const restrict resource, const sklAnimInstance *const restrict previous);
void moduleSkeletonAnimationInstanceFreeArray(sklAnimInstance **const restrict array);
void moduleSkeletonAnimationInstanceClear();

sklAnimFragment *moduleSkeletonAnimationFragmentAppendStatic(sklAnimFragment **const restrict array);
sklAnimFragment *moduleSkeletonAnimationFragmentAppend(sklAnimFragment **const restrict array);
sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfterStatic(sklAnimFragment **const restrict array, sklAnimFragment *const restrict resource);
sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfter(sklAnimFragment **const restrict array, sklAnimFragment *const restrict resource);
sklAnimFragment *moduleSkeletonAnimationFragmentNext(const sklAnimFragment *const restrict i);
void moduleSkeletonAnimationFragmentFree(sklAnimFragment **const restrict array, sklAnimFragment *const restrict resource, const sklAnimFragment *const restrict previous);
void moduleSkeletonAnimationFragmentFreeArray(sklAnimFragment **const restrict array);
void moduleSkeletonAnimationFragmentClear();

#endif
