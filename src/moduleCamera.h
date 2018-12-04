#ifndef MODULECAMERA_H
#define MODULECAMERA_H

#include "camera.h"
#include "memoryList.h"

#define RESOURCE_DEFAULT_CAMERA_SIZE sizeof(camera)
#ifndef RESOURCE_DEFAULT_CAMERA_NUM
	#define RESOURCE_DEFAULT_CAMERA_NUM 2
#endif

extern memoryList __CameraResourceArray;  // Contains scenes.

/** Support locals? Merge all module containers? **/

return_t moduleCameraResourcesInit();
void moduleCameraResourcesReset();
void moduleCameraResourcesDelete();

camera *moduleCameraAllocateStatic();
camera *moduleCameraAllocate();
void moduleCameraFree(camera *resource);
void moduleCameraClear();

#endif
