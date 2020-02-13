#ifndef TEXTUREWRAPPER_H
#define TEXTUREWRAPPER_H

#define GLEW_STATIC
#include <GL/glew.h>
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

extern textureWrapper g_twDefault;

/** twLoad() and twiAnimate() may need some tidying up. **/
void twInit(textureWrapper *const __RESTRICT__ tw);
return_t twLoad(textureWrapper *const __RESTRICT__ tw, const char *const __RESTRICT__ filePath, const size_t filePathLength);
const twFrame *twState(const textureWrapper *const __RESTRICT__ tw, const animationInstance *const __RESTRICT__ animator, const animIndex_t currentAnim, const float interpT);
void twDelete(textureWrapper *const __RESTRICT__ tw);

void twiInit(twInstance *const __RESTRICT__ twi, const textureWrapper *const tw);
void twiTick(twInstance *const __RESTRICT__ twi, const float elapsedTime);
GLuint twiTextureWidth(const twInstance *const __RESTRICT__ twi);
GLuint twiTextureHeight(const twInstance *const __RESTRICT__ twi);
const texture *twiTexture(const twInstance *const __RESTRICT__ twi);
float twiFrameWidth(const twInstance *const __RESTRICT__ twi);
float twiFrameHeight(const twInstance *const __RESTRICT__ twi);
const twFrame *twiState(const twInstance *const __RESTRICT__ twi, const float interpT);
return_t twiTranslucent(const twInstance *const __RESTRICT__ twi);

#endif
