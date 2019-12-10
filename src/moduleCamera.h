#ifndef MODULECAMERA_H
#define MODULECAMERA_H

#include "memoryPool.h"
#include "return.h"

#define RESOURCE_DEFAULT_CAMERA_NUM 2

// Forward declaration for inlining.
extern memoryPool __CameraResourceArray;  // Contains cameras.

typedef struct camera camera;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleCameraResourcesInit();
void moduleCameraResourcesReset();
void moduleCameraResourcesDelete();

camera *moduleCameraAllocateStatic();
camera *moduleCameraAllocate();
void moduleCameraPrepare();
void moduleCameraFree(camera *const restrict resource);
void moduleCameraClear();

#endif
