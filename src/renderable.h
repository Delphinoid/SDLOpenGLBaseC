#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "gfxProgram.h"
#include "model.h"
#include "skeleton.h"
#include "textureWrapper.h"
#include "spaceTransform.h"
#include "renderTransform.h"
#include "camera.h"

#define RNDR_BILLBOARD_X      0x01  // Whether or not the object uses the camera's rotated X axis
#define RNDR_BILLBOARD_Y      0x02  // Whether or not the object uses the camera's rotated Y axis
#define RNDR_BILLBOARD_Z      0x04  // Whether or not the object uses the camera's rotated Z axis
#define RNDR_BILLBOARD_TARGET 0x08  // Whether or not to use a slower billboard method that looks at a target

typedef struct {

	char *name;
	model *mdl;  // Pointer to the model being used. Could be pretty dangerous, might need to change it
	/** Need to rename skl and tex **/
	sklInstance skli;
	twInstance twi;
	spaceTransform sTrans;
	renderTransform rTrans;
	mat4 modelViewProjectionMatrix;  /** Store MVP matrix here, update only when changed **/
	/** Sprite should not be necessary anymore **/
	unsigned char sprite;
	/** Combine the three variables below into one variable as flags using bitwise OR **/
	unsigned char flags;
	/** Remove this along with the changes to the camera **/
	unsigned char hudElement;       // Whether or not the object is part of the HUD. Should be private

} renderable;

void rndrInit(renderable *rndr);
unsigned char rndrLoad(renderable *rndr, const char *prgPath, const char *filePath, cVector *allModels, cVector *allTexWrappers);
unsigned char rndrRenderMethod(renderable *rndr);  // Returns 0 if the model is fully opaque, 1 if the model contains translucency and 2 if the model is fully transparent
void rndrGenerateTransform(renderable *rndr, camera *cam, mat4 *transformMatrix);
void rndrGenerateSprite(renderable *rndr, vertex *vertices, mat4 *transformMatrix);
void rndrOffsetSpriteTexture(vertex *vertices, float texFrag[4], float texWidth, float texHeight);
void rndrDelete(renderable *rndr);
/** Sort out the functions below, some should be associated with entities **/
//size_t rndrBoneNum(renderable *rndr);
//unsigned char rndrGenerateSkeletonState(renderable *rndr);
void rndrSetRotation(renderable *rndr, float newX, float newY, float newZ);
void rndrRotateX(renderable *rndr, float changeX);
void rndrRotateY(renderable *rndr, float changeY);
void rndrRotateZ(renderable *rndr, float changeZ);
void rndrAnimateTexture(renderable *rndr, uint32_t currentTick, float globalDelayMod);
void rndrAnimateSkeleton(renderable *rndr, uint32_t currentTick, float globalDelayMod);

#endif
