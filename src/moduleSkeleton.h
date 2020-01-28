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
