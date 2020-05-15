#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "mat4.h"
#include "flags.h"

#define BILLBOARD_DISABLED            0x00
#define BILLBOARD_LOCK_X              0x01  // Whether or not the object uses the camera's rotated X axis.
#define BILLBOARD_LOCK_Y              0x02  // Whether or not the object uses the camera's rotated Y axis.
#define BILLBOARD_LOCK_Z              0x04  // Whether or not the object uses the camera's rotated Z axis.
#define BILLBOARD_LOCK_XYZ            0x07
#define BILLBOARD_SCALE               0x08  // Rescale the object based on the distance from the camera.
#define BILLBOARD_SPRITE              0x10  // A cheap billboard method for sprites.
#define BILLBOARD_TARGET_SPRITE       0x20  // Billboard like a sprite with support for axis locking.
#define BILLBOARD_TARGET_CAMERA       0x40  // Billboard towards the camera's position.
#define BILLBOARD_TARGET_ARBITRARY    0x80  // Billboard towards a specified target.

#ifndef BILLBOARD_SCALE_CALIBRATION_DISTANCE
	// How far away an object should be for a 1:1 scale.
	#define BILLBOARD_SCALE_CALIBRATION_DISTANCE 7.5f
#endif

typedef struct camera camera;

typedef struct {
	// Billboard data.
	flags_t flags;
	unsigned short sectors;  // Defines how many separate frames there should be per 360 for lenticular billboards.
	vec3 *axis;    // Pointer to billboard axis. NULL if using the camera's up vector.
	vec3 *target;  // Pointer to billboard target. NULL if not using an arbitrary target.
	float scale;   // Scale option calibration distance. Should be between 0 and 1.
} billboard;

void billboardInit(billboard *const __RESTRICT__ data);
mat4 billboardState(const billboard data, const camera *const __RESTRICT__ cam, const vec3 centroid, mat4 configuration);
unsigned int billboardLenticular(const billboard data, const camera *const __RESTRICT__ cam, mat4 configuration);

#endif
