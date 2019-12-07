#ifndef TEXTUREWRAPPER_H
#define TEXTUREWRAPPER_H

#include "texture.h"
#include "animation.h"
#include "rectangle.h"

// Contains details describing a single image.
typedef struct {
	const texture *image;
	rectangle subframe;  // Represents frame bounds.
} twFrame;

// Contains details describing an animation.
typedef struct {
	animationData animData;
	twFrame *frames;
} twAnim;

// Combines the above structures.
typedef struct {
	twAnim *animations;  // Holds twAnims.
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

extern textureWrapper twDefault;

/** twLoad() and twiAnimate() may need some tidying up. **/
void twInit(textureWrapper *const restrict tw);
return_t twLoad(textureWrapper *const restrict tw, const char *const restrict prgPath, const char *const restrict filePath);
void twDelete(textureWrapper *const restrict tw);

void twiInit(twInstance *const restrict twi, const textureWrapper *const tw);
void twiAnimate(twInstance *const restrict twi, const float elapsedTime);
GLuint twiTextureWidth(const twInstance *const restrict twi);
GLuint twiTextureHeight(const twInstance *const restrict twi);
const texture *twiTexture(const twInstance *const restrict twi);
float twiFrameWidth(const twInstance *const restrict twi);
float twiFrameHeight(const twInstance *const restrict twi);
const twFrame *twiRenderState(const twInstance *const restrict twi, const float interpT);
return_t twiTranslucent(const twInstance *const restrict twi);

#endif
