#ifndef ANIMATION_H
#define ANIMATION_H

#include <stdlib.h>
#include <stdint.h>

/*
** NOTE: These structures are just dumb groups of data that make
** animation easier and more consistent across other larger data
** structures (like texture wrappers and skeletons).
*/

typedef uint_least16_t animIndex_t;
typedef uint_least16_t frameIndex_t;

typedef struct {
	uint_least8_t desiredLoops;  // How many times the animation will loop (with -1 being infinite times).
	frameIndex_t frameNum;       // The total number of keyframes in the animation.
	float *frameDelays;          // Represents when each frame ends. "frameEnds" is a more accurate name, but this name is a bit of an inside joke now.
} animationData;

typedef struct {
	uint_least8_t currentLoops;
	frameIndex_t currentFrame;
	frameIndex_t nextFrame;
	float prevElapsedTime;
	float totalElapsedTime;
} animationInstance;

/** The two functions below are REALLY bad, redo them later. **/
void animInstInit(animationInstance *const restrict animInst);
void animDataInit(animationData *const restrict animData);
void animDataDelete(animationData *const restrict animData);
void animAdvance(animationInstance *const restrict animInst, const animationData *const restrict animData, const float elapsedTime);
void animGetRenderData(const animationInstance *const restrict animInst, const animationData *const restrict animData, const float interpT,
                       frameIndex_t *const restrict startFrame, frameIndex_t *const restrict endFrame, float *const restrict animInterpT);

#endif
