#include "moduleSkeleton.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

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
	__SkeletonResourceArray.region->next = NULL;
	region = __SkeletonAnimationResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__SkeletonAnimationResourceArray.region->next = NULL;
	region = __SkeletonAnimationFragmentResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__SkeletonAnimationFragmentResourceArray.region->next = NULL;
	region = __SkeletonAnimationInstanceResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__SkeletonAnimationInstanceResourceArray.region->next = NULL;
}
void moduleSkeletonResourcesDelete(){
	memoryRegion *region;
	sklDelete(moduleSkeletonGetDefault());
	moduleSkeletonClear();
	region = __SkeletonResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	moduleSkeletonAnimationClear();
	region = __SkeletonAnimationResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	moduleSkeletonAnimationFragmentClear();
	region = __SkeletonAnimationFragmentResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	moduleSkeletonAnimationInstanceClear();
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
		void *const memory = memAllocate(
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
__FORCE_INLINE__ void moduleSkeletonFree(skeleton *const restrict resource){
	sklDelete(resource);
	memPoolFree(&__SkeletonResourceArray, (void *)resource);
}
skeleton *moduleSkeletonFind(const char *const restrict name){

	MEMORY_POOL_LOOP_BEGIN(__SkeletonResourceArray, i, skeleton *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__SkeletonResourceArray, i, return NULL;);

	return NULL;

}
void moduleSkeletonClear(){

	MEMORY_POOL_OFFSET_LOOP_BEGIN(
		__SkeletonResourceArray, i, skeleton *,
		__SkeletonResourceArray.region,
		(byte_t *)memPoolFirst(__SkeletonResourceArray.region) + RESOURCE_SKELETON_CONSTANTS * RESOURCE_SKELETON_BLOCK_SIZE
	);

		moduleSkeletonFree(i);

	MEMORY_POOL_OFFSET_LOOP_END(__SkeletonResourceArray, i, return;);

}

__FORCE_INLINE__ sklAnim *moduleSkeletonAnimationAllocateStatic(){
	return memPoolAllocate(&__SkeletonAnimationResourceArray);
}
__FORCE_INLINE__ sklAnim *moduleSkeletonAnimationAllocate(){
	sklAnim *r = memPoolAllocate(&__SkeletonAnimationResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
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
__FORCE_INLINE__ void moduleSkeletonAnimationFree(sklAnim *const restrict resource){
	sklaDelete(resource);
	memPoolFree(&__SkeletonAnimationResourceArray, (void *)resource);
}
sklAnim *moduleSkeletonAnimationFind(const char *const restrict name){

	MEMORY_POOL_LOOP_BEGIN(__SkeletonAnimationResourceArray, i, sklAnim *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__SkeletonAnimationResourceArray, i, return NULL;);

	return NULL;

}
void moduleSkeletonAnimationClear(){

	MEMORY_POOL_LOOP_BEGIN(__SkeletonAnimationResourceArray, i, sklAnim *);

		moduleSkeletonAnimationFree(i);

	MEMORY_POOL_LOOP_END(__SkeletonAnimationResourceArray, i, return;);

}

__FORCE_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceAppendStatic(sklAnimInstance **const restrict array){
	return memSLinkAppend(&__SkeletonAnimationInstanceResourceArray, (const void **)array);
}
__FORCE_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceAppend(sklAnimInstance **const restrict array){
	sklAnimInstance *r = memSLinkAppend(&__SkeletonAnimationInstanceResourceArray, (const void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM
			)
		);
		if(memSLinkExtend(&__SkeletonAnimationInstanceResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM)){
			r = memSLinkAppend(&__SkeletonAnimationInstanceResourceArray, (const void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfterStatic(sklAnimInstance *const restrict resource){
	return memSLinkInsertAfter(&__SkeletonAnimationInstanceResourceArray, resource);
}
__FORCE_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfter(sklAnimInstance *const restrict resource){
	sklAnimInstance *r = memSLinkInsertAfter(&__SkeletonAnimationInstanceResourceArray, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM
			)
		);
		if(memSLinkExtend(&__SkeletonAnimationInstanceResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM)){
			r = memSLinkInsertAfter(&__SkeletonAnimationInstanceResourceArray, resource);
		}
	}
	return r;
}
__FORCE_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceNext(const sklAnimInstance *const restrict i){
	return (sklAnimInstance *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void moduleSkeletonAnimationInstanceFree(sklAnimInstance **const restrict array, sklAnimInstance *const restrict resource, sklAnimInstance *const restrict previous){
	sklaiDelete(resource);
	memSLinkFree(&__SkeletonAnimationInstanceResourceArray, (const void **)array, (void *)resource, (const void *)previous);
}
void moduleSkeletonAnimationInstanceFreeArray(sklAnimInstance **const restrict array){
	sklAnimInstance *resource = *array;
	while(resource != NULL){
		sklaiDelete(resource);
		memSLinkFree(&__SkeletonAnimationInstanceResourceArray, (const void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void moduleSkeletonAnimationInstanceClear(){

	MEMORY_SLINK_LOOP_BEGIN(__SkeletonAnimationInstanceResourceArray, i, sklAnimInstance *);

		moduleSkeletonAnimationInstanceFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__SkeletonAnimationInstanceResourceArray, i, return;);

}

__FORCE_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentAppendStatic(sklAnimFragment **const restrict array){
	return memSLinkAppend(&__SkeletonAnimationFragmentResourceArray, (const void **)array);
}
__FORCE_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentAppend(sklAnimFragment **const restrict array){
	sklAnimFragment *r = memSLinkAppend(&__SkeletonAnimationFragmentResourceArray, (const void **)array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM
			)
		);
		if(memSLinkExtend(&__SkeletonAnimationFragmentResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM)){
			r = memSLinkAppend(&__SkeletonAnimationFragmentResourceArray, (const void **)array);
		}
	}
	return r;
}
__FORCE_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfterStatic(sklAnimFragment *const restrict resource){
	return memSLinkInsertAfter(&__SkeletonAnimationFragmentResourceArray, resource);
}
__FORCE_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfter(sklAnimFragment *const restrict resource){
	sklAnimFragment *r = memSLinkInsertAfter(&__SkeletonAnimationFragmentResourceArray, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE,
				RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM
			)
		);
		if(memSLinkExtend(&__SkeletonAnimationFragmentResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM)){
			r = memSLinkInsertAfter(&__SkeletonAnimationFragmentResourceArray, resource);
		}
	}
	return r;
}
__FORCE_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentNext(const sklAnimFragment *const restrict i){
	return (sklAnimFragment *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void moduleSkeletonAnimationFragmentFree(sklAnimFragment **const restrict array, sklAnimFragment *const restrict resource, sklAnimFragment *const restrict previous){
	memSLinkFree(&__SkeletonAnimationFragmentResourceArray, (const void **)array, (void *)resource, (const void *)previous);
}
void moduleSkeletonAnimationFragmentFreeArray(sklAnimFragment **const restrict array){
	sklAnimFragment *resource = *array;
	while(resource != NULL){
		memSLinkFree(&__SkeletonAnimationFragmentResourceArray, (const void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void moduleSkeletonAnimationFragmentClear(){

	MEMORY_SLINK_LOOP_BEGIN(__SkeletonAnimationFragmentResourceArray, i, sklAnimFragment *);

		moduleSkeletonAnimationFragmentFree(NULL, i, NULL);

	MEMORY_SLINK_LOOP_END(__SkeletonAnimationFragmentResourceArray, i, return;);

}