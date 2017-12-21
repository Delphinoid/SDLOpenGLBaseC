#include "stateManager.h"

unsigned char smInit(stateManager *sm, const size_t renderableCapacity, const size_t sceneCapacity, const size_t cameraCapacity, const size_t stateNum){

	sm->stateNum = stateNum;
	sm->currentStateID = 0;


	sm->renderableCapacity = renderableCapacity;
	sm->renderables = malloc(sm->renderableCapacity * sizeof(stateRenderable));
	if(sm->renderables == NULL){
		/** Memory allocation failure. **/
		return 0;
	}

	sm->sceneCapacity = sceneCapacity;
	sm->scenes = malloc(sm->sceneCapacity * sizeof(stateScene));
	if(sm->scenes == NULL){
		/** Memory allocation failure. **/
		free(sm->renderables);
		return 0;
	}

	sm->cameraCapacity = cameraCapacity;
	sm->cameras = malloc(sm->cameraCapacity * sizeof(stateCamera));
	if(sm->cameras == NULL){
		/** Memory allocation failure. **/
		free(sm->scenes);
		free(sm->renderables);
		return 0;
	}


	size_t i, j;

	/* Allocate memory for each renderable's states and initialize their pointers to NULL. */
	for(i = 0; i < sm->renderableCapacity; ++i){
		sm->renderables[i].active = 0;
		sm->renderables[i].state = malloc(sm->stateNum * sizeof(renderable *));
		if(sm->renderables[i].state == NULL){
			break;
		}
		for(j = 0; j < sm->stateNum; ++j){
			sm->renderables[i].state[j] = NULL;
		}
	}
	/* Handle a malloc failure. */
	if(i < sm->renderableCapacity){
		/** Memory allocation failure. **/
		while(i > 0){
			--i;
			free(sm->renderables[i].state);
		}
		free(sm->cameras);
		free(sm->scenes);
		free(sm->renderables);
		return 0;
	}

	/* Allocate memory for each scene's states and initialize their pointers to NULL. */
	for(i = 0; i < sm->sceneCapacity; ++i){
		sm->scenes[i].active = 0;
		sm->scenes[i].state = malloc(sm->stateNum * sizeof(scene *));
		if(sm->scenes[i].state == NULL){
			break;
		}
		for(j = 0; j < sm->stateNum; ++j){
			sm->scenes[i].state[j] = NULL;
		}
	}
	/* Handle a malloc failure. */
	if(i < sm->sceneCapacity){
		/** Memory allocation failure. **/
		while(i > 0){
			--i;
			free(sm->scenes[i].state);
		}
		free(sm->cameras);
		free(sm->scenes);
		free(sm->renderables);
		return 0;
	}

	/* Allocate memory for each camera's states and initialize their pointers to NULL. */
	for(i = 0; i < sm->cameraCapacity; ++i){
		sm->cameras[i].active = 0;
		sm->cameras[i].state = malloc(sm->stateNum * sizeof(camera *));
		if(sm->cameras[i].state == NULL){
			break;
		}
		for(j = 0; j < sm->stateNum; ++j){
			sm->cameras[i].state[j] = NULL;
		}
	}
	/* Handle a malloc failure. */
	if(i < sm->cameraCapacity){
		/** Memory allocation failure. **/
		while(i > 0){
			--i;
			free(sm->cameras[i].state);
		}
		free(sm->cameras);
		free(sm->scenes);
		free(sm->renderables);
		return 0;
	}


	return 1;

}

static inline unsigned char smRenderablesUpdate(stateManager *sm){
	/*
	** For each object, shift its state pointers over. Move its last state
	** object pointer to the front and copy its latest state object into it.
	*/
	void *lastState;
	size_t i, j, validStates;
	for(i = 0; i < sm->renderableCapacity; ++i){
		if(sm->renderables[i].active){

			if(sm->stateNum > 0){

				/* Shift each state for the current renderable over. */
				validStates = 0;
				j = sm->stateNum-1;
				lastState = sm->renderables[i].state[j];
				while(j > 0){
					if(sm->renderables[i].state[j-1] != NULL){
						++validStates;
					}
					sm->renderables[i].state[j] = sm->renderables[i].state[j-1];
					--j;
				}

				if(sm->renderables[i].state[0] != NULL){

					/*
					** Move lastState's pointer into state 0 and copy the last state into it.
					*/
					if(lastState == NULL){
						/* If lastState is NULL, allocate memory for it. */
						sm->renderables[i].state[0] = malloc(sizeof(renderable));
						if(sm->renderables[i].state[0] == NULL){
							/** Memory allocation failure. **/
							return 0;
						}
					}else{
						/** Delete this line: **/
						rndrDelete(lastState);
						sm->renderables[i].state[0] = lastState;
					}

					if(!rndrStateCopy(sm->renderables[i].state[1], sm->renderables[i].state[0])){
						/** Memory allocation failure. **/
						return 0;
					}

					// Done once again below.
					rndrResetInterpolation(sm->renderables[i].state[0]);

				}else if(lastState != NULL){

					/*
					** The renderable is being deleted, just free lastState.
					*/
					rndrDelete(lastState);
					free(lastState);

				}
				if(validStates == 0){

					/*
					** lastState is the only potentially valid state.
					** Free it if possible and set this renderable to inactive.
					*/
					sm->renderables[i].active = 0;

				}

			}else{
				// Done once again above.
				rndrResetInterpolation(sm->renderables[i].state[0]);
			}

		}
	}
	return 1;
}
unsigned char smRenderableNew(stateManager *sm, size_t *renderableID){
	/* Search for an inactive renderable and allocate a state for it. */
	size_t i;
	for(i = 0; i < sm->renderableCapacity; ++i){
		if(!sm->renderables[i].active){
			sm->renderables[i].state[0] = malloc(sizeof(renderable));
			if(sm->renderables[i].state[0] == NULL){
				/** Memory allocation failure. **/
				return 0;
			}
			sm->renderables[i].active = 1;
			*renderableID = i;
			return 1;
		}
	}
	/* The renderable array is full, double its size and get a new renderable. */
	sm->renderableCapacity *= 2;
	stateRenderable *tempBuffer = realloc(sm->renderables, sm->renderableCapacity * sizeof(stateRenderable *));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	sm->renderables = tempBuffer;
	sm->renderables[i].state[0] = malloc(sizeof(renderable));
	if(sm->renderables[i].state[0] == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	sm->renderables[i].active = 1;
	*renderableID = i;
	return 1;
}
void smRenderableDelete(stateManager *sm, const size_t renderableID){
	rndrDelete(sm->renderables[renderableID].state[0]);
	free(sm->renderables[renderableID].state[0]);
	sm->renderables[renderableID].state[0] = NULL;
}

static inline unsigned char smScenesUpdate(stateManager *sm){
	/*
	** For each object, shift its state pointers over. Move its last state
	** object pointer to the front and copy its latest state object into it.
	*/
	void *lastState;
	size_t i, j, validStates;
	for(i = 0; i < sm->sceneCapacity; ++i){
		if(sm->scenes[i].active){
			if(sm->stateNum > 0){

				/* Shift each state for the current scene over. */
				validStates = 0;
				j = sm->stateNum-1;
				lastState = sm->scenes[i].state[j];
				while(j > 0){
					if(sm->scenes[i].state[j-1] != NULL){
						++validStates;
					}
					sm->scenes[i].state[j] = sm->scenes[i].state[j-1];
					--j;
				}

				if(sm->scenes[i].state[0] != NULL){

					/*
					** Move lastState's pointer into state 0 and copy the last state into it.
					*/
					if(lastState == NULL){
						/* If lastState is NULL, allocate memory for it. */
						sm->scenes[i].state[0] = malloc(sizeof(scene));
						if(sm->scenes[i].state[0] == NULL){
							/** Memory allocation failure. **/
							return 0;
						}
					}else{
						/** Delete this line: **/
						scnDelete(lastState);
						sm->scenes[i].state[0] = lastState;
					}

					if(!scnStateCopy(sm->scenes[i].state[1], sm->scenes[i].state[0])){
						/** Memory allocation failure. **/
						return 0;
					}

				}else if(lastState != NULL){

					/*
					** The scene is being deleted, just free lastState.
					*/
					scnDelete(lastState);
					free(lastState);

				}

				if(validStates == 0){

					/*
					** lastState is the only potentially valid state.
					** Free it if possible and set this scene to inactive.
					*/
					sm->scenes[i].active = 0;

				}

			}
		}
	}
	return 1;
}
unsigned char smSceneNew(stateManager *sm, size_t *sceneID){
	/* Search for an inactive scene and allocate a state for it. */
	size_t i;
	for(i = 0; i < sm->sceneCapacity; ++i){
		if(!sm->scenes[i].active){
			sm->scenes[i].state[0] = malloc(sizeof(scene));
			if(sm->scenes[i].state[0] == NULL){
				/** Memory allocation failure. **/
				return 0;
			}
			sm->scenes[i].active = 1;
			*sceneID = i;
			return 1;
		}
	}
	/* The scene array is full, double its size and get a new scene. */
	sm->sceneCapacity *= 2;
	stateScene *tempBuffer = realloc(sm->scenes, sm->sceneCapacity * sizeof(stateScene *));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	sm->scenes = tempBuffer;
	sm->scenes[i].state[0] = malloc(sizeof(scene));
	if(sm->scenes[i].state[0] == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	sm->scenes[i].active = 1;
	*sceneID = i;
	return 1;
}
void smSceneDelete(stateManager *sm, const size_t sceneID){
	scnDelete(sm->scenes[sceneID].state[0]);
	free(sm->scenes[sceneID].state[0]);
	sm->scenes[sceneID].state[0] = NULL;
}

static inline unsigned char smCamerasUpdate(stateManager *sm){
	/*
	** For each object, shift its state pointers over. Move its last state
	** object pointer to the front and copy its latest state object into it.
	*/
	void *lastState;
	size_t i, j, validStates;
	for(i = 0; i < sm->cameraCapacity; ++i){
		if(sm->cameras[i].active){
			if(sm->stateNum > 0){

				/* Shift each state for the current camera over. */
				validStates = 0;
				j = sm->stateNum-1;
				lastState = sm->cameras[i].state[j];
				while(j > 0){
					if(sm->cameras[i].state[j-1] != NULL){
						++validStates;
					}
					sm->cameras[i].state[j] = sm->cameras[i].state[j-1];
					--j;
				}

				if(sm->cameras[i].state[0] != NULL){

					/*
					** Move lastState's pointer into state 0 and copy the last state into it.
					*/
					if(lastState == NULL){
						/* If lastState is NULL, allocate memory for it. */
						sm->cameras[i].state[0] = malloc(sizeof(camera));
						if(sm->cameras[i].state[0] == NULL){
							/** Memory allocation failure. **/
							return 0;
						}
					}else{
						/** Delete this line: **/
						//camDelete(lastState);
						sm->cameras[i].state[0] = lastState;
					}

					camStateCopy(sm->cameras[i].state[1], sm->cameras[i].state[0]);
					/**if(!camStateCopy(sm->cameras[i].state[j], sm->cameras[i].state[0])){
						** Memory allocation failure. **
						return 0;
					}**/

					// Done once again below.
					camResetInterpolation(sm->cameras[i].state[0]);

				}else if(lastState != NULL){

					/*
					** The camera is being deleted, just free lastState.
					*/
					/**camDelete(lastState);**/
					free(lastState);

				}
				if(validStates == 0){

					/*
					** lastState is the only potentially valid state.
					** Free it if possible and set this camera to inactive.
					*/
					sm->renderables[i].active = 0;

				}

			}else{
				// Done once again above.
				camResetInterpolation(sm->cameras[i].state[0]);
			}

		}
	}
	return 1;
}
unsigned char smCameraNew(stateManager *sm, size_t *cameraID){
	/* Search for an inactive camera and allocate a state for it. */
	size_t i;
	for(i = 0; i < sm->cameraCapacity; ++i){
		if(!sm->cameras[i].active){
			sm->cameras[i].state[0] = malloc(sizeof(camera));
			if(sm->cameras[i].state[0] == NULL){
				/** Memory allocation failure. **/
				return 0;
			}
			sm->cameras[i].active = 1;
			*cameraID = i;
			return 1;
		}
	}
	/* The camera array is full, double its size and get a new camera. */
	sm->cameraCapacity *= 2;
	stateCamera *tempBuffer = realloc(sm->cameras, sm->cameraCapacity * sizeof(stateCamera *));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	sm->cameras = tempBuffer;
	sm->cameras[i].state[0] = malloc(sizeof(camera));
	if(sm->cameras[i].state[0] == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	sm->cameras[i].active = 1;
	*cameraID = i;
	return 1;
}
void smCameraDelete(stateManager *sm, const size_t cameraID){
	/**camDelete(sm->cameras[cameraID].state[0]);**/
	free(sm->cameras[cameraID].state[0]);
	sm->cameras[cameraID].state[0] = NULL;
}

unsigned char smPrepareNextState(stateManager *sm){
	++sm->currentStateID;
	return smRenderablesUpdate(sm) && smScenesUpdate(sm) && smCamerasUpdate(sm);
}

unsigned char smGenerateDeltaState(const stateManager *sm, const size_t stateID){
	return 1;
}

void smDelete(stateManager *sm){

	size_t i, j;

	for(i = 0; i < sm->renderableCapacity; ++i){
		for(j = 0; j < sm->stateNum; ++j){
			if(sm->renderables[i].state[j] != NULL){
				rndrDelete(sm->renderables[i].state[j]);
				free(sm->renderables[i].state[j]);
			}
		}
		free(sm->renderables[i].state);
	}
	free(sm->renderables);

	for(i = 0; i < sm->sceneCapacity; ++i){
		for(j = 0; j < sm->stateNum; ++j){
			if(sm->scenes[i].state[j] != NULL){
				scnDelete(sm->scenes[i].state[j]);
				free(sm->scenes[i].state[j]);
			}
		}
		free(sm->scenes[i].state);
	}
	free(sm->renderables);

	for(i = 0; i < sm->cameraCapacity; ++i){
		for(j = 0; j < sm->stateNum; ++j){
			if(sm->cameras[i].state[j] != NULL){
				/**camDelete(sm->cameras[i].state[j]);**/
				free(sm->cameras[i].state[j]);
			}
		}
		free(sm->cameras[i].state);
	}
	free(sm->cameras);

}

/*static void stateInit(state *s){
	s->renderableNum = 0;
	s->renderableCapacity = 0;
	s->renderables = NULL;
	s->sceneNum = 0;
	s->sceneCapacity = 0;
	s->scenes = NULL;
	s->cameraNum = 0;
	s->cameraCapacity = 0;
	s->cameras = NULL;
}

static unsigned char stateNew(state *s){

	s->renderables = malloc(STATE_INITIAL_RENDERABLE_CAPACITY * sizeof(renderable));
	if(s->renderables == NULL){
		** Memory allocation failure. **
		return 0;
	}

	s->scenes = malloc(STATE_INITIAL_SCENE_CAPACITY * sizeof(scene));
	if(s->scenes == NULL){
		** Memory allocation failure. **
		free(s->renderables);
		return 0;
	}

	s->cameras = malloc(STATE_INITIAL_CAMERA_CAPACITY * sizeof(camera));
	if(s->cameras == NULL){
		** Memory allocation failure. **
		free(s->scenes);
		free(s->renderables);
		return 0;
	}

	s->renderableNum = 0;
	s->renderableCapacity = STATE_INITIAL_RENDERABLE_CAPACITY;
	s->sceneNum = 0;
	s->sceneCapacity = STATE_INITIAL_SCENE_CAPACITY;
	s->cameraNum = 0;
	s->cameraCapacity = STATE_INITIAL_CAMERA_CAPACITY;

	return 1;

}

static inline unsigned char stateCopy(state *o, state *c){

	c->renderables = o->renderables;
	o->renderables = malloc(o->renderableNum * sizeof(renderable));
	if(o->renderables == NULL){
		** Memory allocation failure. **
		return 0;
	}
	c->scenes = o->scenes;
	o->scenes = malloc(o->sceneNum * sizeof(scene));
	if(o->scenes == NULL){
		** Memory allocation failure. **
		free(o->renderables);
		return 0;
	}
	c->cameras = o->cameras;
	o->cameras = malloc(o->cameraNum * sizeof(camera));
	if(o->cameras == NULL){
		** Memory allocation failure. **
		free(o->scenes);
		free(o->renderables);
		return 0;
	}

	size_t i;
	* Copy each renderable over. *
	for(i = 0; i < o->renderableNum; ++i){
		if(rndrStateCopy(&c->renderables[i], &o->renderables[i])){
			rndrResetInterpolation(&c->renderables[i]);
		}else{
			break;
		}
	}

	// Check if every renderable was copied properly. If not, free and return.
	if(i < o->renderableNum){
		rndrDelete(&o->renderables[i]);
		while(i > 0){
			--i;
			rndrDelete(&o->renderables[i]);
		}
		** Memory allocation failure. **
		free(o->cameras);
		free(o->scenes);
		free(o->renderables);
		return 0;
	}

	* Copy each scene over. *
	for(i = 0; i < o->sceneNum; ++i){
		if(!scnStateCopy(&c->scenes[i], &o->scenes[i])){
			break;
		}
	}
	// Check if every scene was copied properly. If not, free and return.
	if(i < o->sceneNum){
		scnDelete(&o->scenes[i]);
		while(i > 0){
			--i;
			scnDelete(&o->scenes[i]);
		}
		** Memory allocation failure. **
		free(o->cameras);
		free(o->scenes);
		free(o->renderables);
		return 0;
	}

	* Copy each camera over. *
	for(i = 0; i < o->cameraNum; ++i){
		camStateCopy(&c->cameras[i], &o->cameras[i]);
		** This is REALLY bad! Same in sklaiStateCopy! **
		o->cameras[i].targetScene = c->cameras[i].targetScene - c->scenes + o->scenes;
		camResetInterpolation(&c->cameras[i]);
	}

	c->renderableNum = o->renderableNum;
	c->renderableCapacity = o->renderableCapacity;
	o->renderableCapacity = o->renderableNum;
	c->sceneNum = o->sceneNum;
	c->sceneCapacity = o->sceneCapacity;
	o->sceneCapacity = o->sceneNum;
	c->cameraNum = o->cameraNum;
	c->cameraCapacity = o->cameraCapacity;
	o->cameraCapacity = o->cameraNum;

	return 1;

}

static inline void stateDelete(state *s){
	size_t i;
	if(s->renderables != NULL){
		for(i = 0; i < s->renderableNum; ++i){
			rndrDelete(&s->renderables[i]);
		}
		free(s->renderables);
	}
	if(s->scenes != NULL){
		for(i = 0; i < s->sceneNum; ++i){
			scnDelete(&s->scenes[i]);
		}
		free(s->scenes);
	}
	if(s->cameras != NULL){
		for(i = 0; i < s->cameraNum; ++i){
			camDelete(&s->cameras[i]);
		}
		free(s->cameras);
	}
}

unsigned char smInit(stateManager *sm, const size_t stateNum){
	sm->stateNum = stateNum;
	sm->currentStateID = 0;
	sm->stateArray = malloc(stateNum*sizeof(state));
	if(sm->stateArray == NULL){
		** Memory allocation failure. **
		return 0;
	}
	if(!stateNew(&sm->stateArray[0])){
		** Memory allocation failure. **
		free(sm->stateArray);
		return 0;
	}else{
		size_t i;
		for(i = 1; i < stateNum; ++i){
			stateInit(&sm->stateArray[i]);
		}
	}
	return 1;
}

unsigned char smResize(stateManager *sm, const size_t stateNum){

	size_t i;

	if(stateNum < sm->stateNum){
		i = sm->stateNum;
		while(i > stateNum){
			--i;
			stateDelete(&sm->stateArray[i]);
		}
	}

	state *tempBuffer = realloc(sm->stateArray, stateNum*sizeof(state));
	if(tempBuffer == NULL){
		** Memory allocation failure. **
		return 0;
	}
	sm->stateArray = tempBuffer;

	for(i = sm->stateNum; i < stateNum; ++i){
		stateInit(&sm->stateArray[i]);
	}
	sm->stateNum = stateNum;

	return 1;

}

void smGetState(const stateManager *sm, const size_t stateID, state **s){
	if(stateID < sm->currentStateID && stateID > sm->currentStateID-sm->stateNum){
		*s = &sm->stateArray[sm->currentStateID-stateID];
		if((*s)->renderables == NULL || (*s)->scenes == NULL || (*s)->cameras == NULL){
			*s = NULL;
		}
	}else{
		*s = NULL;
	}
}

unsigned char smPrepareNextState(stateManager *sm){
	*
	** Shift each state over and prepare a new state.
	** Returns 0 on a memory allocation failure.
	*
	size_t i;
	++sm->currentStateID;
	if(sm->stateNum > 1){
		i = sm->stateNum-1;
		stateDelete(&sm->stateArray[i]);
		while(i > 0){
			sm->stateArray[i] = sm->stateArray[i-1];
			--i;
		}
		return stateCopy(&sm->stateArray[1], &sm->stateArray[0]);
	}else{
		for(i = 0; i < sm->stateArray->renderableNum; ++i){
			rndrResetInterpolation(&sm->stateArray->renderables[i]);
		}
		for(i = 0; i < sm->stateArray->cameraNum; ++i){
			camResetInterpolation(&sm->stateArray->cameras[i]);
		}
	}
	return 1;
}

unsigned char smGenerateDeltaState(const stateManager *sm, const size_t stateID, state *deltaState){
	return 1;
}

void smDelete(stateManager *sm){
	size_t i = sm->stateNum;
	while(i > 0){
		--i;
		stateDelete(&sm->stateArray[i]);
	}
	free(sm->stateArray);
}*/
