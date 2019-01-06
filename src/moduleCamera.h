#ifndef MODULECAMERA_H
#define MODULECAMERA_H

#include "camera.h"
#include "memoryPool.h"

#define RESOURCE_DEFAULT_CAMERA_SIZE sizeof(camera)
#define RESOURCE_DEFAULT_CAMERA_NUM 2

// Forward declaration for inlining.
extern memoryPool __CameraResourceArray;  // Contains cameras.

/** Support locals? Merge all module containers? **/

return_t moduleCameraResourcesInit();
void moduleCameraResourcesReset();
void moduleCameraResourcesDelete();

camera *moduleCameraAllocateStatic();
camera *moduleCameraAllocate();
void moduleCameraPrepare();
void moduleCameraFree(camera *const restrict resource);
void moduleCameraClear();

#endif
