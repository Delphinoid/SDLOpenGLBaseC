#include "animation.h"

/** These animation functions are particularly bad, redo them later maybe? **/

unsigned char animInstInit(animationInstance *animInst, const size_t stateNum){
	float bytesFloat = stateNum*sizeof(float);
	size_t bytesSizeT = stateNum*sizeof(size_t);
	animInst->currentLoops = 0;
	animInst->blendFrameProgress = malloc(bytesFloat);
	if(animInst->blendFrameProgress == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	animInst->blendFrameEnd = malloc(bytesFloat);
	if(animInst->blendFrameEnd == NULL){
		/** Memory allocation failure. **/
		free(animInst->blendFrameProgress);
		return 0;
	}
	animInst->currentAnim = malloc(bytesSizeT);
	if(animInst->currentAnim == NULL){
		/** Memory allocation failure. **/
		free(animInst->blendFrameProgress);
		free(animInst->blendFrameEnd);
		return 0;
	}
	animInst->currentFrame = malloc(bytesSizeT);
	if(animInst->currentFrame == NULL){
		/** Memory allocation failure. **/
		free(animInst->blendFrameProgress);
		free(animInst->blendFrameEnd);
		free(animInst->currentAnim);
		return 0;
	}
	animInst->nextAnim = malloc(bytesSizeT);
	if(animInst->nextAnim == NULL){
		/** Memory allocation failure. **/
		free(animInst->blendFrameProgress);
		free(animInst->blendFrameEnd);
		free(animInst->currentAnim);
		free(animInst->currentFrame);
		return 0;
	}
	animInst->nextFrame = malloc(bytesSizeT);
	if(animInst->nextFrame == NULL){
		/** Memory allocation failure. **/
		free(animInst->blendFrameProgress);
		free(animInst->blendFrameEnd);
		free(animInst->currentAnim);
		free(animInst->currentFrame);
		free(animInst->nextAnim);
		return 0;
	}
	animInst->prevElapsedTime = malloc(bytesFloat);
	if(animInst->prevElapsedTime == NULL){
		/** Memory allocation failure. **/
		free(animInst->blendFrameProgress);
		free(animInst->blendFrameEnd);
		free(animInst->currentAnim);
		free(animInst->currentFrame);
		free(animInst->nextAnim);
		free(animInst->nextFrame);
		return 0;
	}
	animInst->totalElapsedTime = malloc(bytesFloat);
	if(animInst->totalElapsedTime == NULL){
		/** Memory allocation failure. **/
		free(animInst->blendFrameProgress);
		free(animInst->blendFrameEnd);
		free(animInst->currentAnim);
		free(animInst->currentFrame);
		free(animInst->nextAnim);
		free(animInst->nextFrame);
		free(animInst->prevElapsedTime);
		return 0;
	}
	size_t i;
	for(i = 0; i < stateNum; ++i){
		// Set each value to 0.
		animInst->blendFrameProgress[i] = 0.f;
		animInst->blendFrameEnd[i] = 0.f;
		animInst->currentAnim[i] = 0;
		animInst->currentFrame[i] = 0;
		animInst->nextAnim[i] = 0;
		animInst->nextFrame[i] = 0;
		animInst->prevElapsedTime[i] = 0.f;
		animInst->totalElapsedTime[i] = 0.f;
	}
	return 1;
}
void animInstDelete(animationInstance *animInst){
	if(animInst->blendFrameProgress != NULL){
		free(animInst->blendFrameProgress);
	}
	if(animInst->blendFrameEnd != NULL){
		free(animInst->blendFrameEnd);
	}
	if(animInst->currentAnim != NULL){
		free(animInst->currentAnim);
	}
	if(animInst->currentFrame != NULL){
		free(animInst->currentFrame);
	}
	if(animInst->nextAnim != NULL){
		free(animInst->nextAnim);
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
		// Shift each value over to make room for the new ones.
		animInst->blendFrameProgress[i] = animInst->blendFrameProgress[i-1];
		animInst->blendFrameEnd[i] = animInst->blendFrameEnd[i-1];
		animInst->currentAnim[i] = animInst->currentAnim[i-1];
		animInst->currentFrame[i] = animInst->currentFrame[i-1];
		animInst->nextAnim[i] = animInst->nextAnim[i-1];
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
				// Find how much time totalElapsedTime has to exceed for a loop.
				float animationEnd;
				if(*animInst->blendFrameEnd > 0.f){
					animationEnd = *animInst->blendFrameEnd + animationLength - animData->frameDelays[*animInst->nextFrame];
				}else{
					animationEnd = animationLength;
				}
				// Handle loops if totalElapsedTime indicates the end of the animation has been passed.
				if(*animInst->totalElapsedTime > animationEnd){
					// Reset blendFrameEnd.
					*animInst->totalElapsedTime -= *animInst->blendFrameEnd;
					*animInst->blendFrameEnd = 0.f;
					do {
						*animInst->totalElapsedTime -= animationLength;
						// Check if the animation has finished.
						if(animInst->currentLoops < animData->desiredLoops || animData->desiredLoops < 0){
							++animInst->currentLoops;
						}else{
							*animInst->currentFrame = animData->frameNum-1;
							*animInst->nextFrame = *animInst->currentFrame;
							return;
						}
					} while(*animInst->totalElapsedTime > animationLength);
					*animInst->currentFrame = 0;
					*animInst->nextFrame = 1;
				}
				// Special handling for animation blending.
				if(*animInst->blendFrameEnd > 0.f && *animInst->totalElapsedTime > *animInst->blendFrameEnd){
					*animInst->blendFrameEnd = 0.f;
					*animInst->currentFrame = *animInst->nextFrame;
					*animInst->totalElapsedTime += animData->frameDelays[*animInst->currentFrame];
				}else{
					return;
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
				// If the animation has gone backwards during a blend, set it to currentFrame.
				if(*animInst->blendFrameEnd > 0.f && *animInst->totalElapsedTime < 0.f){
					*animInst->totalElapsedTime += animData->frameDelays[*animInst->currentFrame];
					*animInst->blendFrameEnd = 0.f;
				}
				// Handle loops if totalElapsedTime indicates the end of the animation has been passed.
				if(*animInst->totalElapsedTime < 0.f){
					do {
						*animInst->totalElapsedTime += animationLength;
						// Check if the animation has finished.
						if(animInst->currentLoops > 0){
							--animInst->currentLoops;
						}else if(animData->desiredLoops >= 0){
							*animInst->currentFrame = 0;
							*animInst->nextFrame = 0;
							return;
						}
					} while(*animInst->totalElapsedTime < 0.f);
					*animInst->currentFrame = animData->frameNum-1;
					*animInst->nextFrame = 0;
				}
				// Special handling for animation blending.
				if(*animInst->totalElapsedTime < *animInst->blendFrameEnd){
					*animInst->blendFrameEnd = 0.f;
				}else{
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

}

void animGetRenderData(const animationInstance *animInst, const animationData *animData, const size_t state, const float interpT,
                       size_t *startAnim, size_t *startFrame, float *startProgress, size_t *endAnim, size_t *endFrame, float *animInterpT){

	float interpStartTime = animInst->totalElapsedTime[state] - (animInst->prevElapsedTime[state] * (1.f - interpT));

	if(animInst->blendFrameEnd[state] > 0.f){
		if(interpStartTime < animInst->blendFrameEnd[state]){
			// Interpolating between frames from two separate animations.
			*startAnim = animInst->currentAnim[state];
			*startFrame = animInst->currentFrame[state];
			if(startProgress != NULL){
				*startProgress = animInst->blendFrameProgress[state];
			}
			if(endAnim != NULL){
				*endAnim = animInst->nextAnim[state];
			}
			if(endFrame != NULL){
				*endFrame = animInst->nextFrame[state];
			}
			if(animInterpT != NULL){
				*animInterpT = interpStartTime / animInst->blendFrameEnd[state];
			}
			return;
		}else{
			interpStartTime += animData->frameDelays[animInst->nextFrame[state]] - animInst->blendFrameEnd[state];
		}
	}

	*startAnim = animInst->nextAnim[state];
	*startFrame = 0;
	if(startProgress != NULL){
		*startProgress = 0.f;
	}
	if(endAnim != NULL){
		*endAnim = animInst->nextAnim[state];
	}

	const float animLength = animData->frameDelays[animData->frameNum-1];
	if(animLength > 0.f){

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
		if(animInterpT != NULL){
			*animInterpT = 0.f;
		}
	}

}
