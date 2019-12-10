#include "moduleSkeleton.h"
#include "moduleSettings.h"
#include "skeleton.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

#define RESOURCE_DEFAULT_SKELETON_SIZE sizeof(skeleton)
#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE sizeof(sklAnim)
#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE sizeof(sklAnimFragment)
#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE sizeof(sklAnimInstance)
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
void moduleSkeletonResourcesReset(){
	memoryRegion *region;
	moduleSkeletonClear();
	region = __SkeletonResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__SkeletonResourceArray.region->next = NULL;
	moduleSkeletonAnimationClear();
	region = __SkeletonAnimationResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__SkeletonAnimationResourceArray.region->next = NULL;
	moduleSkeletonAnimationFragmentClear();
	region = __SkeletonAnimationFragmentResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__SkeletonAnimationFragmentResourceArray.region->next = NULL;
	moduleSkeletonAnimationInstanceClear();
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
	sklDelete(&sklDefault);
}

__HINT_INLINE__ skeleton *moduleSkeletonAllocateStatic(){
	return memPoolAllocate(&__SkeletonResourceArray);
}
__HINT_INLINE__ skeleton *moduleSkeletonAllocate(){
	skeleton *r = moduleSkeletonAllocateStatic();
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
			r = moduleSkeletonAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void moduleSkeletonFree(skeleton *const restrict resource){
	sklDelete(resource);
	memPoolFree(&__SkeletonResourceArray, (void *)resource);
}
skeleton *moduleSkeletonFind(const char *const restrict name){

	if(strcmp(name, sklDefault.name) == 0){
		return &sklDefault;
	}

	MEMORY_POOL_LOOP_BEGIN(__SkeletonResourceArray, i, skeleton *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__SkeletonResourceArray, i, return NULL;);

	return NULL;

}
void moduleSkeletonClear(){

	MEMORY_POOL_LOOP_BEGIN(__SkeletonResourceArray, i, skeleton *);

		moduleSkeletonFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__SkeletonResourceArray, i, return;);

}

__HINT_INLINE__ sklAnim *moduleSkeletonAnimationAllocateStatic(){
	return memPoolAllocate(&__SkeletonAnimationResourceArray);
}
__HINT_INLINE__ sklAnim *moduleSkeletonAnimationAllocate(){
	sklAnim *r = moduleSkeletonAnimationAllocateStatic();
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
			r = moduleSkeletonAnimationAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void moduleSkeletonAnimationFree(sklAnim *const restrict resource){
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
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__SkeletonAnimationResourceArray, i, return;);

}

__HINT_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceAppendStatic(sklAnimInstance **const restrict array){
	return memSLinkAppend(&__SkeletonAnimationInstanceResourceArray, (void **)array);
}
__HINT_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceAppend(sklAnimInstance **const restrict array){
	sklAnimInstance *r = moduleSkeletonAnimationInstanceAppendStatic(array);
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
			r = moduleSkeletonAnimationInstanceAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfterStatic(sklAnimInstance **const restrict array, sklAnimInstance *const restrict resource){
	return memSLinkInsertAfter(&__SkeletonAnimationInstanceResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfter(sklAnimInstance **const restrict array, sklAnimInstance *const restrict resource){
	sklAnimInstance *r = moduleSkeletonAnimationInstanceInsertAfterStatic(array, resource);
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
			r = moduleSkeletonAnimationInstanceInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceNext(const sklAnimInstance *const restrict i){
	return (sklAnimInstance *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void moduleSkeletonAnimationInstanceFree(sklAnimInstance **const restrict array, sklAnimInstance *const restrict resource, const sklAnimInstance *const restrict previous){
	sklaiDelete(resource);
	memSLinkFree(&__SkeletonAnimationInstanceResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void moduleSkeletonAnimationInstanceFreeArray(sklAnimInstance **const restrict array){
	sklAnimInstance *resource = *array;
	while(resource != NULL){
		moduleSkeletonAnimationInstanceFree(array, resource, NULL);
		resource = *array;
	}
}
void moduleSkeletonAnimationInstanceClear(){

	MEMORY_SLINK_LOOP_BEGIN(__SkeletonAnimationInstanceResourceArray, i, sklAnimInstance *);

		moduleSkeletonAnimationInstanceFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__SkeletonAnimationInstanceResourceArray, i, return;);

}

__HINT_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentAppendStatic(sklAnimFragment **const restrict array){
	return memSLinkAppend(&__SkeletonAnimationFragmentResourceArray, (void **)array);
}
__HINT_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentAppend(sklAnimFragment **const restrict array){
	sklAnimFragment *r = moduleSkeletonAnimationFragmentAppendStatic(array);
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
			r = moduleSkeletonAnimationFragmentAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfterStatic(sklAnimFragment **const restrict array, sklAnimFragment *const restrict resource){
	return memSLinkInsertAfter(&__SkeletonAnimationFragmentResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfter(sklAnimFragment **const restrict array, sklAnimFragment *const restrict resource){
	sklAnimFragment *r = moduleSkeletonAnimationFragmentInsertAfterStatic(array, resource);
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
			r = moduleSkeletonAnimationFragmentInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentNext(const sklAnimFragment *const restrict i){
	return (sklAnimFragment *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void moduleSkeletonAnimationFragmentFree(sklAnimFragment **const restrict array, sklAnimFragment *const restrict resource, const sklAnimFragment *const restrict previous){
	memSLinkFree(&__SkeletonAnimationFragmentResourceArray, (void **)array, (void *)resource, (void *)previous);
}
void moduleSkeletonAnimationFragmentFreeArray(sklAnimFragment **const restrict array){
	sklAnimFragment *resource = *array;
	while(resource != NULL){
		moduleSkeletonAnimationFragmentFree(array, resource, NULL);
		resource = *array;
	}
}
void moduleSkeletonAnimationFragmentClear(){

	MEMORY_SLINK_LOOP_BEGIN(__SkeletonAnimationFragmentResourceArray, i, sklAnimFragment *);

		moduleSkeletonAnimationFragmentFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__SkeletonAnimationFragmentResourceArray, i, return;);

}