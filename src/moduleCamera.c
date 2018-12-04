#include "moduleCamera.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

memoryList __CameraResourceArray;  // Contains scenes.

return_t moduleCameraResourcesInit(){
	void *memory = memAllocate(
		memListAllocationSize(
			NULL,
			RESOURCE_DEFAULT_CAMERA_SIZE,
			RESOURCE_DEFAULT_CAMERA_NUM
		)
	);
	if(memListCreate(&__CameraResourceArray, memory, RESOURCE_DEFAULT_CAMERA_SIZE, RESOURCE_DEFAULT_CAMERA_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleCameraResourcesReset(){
	memoryRegion *region;
	moduleCameraClear();
	region = __CameraResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__CameraResourceArray.region->next = NULL;
}
void moduleCameraResourcesDelete(){
	memoryRegion *region;
	moduleCameraClear();
	region = __CameraResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__FORCE_INLINE__ camera *moduleCameraAllocateStatic(){
	return memListAllocate(&__CameraResourceArray);
}
__FORCE_INLINE__ camera *moduleCameraAllocate(){
	camera *r = memListAllocate(&__CameraResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *memory = memAllocate(
			memListAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CAMERA_SIZE,
				RESOURCE_DEFAULT_CAMERA_NUM
			)
		);
		if(memListExtend(&__CameraResourceArray, memory, RESOURCE_DEFAULT_CAMERA_SIZE, RESOURCE_DEFAULT_CAMERA_NUM)){
			r = memListAllocate(&__CameraResourceArray);
		}
	}
	return r;
}
__FORCE_INLINE__ void moduleCameraFree(camera *resource){
	camDelete(resource);
	memListFree(&__CameraResourceArray, (void *)resource);
}
void moduleCameraClear(){

	memoryRegion *region = __CameraResourceArray.region;
	camera *i;
	do {
		i = memListFirst(region);
		while(i < (camera *)memAllocatorEnd(region)){

			moduleCameraFree(i);
			i = memListBlockNext(__CameraResourceArray, i);

		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}