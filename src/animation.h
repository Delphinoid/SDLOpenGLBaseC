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
	float *blendFrameProgress;
	float *blendFrameEnd;
	size_t *currentAnim;
	size_t *currentFrame;
	size_t *nextAnim;
	size_t *nextFrame;
	float *prevElapsedTime;
	float *totalElapsedTime;
} animationInstance;

/** The two functions below are REALLY bad, redo them later. **/
unsigned char animInstInit(animationInstance *animInst, const size_t stateNum);
void animInstDelete(animationInstance *animInst);
void animDataInit(animationData *animData);
void animDataDelete(animationData *animData);
void animResetInterpolation(animationInstance *animInst, const size_t stateNum);
void animAdvance(animationInstance *animInst, const animationData *animData, const float elapsedTime);
void animGetRenderData(const animationInstance *animInst, const animationData *animData, const size_t state, const float interpT,
                       size_t *startAnim, size_t *startFrame, float *startProgress, size_t *endAnim, size_t *endFrame, float *animInterpT);

#endif
