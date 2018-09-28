#ifndef MODULESKELETON_H
#define MODULESKELETON_H

#include "moduleSettings.h"
#include "memoryManager.h"
#include "skeleton.h"

/** Support locals? Merge all module containers? **/

return_t moduleSkeletonResourcesInit();
void moduleSkeletonResourcesDelete();

skeleton *moduleSkeletonGetDefault();
skeleton *moduleSkeletonAllocate();
void moduleSkeletonFree(skeleton *resource);
skeleton *moduleSkeletonFind(const char *name);
void moduleSkeletonClear();

sklAnim *moduleSkeletalAnimationAllocate();
void moduleSkeletalAnimationFree(sklAnim *resource);
sklAnim *moduleSkeletalAnimationFind(const char *name);
void moduleSkeletalAnimationClear();

sklAnimFragment *moduleSkeletalAnimationFragmentAppend(sklAnimFragment **array);
void moduleSkeletalAnimationFragmentFree(sklAnimFragment **array, sklAnimFragment *resource, sklAnimFragment *previous);

sklAnimInstance *moduleSkeletalAnimationInstanceAllocate(sklAnimInstance **array);
void moduleSkeletalAnimationInstanceFree(sklAnimInstance **array, sklAnimInstance *resource, sklAnimInstance *previous);

#endif
