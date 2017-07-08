#ifndef ANIMATIONHANDLER_H
#define ANIMATIONHANDLER_H

#include <stdint.h>
#include "cVector.h"

typedef struct {
	float delayMod;
	size_t currentAnim;   // Position of the current animation in the animations vector
	size_t currentFrame;  // The current frame of the animation
	int currentLoops;
    uint32_t lastUpdate;  // Set to SDL_GetTicks() at the beginning of each animation, while
	                      // lastUpdate-SDL_GetTicks() > frameDelay, add frameDelay and advance
	                      // the animation
} animationHandler;

void animInit(animationHandler *a);
void animChange(animationHandler *a, size_t newAnim);
void animAdvance(animationHandler *a, cVector *frameDelays,
                 int desiredLoops, uint32_t currentTick, float speedMod);
#endif
