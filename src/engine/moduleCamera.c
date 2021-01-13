#include "moduleCamera.h"
#include "moduleSettings.h"
#include "camera.h"
#include "memoryManager.h"
#include <string.h>

#define RESOURCE_DEFAULT_CAMERA_SIZE sizeof(camera)

memoryPool __g_CameraResourceArray;  // Contains cameras.

return_t moduleCameraResourcesInit(){
	void *const memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_CAMERA_SIZE,
			RESOURCE_DEFAULT_CAMERA_NUM
		)
	);
	if(memPoolCreate(&__g_CameraResourceArray, memory, RESOURCE_DEFAULT_CAMERA_SIZE, RESOURCE_DEFAULT_CAMERA_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleCameraResourcesReset(){
	memoryRegion *region;
	moduleCameraClear();
	region = __g_CameraResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__g_CameraResourceArray.region->next = NULL;
}
void moduleCameraResourcesDelete(){
	memoryRegion *region;
	moduleCameraClear();
	region = __g_CameraResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
}

__HINT_INLINE__ camera *moduleCameraAllocateStatic(){
	return memPoolAllocate(&__g_CameraResourceArray);
}
__HINT_INLINE__ camera *moduleCameraAllocate(){
	camera *r = moduleCameraAllocateStatic();
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_CAMERA_SIZE,
				RESOURCE_DEFAULT_CAMERA_NUM
			)
		);
		if(memPoolExtend(&__g_CameraResourceArray, memory, RESOURCE_DEFAULT_CAMERA_SIZE, RESOURCE_DEFAULT_CAMERA_NUM)){
			r = moduleCameraAllocateStatic();
		}
	}
	return r;
}
void moduleCameraPrepare(){

	MEMORY_POOL_LOOP_BEGIN(__g_CameraResourceArray, i, camera *);

		camResetInterpolation(i);

	MEMORY_POOL_LOOP_END(__g_CameraResourceArray, i, return;);

}
__HINT_INLINE__ void moduleCameraFree(camera *const __RESTRICT__ resource){
	camDelete(resource);
	memPoolFree(&__g_CameraResourceArray, (void *)resource);
}
void moduleCameraClear(){

	MEMORY_POOL_LOOP_BEGIN(__g_CameraResourceArray, i, camera *);

		moduleCameraFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__g_CameraResourceArray, i, return;);

}