#include "animation.h"

void animAdvance(animationInstance *animInst, const animationData *animData, const float elapsedTime){

	animInst->currentFrameProgress += elapsedTime;

	// Check if the frame has finished playing (forwards).
	if(animInst->currentFrameProgress > animInst->currentFrameLength && animInst->currentFrameLength > 0.f){
		do {
			/* Advance the animation to the next frame. */
			animInst->currentFrame = animInst->nextFrame;
			animInst->currentFrameProgress -= animInst->currentFrameLength;
			animInst->currentFrameLength = animData->frameDelays[animInst->currentFrame];
			// Check if the animation should loop (it's going to go past the last frame).
			if(++animInst->nextFrame >= animData->frameNum){
				// Check if the animation can still loop.
				if(++animInst->currentLoops < animData->desiredLoops || animData->desiredLoops < 0){
					// If it can, reset it to the beginning.
					animInst->nextFrame = 0;
				}else{
					animInst->nextFrame = animData->frameNum-1;
				}
			}
		} while(animInst->currentFrameProgress > animInst->currentFrameLength && animInst->currentFrameLength > 0.f);

	// Check if the frame has finished playing (backwards).
	}else if(animInst->currentFrameProgress < 0.f && animInst->currentFrameLength > 0.f){
		do {
			/* Advance the animation to the previous frame. */
			animInst->nextFrame = animInst->currentFrame;
			// Check if the animation should loop (it's going to go before the first frame).
			if(animInst->currentFrame <= 0){
				// Check if the animation can still loop.
				if(--animInst->currentLoops >= 0 || animData->desiredLoops < 0){
					// If it can, reset it to the end.
					animInst->currentFrame = animData->frameNum-1;
				}else{
					animInst->currentLoops = 0;
				}
			}else{
				--animInst->currentFrame;
			}
			animInst->currentFrameLength = animData->frameDelays[animInst->currentFrame];
			animInst->currentFrameProgress += animInst->currentFrameLength;
		} while(animInst->currentFrameProgress < 0.f && animInst->currentFrameLength > 0.f);
	}

}
