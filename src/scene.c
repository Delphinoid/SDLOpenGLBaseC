#include "scene.h"
#include <string.h>

#define SCENE_START_CAPACITY 1

signed char scnInit(void *scn){
	((scene *)scn)->objectNum = 0;
	((scene *)scn)->objectCapacity = 0;
	((scene *)scn)->objectIDs = NULL;
	return 1;
}

signed char scnStateCopy(void *o, void *c){
	if(((scene *)c)->objectCapacity != ((scene *)o)->objectCapacity){
		/*
		** We need to allocate more or less memory so that
		** the memory allocated for both scenes match.
		*/
		size_t *tempBuffer = malloc(((scene *)o)->objectCapacity*sizeof(size_t));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		free(((scene *)c)->objectIDs);
		((scene *)c)->objectIDs = tempBuffer;
		((scene *)c)->objectCapacity = ((scene *)o)->objectCapacity;
	}
	memcpy(((scene *)c)->objectIDs, ((scene *)o)->objectIDs, ((scene *)o)->objectNum*sizeof(size_t));
	((scene *)c)->objectNum = ((scene *)o)->objectNum;
	((scene *)c)->objectCapacity = ((scene *)o)->objectCapacity;
	return 1;
}

void scnResetInterpolation(void *scn){
	//
}

signed char scnLoad(scene *scn){
	return 1;
}

signed char scnRenderableAdd(scene *scn, const size_t objectID){
	if(scn->objectNum >= scn->objectCapacity){
		/* Allocate room for more object IDs. */
		/**if(scn->objectCapacity == 0){
			scn->objectCapacity = SCENE_START_CAPACITY;
		}**/
		size_t tempCapacity;
		size_t *tempBuffer;
		if(scn->objectCapacity == 0){
			tempCapacity = 1;
			tempBuffer = malloc(tempCapacity*sizeof(size_t));
		}else{
			tempCapacity = scn->objectCapacity*2;
			tempBuffer = realloc(scn->objectIDs, tempCapacity*sizeof(size_t));
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

void scnRenderableRemove(scene *scn, size_t pos){
	if(pos < scn->objectNum){
		--scn->objectNum;
		if(pos != scn->objectNum){
			memmove(&scn->objectIDs[pos], &scn->objectIDs[pos+1], scn->objectNum+1-pos);
		}
	}
}

void scnDelete(void *scn){
	if(((scene *)scn)->objectIDs != NULL){
		free(((scene *)scn)->objectIDs);
	}
}
