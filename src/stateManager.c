#include "stateManager.h"
#include <string.h>

signed char stateObjectTypeInit(stateObjectType *objType,
                                signed char (*stateInit)(void*), signed char (*stateCopy)(void*, void*),
                                void (*stateResetInterpolation)(void*), void (*stateDelete)(void*),
                                const size_t size, const size_t capacity, const size_t stateNum){

	size_t i, j;

	/* Allocate memory for the new type's object array. */
	objType->instance = malloc(capacity * sizeof(stateObject));
	if(objType->instance == NULL){
		/** Memory allocation failure. **/
		return -1;
	}

	/* Allocate memory for each object's states and initialize their pointers to NULL. */
	for(i = 0; i < capacity; ++i){
		objType->instance[i].active = 0;
		objType->instance[i].state = malloc(stateNum * sizeof(void *));
		if(objType->instance[i].state == NULL){
			/** Memory allocation failure. **/
			break;
		}
		for(j = 0; j < stateNum; ++j){
			objType->instance[i].state[j] = NULL;
		}
	}
	/* Handle a malloc failure. */
	if(i < capacity){
		/** Memory allocation failure. **/
		while(i > 0){
			--i;
			free(objType->instance[i].state);
		}
		free(objType->instance);
		return -1;
	}

	/* Set various variables. */
	objType->stateInit = stateInit;
	objType->stateCopy = stateCopy;
	objType->stateResetInterpolation = stateResetInterpolation;
	objType->stateDelete = stateDelete;
	objType->size = size;
	objType->capacity = capacity;
	objType->stateNum = stateNum;

	return 1;

}
signed char stateObjectTypeUpdate(stateObjectType *objType){
	/*
	** For each object, shift its state pointers over. Move its last state
	** object pointer to the front and copy the latest state object into it.
	*/
	void *lastState;
	size_t i, j, validStates;
	for(i = 0; i < objType->capacity; ++i){
		if(objType->instance[i].active){
			if(objType->stateNum > 1){

				/* Shift each state for the current object over. */
				validStates = 0;
				j = objType->stateNum-1;
				lastState = objType->instance[i].state[j];
				while(j > 0){
					if(objType->instance[i].state[j-1] != NULL){
						++validStates;
					}
					objType->instance[i].state[j] = objType->instance[i].state[j-1];
					--j;
				}

				if(objType->instance[i].state[0] != NULL){

					/*
					** Move lastState's pointer into state 0 and copy the last state into it.
					*/
					if(lastState == NULL){
						/* If lastState is NULL, allocate memory for it. */
						objType->instance[i].state[0] = malloc(objType->size);
						if(objType->instance[i].state[0] == NULL){
							/** Memory allocation failure. **/
							return -1;
						}
						/**/
						/* Prevents crashing in stateCopy() due to uninitialized variables. */
						if((*objType->stateInit)(objType->instance[i].state[0]) == -1){
							/** Memory allocation failure. **/
							return -1;
						}
						/**/
					}else{
						objType->instance[i].state[0] = lastState;
					}

					if((*objType->stateCopy)(objType->instance[i].state[1], objType->instance[i].state[0]) == -1){
						/** Memory allocation failure. **/
						return -1;
					}
					// Swap states 0 and 1 so that the pointer to 0 stays the same.
					lastState = objType->instance[i].state[0];
					objType->instance[i].state[0] = objType->instance[i].state[1];
					objType->instance[i].state[1] = lastState;

					// Done once again below.
					(*objType->stateResetInterpolation)(objType->instance[i].state[0]);

				}else if(lastState != NULL){

					/*
					** The object is being deleted, just free lastState.
					*/
					(*objType->stateDelete)(lastState);
					free(lastState);

				}

				if(validStates == 0){

					/*
					** lastState is the only potentially valid state.
					** Free it if possible and set this object to inactive.
					*/
					objType->instance[i].active = 0;

				}

			}else{
				// Done once again above.
				(*objType->stateResetInterpolation)(objType->instance[i].state[0]);
			}

		}
	}
	return 1;
}
void stateObjectTypeDelete(stateObjectType *objType){
	size_t i, j;
	for(i = 0; i < objType->capacity; ++i){
		for(j = 0; j < objType->stateNum; ++j){
			if(objType->instance[i].state[j] != NULL){
				objType->stateDelete(objType->instance[i].state[j]);
				free(objType->instance[i].state[j]);
			}
		}
		free(objType->instance[i].state);
	}
	free(objType->instance);
}

signed char smObjectTypeNew(stateManager *sm,
                            signed char (*stateInit)(void*), signed char (*stateCopy)(void*, void*),
                            void (*stateResetInterpolation)(void*), void (*stateDelete)(void*),
                            const size_t size, const size_t capacity, const size_t stateNum){

	/* Create a temporary objectType array that will later replace the current one. */
	stateObjectType *tempBuffer = malloc((sm->objectTypeNum+1) * sizeof(stateObjectType));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}

	/* Initialize the new object type. */
	if(stateObjectTypeInit(&tempBuffer[sm->objectTypeNum],
	                       stateInit, stateCopy,
	                       stateResetInterpolation, stateDelete,
	                       size, capacity, stateNum) == -1){
		free(tempBuffer);
		return -1;
	}

	/* Copy the old objectTypes over. */
	memcpy(tempBuffer, sm->objectType, sm->objectTypeNum * sizeof(stateObjectType));
	free(sm->objectType);
	sm->objectType = tempBuffer;

	/* Set various variables. */
	++sm->objectTypeNum;

	return 1;

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
		/*
		** The object array is full, double its size.
		** Increasing it by 1 may be better in the
		** long run, but would also probably lead
		** to more fragmentation, so who knows.
		*/
		if(sm->objectType[objectTypeID].capacity == 0){
			sm->objectType[objectTypeID].capacity = 1;
		}else{
			sm->objectType[objectTypeID].capacity *= 2;
		}
		stateObject *tempBuffer = realloc(sm->objectType[objectTypeID].instance, sm->objectType[objectTypeID].capacity * sizeof(stateObject *));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		sm->objectType[objectTypeID].instance = tempBuffer;
	}
	sm->objectType[objectTypeID].instance[i].state[0] = malloc(sm->objectType[objectTypeID].size);
	if(sm->objectType[objectTypeID].instance[i].state[0] == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	if((*sm->objectType[objectTypeID].stateInit)(sm->objectType[objectTypeID].instance[i].state[0]) == -1){
		/** Memory allocation failure. **/
		free(sm->objectType[objectTypeID].instance[i].state[0]);
		sm->objectType[objectTypeID].instance[i].state[0] = NULL;
		return -1;
	}
	sm->objectType[objectTypeID].instance[i].active = 1;
	*objectID = i;
	return 1;
}
void smObjectDelete(stateManager *sm, const size_t objectTypeID, const size_t objectID){
	if(sm->objectType[objectTypeID].instance[objectID].state[0] != NULL){
		(*sm->objectType[objectTypeID].stateDelete)(sm->objectType[objectTypeID].instance[objectID].state[0]);
		free(sm->objectType[objectTypeID].instance[objectID].state[0]);
		sm->objectType[objectTypeID].instance[objectID].state[0] = NULL;
	}
}

void smInit(stateManager *sm){
	sm->currentStateID = 0;
	sm->objectTypeNum = 0;
	sm->objectType = NULL;
}
signed char smPrepareNextState(stateManager *sm){
	size_t i;
	++sm->currentStateID;
	for(i = 0; i < sm->objectTypeNum; ++i){
		if(stateObjectTypeUpdate(&sm->objectType[i]) == -1){
			return -1;
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
		stateObjectTypeDelete(&sm->objectType[i]);
	}
}
