#ifndef ANIMATION_H
#define ANIMATION_H

#include <stdlib.h>

/*
** NOTE: These structures are just dumb groups of data that make
** animation easier and more consistent across other larger data
** structures (like texture wrappers and skeletons).
*/

typedef struct {
	int desiredLoops;    // How many times the animation will loop (with -1 being infinite times).
	size_t frameNum;     // The total number of keyframes in the animation.
	float *frameDelays;  // Represents when each frame ends. "frameEnds" is a more accurate name, but this name is a bit of an inside joke now.
} animationData;

typedef struct {
	int currentLoops;
	float blendFrameEnd;
	size_t currentFrame;
	size_t nextFrame;
	float prevElapsedTime;
	float totalElapsedTime;
} animationInstance;

/** The two functions below are REALLY bad, redo them later. **/
void animInit(animationInstance *animInst);
void animDelete(animationData *animData);
void animAdvance(animationInstance *animInst, const animationData *animData, const float elapsedTime);
float animGetInterpProgress(const animationInstance *animInst, const animationData *animData, const float interpT);
size_t animGetInterpFrame(const animationInstance *animInst, const animationData *animData, const float animInterpProgress);
float animGetInterpT(const animationInstance *animInst, const animationData *animData, const float animInterpProgress, const size_t animInterpFrame, size_t *animInterpFrameNext);

#endif
