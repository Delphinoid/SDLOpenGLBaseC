#ifndef TEXTUREWRAPPER_H
#define TEXTUREWRAPPER_H

#include "texture.h"
#include "cVector.h"

// Simple rectangle structure for the frame boundaries when using sprite sheets
typedef struct twBounds {
	float x, y, w, h;
} twBounds;

// Contains details describing a single image
typedef struct twFrame {
	texture *baseTexture;    // The actual image
	cVector subframes;       // Holds twBounds; represents frame bounds (always contains one element, or multiple for sprite sheets)
	texture *normalTexture;  // Normal map for the image
} twFrame;

// Contains details describing an animation
typedef struct twAnim {
	int loop;             // How many times the animation will loop (with -1 being infinite times)
	cVector frameIDs;     // Holds unsigned ints; represents the position of the frame in allFrames
	cVector subframeIDs;  // Holds unsigned ints; represents the position of the subframes in textureFrame.subframes
	cVector frameDelays;  // Holds floats; represents how long each frame should last
} twAnim;

// Combines the above structures
typedef struct textureWrapper {
	char *name;
	cVector frames;      // Holds twFrames
	cVector animations;  // Holds twAnims
} textureWrapper;

/** twLoad() and twAnimate() may need some tidying up **/
void twInit(textureWrapper *texWrap);
/** I don't like allTextures being passed in here at all **/
unsigned char twLoad(textureWrapper *texWrap, const char *prgPath, const char *filePath, cVector *allTextures);
void twAnimate(textureWrapper *texWrap, float speedMod, unsigned int *currentAnim, unsigned int *currentFrame, float *frameProgress, int *timesLooped);
void twChangeAnim(textureWrapper *texWrap, unsigned int newAnim, unsigned int *currentAnim, unsigned int *currentFrame, float *frameProgress, int *timesLooped);
GLuint twGetTexWidth(textureWrapper *texWrap, unsigned int currentAnim, unsigned int currentFrame);
GLuint twGetTexHeight(textureWrapper *texWrap, unsigned int currentAnim, unsigned int currentFrame);
GLuint twGetTexID(textureWrapper *texWrap, unsigned int currentAnim, unsigned int currentFrame);
void twGetFrameInfo(textureWrapper *texWrap, unsigned int currentAnim, unsigned int currentFrame, float *x, float *y, float *w, float *h, GLuint *frameTexID);
unsigned char twContainsTranslucency(textureWrapper *texWrap, unsigned int currentAnim, unsigned int currentFrame);
void twDelete(textureWrapper *texWrap);

#endif
