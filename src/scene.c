#include "scene.h"
#include "stateManagerHelpers.h"
#include <string.h>

#define SCENE_START_CAPACITY 1

signed char scnInit(void *scn){
	((scene *)scn)->objectNum = 0;
	((scene *)scn)->objectCapacity = 0;
	((scene *)scn)->objectIDs = NULL;
	physSolverInit(&((scene *)scn)->solver);
	return 1;
}

signed char scnStateCopy(void *o, void *c){
	if(((scene *)c)->objectCapacity != ((scene *)o)->objectCapacity){
		/*
		** We need to allocate more or less memory so that
		** the memory allocated for both scenes match.
		*/
		objectIndex_t *tempBuffer = malloc(((scene *)o)->objectCapacity*sizeof(objectIndex_t));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		free(((scene *)c)->objectIDs);
		((scene *)c)->objectIDs = tempBuffer;
		((scene *)c)->objectCapacity = ((scene *)o)->objectCapacity;
	}
	memcpy(((scene *)c)->objectIDs, ((scene *)o)->objectIDs, ((scene *)o)->objectNum*sizeof(objectIndex_t));
	((scene *)c)->objectNum = ((scene *)o)->objectNum;
	((scene *)c)->objectCapacity = ((scene *)o)->objectCapacity;
	// No point copying the physics solver.
	return 1;
}

void scnResetInterpolation(void *scn){
	//
}

signed char scnLoad(scene *scn){
	return 1;
}

signed char scnObjectAdd(scene *scn, const objectIndex_t objectID){
	if(scn->objectNum >= scn->objectCapacity){
		/* Allocate room for more object IDs. */
		/**if(scn->objectCapacity == 0){
			scn->objectCapacity = SCENE_START_CAPACITY;
		}**/
		objectIndex_t tempCapacity;
		objectIndex_t *tempBuffer;
		if(scn->objectCapacity == 0){
			tempCapacity = 1;
			tempBuffer = malloc(tempCapacity*sizeof(objectIndex_t));
		}else{
			tempCapacity = scn->objectCapacity*2;
			tempBuffer = realloc(scn->objectIDs, tempCapacity*sizeof(objectIndex_t));
		}
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		scn->objectCapacity = tempCapacity;
		scn->objectIDs = tempBuffer;
	}
	scn->objectIDs[scn->objectNum] = objectID;
	++scn->objectNum;
	return 1;
}

void scnObjectRemove(scene *scn, stateManager *sm, const objectIndex_t pos){
	if(pos < scn->objectNum){
		smObjectDelete(sm, SM_TYPE_OBJECT, scn->objectIDs[pos]);
		--scn->objectNum;
		if(pos != scn->objectNum){
			memmove(&scn->objectIDs[pos], &scn->objectIDs[pos+1], scn->objectNum+1-pos);
		}
	}
}

void scnUpdate(scene *scn, stateManager *sm, const float elapsedTime, const float dt){

	size_t i;

	physSolverReset(&scn->solver);

	for(i = 0; i < scn->objectNum; ++i){

		// Update each object in the scene.
		objInstance *obji = objGetState(sm, scn->objectIDs[i], 0);
		if(obji != NULL){

			objiUpdate(obji, &scn->solver, elapsedTime, dt);

		}else{
			// If the object's state is NULL,
			// remove it from the scene.
			scnObjectRemove(scn, sm, i);
		}

	}

	physSolverUpdate(&scn->solver);

}

void scnDelete(void *scn){
	if(((scene *)scn)->objectIDs != NULL){
		free(((scene *)scn)->objectIDs);
	}
}
