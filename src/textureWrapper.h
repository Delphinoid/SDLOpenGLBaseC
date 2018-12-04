#ifndef TEXTUREWRAPPER_H
#define TEXTUREWRAPPER_H

#include "texture.h"
#include "animation.h"
#include "rectangle.h"

// Contains details describing a single image.
typedef struct {
	rectangle *subframes;  // Represents frame bounds (always contains one element, or multiple for sprite sheets).
	texture *diffuse;
	texture *normals;
	texture *specular;
	frameIndex_t subframeNum;
} twFrame;

typedef struct {
	frameIndex_t frameID;
	frameIndex_t subframeID;
} twFramePair;

// Contains details describing an animation.
typedef struct {
	animationData animData;
	twFramePair *frames;  // Represents the positions of the frames in textureWrapper.frames.
} twAnim;

// Combines the above structures.
typedef struct {
	twFrame *frames;     // Holds twFrames.
	twAnim *animations;  // Holds twAnims.
	frameIndex_t frameNum;
	animIndex_t animationNum;
	char *name;
} textureWrapper;

// Texture wrapper instance.
typedef struct {
	textureWrapper *tw;
	float timeMod;
	animIndex_t currentAnim;
	animationInstance animator;
} twInstance;

/** twLoad() and twiAnimate() may need some tidying up. **/
void twInit(textureWrapper *tw);
return_t twLoad(textureWrapper *tw, const char *prgPath, const char *filePath);
return_t twDefault(textureWrapper *tw);
void twDelete(textureWrapper *tw);

void twiInit(twInstance *twi, textureWrapper *tw);
void twiAnimate(twInstance *twi, const float elapsedTime);
GLuint twiGetTexWidth(const twInstance *twi);
GLuint twiGetTexHeight(const twInstance *twi);
GLuint twiGetTexID(const twInstance *twi);
float twiGetFrameWidth(const twInstance *twi);
float twiGetFrameHeight(const twInstance *twi);
void twiGetFrameInfo(const twInstance *twi, float *x, float *y, float *w, float *h, GLuint *frameTexID, const float interpT);
return_t twiContainsTranslucency(const twInstance *twi);

#endif
