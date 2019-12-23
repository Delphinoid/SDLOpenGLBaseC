#ifndef TEXTUREWRAPPER_H
#define TEXTUREWRAPPER_H

#include <SDL2/SDL_opengl.h>
#include "animation.h"
#include "rectangle.h"
#include "return.h"

typedef struct texture texture;

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
typedef struct textureWrapper {
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
const twFrame *twState(const textureWrapper *const restrict tw, const animationInstance *const restrict animator, const animIndex_t currentAnim, const float interpT);
void twDelete(textureWrapper *const restrict tw);

void twiInit(twInstance *const restrict twi, const textureWrapper *const tw);
void twiTick(twInstance *const restrict twi, const float elapsedTime);
GLuint twiTextureWidth(const twInstance *const restrict twi);
GLuint twiTextureHeight(const twInstance *const restrict twi);
const texture *twiTexture(const twInstance *const restrict twi);
float twiFrameWidth(const twInstance *const restrict twi);
float twiFrameHeight(const twInstance *const restrict twi);
const twFrame *twiState(const twInstance *const restrict twi, const float interpT);
return_t twiTranslucent(const twInstance *const restrict twi);

#endif
