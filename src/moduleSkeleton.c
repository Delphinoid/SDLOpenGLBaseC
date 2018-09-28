#include "moduleSkeleton.h"
#include "memoryPool.h"
#include "memorySLink.h"
#include "inline.h"
#include <string.h>

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

static memoryPool  __SkeletonResourceArray;                   // Contains skeletons.
static memoryPool  __SkeletalAnimationResourceArray;          // Contains sklAnims.
static memorySLink __SkeletalAnimationFragmentResourceArray;  // Contains sklAnimFragments.
static memorySLink __SkeletalAnimationInstanceResourceArray;  // Contains sklAnimInstances.

return_t moduleSkeletonResourcesInit(){
	void *memory = memForceAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SKELETON_SIZE,
			RESOURCE_DEFAULT_SKELETON_NUM
		)
	);
	if(memPoolCreate(&__SkeletonResourceArray, memory, RESOURCE_DEFAULT_SKELETON_SIZE, RESOURCE_DEFAULT_SKELETON_NUM) == NULL){
		return -1;
	}
	memory = memForceAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM
		)
	);
	if(memPoolCreate(&__SkeletalAnimationResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM) == NULL){
		return -1;
	}
	memory = memForceAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM
		)
	);
	if(memSLinkCreate(&__SkeletalAnimationFragmentResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM) == NULL){
		return -1;
	}
	memory = memForceAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM
		)
	);
	if(memSLinkCreate(&__SkeletalAnimationInstanceResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleSkeletonResourcesDelete(){
	memoryRegion *region;
	moduleSkeletonClear();
	moduleSkeletalAnimationClear();
	region = __SkeletonResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	region = __SkeletalAnimationResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	region = __SkeletalAnimationFragmentResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	region = __SkeletalAnimationInstanceResourceArray.region;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
}

__FORCE_INLINE__ skeleton *moduleSkeletonGetDefault(){
	return memPoolFirst(__SkeletonResourceArray);
}
__FORCE_INLINE__ skeleton *moduleSkeletonAllocate(){
	return memPoolAllocate(&__SkeletonResourceArray);
}
__FORCE_INLINE__ void moduleSkeletonFree(skeleton *resource){
	memPoolFree(&__SkeletonResourceArray, (void *)resource);
}
skeleton *moduleSkeletonFind(const char *name){

	memoryRegion *region = __SkeletonResourceArray.region;
	do {
		skeleton *i = memPoolFirst(__SkeletonResourceArray);
		while(i < (skeleton *)memPoolEnd(__SkeletonResourceArray)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				// Compare the resources' names.
				if(strcmp(name, i->name) == 0){
					return i;
				}

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return NULL;
			}
			memPoolBlockNext(__SkeletonResourceArray, i);
		}
		region = memPoolChunkNext(__SkeletonResourceArray);
	} while(region != NULL);

	return NULL;

}
void moduleSkeletonClear(){

	memoryRegion *region = __SkeletonResourceArray.region;
	do {
		skeleton *i = memPoolFirst(__SkeletonResourceArray);
		while(i < (skeleton *)memPoolEnd(__SkeletonResourceArray)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				sklDelete(i);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			memPoolBlockNext(__SkeletonResourceArray, i);
		}
		region = memPoolChunkNext(__SkeletonResourceArray);
	} while(region != NULL);

}

__FORCE_INLINE__ sklAnim *moduleSkeletalAnimationAllocate(){
	return memPoolAllocate(&__SkeletalAnimationResourceArray);
}
__FORCE_INLINE__ void moduleSkeletalAnimationFree(sklAnim *resource){
	memPoolFree(&__SkeletalAnimationResourceArray, (void *)resource);
}
sklAnim *moduleSkeletalAnimationFind(const char *name){

	memoryRegion *region = __SkeletalAnimationResourceArray.region;
	do {
		sklAnim *i = memPoolFirst(__SkeletalAnimationResourceArray);
		while(i < (sklAnim *)memPoolEnd(__SkeletalAnimationResourceArray)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				// Compare the resources' names.
				if(strcmp(name, i->name) == 0){
					return i;
				}

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return NULL;
			}
			memPoolBlockNext(__SkeletalAnimationResourceArray, i);
		}
		region = memPoolChunkNext(__SkeletalAnimationResourceArray);
	} while(region != NULL);

	return NULL;

}
void moduleSkeletalAnimationClear(){

	memoryRegion *region = __SkeletalAnimationResourceArray.region;
	do {
		sklAnim *i = memPoolFirst(__SkeletalAnimationResourceArray);
		while(i < (sklAnim *)memPoolEnd(__SkeletalAnimationResourceArray)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				sklaDelete(i);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			memPoolBlockNext(__SkeletalAnimationResourceArray, i);
		}
		region = memPoolChunkNext(__SkeletalAnimationResourceArray);
	} while(region != NULL);

}

__FORCE_INLINE__ sklAnimFragment *moduleSkeletalAnimationFragmentAppend(sklAnimFragment **array){
	return memSLinkAppend(&__SkeletalAnimationFragmentResourceArray, (void **)array);
}
__FORCE_INLINE__ void moduleSkeletalAnimationFragmentFree(sklAnimFragment **array, sklAnimFragment *resource, sklAnimFragment *previous){
	memSLinkFree(&__SkeletalAnimationFragmentResourceArray, (void **)array, (void *)resource, (void *)previous);
}

__FORCE_INLINE__ sklAnimInstance *moduleSkeletalAnimationInstanceAllocate(sklAnimInstance **array){
	return memSLinkAppend(&__SkeletalAnimationInstanceResourceArray, (void **)array);
}
__FORCE_INLINE__ void moduleSkeletalAnimationInstanceFree(sklAnimInstance **array, sklAnimInstance *resource, sklAnimInstance *previous){
	memSLinkFree(&__SkeletalAnimationInstanceResourceArray, (void **)array, (void *)resource, (void *)previous);
}
