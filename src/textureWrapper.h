#ifndef TEXTUREWRAPPER_H
#define TEXTUREWRAPPER_H

#include "texture.h"
#include "animation.h"
#include "rectangle.h"

// Contains details describing a single image.
typedef struct {
	rectangle *subframes;  // Represents frame bounds (always contains one element, or multiple for sprite sheets).
	const texture *diffuse;
	const texture *normals;
	const texture *specular;
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
	const textureWrapper *tw;
	float timeMod;
	animIndex_t currentAnim;
	animationInstance animator;
} twInstance;

/** twLoad() and twiAnimate() may need some tidying up. **/
void twInit(textureWrapper *const restrict tw);
return_t twLoad(textureWrapper *const restrict tw, const char *const restrict prgPath, const char *const restrict filePath);
return_t twDefault(textureWrapper *const restrict tw);
void twDelete(textureWrapper *const restrict tw);

void twiInit(twInstance *const restrict twi, const textureWrapper *const tw);
void twiAnimate(twInstance *const restrict twi, const float elapsedTime);
GLuint twiGetTexWidth(const twInstance *const restrict twi);
GLuint twiGetTexHeight(const twInstance *const restrict twi);
GLuint twiGetTexID(const twInstance *const restrict twi);
float twiGetFrameWidth(const twInstance *const restrict twi);
float twiGetFrameHeight(const twInstance *const restrict twi);
void twiGetFrameInfo(const twInstance *const restrict twi, float *const restrict x, float *const restrict y, float *const restrict w, float *const restrict h, GLuint *const restrict frameTexID, const float interpT);
return_t twiContainsTranslucency(const twInstance *const restrict twi);

#endif
