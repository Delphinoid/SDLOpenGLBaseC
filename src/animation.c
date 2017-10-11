#include "animation.h"

/** These animation functions are particularly bad, redo them later maybe? **/

void animInit(animationInstance *animInst){
	animInst->currentLoops = 0;
	animInst->blendFrameEnd = 0.f;
	animInst->currentFrame = 0;
	animInst->nextFrame = 0;
	animInst->prevElapsedTime = 0.f;
	animInst->totalElapsedTime = 0.f;
}
void animDelete(animationData *animData){
	if(animData->frameDelays != NULL){
		free(animData->frameDelays);
	}
}

void animAdvance(animationInstance *animInst, const animationData *animData, const float elapsedTime){

	if(animData->frameNum > 0){

		const float animationLength = animData->frameDelays[animData->frameNum-1];

		animInst->prevElapsedTime = elapsedTime;
		if(animationLength > 0.f && elapsedTime != 0.f){

			animInst->totalElapsedTime += elapsedTime;
			if(elapsedTime >= 0.f){

				/* Animation is going forwards. */
				// Find how much time totalElapsedTime has to exceed for a loop.
				float animationEnd;
				if(animInst->blendFrameEnd > 0.f){
					animationEnd = animInst->blendFrameEnd + animationLength - animData->frameDelays[animInst->nextFrame];
				}else{
					animationEnd = animationLength;
				}
				// Handle loops if totalElapsedTime indicates the end of the animation has been passed.
				if(animInst->totalElapsedTime > animationEnd){
					// Reset blendFrameEnd.
					animInst->totalElapsedTime -= animInst->blendFrameEnd;
					animInst->blendFrameEnd = 0.f;
					do {
						animInst->totalElapsedTime -= animationLength;
						// Check if the animation has finished.
						if(animInst->currentLoops < animData->desiredLoops || animData->desiredLoops < 0){
							++animInst->currentLoops;
						}else{
							animInst->currentFrame = animData->frameNum-1;
							animInst->nextFrame = animInst->currentFrame;
							return;
						}
					} while(animInst->totalElapsedTime > animationLength);
					animInst->currentFrame = 0;
					animInst->nextFrame = 1;
				}
				// Special handling for animation blending.
				if(animInst->blendFrameEnd > 0.f && animInst->totalElapsedTime > animInst->blendFrameEnd){
					animInst->blendFrameEnd = 0.f;
					animInst->currentFrame = animInst->nextFrame;
					animInst->totalElapsedTime += animData->frameDelays[animInst->currentFrame];
				}else{
					return;
				}
				// Advance currentFrame.
				while(animInst->totalElapsedTime > animData->frameDelays[animInst->currentFrame]){
					++animInst->currentFrame;
				}
				// Set nextFrame.
				animInst->nextFrame = animInst->currentFrame+1;
				if(animInst->nextFrame == animData->frameNum){
					animInst->nextFrame = 0;
				}

			}else{

				/* Animation is going backwards. */
				// If the animation has gone backwards during a blend, set it to currentFrame.
				if(animInst->blendFrameEnd > 0.f && animInst->totalElapsedTime < 0.f){
					animInst->totalElapsedTime += animData->frameDelays[animInst->currentFrame];
					animInst->blendFrameEnd = 0.f;
				}
				// Handle loops if totalElapsedTime indicates the end of the animation has been passed.
				if(animInst->totalElapsedTime < 0.f){
					do {
						animInst->totalElapsedTime += animationLength;
						// Check if the animation has finished.
						if(animInst->currentLoops > 0){
							--animInst->currentLoops;
						}else if(animData->desiredLoops >= 0){
							animInst->currentFrame = 0;
							animInst->nextFrame = 0;
							return;
						}
					} while(animInst->totalElapsedTime < 0.f);
					animInst->currentFrame = animData->frameNum-1;
					animInst->nextFrame = 0;
				}
				// Special handling for animation blending.
				if(animInst->totalElapsedTime < animInst->blendFrameEnd){
					animInst->blendFrameEnd = 0.f;
				}else{
					// Advance currentFrame.
					size_t currentFrameStart;
					if(animInst->currentFrame == 0){
						currentFrameStart = 0.f;
					}else{
						currentFrameStart = animData->frameDelays[animInst->currentFrame-1];
					}
					while(animInst->totalElapsedTime < currentFrameStart){
						animInst->nextFrame = animInst->currentFrame;
						if(animInst->currentFrame == 0){
							if(animInst->currentLoops > 0){
								animInst->currentFrame = animData->frameNum-1;
								animInst->totalElapsedTime = animationLength - animInst->totalElapsedTime;
								--animInst->currentLoops;
							}else if(animData->desiredLoops < 0){
								animInst->currentFrame = animData->frameNum-1;
								animInst->totalElapsedTime = animationLength - animInst->totalElapsedTime;
							}else{
								return;
							}
						}else{
							--animInst->currentFrame;
						}
						if(animInst->currentFrame == 0){
							currentFrameStart = 0.f;
						}else{
							currentFrameStart = animData->frameDelays[animInst->currentFrame-1];
						}
					}
				}

			}

		}

	}

}

float animGetInterpProgress(const animationInstance *animInst, const animationData *animData, const float interpT){
	/* Returns the (interpolated) progress through the current animation. */
	const float animLength = animData->frameDelays[animData->frameNum-1];
	float animInterpProgress = 0.f;
	if(animLength > 0.f){
		animInterpProgress = animInst->totalElapsedTime - animInst->prevElapsedTime * (1.f - interpT);
		while(animInterpProgress < 0.f){
			animInterpProgress += animLength;
		}
		while(animInterpProgress > animLength){
			animInterpProgress -= animLength;
		}
	}
	return animInterpProgress;
}
size_t animGetInterpFrame(const animationInstance *animInst, const animationData *animData, const float animInterpProgress){
	if(animInterpProgress < animInst->blendFrameEnd){
		// Special handling for animation blending.
		return -1;
	}
	size_t animInterpFrame = 0;
	while(animInterpProgress > animData->frameDelays[animInterpFrame]){
		++animInterpFrame;
	}
	return animInterpFrame;
}
float animGetInterpT(const animationInstance *animInst, const animationData *animData, const float animInterpProgress,
                     const size_t animInterpFrame, size_t *animInterpFrameNext){
	float animInterpFrameStart;
	float animInterpFrameEnd;
	if(animInterpFrame < animData->frameNum){
		animInterpFrameEnd = animData->frameDelays[animInterpFrame];
		*animInterpFrameNext = animInterpFrame+1;
		if(*animInterpFrameNext >= animData->frameNum){
			*animInterpFrameNext = 0;
		}
		if(animInterpFrame == 0){
			animInterpFrameStart = 0.f;
		}else{
			animInterpFrameStart = animData->frameDelays[animInterpFrame-1];
		}
	}else{
		// Special handling for animation blending
		*animInterpFrameNext = animInst->nextFrame;
		animInterpFrameStart = 0.f;
		animInterpFrameEnd = animInst->blendFrameEnd;
	}
	return (animInterpProgress - animInterpFrameStart) / (animInterpFrameEnd - animInterpFrameStart);
}
