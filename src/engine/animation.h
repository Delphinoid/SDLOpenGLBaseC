#ifndef ANIMATION_H
#define ANIMATION_H

#include "qualifiers.h"
#include <stddef.h>
#include <stdint.h>

// NOTE: These structures are just dumb groups of data that make
// animation easier and more consistent across other larger data
// structures (like texture wrappers and skeletons).
//
// Additionally, all time intervals are in milliseconds.

typedef uint_least16_t animIndex_t;
typedef uint_least16_t frameIndex_t;

typedef struct {
	uint_least8_t desiredLoops;  // How many times the animation will loop (with -1 being infinite times).
	frameIndex_t frameNum;       // The total number of keyframes in the animation.
	float *frameDelays;          // Represents when each frame ends. "frameEnds" is a more accurate name, but this name is a bit of an inside joke now.
} animationData;

typedef struct {
	frameIndex_t currentFrame;
	frameIndex_t nextFrame;
	float prevElapsedTime;
	float totalElapsedTime;
	uint_least8_t currentLoops;
} animationInstance;

/** The two functions below are REALLY bad, redo them later. **/
void animInstInit(animationInstance *const __RESTRICT__ animInst);
void animDataInit(animationData *const __RESTRICT__ animData);
void animDataDelete(animationData *const __RESTRICT__ animData);
void animTick(animationInstance *const __RESTRICT__ animInst, const animationData *const __RESTRICT__ animData, const float dt_ms);
void animState(const animationInstance *const __RESTRICT__ animInst, const animationData *const __RESTRICT__ animData, const float interpT,
               frameIndex_t *const __RESTRICT__ startFrame, frameIndex_t *const __RESTRICT__ endFrame, float *const __RESTRICT__ animInterpT);

#endif
