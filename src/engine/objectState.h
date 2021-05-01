#ifndef OBJECTSTATE_H
#define OBJECTSTATE_H

#include "state.h"
#include "skeletonShared.h"
#include "transform.h"

typedef struct sklInstance sklInstance;

/** This should have its own allocator. **/
typedef struct objState objState;
typedef struct objState {
	transform *configuration;  // Skeleton state.
	objState *previous;   // Previous state.
} objState;

return_t objStateAllocate(objState ***oldestStatePrevious, const sklInstance *const __RESTRICT__ skeletonData);
void objStateCopyBone(objState *state, const boneIndex_t i);

#endif
