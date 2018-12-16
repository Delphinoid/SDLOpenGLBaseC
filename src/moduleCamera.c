#include "moduleCamera.h"
#include "moduleSettings.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

memoryPool __CameraResourceArray;  // Contains cameras.

return_t moduleCameraResourcesInit(){
	void *const memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_CAMERA_SIZE,
			RESOURCE_DEFAULT_CAMERA_NUM
		)
	);
	if(memPoolCreate(&__CameraResourceArray, memory, RESOURCE_DEFAULT_CAMERA_SIZE, RESOURCE_DEFAULT_CAMERA_NUM) == NULL){
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
	return memPoolAllocate(&__CameraResourceArray);
}
__FORCE_INLINE__ camera *moduleCameraAllocate(){
	camera *r = memPoolAllocate(&__CameraResourceArray);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CAMERA_SIZE,
				RESOURCE_DEFAULT_CAMERA_NUM
			)
		);
		if(memPoolExtend(&__CameraResourceArray, memory, RESOURCE_DEFAULT_CAMERA_SIZE, RESOURCE_DEFAULT_CAMERA_NUM)){
			r = memPoolAllocate(&__CameraResourceArray);
		}
	}
	return r;
}
void moduleCameraPrepare(){

	MEMORY_POOL_LOOP_BEGIN(__CameraResourceArray, i, camera *);

		camResetInterpolation(i);

	MEMORY_POOL_LOOP_END(__CameraResourceArray, i, return;);

}
__FORCE_INLINE__ void moduleCameraFree(camera *const restrict resource){
	camDelete(resource);
	memPoolFree(&__CameraResourceArray, (void *)resource);
}
void moduleCameraClear(){

	MEMORY_POOL_LOOP_BEGIN(__CameraResourceArray, i, camera *);

		moduleCameraFree(i);

	MEMORY_POOL_LOOP_END(__CameraResourceArray, i, return;);

}