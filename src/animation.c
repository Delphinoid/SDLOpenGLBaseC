#include "animation.h"

void animAdvance(animationInstance *animInst, const animationData *animData, const float timeElapsed){

	// Only continue if there's more than one frame.
	if(animData->frameNum > 1){

		animInst->currentFrameProgress += timeElapsed;

		// Check if the frame has finished playing (forwards).
		if(animInst->currentFrameProgress > animInst->currentFrameLength){
			do {
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
						animInst->nextFrame = animData->frameNum;
					}
				}
			} while(animInst->currentFrameProgress > animInst->currentFrameLength);

		// Check if the frame has finished playing (backwards).
		}else if(animInst->currentFrameProgress < 0.f){
			do {
				animInst->nextFrame = animInst->currentFrame;
				animInst->currentFrameLength = animData->frameDelays[animInst->currentFrame];
				animInst->currentFrameProgress += animInst->currentFrameLength;
				// Check if the animation should loop (it's going to go before the first frame).
				if(animInst->currentFrame <= 0){
					// Check if the animation can still loop.
					if(--animInst->currentLoops >= 0 || animData->desiredLoops < 0){
						// If it can, reset it to the end.
						animInst->nextFrame = 0;
					}else{
						animInst->currentLoops = 0;
					}
				}else{
					--animInst->currentFrame;
				}
			} while(animInst->currentFrameProgress < 0.f);
		}

	}

}
