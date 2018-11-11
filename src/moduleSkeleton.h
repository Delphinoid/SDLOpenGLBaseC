#ifndef MODULESKELETON_H
#define MODULESKELETON_H

#include "skeleton.h"
#include "memoryPool.h"
#include "memorySLink.h"

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

sklAnimFragment *moduleSkeletonAnimationFragmentAppendFixed(sklAnimFragment **array);
sklAnimFragment *moduleSkeletonAnimationFragmentAppendForced(sklAnimFragment **array);
void moduleSkeletonAnimationFragmentFree(sklAnimFragment **array, sklAnimFragment *resource, sklAnimFragment *previous);

sklAnimInstance *moduleSkeletonAnimationInstanceAppendFixed(sklAnimInstance **array);
sklAnimInstance *moduleSkeletonAnimationInstanceAppendForced(sklAnimInstance **array);
void moduleSkeletonAnimationInstanceFree(sklAnimInstance **array, sklAnimInstance *resource, sklAnimInstance *previous);

#endif
