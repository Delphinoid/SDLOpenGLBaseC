#include "objectState.h"
#include "memoryManager.h"
#include "skeleton.h"

__FORCE_INLINE__ return_t objStateAllocate(objState ***oldestStatePrevious, const sklInstance *const __RESTRICT__ skeletonData){
	objState *const state = memAllocate(sizeof(objState) + skeletonData->skl->boneNum * sizeof(transform));
	if(state != NULL){
		state->configuration = (transform *)((byte_t *)state + sizeof(objState));
		state->previous = NULL;
		**oldestStatePrevious = state;
		*oldestStatePrevious = &state->previous;
		return 1;
	}
	return -1;
}

__FORCE_INLINE__ void objStateCopyBone(objState *state, const boneIndex_t i){
	transform last = state->configuration[i];
	while(state->previous != NULL){
		const transform swap = state->previous->configuration[i];
		state->previous->configuration[i] = last;
		last = swap;
		state = state->previous;
	}
}