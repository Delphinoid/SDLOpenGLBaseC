#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "gfxProgram.h"
#include "model.h"
#include "skeleton.h"
#include "textureWrapper.h"
#include "spaceTransform.h"
#include "renderTransform.h"
#include "camera.h"

typedef struct {

	char *name;
	model *mdl;  // Pointer to the model being used. Could be pretty dangerous, might need to change it
	/** Need to rename skl and tex **/
	sklInstance skl;
	twInstance tex;
	spaceTransform sTrans;
	renderTransform rTrans;
	mat4 modelViewProjectionMatrix;  /** Store MVP matrix here, update only when changed **/
	/** Sprite, width and height shouldn't be necessary here **/
	unsigned char sprite;
	float width, height;
	unsigned char billboardX;       // Whether or not the object uses the camera's rotated X axis
	unsigned char billboardY;       // Whether or not the object uses the camera's rotated Y axis
	unsigned char billboardZ;       // Whether or not the object uses the camera's rotated Z axis
	unsigned char targetBillboard;  // Whether or not to use a slower billboard method that looks at a target
	/** Store the elements below in entity **/
	unsigned char hudElement;       // Whether or not the object is part of the HUD. Should be private
	unsigned char hudScaleMode;     // 0 = nothing special, 1 = position scaled off window size, 2 = width and height scaled off window size, 3 = both

} renderable;

void rndrInit(renderable *rndr);
unsigned char rndrLoad(renderable *rndr, const char *prgPath, const char *filePath, cVector *allModels, cVector *allTexWrappers);
unsigned char rndrRenderMethod(renderable *rndr);  // Returns 0 if the model is fully opaque, 1 if the model contains translucency and 2 if the model is fully transparent
void rndrGenerateTransform(renderable *rndr, mat4 *transformMatrix, gfxProgram *gfxPrg, camera *cam);
void rndrGenerateSprite(renderable *rndr, vertex *vertices, mat4 *transformMatrix, gfxProgram *gfxPrg);
void rndrOffsetSpriteTexture(vertex *vertices, float texFrag[4], float texWidth, float texHeight);
void rndrDelete(renderable *rndr);
/** Sort out the functions below, some should be associated with entities **/
size_t rndrBoneNum(renderable *rndr);
unsigned char rndrGenerateSkeletonState(renderable *rndr);
void rndrHudElement(renderable *rndr, unsigned char isHudElement);
void rndrSetRotation(renderable *rndr, float newX, float newY, float newZ);
void rndrRotateX(renderable *rndr, float changeX);
void rndrRotateY(renderable *rndr, float changeY);
void rndrRotateZ(renderable *rndr, float changeZ);
void rndrAnimateTex(renderable *rndr, float globalDelayMod);

#endif
