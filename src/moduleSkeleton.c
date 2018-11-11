#include "moduleSkeleton.h"
#include "moduleSettings.h"
#include "memoryManager.h"
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

#define RESOURCE_SKELETON_CONSTANTS  1
#define RESOURCE_SKELETON_BLOCK_SIZE memPoolBlockSize(sizeof(skeleton))

memoryPool  __SkeletonResourceArray;                   // Contains skeletons.
memoryPool  __SkeletonAnimationResourceArray;          // Contains sklAnims.
memorySLink __SkeletonAnimationFragmentResourceArray;  // Contains sklAnimFragments.
memorySLink __SkeletonAnimationInstanceResourceArray;  // Contains sklAnimInstances.

return_t moduleSkeletonResourcesInit(){
	void *memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SKELETON_SIZE,
			RESOURCE_DEFAULT_SKELETON_NUM
		)
	);
	if(memPoolCreate(&__SkeletonResourceArray, memory, RESOURCE_DEFAULT_SKELETON_SIZE, RESOURCE_DEFAULT_SKELETON_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM
		)
	);
	if(memPoolCreate(&__SkeletonAnimationResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM
		)
	);
	if(memSLinkCreate(&__SkeletonAnimationFragmentResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM
		)
	);
	if(memSLinkCreate(&__SkeletonAnimationInstanceResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM) == NULL){
		return -1;
	}
	return 1;
}
return_t moduleSkeletonResourcesInitConstants(){
	skeleton *tempSkl = moduleSkeletonAllocateStatic();
	if(tempSkl == NULL){
		return -1;
	}
	sklDefault(tempSkl);
	return 1;
}
void moduleSkeletonResourcesReset(){
	memoryRegion *region;
	moduleSkeletonAnimationClear();
	moduleSkeletonClear();
	region = __SkeletonResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	region = __SkeletonAnimationResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	region = __SkeletonAnimationFragmentResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	region = __SkeletonAnimationInstanceResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
}
void moduleSkeletonResourcesDelete(){
	memoryRegion *region;
	moduleSkeletonAnimationClear();
	moduleSkeletonClear();
	sklDelete(moduleSkeletonGetDefault());
	region = __SkeletonResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	region = __SkeletonAnimationResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	region = __SkeletonAnimationFragmentResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	region = __SkeletonAnimationInstanceResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__FORCE_INLINE__ skeleton *moduleSkeletonGetDefault(){
	return memPoolFirst(__SkeletonResourceArray.region);
}
__FORCE_INLINE__ skeleton *moduleSkeletonAllocateStatic(){
	return memPoolAllocate(&__SkeletonResourceArray);
}
__FORCE_INLINE__ skeleton *moduleSkeletonAllocate(){
	skeleton *r = memPoolAllocate(&__SkeletonResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SKELETON_SIZE,
				RESOURCE_DEFAULT_SKELETON_NUM
			)
		);
		if(memPoolExtend(&__SkeletonResourceArray, memory, RESOURCE_DEFAULT_SKELETON_SIZE, RESOURCE_DEFAULT_SKELETON_NUM)){
			r = memPoolAllocate(&__SkeletonResourceArray);
		}
	}
	return r;
}
__FORCE_INLINE__ void moduleSkeletonFree(skeleton *resource){
	memPoolFree(&__SkeletonResourceArray, (void *)resource);
}
skeleton *moduleSkeletonFind(const char *name){

	memoryRegion *region = __SkeletonResourceArray.region;
	skeleton *i;
	do {
		i = memPoolFirst(region);
		while(i < (skeleton *)memAllocatorEnd(region)){
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
		region = memAllocatorNext(region);
	} while(region != NULL);

	return NULL;

}
void moduleSkeletonClear(){

	memoryRegion *region = __SkeletonResourceArray.region;
	// Start after the constant resources.
	skeleton *i = (skeleton *)((byte_t *)memPoolFirst(region) + RESOURCE_SKELETON_CONSTANTS * RESOURCE_SKELETON_BLOCK_SIZE);
	for(;;){
		while(i < (skeleton *)memAllocatorEnd(region)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				sklDelete(i);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			memPoolBlockNext(__SkeletonResourceArray, i);
		}
		region = memAllocatorNext(region);
		if(region == NULL){
			return;
		}
		i = memPoolFirst(region);
	}

}

__FORCE_INLINE__ sklAnim *moduleSkeletonAnimationAllocateStatic(){
	return memPoolAllocate(&__SkeletonAnimationResourceArray);
}
__FORCE_INLINE__ sklAnim *moduleSkeletonAnimationAllocate(){
	sklAnim *r = memPoolAllocate(&__SkeletonAnimationResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM
			)
		);
		if(memPoolExtend(&__SkeletonAnimationResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM)){
			r = memPoolAllocate(&__SkeletonAnimationResourceArray);
		}
	}
	return r;
}
__FORCE_INLINE__ void moduleSkeletonAnimationFree(sklAnim *resource){
	memPoolFree(&__SkeletonAnimationResourceArray, (void *)resource);
}
sklAnim *moduleSkeletonAnimationFind(const char *name){

	memoryRegion *region = __SkeletonAnimationResourceArray.region;
	sklAnim *i;
	do {
		i = memPoolFirst(region);
		while(i < (sklAnim *)memAllocatorEnd(region)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				// Compare the resources' names.
				if(strcmp(name, i->name) == 0){
					return i;
				}

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return NULL;
			}
			memPoolBlockNext(__SkeletonAnimationResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

	return NULL;

}
void moduleSkeletonAnimationClear(){

	memoryRegion *region = __SkeletonAnimationResourceArray.region;
	sklAnim *i;
	do {
		i = memPoolFirst(region);
		while(i < (sklAnim *)memAllocatorEnd(region)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				sklaDelete(i);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			memPoolBlockNext(__SkeletonAnimationResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}

__FORCE_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentAppend(sklAnimFragment **array){
	return memSLinkAppend(&__SkeletonAnimationFragmentResourceArray, (void **)array);
}
__FORCE_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentAppendForced(sklAnimFragment **array){
	sklAnimFragment *r = memSLinkAppend(&__SkeletonAnimationFragmentResourceArray, (void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM
			)
		);
		if(memSLinkExtend(&__SkeletonAnimationFragmentResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM)){
			r = memSLinkAppend(&__SkeletonAnimationFragmentResourceArray, (void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ void moduleSkeletonAnimationFragmentFree(sklAnimFragment **array, sklAnimFragment *resource, sklAnimFragment *previous){
	memSLinkFree(&__SkeletonAnimationFragmentResourceArray, (void **)array, (void *)resource, (void *)previous);
}

__FORCE_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceAppend(sklAnimInstance **array){
	return memSLinkAppend(&__SkeletonAnimationInstanceResourceArray, (void **)array);
}
__FORCE_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceAppendForced(sklAnimInstance **array){
	sklAnimInstance *r = memSLinkAppend(&__SkeletonAnimationInstanceResourceArray, (void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM
			)
		);
		if(memSLinkExtend(&__SkeletonAnimationInstanceResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM)){
			r = memSLinkAppend(&__SkeletonAnimationInstanceResourceArray, (void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ void moduleSkeletonAnimationInstanceFree(sklAnimInstance **array, sklAnimInstance *resource, sklAnimInstance *previous){
	memSLinkFree(&__SkeletonAnimationInstanceResourceArray, (void **)array, (void *)resource, (void *)previous);
}
