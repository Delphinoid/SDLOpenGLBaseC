#include "moduleSkeleton.h"
#include "moduleSettings.h"
#include "skeleton.h"
#include "memoryManager.h"
#include <string.h>

#define RESOURCE_DEFAULT_SKELETON_SIZE sizeof(skeleton)
#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE sizeof(sklAnim)
#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE sizeof(sklAnimFragment)
#define RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE sizeof(sklAnimInstance)
#define RESOURCE_SKELETON_BLOCK_SIZE memPoolBlockSize(sizeof(skeleton))

memoryPool  __g_SkeletonResourceArray;                   // Contains skeletons.
memoryPool  __g_SkeletonAnimationResourceArray;          // Contains sklAnims.
memorySLink __g_SkeletonAnimationFragmentResourceArray;  // Contains sklAnimFragments.
memorySLink __g_SkeletonAnimationInstanceResourceArray;  // Contains sklAnimInstances.

return_t moduleSkeletonResourcesInit(){
	void *memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SKELETON_SIZE,
			RESOURCE_DEFAULT_SKELETON_NUM
		)
	);
	if(memPoolCreate(&__g_SkeletonResourceArray, memory, RESOURCE_DEFAULT_SKELETON_SIZE, RESOURCE_DEFAULT_SKELETON_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM
		)
	);
	if(memPoolCreate(&__g_SkeletonAnimationResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM
		)
	);
	if(memSLinkCreate(&__g_SkeletonAnimationFragmentResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE,
			RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM
		)
	);
	if(memSLinkCreate(&__g_SkeletonAnimationInstanceResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleSkeletonResourcesReset(){
	memoryRegion *region;
	moduleSkeletonClear();
	region = __g_SkeletonResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_SkeletonResourceArray.region->next = NULL;
	moduleSkeletonAnimationClear();
	region = __g_SkeletonAnimationResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_SkeletonAnimationResourceArray.region->next = NULL;
	moduleSkeletonAnimationFragmentClear();
	region = __g_SkeletonAnimationFragmentResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_SkeletonAnimationFragmentResourceArray.region->next = NULL;
	moduleSkeletonAnimationInstanceClear();
	region = __g_SkeletonAnimationInstanceResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		memFree(region->start);
		region = next;
	}
	__g_SkeletonAnimationInstanceResourceArray.region->next = NULL;
}
void moduleSkeletonResourcesDelete(){
	memoryRegion *region;
	moduleSkeletonClear();
	region = __g_SkeletonResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	moduleSkeletonAnimationClear();
	region = __g_SkeletonAnimationResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	moduleSkeletonAnimationFragmentClear();
	region = __g_SkeletonAnimationFragmentResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	moduleSkeletonAnimationInstanceClear();
	region = __g_SkeletonAnimationInstanceResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	sklDelete(&g_sklDefault);
}

__HINT_INLINE__ skeleton *moduleSkeletonAllocateStatic(){
	return memPoolAllocate(&__g_SkeletonResourceArray);
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
		if(memPoolExtend(&__g_SkeletonResourceArray, memory, RESOURCE_DEFAULT_SKELETON_SIZE, RESOURCE_DEFAULT_SKELETON_NUM)){
			r = moduleSkeletonAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void moduleSkeletonFree(skeleton *const __RESTRICT__ resource){
	sklDelete(resource);
	memPoolFree(&__g_SkeletonResourceArray, (void *)resource);
}
skeleton *moduleSkeletonFind(const char *const __RESTRICT__ name){

	if(strcmp(name, g_sklDefault.name) == 0){
		return &g_sklDefault;
	}

	MEMORY_POOL_LOOP_BEGIN(__g_SkeletonResourceArray, i, skeleton *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__g_SkeletonResourceArray, i, return NULL;);

	return NULL;

}
void moduleSkeletonClear(){

	MEMORY_POOL_LOOP_BEGIN(__g_SkeletonResourceArray, i, skeleton *);

		moduleSkeletonFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__g_SkeletonResourceArray, i, return;);

}

__HINT_INLINE__ sklAnim *moduleSkeletonAnimationAllocateStatic(){
	return memPoolAllocate(&__g_SkeletonAnimationResourceArray);
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
		if(memPoolExtend(&__g_SkeletonAnimationResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM)){
			r = moduleSkeletonAnimationAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void moduleSkeletonAnimationFree(sklAnim *const __RESTRICT__ resource){
	sklaDelete(resource);
	memPoolFree(&__g_SkeletonAnimationResourceArray, (void *)resource);
}
sklAnim *moduleSkeletonAnimationFind(const char *const __RESTRICT__ name){

	MEMORY_POOL_LOOP_BEGIN(__g_SkeletonAnimationResourceArray, i, sklAnim *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__g_SkeletonAnimationResourceArray, i, return NULL;);

	return NULL;

}
void moduleSkeletonAnimationClear(){

	MEMORY_POOL_LOOP_BEGIN(__g_SkeletonAnimationResourceArray, i, sklAnim *);

		moduleSkeletonAnimationFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__g_SkeletonAnimationResourceArray, i, return;);

}

__HINT_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceAppendStatic(sklAnimInstance **const __RESTRICT__ array){
	return memSLinkAppend(&__g_SkeletonAnimationInstanceResourceArray, (void **)array);
}
__HINT_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceAppend(sklAnimInstance **const __RESTRICT__ array){
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
		if(memSLinkExtend(&__g_SkeletonAnimationInstanceResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM)){
			r = moduleSkeletonAnimationInstanceAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfterStatic(sklAnimInstance **const __RESTRICT__ array, sklAnimInstance *const __RESTRICT__ resource){
	return memSLinkInsertAfter(&__g_SkeletonAnimationInstanceResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfter(sklAnimInstance **const __RESTRICT__ array, sklAnimInstance *const __RESTRICT__ resource){
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
		if(memSLinkExtend(&__g_SkeletonAnimationInstanceResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM)){
			r = moduleSkeletonAnimationInstanceInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceNext(const sklAnimInstance *const __RESTRICT__ i){
	return (sklAnimInstance *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void moduleSkeletonAnimationInstanceFree(sklAnimInstance **const __RESTRICT__ array, sklAnimInstance *const __RESTRICT__ resource, const sklAnimInstance *const __RESTRICT__ previous){
	sklaiDelete(resource);
	memSLinkFree(&__g_SkeletonAnimationInstanceResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void moduleSkeletonAnimationInstanceFreeArray(sklAnimInstance **const __RESTRICT__ array){
	sklAnimInstance *resource = *array;
	while(resource != NULL){
		moduleSkeletonAnimationInstanceFree(array, resource, NULL);
		resource = *array;
	}
}
void moduleSkeletonAnimationInstanceClear(){

	MEMORY_SLINK_LOOP_BEGIN(__g_SkeletonAnimationInstanceResourceArray, i, sklAnimInstance *);

		moduleSkeletonAnimationInstanceFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__g_SkeletonAnimationInstanceResourceArray, i, return;);

}

__HINT_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentAppendStatic(sklAnimFragment **const __RESTRICT__ array){
	return memSLinkAppend(&__g_SkeletonAnimationFragmentResourceArray, (void **)array);
}
__HINT_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentAppend(sklAnimFragment **const __RESTRICT__ array){
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
		if(memSLinkExtend(&__g_SkeletonAnimationFragmentResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM)){
			r = moduleSkeletonAnimationFragmentAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfterStatic(sklAnimFragment **const __RESTRICT__ array, sklAnimFragment *const __RESTRICT__ resource){
	return memSLinkInsertAfter(&__g_SkeletonAnimationFragmentResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfter(sklAnimFragment **const __RESTRICT__ array, sklAnimFragment *const __RESTRICT__ resource){
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
		if(memSLinkExtend(&__g_SkeletonAnimationFragmentResourceArray, memory, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE, RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM)){
			r = moduleSkeletonAnimationFragmentInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentNext(const sklAnimFragment *const __RESTRICT__ i){
	return (sklAnimFragment *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void moduleSkeletonAnimationFragmentFree(sklAnimFragment **const __RESTRICT__ array, sklAnimFragment *const __RESTRICT__ resource, const sklAnimFragment *const __RESTRICT__ previous){
	memSLinkFree(&__g_SkeletonAnimationFragmentResourceArray, (void **)array, (void *)resource, (void *)previous);
}
void moduleSkeletonAnimationFragmentFreeArray(sklAnimFragment **const __RESTRICT__ array){
	sklAnimFragment *resource = *array;
	while(resource != NULL){
		moduleSkeletonAnimationFragmentFree(array, resource, NULL);
		resource = *array;
	}
}
void moduleSkeletonAnimationFragmentClear(){

	MEMORY_SLINK_LOOP_BEGIN(__g_SkeletonAnimationFragmentResourceArray, i, sklAnimFragment *);

		moduleSkeletonAnimationFragmentFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__g_SkeletonAnimationFragmentResourceArray, i, return;);

}