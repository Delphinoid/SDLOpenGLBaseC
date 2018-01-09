#include "stateManager.h"
#include <string.h>

signed char smObjectTypeNew(stateManager *sm, signed char (*stateInit)(void*),
                            signed char (*stateNew)(void*), signed char (*stateCopy)(void*, void*),
                            void (*stateResetInterpolation)(void*), void (*stateDelete)(void*),
                            const size_t size, const size_t capacity){

	size_t i, j;

	/* Create a temporary objectType array that will later replace the current one. */
	stateObjectType *tempBuffer = malloc((sm->objectTypeNum+1) * sizeof(stateObjectType));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return 0;
	}

	/* Allocate memory for the new type's object array. */
	tempBuffer[sm->objectTypeNum].instance = malloc(capacity * sizeof(stateObject));
	if(tempBuffer[sm->objectTypeNum].instance == NULL){
		/** Memory allocation failure. **/
		free(tempBuffer);
		return 0;
	}

	/* Allocate memory for each object's states and initialize their pointers to NULL. */
	for(i = 0; i < capacity; ++i){
		tempBuffer[sm->objectTypeNum].instance[i].active = 0;
		tempBuffer[sm->objectTypeNum].instance[i].state = malloc(sm->stateNum * sizeof(void *));
		if(tempBuffer[sm->objectTypeNum].instance[i].state == NULL){
			break;
		}
		for(j = 0; j < sm->stateNum; ++j){
			tempBuffer[sm->objectTypeNum].instance[i].state[j] = NULL;
		}
	}
	/* Handle a malloc failure. */
	if(i < capacity){
		/** Memory allocation failure. **/
		while(i > 0){
			--i;
			free(tempBuffer[sm->objectTypeNum].instance[i].state);
		}
		free(tempBuffer[sm->objectTypeNum].instance);
		free(tempBuffer);
		return 0;
	}

	/* Copy the old objectTypes over. */
	memcpy(tempBuffer, sm->objectType, sm->objectTypeNum * sizeof(stateObjectType));
	free(sm->objectType);
	sm->objectType = tempBuffer;

	/* Set various variables. */
	sm->objectType[sm->objectTypeNum].stateInit = stateInit;
	sm->objectType[sm->objectTypeNum].stateNew = stateNew;
	sm->objectType[sm->objectTypeNum].stateCopy = stateCopy;
	sm->objectType[sm->objectTypeNum].stateResetInterpolation = stateResetInterpolation;
	sm->objectType[sm->objectTypeNum].stateDelete = stateDelete;
	sm->objectType[sm->objectTypeNum].size = size;
	sm->objectType[sm->objectTypeNum].capacity = capacity;
	++sm->objectTypeNum;

	return 1;

}
static inline signed char smObjectTypeUpdate(stateManager *sm, const size_t objectTypeID){
	/*
	** For each object, shift its state pointers over. Move its last state
	** object pointer to the front and copy the latest state object into it.
	*/
	void *lastState;
	size_t i, j, validStates;
	for(i = 0; i < sm->objectType[objectTypeID].capacity; ++i){
		if(sm->objectType[objectTypeID].instance[i].active){
			if(sm->stateNum > 1){

				/* Shift each state for the current object over. */
				validStates = 0;
				j = sm->stateNum-1;
				lastState = sm->objectType[objectTypeID].instance[i].state[j];
				while(j > 0){
					if(sm->objectType[objectTypeID].instance[i].state[j-1] != NULL){
						++validStates;
					}
					sm->objectType[objectTypeID].instance[i].state[j] = sm->objectType[objectTypeID].instance[i].state[j-1];
					--j;
				}

				if(sm->objectType[objectTypeID].instance[i].state[0] != NULL){

					/*
					** Move lastState's pointer into state 0 and copy the last state into it.
					*/
					if(lastState == NULL){
						/* If lastState is NULL, allocate memory for it. */
						sm->objectType[objectTypeID].instance[i].state[0] = malloc(sm->objectType[objectTypeID].size);
						if(sm->objectType[objectTypeID].instance[i].state[0] == NULL){
							/** Memory allocation failure. **/
							return 0;
						}
						/**/
						/* Prevents crashing in stateCopy() due to uninitialized variables. */
						(*sm->objectType[objectTypeID].stateInit)(sm->objectType[objectTypeID].instance[i].state[0]);
						/**/
					}else{
						sm->objectType[objectTypeID].instance[i].state[0] = lastState;
					}

					if(!(*sm->objectType[objectTypeID].stateCopy)(sm->objectType[objectTypeID].instance[i].state[1], sm->objectType[objectTypeID].instance[i].state[0])){
						/** Memory allocation failure. **/
						return 0;
					}

					// Done once again below.
					(*sm->objectType[objectTypeID].stateResetInterpolation)(sm->objectType[objectTypeID].instance[i].state[0]);

				}else if(lastState != NULL){

					/*
					** The object is being deleted, just free lastState.
					*/
					(*sm->objectType[objectTypeID].stateDelete)(lastState);
					free(lastState);

				}

				if(validStates == 0){

					/*
					** lastState is the only potentially valid state.
					** Free it if possible and set this object to inactive.
					*/
					sm->objectType[objectTypeID].instance[i].active = 0;

				}

			}else{
				// Done once again above.
				(*sm->objectType[objectTypeID].stateResetInterpolation)(sm->objectType[objectTypeID].instance[i].state[0]);
			}

		}
	}
	return 1;
}
static inline void smObjectTypeDelete(stateManager *sm, const size_t objectTypeID){
	size_t i, j;
	for(i = 0; i < sm->objectType[objectTypeID].capacity; ++i){
		for(j = 0; j < sm->stateNum; ++j){
			if(sm->objectType[objectTypeID].instance[i].state[j] != NULL){
				sm->objectType[objectTypeID].stateDelete(sm->objectType[objectTypeID].instance[i].state[j]);
				free(sm->objectType[objectTypeID].instance[i].state[j]);
			}
		}
		free(sm->objectType[objectTypeID].instance[i].state);
	}
	free(sm->objectType[objectTypeID].instance);
}

signed char smObjectNew(stateManager *sm, const size_t objectTypeID, size_t *objectID){
	/* Search for an inactive scene and allocate a state for it. */
	size_t i;
	for(i = 0; i < sm->objectType[objectTypeID].capacity; ++i){
		if(!sm->objectType[objectTypeID].instance[i].active){
			break;
		}
	}
	if(i == sm->objectType[objectTypeID].capacity){
		/* The scene array is full, double its size and get a new scene. */
		sm->objectType[objectTypeID].capacity *= 2;
		stateObject *tempBuffer = realloc(sm->objectType[objectTypeID].instance, sm->objectType[objectTypeID].capacity * sizeof(stateObject *));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return 0;
		}
		sm->objectType[objectTypeID].instance = tempBuffer;
	}
	sm->objectType[objectTypeID].instance[i].state[0] = malloc(sm->objectType[objectTypeID].size);
	if(sm->objectType[objectTypeID].instance[i].state[0] == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	(*sm->objectType[objectTypeID].stateNew)(sm->objectType[objectTypeID].instance[i].state[0]);
	sm->objectType[objectTypeID].instance[i].active = 1;
	*objectID = i;
	return 1;
}
void smObjectDelete(stateManager *sm, const size_t objectTypeID, const size_t objectID){
	(*sm->objectType[objectTypeID].stateDelete)(sm->objectType[objectTypeID].instance[objectID].state[0]);
	free(sm->objectType[objectTypeID].instance[objectID].state[0]);
	sm->objectType[objectTypeID].instance[objectID].state[0] = NULL;
}

void smInit(stateManager *sm, const size_t stateNum){
	sm->stateNum = stateNum;
	sm->currentStateID = 0;
	sm->objectTypeNum = 0;
	sm->objectType = NULL;
}
signed char smPrepareNextState(stateManager *sm){
	size_t i;
	++sm->currentStateID;
	for(i = 0; i < sm->objectTypeNum; ++i){
		if(!smObjectTypeUpdate(sm, i)){
			return 0;
		}
	}
	return 1;
}
signed char smGenerateDeltaState(const stateManager *sm, const size_t stateID){
	return 1;
}
void smDelete(stateManager *sm){
	size_t i;
	for(i = 0; i < sm->objectTypeNum; ++i){
		smObjectTypeDelete(sm, i);
	}
}
