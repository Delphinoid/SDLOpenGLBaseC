#include "moduleObject.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

memoryPool __ObjectResourceArray;          // Contains objects.
memoryPool __ObjectInstanceResourceArray;  // Contains object instances.

return_t moduleObjectResourcesInit(){
	void *memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_OBJECT_SIZE,
			RESOURCE_DEFAULT_OBJECT_NUM
		)
	);
	if(memPoolCreate(&__ObjectResourceArray, memory, RESOURCE_DEFAULT_OBJECT_SIZE, RESOURCE_DEFAULT_OBJECT_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_OBJECT_INSTANCE_SIZE,
			RESOURCE_DEFAULT_OBJECT_INSTANCE_NUM
		)
	);
	if(memPoolCreate(&__ObjectInstanceResourceArray, memory, RESOURCE_DEFAULT_OBJECT_INSTANCE_SIZE, RESOURCE_DEFAULT_OBJECT_INSTANCE_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleObjectResourcesReset(){
	memoryRegion *region;
	moduleObjectClear();
	region = __ObjectResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__ObjectResourceArray.region->next = NULL;
	moduleObjectInstanceClear();
	region = __ObjectInstanceResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__ObjectInstanceResourceArray.region->next = NULL;
}
void moduleObjectResourcesDelete(){
	memoryRegion *region;
	moduleObjectClear();
	region = __ObjectResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	moduleObjectInstanceClear();
	region = __ObjectInstanceResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__FORCE_INLINE__ object *moduleObjectAllocateStatic(){
	return memPoolAllocate(&__ObjectResourceArray);
}
__FORCE_INLINE__ object *moduleObjectAllocate(){
	object *r = memPoolAllocate(&__ObjectResourceArray);
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
			r = memPoolAllocate(&__ObjectResourceArray);
		}
	}
	return r;
}
__FORCE_INLINE__ void moduleObjectFree(object *const restrict resource){
	objDelete(resource);
	memPoolFree(&__ObjectResourceArray, (void *)resource);
}
object *moduleObjectFind(const char *const restrict name){

	MEMORY_POOL_LOOP_BEGIN(__ObjectResourceArray, i, object *);

		// Compare the resources' names.
		if(strcmp(name, i->name) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__ObjectResourceArray, i, return NULL;);

	return NULL;

}
void moduleObjectClear(){

	MEMORY_POOL_LOOP_BEGIN(__ObjectResourceArray, i, object *);

		moduleObjectFree(i);

	MEMORY_POOL_LOOP_END(__ObjectResourceArray, i, return;);

}

__FORCE_INLINE__ objInstance *moduleObjectInstanceAllocateStatic(){
	return memPoolAllocate(&__ObjectInstanceResourceArray);
}
__FORCE_INLINE__ objInstance *moduleObjectInstanceAllocate(){
	objInstance *r = memPoolAllocate(&__ObjectInstanceResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_OBJECT_INSTANCE_SIZE,
				RESOURCE_DEFAULT_OBJECT_INSTANCE_NUM
			)
		);
		if(memPoolExtend(&__ObjectInstanceResourceArray, memory, RESOURCE_DEFAULT_OBJECT_INSTANCE_SIZE, RESOURCE_DEFAULT_OBJECT_INSTANCE_NUM)){
			r = memPoolAllocate(&__ObjectInstanceResourceArray);
		}
	}
	return r;
}
__FORCE_INLINE__ objInstance *moduleObjectInstanceIndex(const size_t i){
	return memPoolIndex(&__ObjectInstanceResourceArray, i);
}
__FORCE_INLINE__ void moduleObjectInstanceFree(objInstance *const restrict resource){
	objiDelete(resource);
	memPoolFree(&__ObjectInstanceResourceArray, (void *)resource);
}
void moduleObjectInstanceClear(){

	MEMORY_POOL_LOOP_BEGIN(__ObjectInstanceResourceArray, i, objInstance *);

		moduleObjectInstanceFree(i);

	MEMORY_POOL_LOOP_END(__ObjectInstanceResourceArray, i, return;);

}
