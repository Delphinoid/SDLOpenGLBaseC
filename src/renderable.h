#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "textureWrapper.h"
#include "mat4.h"
#include "flags.h"

#define BILLBOARD_DISABLED         0x00
#define BILLBOARD_LOCK_X           0x01  // Whether or not the object uses the camera's rotated X axis.
#define BILLBOARD_LOCK_Y           0x02  // Whether or not the object uses the camera's rotated Y axis.
#define BILLBOARD_LOCK_Z           0x04  // Whether or not the object uses the camera's rotated Z axis.
#define BILLBOARD_LOCK_XYZ         0x07
#define BILLBOARD_SCALE            0x08  // Rescale the object based on the distance from the camera.
#define BILLBOARD_SPRITE           0x10  // A cheap billboard method for sprites.
#define BILLBOARD_TARGET_SPRITE    0x20  // Billboard like a sprite with support for axis locking.
#define BILLBOARD_TARGET_CAMERA    0x40  // Billboard towards the camera's position.
#define BILLBOARD_TARGET_ARBITRARY 0x80  // Billboard towards a specified target.

#ifndef BILLBOARD_SCALE_CALIBRATION_DISTANCE
	#define BILLBOARD_SCALE_CALIBRATION_DISTANCE 7.5f  // How far away an object should be for a 1:1 scale.
#endif

typedef struct model model;
typedef struct camera camera;

/** Add default state data to renderable base. **/
typedef struct renderableBase {
	model *mdl;          // A pointer to the renderable's associated model.
	textureWrapper *tw;  // A pointer to the renderable's associated texture wrapper.
} renderableBase;

typedef struct {

	// Billboard data.
	flags_t flags;
	vec3 *axis;    // Pointer to billboard axis. NULL if using the camera's up vector.
	vec3 *target;  // Pointer to billboard target. NULL if not using an arbitrary target.
	float scale;   // Scale option calibration distance. Should be between 0 and 1.

	// Render data.
	float alpha;          // Updated alpha.
	/** Is this really worth it? **/
	float alphaCurrent;   // Current alpha for rendering.
	float alphaPrevious;  // Previous alpha for rendering.

} rndrState;

typedef struct renderable {
	model *mdl;
	twInstance twi;
	rndrState state;
} renderable;

void rndrBaseInit(renderableBase *const restrict base);

void rndrInit(renderable *const restrict rndr);
void rndrInstantiate(renderable *const restrict rndr, const renderableBase *const base);
void rndrTick(renderable *const restrict rndr, const float elapsedTime);

mat4 rndrStateBillboard(const rndrState state, const camera *const restrict cam, const vec3 centroid, mat4 configuration);

#endif
