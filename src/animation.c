#include "animation.h"

/** These animation functions are particularly bad, redo them later maybe? **/

unsigned char animInstInit(animationInstance *animInst, const size_t stateNum){
	float bytesFloat = stateNum*sizeof(float);
	size_t bytesSizeT = stateNum*sizeof(size_t);
	animInst->currentLoops = 0;
	animInst->currentAnim = malloc(bytesSizeT);
	if(animInst->currentAnim == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	animInst->currentFrame = malloc(bytesSizeT);
	if(animInst->currentFrame == NULL){
		/** Memory allocation failure. **/
		free(animInst->currentAnim);
		return 0;
	}
	animInst->nextFrame = malloc(bytesSizeT);
	if(animInst->nextFrame == NULL){
		/** Memory allocation failure. **/
		free(animInst->currentAnim);
		free(animInst->currentFrame);
		return 0;
	}
	animInst->prevElapsedTime = malloc(bytesFloat);
	if(animInst->prevElapsedTime == NULL){
		/** Memory allocation failure. **/
		free(animInst->currentAnim);
		free(animInst->currentFrame);
		free(animInst->nextFrame);
		return 0;
	}
	animInst->totalElapsedTime = malloc(bytesFloat);
	if(animInst->totalElapsedTime == NULL){
		/** Memory allocation failure. **/
		free(animInst->currentAnim);
		free(animInst->currentFrame);
		free(animInst->nextFrame);
		free(animInst->prevElapsedTime);
		return 0;
	}
	size_t i;
	for(i = 0; i < stateNum; ++i){
		// Set each value to 0.
		animInst->currentAnim[i] = 0;
		animInst->currentFrame[i] = 0;
		animInst->nextFrame[i] = 0;
		animInst->prevElapsedTime[i] = 0.f;
		animInst->totalElapsedTime[i] = 0.f;
	}
	return 1;
}
void animInstDelete(animationInstance *animInst){
	if(animInst->currentAnim != NULL){
		free(animInst->currentAnim);
	}
	if(animInst->currentFrame != NULL){
		free(animInst->currentFrame);
	}
	if(animInst->nextFrame != NULL){
		free(animInst->nextFrame);
	}
	if(animInst->prevElapsedTime != NULL){
		free(animInst->prevElapsedTime);
	}
	if(animInst->totalElapsedTime != NULL){
		free(animInst->totalElapsedTime);
	}
}
void animDataInit(animationData *animData){
	animData->desiredLoops = -1;
	animData->frameNum = 0;
	animData->frameDelays = NULL;
}
void animDataDelete(animationData *animData){
	if(animData->frameDelays != NULL){
		free(animData->frameDelays);
	}
}

void animResetInterpolation(animationInstance *animInst, const size_t stateNum){
	size_t i;
	for(i = stateNum-1; i > 0; --i){
		animInst->currentAnim[i] = animInst->currentAnim[i-1];
		animInst->currentFrame[i] = animInst->currentFrame[i-1];
		animInst->nextFrame[i] = animInst->nextFrame[i-1];
		animInst->prevElapsedTime[i] = animInst->prevElapsedTime[i-1];
		animInst->totalElapsedTime[i] = animInst->totalElapsedTime[i-1];
	}
}

void animAdvance(animationInstance *animInst, const animationData *animData, const float elapsedTime){

	if(animData->frameNum > 0){

		const float animationLength = animData->frameDelays[animData->frameNum-1];

		*animInst->prevElapsedTime = elapsedTime;
		if(animationLength > 0.f && elapsedTime != 0.f){

			*animInst->totalElapsedTime += elapsedTime;
			if(elapsedTime >= 0.f){

				/* Animation is going forwards. */
				// Handle loops if totalElapsedTime indicates the end of the animation has been passed.
				if(*animInst->totalElapsedTime > animationLength){
					// Iteratively reset totalElapsedTime so it is once again within the bounds of 0 and animationLength.
					do {
						*animInst->totalElapsedTime -= animationLength;
						++animInst->currentLoops;
					} while(*animInst->totalElapsedTime > animationLength);
					// Check if we've looped too far.
					if(animInst->currentLoops > animData->desiredLoops){
						animInst->currentLoops = animData->desiredLoops;
						if(animData->desiredLoops != -1){
							// We're not looping infinitely, so place the animation at the end and return.
							*animInst->currentFrame = animData->frameNum-1;
							*animInst->nextFrame = *animInst->currentFrame;
							*animInst->totalElapsedTime = animationLength;
							return;
						}
					}else{
						*animInst->currentFrame = 0;
						*animInst->nextFrame = 1;
					}
				}
				// Advance currentFrame.
				while(*animInst->totalElapsedTime > animData->frameDelays[*animInst->currentFrame]){
					++(*animInst->currentFrame);
				}
				// Set nextFrame.
				*animInst->nextFrame = (*animInst->currentFrame)+1;
				if(*animInst->nextFrame == animData->frameNum){
					*animInst->nextFrame = 0;
				}

			}else{

				/* Animation is going backwards. */
				// Handle loops if totalElapsedTime indicates the end of the animation has been passed.
				if(*animInst->totalElapsedTime < 0.f){
					unsigned int prevLoops = animInst->currentLoops;
					// Iteratively reset totalElapsedTime so it is once again within the bounds of 0 and animationLength.
					do {
						*animInst->totalElapsedTime += animationLength;
						--animInst->currentLoops;
					} while(*animInst->totalElapsedTime < 0.f);
					// Check if we've looped too far and currentLoops has reset (as it is unsigned).
					if(animInst->currentLoops > prevLoops){
						animInst->currentLoops = 0;
						if(animData->desiredLoops != -1){
							// We're not looping infinitely, so place the animation at the beginning and return.
							*animInst->currentFrame = 0;
							*animInst->nextFrame = 0;
							*animInst->totalElapsedTime = 0.f;
							return;
						}
					}else{
						*animInst->currentFrame = animData->frameNum-1;
						*animInst->nextFrame = 0;
					}
				}
				// Advance currentFrame.
				size_t currentFrameStart;
				if(animInst->currentFrame == 0){
					currentFrameStart = 0.f;
				}else{
					currentFrameStart = animData->frameDelays[(*animInst->currentFrame)-1];
				}
				while(*animInst->totalElapsedTime < currentFrameStart){
					*animInst->nextFrame = *animInst->currentFrame;
					if(*animInst->currentFrame == 0){
						if(animInst->currentLoops > 0){
							*animInst->currentFrame = animData->frameNum-1;
							*animInst->totalElapsedTime = animationLength - *animInst->totalElapsedTime;
							--animInst->currentLoops;
						}else if(animData->desiredLoops < 0){
							*animInst->currentFrame = animData->frameNum-1;
							*animInst->totalElapsedTime = animationLength - *animInst->totalElapsedTime;
						}else{
							return;
						}
					}else{
						--(*animInst->currentFrame);
					}
					if(*animInst->currentFrame == 0){
						currentFrameStart = 0.f;
					}else{
						currentFrameStart = animData->frameDelays[(*animInst->currentFrame)-1];
					}
				}

			}

		}

	}

}

void animGetRenderData(const animationInstance *animInst, const animationData *animData, const size_t state, const float interpT,
                       size_t *startFrame, size_t *endFrame, float *animInterpT){

	*startFrame = 0;

	const float animLength = animData->frameDelays[animData->frameNum-1];
	if(animLength > 0.f){

		float interpStartTime = animInst->totalElapsedTime[state] - (animInst->prevElapsedTime[state] * (1.f - interpT));

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
			if(*startFrame == 0){
				startFrameTime = 0.f;
			}else{
				startFrameTime = animData->frameDelays[*startFrame-1];
			}
			float endFrameTime = animData->frameDelays[*startFrame];
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
