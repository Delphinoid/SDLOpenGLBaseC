#include "animation.h"
#include "memoryManager.h"

/** These animation functions are particularly bad, redo them later maybe? **/

void animInstInit(animationInstance *animInst){
	animInst->currentLoops = 0;
	animInst->currentFrame = 0;
	animInst->nextFrame = 0;
	animInst->prevElapsedTime = 0.f;
	animInst->totalElapsedTime = 0.f;
}
void animDataInit(animationData *animData){
	animData->desiredLoops = -1;
	animData->frameNum = 0;
	animData->frameDelays = NULL;
}
void animDataDelete(animationData *animData){
	if(animData->frameDelays != NULL){
		memFree(animData->frameDelays);
	}
}

void animAdvance(animationInstance *animInst, const animationData *animData, const float elapsedTime){

	if(animData->frameNum > 1){

		const float animationLength = animData->frameDelays[animData->frameNum-1];

		animInst->prevElapsedTime = elapsedTime;
		if(animationLength > 0.f && elapsedTime != 0.f){

			animInst->totalElapsedTime += elapsedTime;
			if(elapsedTime >= 0.f){

				/* Animation is going forwards. */
				// Handle loops if totalElapsedTime indicates the end of the animation has been passed.
				if(animInst->totalElapsedTime > animationLength){
					// Iteratively reset totalElapsedTime so it is once again within the bounds of 0 and animationLength.
					do {
						/**
						*** At some point I added the two lines below, which
						*** are quite obviously wrong and end up causing the
						*** current and next frame variables to overflow very
						*** quickly. Why???
						**/
						//animInst->currentFrame -= animData->frameNum;
						//animInst->nextFrame = animInst->currentFrame;
						animInst->totalElapsedTime -= animationLength;
						++animInst->currentLoops;
					} while(animInst->totalElapsedTime > animationLength);
					// Check if we've looped too far.
					if(animInst->currentLoops > animData->desiredLoops){
						animInst->currentLoops = animData->desiredLoops;
						if(animData->desiredLoops >= 0){
							// We're not looping infinitely, so place the animation at the end and return.
							animInst->currentFrame = animData->frameNum-1;
							animInst->nextFrame = animInst->currentFrame;
							animInst->totalElapsedTime = animationLength;
							return;
						}
					}
					animInst->currentFrame = 0;
					animInst->nextFrame = 1;
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
				frameIndex_t currentFrameStart;

				// Handle loops if totalElapsedTime indicates the end of the animation has been passed.
				if(animInst->totalElapsedTime < 0.f){
					uint_least8_t prevLoops = animInst->currentLoops;
					// Iteratively reset totalElapsedTime so it is once again within the bounds of 0 and animationLength.
					do {
						/**
						*** Same as the comment above.
						**/
						//animInst->currentFrame += animData->frameNum;
						//animInst->nextFrame = animInst->currentFrame;
						animInst->totalElapsedTime += animationLength;
						--animInst->currentLoops;
					} while(animInst->totalElapsedTime < 0.f);
					// Check if we've looped too far and currentLoops has overflowed (as it is unsigned).
					if(animInst->currentLoops < prevLoops){
						animInst->currentLoops = 0;
						if(animData->desiredLoops >= 0){
							// We're not looping infinitely, so place the animation at the beginning and return.
							animInst->currentFrame = 0;
							animInst->nextFrame = 0;
							animInst->totalElapsedTime = 0.f;
							return;
						}
					}
					animInst->currentFrame = animData->frameNum-1;
					animInst->nextFrame = 0;
				}
				// Advance currentFrame.
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

void animGetRenderData(const animationInstance *animInst, const animationData *animData, const float interpT,
                       frameIndex_t *startFrame, frameIndex_t *endFrame, float *animInterpT){

	/*
	** Calculates the start frame, the end frame and the progress through the two based on the provided data.
	*/

	*startFrame = 0;

	const float animLength = animData->frameDelays[animData->frameNum-1];
	if(animLength > 0.f){

		float interpStartTime = animInst->totalElapsedTime - (animInst->prevElapsedTime * (1.f - interpT));

		while(interpStartTime < 0.f){
			interpStartTime += animLength;
		}
		while(interpStartTime > animLength){
			interpStartTime -= animLength;
		}

		// Find the two frames to interpolate between.
		while(interpStartTime > animData->frameDelays[*startFrame]){
			++(*startFrame);
		}
		if(endFrame != NULL){
			*endFrame = *startFrame + 1;
			if(*endFrame >= animData->frameNum){
				*endFrame = 0;
			}
		}

		// Calculate progress through the two frames.
		if(animInterpT != NULL){
			float startFrameTime;
			float endFrameTime;
			if(*startFrame == 0){
				startFrameTime = 0.f;
			}else{
				startFrameTime = animData->frameDelays[*startFrame-1];
			}
			endFrameTime = animData->frameDelays[*startFrame];
			*animInterpT = (interpStartTime - startFrameTime) / (endFrameTime - startFrameTime);
		}

	}else{
		if(endFrame != NULL){
			*endFrame = 0;
		}
		if(animInterpT != NULL){
			*animInterpT = 0.f;
		}
	}

}
