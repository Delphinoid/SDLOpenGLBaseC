#ifndef MODULESKELETON_H
#define MODULESKELETON_H

#include "memoryPool.h"
#include "memorySLink.h"
#include "return.h"

#define RESOURCE_DEFAULT_SKELETON_NUM 4096
#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM 4096
#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM 4096
#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM 4096
#define RESOURCE_SKELETON_CONSTANTS 1

// Forward declarations for inlining.
extern memoryPool  __g_SkeletonResourceArray;                   // Contains skeletons.
extern memoryPool  __g_SkeletonAnimationResourceArray;          // Contains sklAnims.
extern memorySLink __g_SkeletonAnimationFragmentResourceArray;  // Contains sklAnimFragments.
extern memorySLink __g_SkeletonAnimationInstanceResourceArray;  // Contains sklAnimInstances.

typedef struct skeleton skeleton;
typedef struct sklAnim sklAnim;
typedef struct sklAnimFragment sklAnimFragment;
typedef struct sklAnimInstance sklAnimInstance;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleSkeletonResourcesInit();
void moduleSkeletonResourcesReset();
void moduleSkeletonResourcesDelete();

skeleton *moduleSkeletonAllocateStatic();
skeleton *moduleSkeletonAllocate();
void moduleSkeletonFree(skeleton *const __RESTRICT__ resource);
skeleton *moduleSkeletonFind(const char *const __RESTRICT__ name, const size_t nameLength);
void moduleSkeletonClear();

sklAnim *moduleSkeletonAnimationAllocateStatic();
sklAnim *moduleSkeletonAnimationAllocate();
void moduleSkeletonAnimationFree(sklAnim *const __RESTRICT__ resource);
sklAnim *moduleSkeletonAnimationFind(const char *const __RESTRICT__ name, const size_t nameLength);
void moduleSkeletonAnimationClear();

sklAnimInstance *moduleSkeletonAnimationInstanceAppendStatic(sklAnimInstance **const __RESTRICT__ array);
sklAnimInstance *moduleSkeletonAnimationInstanceAppend(sklAnimInstance **const __RESTRICT__ array);
sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfterStatic(sklAnimInstance **const __RESTRICT__ array, sklAnimInstance *const __RESTRICT__ resource);
sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfter(sklAnimInstance **const __RESTRICT__ array, sklAnimInstance *const __RESTRICT__ resource);
sklAnimInstance *moduleSkeletonAnimationInstanceNext(const sklAnimInstance *const __RESTRICT__ i);
void moduleSkeletonAnimationInstanceFree(sklAnimInstance **const __RESTRICT__ array, sklAnimInstance *const __RESTRICT__ resource, const sklAnimInstance *const __RESTRICT__ previous);
void moduleSkeletonAnimationInstanceFreeArray(sklAnimInstance **const __RESTRICT__ array);
void moduleSkeletonAnimationInstanceClear();

sklAnimFragment *moduleSkeletonAnimationFragmentAppendStatic(sklAnimFragment **const __RESTRICT__ array);
sklAnimFragment *moduleSkeletonAnimationFragmentAppend(sklAnimFragment **const __RESTRICT__ array);
sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfterStatic(sklAnimFragment **const __RESTRICT__ array, sklAnimFragment *const __RESTRICT__ resource);
sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfter(sklAnimFragment **const __RESTRICT__ array, sklAnimFragment *const __RESTRICT__ resource);
sklAnimFragment *moduleSkeletonAnimationFragmentNext(const sklAnimFragment *const __RESTRICT__ i);
void moduleSkeletonAnimationFragmentFree(sklAnimFragment **const __RESTRICT__ array, sklAnimFragment *const __RESTRICT__ resource, const sklAnimFragment *const __RESTRICT__ previous);
void moduleSkeletonAnimationFragmentFreeArray(sklAnimFragment **const __RESTRICT__ array);
void moduleSkeletonAnimationFragmentClear();

#endif
