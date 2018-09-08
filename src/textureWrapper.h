#ifndef TEXTUREWRAPPER_H
#define TEXTUREWRAPPER_H

#include "texture.h"
#include "animation.h"
#include "cVector.h"

#define TW_MAX_ANIMATION_NUM 256
#define TW_DEFAULT_SIZE sizeof(textureWrapper) + sizeof(twAnim) + sizeof(twFrame) + sizeof(twBounds) + sizeof(frameIndex_t) + sizeof(frameIndex_t)

// Simple rectangle structure for the frame boundaries when using sprite sheets
typedef struct {
	float x, y, w, h;
} twBounds;

/** Optimize subframes. **/
// Contains details describing a single image
typedef struct {
	texture *baseTexture;     // The actual image
	//texture *normalTexture;   // Normal map for the image
	frameIndex_t subframeNum;
	twBounds *subframes;      // Holds twBounds; represents frame bounds (always contains one element, or multiple for sprite sheets)
} twFrame;

// Contains details describing an animation
typedef struct {
	animationData animData;
	frameIndex_t *frameIDs;     // Represents the positions of the frames in textureWrapper.frames
	frameIndex_t *subframeIDs;  // Represents the positions of the subframes in twFrame.subframes
} twAnim;

// Combines the above structures
typedef struct {
	char *name;
	frameIndex_t frameNum;
	animIndex_t animationNum;
	twFrame *frames;     // Holds twFrames
	twAnim *animations;  // Holds twAnims
} textureWrapper;

// Texture wrapper instance
typedef struct {
	textureWrapper *tw;
	float timeMod;
	animIndex_t currentAnim;
	animationInstance animator;
} twInstance;

/** twLoad() and twiAnimate() may need some tidying up. **/
void twInit(textureWrapper *tw);
/** I don't like the cVector being passed in here at all. **/
signed char twLoad(textureWrapper *tw, const char *prgPath, const char *filePath, cVector *allTextures);
signed char twDefault(textureWrapper *tw, cVector *allTextures);
void twDelete(textureWrapper *tw);

void twiInit(twInstance *twi, textureWrapper *tw);
void twiAnimate(twInstance *twi, const float elapsedTime);
GLuint twiGetTexWidth(const twInstance *twi);
GLuint twiGetTexHeight(const twInstance *twi);
GLuint twiGetTexID(const twInstance *twi);
float twiGetFrameWidth(const twInstance *twi);
float twiGetFrameHeight(const twInstance *twi);
void twiGetFrameInfo(const twInstance *twi, float *x, float *y, float *w, float *h, GLuint *frameTexID, const float interpT);
signed char twiContainsTranslucency(const twInstance *twi);

#endif
