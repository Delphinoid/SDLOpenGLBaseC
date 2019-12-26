#include "objectState.h"
#include "memoryManager.h"
#include "skeleton.h"
#include "inline.h"

__FORCE_INLINE__ return_t objStateAllocate(objectState ***oldestStatePrevious, const sklInstance *const restrict skeletonData){
	objectState *const state = memAllocate(sizeof(objectState) + skeletonData->skl->boneNum * sizeof(bone));
	if(state != NULL){
		state->configuration = (bone *)((byte_t *)state + sizeof(objectState));
		state->previous = NULL;
		**oldestStatePrevious = state;
		*oldestStatePrevious = &state->previous;
		return 1;
	}
	return -1;
}

__FORCE_INLINE__ void objStateCopyBone(objectState *state, const boneIndex_t i){
	bone last = state->configuration[i];
	while(state->previous != NULL){
		const bone swap = state->previous->configuration[i];
		state->previous->configuration[i] = last;
		last = swap;
		state = state->previous;
	}
}