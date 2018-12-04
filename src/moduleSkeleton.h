#ifndef MODULESKELETON_H
#define MODULESKELETON_H

#include "skeleton.h"
#include "memoryPool.h"
#include "memorySLink.h"

#define RESOURCE_DEFAULT_SKELETON_SIZE sizeof(skeleton)
#ifndef RESOURCE_DEFAULT_SKELETON_NUM
	#define RESOURCE_DEFAULT_SKELETON_NUM 4096
#endif

#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE sizeof(sklAnim)
#ifndef RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM
	#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM 4096
#endif

#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE sizeof(sklAnimFragment)
#ifndef RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM
	#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM 4096
#endif

#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE sizeof(sklAnimInstance)
#ifndef RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM
	#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM 4096
#endif

#ifndef RESOURCE_SKELETON_CONSTANTS
	#define RESOURCE_SKELETON_CONSTANTS 1
#endif
#define RESOURCE_SKELETON_BLOCK_SIZE memPoolBlockSize(sizeof(skeleton))

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
void moduleSkeletonFree(skeleton *resource);
skeleton *moduleSkeletonFind(const char *name);
void moduleSkeletonClear();

sklAnim *moduleSkeletonAnimationAllocateStatic();
sklAnim *moduleSkeletonAnimationAllocate();
void moduleSkeletonAnimationFree(sklAnim *resource);
sklAnim *moduleSkeletonAnimationFind(const char *name);
void moduleSkeletonAnimationClear();

sklAnimInstance *moduleSkeletonAnimationInstanceAppendStatic(sklAnimInstance **array);
sklAnimInstance *moduleSkeletonAnimationInstanceAppend(sklAnimInstance **array);
sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfterStatic(sklAnimInstance *resource);
sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfter(sklAnimInstance *resource);
sklAnimInstance *moduleSkeletonAnimationInstanceNext(sklAnimInstance *i);
void moduleSkeletonAnimationInstanceFree(sklAnimInstance **array, sklAnimInstance *resource, sklAnimInstance *previous);
void moduleSkeletonAnimationInstanceFreeArray(sklAnimInstance **array);
void moduleSkeletonAnimationInstanceClear();

sklAnimFragment *moduleSkeletonAnimationFragmentAppendStatic(sklAnimFragment **array);
sklAnimFragment *moduleSkeletonAnimationFragmentAppend(sklAnimFragment **array);
sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfterStatic(sklAnimFragment *resource);
sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfter(sklAnimFragment *resource);
sklAnimFragment *moduleSkeletonAnimationFragmentNext(sklAnimFragment *i);
void moduleSkeletonAnimationFragmentFree(sklAnimFragment **array, sklAnimFragment *resource, sklAnimFragment *previous);
void moduleSkeletonAnimationFragmentFreeArray(sklAnimFragment **array);
void moduleSkeletonAnimationFragmentClear();

#endif
