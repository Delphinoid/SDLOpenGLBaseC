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
	sklDelete(resource);
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
			i = memPoolBlockNext(__SkeletonResourceArray, i);
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

				moduleSkeletonFree(i);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			i = memPoolBlockNext(__SkeletonResourceArray, i);
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
	sklaDelete(resource);
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
			i = memPoolBlockNext(__SkeletonAnimationResourceArray, i);
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

				moduleSkeletonAnimationFree(i);

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			i = memPoolBlockNext(__SkeletonAnimationResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}

__FORCE_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceAppendStatic(sklAnimInstance **array){
	return memSLinkAppend(&__SkeletonAnimationInstanceResourceArray, (void **)array);
}
__FORCE_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceAppend(sklAnimInstance **array){
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
__FORCE_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfterStatic(sklAnimInstance *resource){
	return memSLinkInsertAfter(&__SkeletonAnimationInstanceResourceArray, resource);
}
__FORCE_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceInsertAfter(sklAnimInstance *resource){
	sklAnimInstance *r = memSLinkInsertAfter(&__SkeletonAnimationInstanceResourceArray, resource);
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
			r = memSLinkInsertAfter(&__SkeletonAnimationInstanceResourceArray, resource);
		}
	}
	return r;
}
__FORCE_INLINE__ sklAnimInstance *moduleSkeletonAnimationInstanceNext(sklAnimInstance *i){
	return (sklAnimInstance *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void moduleSkeletonAnimationInstanceFree(sklAnimInstance **array, sklAnimInstance *resource, sklAnimInstance *previous){
	sklaiDelete(resource);
	memSLinkFree(&__SkeletonAnimationInstanceResourceArray, (void **)array, (void *)resource, (void *)previous);
}
void moduleSkeletonAnimationInstanceFreeArray(sklAnimInstance **array){
	sklAnimInstance *resource = *array;
	while(resource != NULL){
		sklaiDelete(resource);
		memSLinkFree(&__SkeletonAnimationInstanceResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void moduleSkeletonAnimationInstanceClear(){

	memoryRegion *region = __SkeletonAnimationInstanceResourceArray.region;
	sklAnimInstance *i;
	do {
		i = memSLinkFirst(region);
		while(i < (sklAnimInstance *)memAllocatorEnd(region)){
			const byte_t flag = memSLinkBlockStatus(i);
			if(flag == MEMORY_SLINK_BLOCK_ACTIVE){

				moduleSkeletonAnimationInstanceFree(NULL, i, NULL);

			}else if(flag == MEMORY_SLINK_BLOCK_INVALID){
				return;
			}
			i = memSLinkBlockNext(__SkeletonAnimationInstanceResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}

__FORCE_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentAppendStatic(sklAnimFragment **array){
	return memSLinkAppend(&__SkeletonAnimationFragmentResourceArray, (void **)array);
}
__FORCE_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentAppend(sklAnimFragment **array){
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
__FORCE_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfterStatic(sklAnimFragment *resource){
	return memSLinkInsertAfter(&__SkeletonAnimationFragmentResourceArray, resource);
}
__FORCE_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentInsertAfter(sklAnimFragment *resource){
	sklAnimFragment *r = memSLinkInsertAfter(&__SkeletonAnimationFragmentResourceArray, resource);
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
			r = memSLinkInsertAfter(&__SkeletonAnimationFragmentResourceArray, resource);
		}
	}
	return r;
}
__FORCE_INLINE__ sklAnimFragment *moduleSkeletonAnimationFragmentNext(sklAnimFragment *i){
	return (sklAnimFragment *)memSLinkDataGetNext(i);
}
__FORCE_INLINE__ void moduleSkeletonAnimationFragmentFree(sklAnimFragment **array, sklAnimFragment *resource, sklAnimFragment *previous){
	memSLinkFree(&__SkeletonAnimationFragmentResourceArray, (void **)array, (void *)resource, (void *)previous);
}
void moduleSkeletonAnimationFragmentFreeArray(sklAnimFragment **array){
	sklAnimFragment *resource = *array;
	while(resource != NULL){
		memSLinkFree(&__SkeletonAnimationFragmentResourceArray, (void **)array, (void *)resource, NULL);
		resource = *array;
	}
}
void moduleSkeletonAnimationFragmentClear(){

	memoryRegion *region = __SkeletonAnimationFragmentResourceArray.region;
	sklAnimFragment *i;
	do {
		i = memSLinkFirst(region);
		while(i < (sklAnimFragment *)memAllocatorEnd(region)){
			const byte_t flag = memSLinkBlockStatus(i);
			if(flag == MEMORY_SLINK_BLOCK_ACTIVE){

				moduleSkeletonAnimationFragmentFree(NULL, i, NULL);

			}else if(flag == MEMORY_SLINK_BLOCK_INVALID){
				return;
			}
			i = memSLinkBlockNext(__SkeletonAnimationFragmentResourceArray, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}