#ifndef OBJECTSTATE_H
#define OBJECTSTATE_H

#include "skeletonShared.h"
#include "bone.h"

typedef struct sklInstance sklInstance;

/** This should have its own allocator. **/
typedef struct objectState objectState;
typedef struct objectState {
	bone *configuration;  // Skeleton state.
	objectState *previous;   // Previous state.
} objectState;

return_t objStateAllocate(objectState ***oldestStatePrevious, const sklInstance *const restrict skeletonData);
void objStateCopyBone(objectState *state, const boneIndex_t i);

#endif
