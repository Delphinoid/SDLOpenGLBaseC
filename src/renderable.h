#ifndef RENDERABLE_H
#define RENDERABLE_H

//#include "gfxProgram.h"
#include "model.h"
#include "textureWrapper.h"
#include "interpState.h"
#include "camera.h"

#define RNDR_BILLBOARD_X             0x01  // Whether or not the object uses the camera's rotated X axis.
#define RNDR_BILLBOARD_Y             0x02  // Whether or not the object uses the camera's rotated Y axis.
#define RNDR_BILLBOARD_Z             0x04  // Whether or not the object uses the camera's rotated Z axis.
#define RNDR_BILLBOARD_SPRITE        0x08  // A cheap billboard method for sprites.
#define RNDR_BILLBOARD_TARGET        0x10  // Billboard towards a specified target.
#define RNDR_BILLBOARD_TARGET_CAMERA 0x20  // Billboard towards the camera's position.

typedef struct {
	model *mdl;         // A pointer to the bodygroup's associated model.
	twInstance twi;     // The bodygroup's associated texture wrapper.
} renderable;

/** Move to particle / sprite files? **/
typedef struct {
	interpVec3 position;           // Position of the object relative to its parent.
	interpQuat orientation;        // Quaternion representing orientation.
	interpVec3 pivot;              // The point the object is rotated around (relative to its position).
	interpVec3 targetPosition;     // Target position, used for target billboards.
	interpQuat targetOrientation;  // Target orientation, used for target billboards.
	interpVec3 scale;              // Scale of the object.
	interpFloat alpha;             // A value to multiply against the alpha values of the pixels.
	/** Sprite should not be necessary anymore. **/
	unsigned char sprite;
	unsigned char flags;
} rndrConfig;

void rndrConfigInit(rndrConfig *rc);
void rndrConfigStateCopy(rndrConfig *o, rndrConfig *c);
void rndrConfigResetInterpolation(rndrConfig *rc);

signed char rndrConfigRenderUpdate(rndrConfig *rc, const float interpT);

void rndrConfigGenerateTransform(const rndrConfig *rc, const camera *cam, mat4 *transformMatrix);
void rndrConfigGenerateSprite(const rndrConfig *rc, const twInstance *twi, vertex *vertices, const mat4 *transformMatrix);
void rndrConfigOffsetSpriteTexture(vertex *vertices, const float texFrag[4], const float texWidth, const float texHeight);

#endif
