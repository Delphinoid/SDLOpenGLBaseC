#ifndef TEXTUREWRAPPER_H
#define TEXTUREWRAPPER_H

#include "texture.h"
#include "animationHandler.h"

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
	textureWrapper *texWrap;
	animationHandler animator;
} twInstance;

/** twLoad() and twiAnimate() may need some tidying up **/
void twInit(textureWrapper *tw);
/** I don't like allTextures being passed in here at all **/
unsigned char twLoad(textureWrapper *tw, const char *prgPath, const char *filePath, cVector *allTextures);
void twDelete(textureWrapper *tw);
twAnim *twGetAnim(textureWrapper *tw, size_t anim);
twFrame *twGetAnimFrame(textureWrapper *tw, size_t anim, size_t frame);
twBounds *twGetAnimSubframe(textureWrapper *tw, size_t anim, size_t frame);
float *twGetAnimFrameDelay(textureWrapper *tw, size_t anim, size_t frame);

void twiInit(twInstance *twi, textureWrapper *tw);
void twiChangeAnim(twInstance *twi, size_t newAnim);
/**unsigned char twiAnimFinished(twInstance *twi);**/
void twiAnimate(twInstance *twi, float globalDelayMod);
GLuint twiGetTexWidth(twInstance *twi);
GLuint twiGetTexHeight(twInstance *twi);
GLuint twiGetTexID(twInstance *twi);
void twiGetFrameInfo(twInstance *twi, float *x, float *y, float *w, float *h, GLuint *frameTexID);
unsigned char twiContainsTranslucency(twInstance *twi);

#endif
