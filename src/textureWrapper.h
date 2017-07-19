#ifndef TEXTUREWRAPPER_H
#define TEXTUREWRAPPER_H

#include "texture.h"
#include "cVector.h"

// Simple rectangle structure for the frame boundaries when using sprite sheets
typedef struct {
	float x, y, w, h;
} twBounds;

// Contains details describing a single image
typedef struct {
	texture *baseTexture;     // The actual image
	texture *normalTexture;   // Normal map for the image
	size_t subframeNum;
	twBounds *subframes;      // Holds twBounds; represents frame bounds (always contains one element, or multiple for sprite sheets)
} twFrame;

// Contains details describing an animation
typedef struct {
	int desiredLoops;     // How many times the animation will loop (with -1 being infinite times)
	size_t frameNum;
	size_t *frameIDs;     // Represents the positions of the frames in textureWrapper.frames
	size_t *subframeIDs;  // Represents the positions of the subframes in twFrame.subframes
	float *frameDelays;   // Represents how long each frame should last
} twAnim;

// Combines the above structures
typedef struct {
	char *name;
	size_t frameNum;
	size_t animationNum;
	twFrame *frames;     // Holds twFrames
	twAnim *animations;  // Holds twAnims
} textureWrapper;

// Texture wrapper instance
typedef struct {
	textureWrapper *tw;
	float delayMod;
	size_t currentAnim;
	size_t currentFrame;
	int currentLoops;
	float lastUpdate;
} twInstance;

/** twLoad() and twiAnimate() may need some tidying up **/
void twInit(textureWrapper *tw);
/** I don't like allTextures being passed in here at all **/
unsigned char twLoad(textureWrapper *tw, const char *prgPath, const char *filePath, cVector *allTextures);
void twDelete(textureWrapper *tw);

void twiInit(twInstance *twi, textureWrapper *tw);
/** Use time passed instead of currentTick **/
void twiAnimate(twInstance *twi, uint32_t currentTick, float globalDelayMod);
GLuint twiGetTexWidth(twInstance *twi);
GLuint twiGetTexHeight(twInstance *twi);
GLuint twiGetTexID(twInstance *twi);
void twiGetFrameInfo(twInstance *twi, float *x, float *y, float *w, float *h, GLuint *frameTexID);
unsigned char twiContainsTranslucency(twInstance *twi);

#endif
