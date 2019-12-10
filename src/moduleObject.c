#include "moduleObject.h"
#include "moduleSettings.h"
#include "object.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

#define RESOURCE_DEFAULT_OBJECT_BASE_SIZE sizeof(objectBase)
#define RESOURCE_DEFAULT_OBJECT_SIZE sizeof(object)

memoryPool __ObjectBaseResourceArray;  // Contains objectBases.
memoryPool __ObjectResourceArray;      // Contains objects.

return_t moduleObjectResourcesInit(){
	void *memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_OBJECT_BASE_SIZE,
			RESOURCE_DEFAULT_OBJECT_BASE_NUM
		)
	);
	if(memPoolCreate(&__ObjectBaseResourceArray, memory, RESOURCE_DEFAULT_OBJECT_BASE_SIZE, RESOURCE_DEFAULT_OBJECT_BASE_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_OBJECT_SIZE,
			RESOURCE_DEFAULT_OBJECT_NUM
		)
	);
	if(memPoolCreate(&__ObjectResourceArray, memory, RESOURCE_DEFAULT_OBJECT_SIZE, RESOURCE_DEFAULT_OBJECT_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleObjectResourcesReset(){
	memoryRegion *region;
	moduleObjectBaseClear();
	region = __ObjectBaseResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__ObjectBaseResourceArray.region->next = NULL;
	moduleObjectClear();
	region = __ObjectResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__ObjectResourceArray.region->next = NULL;
}
void moduleObjectResourcesDelete(){
	memoryRegion *region;
	moduleObjectBaseClear();
	region = __ObjectBaseResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	moduleObjectClear();
	region = __ObjectResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__HINT_INLINE__ objectBase *moduleObjectBaseAllocateStatic(){
	return memPoolAllocate(&__ObjectBaseResourceArray);
}
__HINT_INLINE__ objectBase *moduleObjectBaseAllocate(){
	objectBase *r = moduleObjectBaseAllocateStatic();
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_OBJECT_BASE_SIZE,
				RESOURCE_DEFAULT_OBJECT_BASE_NUM
			)
		);
		if(memPoolExtend(&__ObjectBaseResourceArray, memory, RESOURCE_DEFAULT_OBJECT_BASE_SIZE, RESOURCE_DEFAULT_OBJECT_BASE_NUM)){
			r = moduleObjectBaseAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void moduleObjectBaseFree(objectBase *const restrict resource){
	objBaseDelete(resource);
	memPoolFree(&__ObjectBaseResourceArray, (void *)resource);
}
objectBase *moduleObjectBaseFind(const char *const restrict name){

	MEMORY_POOL_LOOP_BEGIN(__ObjectBaseResourceArray, i, objectBase *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__ObjectBaseResourceArray, i, return NULL;);

	return NULL;

}
void moduleObjectBaseClear(){

	MEMORY_POOL_LOOP_BEGIN(__ObjectBaseResourceArray, i, objectBase *);

		moduleObjectBaseFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__ObjectBaseResourceArray, i, return;);

}

__HINT_INLINE__ object *moduleObjectAllocateStatic(){
	return memPoolAllocate(&__ObjectResourceArray);
}
__HINT_INLINE__ object *moduleObjectAllocate(){
	object *r = moduleObjectAllocateStatic();
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_OBJECT_SIZE,
				RESOURCE_DEFAULT_OBJECT_NUM
			)
		);
		if(memPoolExtend(&__ObjectResourceArray, memory, RESOURCE_DEFAULT_OBJECT_SIZE, RESOURCE_DEFAULT_OBJECT_NUM)){
			r = moduleObjectAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ object *moduleObjectIndex(const size_t i){
	return memPoolIndex(&__ObjectResourceArray, i);
}
__HINT_INLINE__ void moduleObjectFree(object *const restrict resource){
	objDelete(resource);
	memPoolFree(&__ObjectResourceArray, (void *)resource);
}
void moduleObjectClear(){

	MEMORY_POOL_LOOP_BEGIN(__ObjectResourceArray, i, object *);

		moduleObjectFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__ObjectResourceArray, i, return;);

}
