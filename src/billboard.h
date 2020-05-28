#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "mat4.h"
#include "flags.h"

#define BILLBOARD_DISABLED           0x0000
#define BILLBOARD_LOCK_X             0x0001  // Whether or not the object uses the camera's rotated X axis.
#define BILLBOARD_LOCK_Y             0x0002  // Whether or not the object uses the camera's rotated Y axis.
#define BILLBOARD_LOCK_Z             0x0004  // Whether or not the object uses the camera's rotated Z axis.
#define BILLBOARD_LOCK_XYZ           0x0007
#define BILLBOARD_SCALE              0x0008  // Rescale the object based on the distance from the camera.
#define BILLBOARD_SPRITE             0x0010  // A cheap billboard method for sprites.
#define BILLBOARD_TARGET_SPRITE      0x0020  // Billboard like a sprite with support for axis locking.
#define BILLBOARD_TARGET_CAMERA      0x0040  // Billboard towards the camera's position.
#define BILLBOARD_TARGET_ARBITRARY   0x0080  // Billboard towards a specified target.
#define BILLBOARD_INVERT_ORIENTATION 0x0100

#ifndef BILLBOARD_SCALE_CALIBRATION_DISTANCE
	// How far away an object should be for a 1:1 scale.
	#define BILLBOARD_SCALE_CALIBRATION_DISTANCE 7.5f
#endif

typedef struct camera camera;

typedef struct {
	// Billboard data.
	flags16_t flags;
	unsigned short sectors;  // Defines how many separate frames there should be per 360 for lenticular billboards.
	vec3 *axis;    // Pointer to billboard axis. NULL if using the camera's up vector.
	vec3 *target;  // Pointer to billboard target. NULL if not using an arbitrary target.
	float scale;   // Scale option calibration distance. Should be between 0 and 1.
} billboard;

void billboardInit(billboard *const __RESTRICT__ data);
mat4 billboardState(const billboard data, const camera *const __RESTRICT__ cam, const vec3 centroid, mat4 configuration);
unsigned int billboardRoot(const billboard data, const camera *const __RESTRICT__ cam, const mat4 configuration);

#endif
