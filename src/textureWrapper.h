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
	texture *baseTexture;    // The actual image
	cVector subframes;       // Holds twBounds; represents frame bounds (always contains one element, or multiple for sprite sheets)
	texture *normalTexture;  // Normal map for the image
} twFrame;

// Contains details describing an animation
typedef struct {
	char *name;
	int desiredLoops;     // How many times the animation will loop (with -1 being infinite times)
	cVector frameIDs;     // Holds size_ts; represents the position of the frame in allFrames
	cVector subframeIDs;  // Holds size_ts; represents the position of the subframes in textureFrame.subframes
	cVector frameDelays;  // Holds floats; represents how long each frame should last
} twAnim;

// Combines the above structures
typedef struct {
	char *name;
	cVector frames;      // Holds twFrames
	cVector animations;  // Holds twAnims
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
