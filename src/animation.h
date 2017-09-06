#ifndef ANIMATION_H
#define ANIMATION_H

#include <stdlib.h>

/*
** NOTE: These structures are just dumb data holders that
** make animation easier and more consistent across other
** larger data types (like texture wrappers and skeletons).
** You must initialize (and, in the case of animationData,
** free) each member manually.
*/

typedef struct {
	int desiredLoops;    // How many times the animation will loop (with -1 being infinite times)
	size_t frameNum;     // The total number of keyframes in the animation
	float *frameDelays;  // Represents how long each frame should last
} animationData;

typedef struct {
	int currentLoops;
	size_t currentFrame;
	float currentFrameProgress;
	float currentFrameLength;
	size_t nextFrame;
} animationInstance;

void animAdvance(animationInstance *animInst, const animationData *animData, const float timeElapsed);

#endif
