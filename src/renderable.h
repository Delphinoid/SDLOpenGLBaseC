#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "gfxProgram.h"
#include "model.h"
#include "skeleton.h"
#include "textureWrapper.h"
#include "spaceTransform.h"
#include "renderTransform.h"
#include "camera.h"

#define RNDR_BILLBOARD_X      0x0001  // Whether or not the object uses the camera's rotated X axis
#define RNDR_BILLBOARD_Y      0x0002  // Whether or not the object uses the camera's rotated Y axis
#define RNDR_BILLBOARD_Z      0x0004  // Whether or not the object uses the camera's rotated Z axis
#define RNDR_BILLBOARD_TARGET 0x0008  // Whether or not to use a slower billboard method that looks at a target

typedef struct {

	char *name;
	model *mdl;  // Pointer to the model being used. Could be pretty dangerous, might need to change it
	/** Need to rename skl and tex **/
	sklInstance skli;
	twInstance tex;
	spaceTransform sTrans;
	renderTransform rTrans;
	mat4 modelViewProjectionMatrix;  /** Store MVP matrix here, update only when changed **/
	/** Sprite, width and height shouldn't be necessary here **/
	unsigned char sprite;
	float width, height;
	/** Combine the three variables below into one variable as flags using bitwise OR **/
	unsigned char billboardFlags;
	/** Store the elements below in entity **/
	unsigned char hudElement;       // Whether or not the object is part of the HUD. Should be private

} renderable;

void rndrInit(renderable *rndr);
unsigned char rndrLoad(renderable *rndr, const char *prgPath, const char *filePath, cVector *allModels, cVector *allTexWrappers);
unsigned char rndrRenderMethod(renderable *rndr);  // Returns 0 if the model is fully opaque, 1 if the model contains translucency and 2 if the model is fully transparent
void rndrGenerateTransform(renderable *rndr, mat4 *transformMatrix, gfxProgram *gfxPrg, camera *cam);
void rndrGenerateSprite(renderable *rndr, vertex *vertices, mat4 *transformMatrix);
void rndrOffsetSpriteTexture(vertex *vertices, float texFrag[4], float texWidth, float texHeight);
void rndrDelete(renderable *rndr);
/** Sort out the functions below, some should be associated with entities **/
//size_t rndrBoneNum(renderable *rndr);
//unsigned char rndrGenerateSkeletonState(renderable *rndr);
void rndrHudElement(renderable *rndr, unsigned char isHudElement);
void rndrSetRotation(renderable *rndr, float newX, float newY, float newZ);
void rndrRotateX(renderable *rndr, float changeX);
void rndrRotateY(renderable *rndr, float changeY);
void rndrRotateZ(renderable *rndr, float changeZ);
void rndrAnimateTex(renderable *rndr, uint32_t currentTick, float globalDelayMod);
void rndrAnimateSkel(renderable *rndr, uint32_t currentTick, float globalDelayMod);

#endif
