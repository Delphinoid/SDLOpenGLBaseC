#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "gfxProgram.h"
#include "model.h"
#include "skeleton.h"
#include "textureWrapper.h"
#include "spaceTransform.h"
#include "renderTransform.h"
//#include "camera.h"

#define RNDR_BILLBOARD_X             0x01  // Whether or not the object uses the camera's rotated X axis
#define RNDR_BILLBOARD_Y             0x02  // Whether or not the object uses the camera's rotated Y axis
#define RNDR_BILLBOARD_Z             0x04  // Whether or not the object uses the camera's rotated Z axis
#define RNDR_BILLBOARD_SPRITE        0x08  // A cheap billboard method for sprites
#define RNDR_BILLBOARD_TARGET        0x10  // Billboard towards a specified target
#define RNDR_BILLBOARD_TARGET_CAMERA 0x20  // Billboard towards the camera's position

typedef struct {

	char *name;
	model *mdl;  // Pointer to the model being used. Could be pretty dangerous, might need to change it
	/** Need to rename skl and tex **/
	sklInstance skli;
	twInstance twi;
	spaceTransform sTrans;
	renderTransform rTrans;
	/** Store model matrix here, update only when changed? **/
	/** Sprite should not be necessary anymore **/
	unsigned char sprite;
	unsigned char flags;

} renderable;

void rndrInit(renderable *rndr);
unsigned char rndrLoad(renderable *rndr, const char *prgPath, const char *filePath, cVector *allModels, cVector *allTexWrappers);
unsigned char rndrRenderMethod(renderable *rndr);  // Returns 0 if the model is fully opaque, 1 if the model contains translucency and 2 if the model is fully transparent
//void rndrGenerateTransform(renderable *rndr, const camera *cam, mat4 *transformMatrix);
void rndrGenerateSprite(const renderable *rndr, vertex *vertices, const mat4 *transformMatrix);
void rndrOffsetSpriteTexture(vertex *vertices, const float texFrag[4], const float texWidth, const float texHeight);
void rndrDelete(renderable *rndr);
/** Sort out the functions below, some should be associated with entities **/
//size_t rndrBoneNum(renderable *rndr);
//unsigned char rndrGenerateSkeletonState(renderable *rndr);
void rndrSetRotation(renderable *rndr, const float newX, const float newY, const float newZ);
void rndrRotateX(renderable *rndr, const float changeX);
void rndrRotateY(renderable *rndr, const float changeY);
void rndrRotateZ(renderable *rndr, const float changeZ);
void rndrAnimateTexture(renderable *rndr, const float elapsedTime);
void rndrAnimateSkeleton(renderable *rndr, const float elapsedTime);

#endif
