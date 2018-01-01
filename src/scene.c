#include "scene.h"

#define SCENE_START_CAPACITY 1

unsigned char scnInit(void *scn){
	((scene *)scn)->renderableNum = 0;
	((scene *)scn)->renderableCapacity = 0;
	((scene *)scn)->renderableIDs = NULL;
	return 1;
}

unsigned char scnStateCopy(const void *o, void *c){
	if(((scene *)c)->renderableCapacity != ((scene *)o)->renderableCapacity){
		/*
		** We need to allocate more or less memory so that
		** the memory allocated for both scenes match.
		*/
		size_t *tempBuffer = malloc(((scene *)o)->renderableCapacity*sizeof(size_t));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return 0;
		}
		free(((scene *)c)->renderableIDs);
		((scene *)c)->renderableIDs = tempBuffer;
		((scene *)c)->renderableCapacity = ((scene *)o)->renderableCapacity;
	}
	memcpy(((scene *)c)->renderableIDs, ((scene *)o)->renderableIDs, ((scene *)o)->renderableNum*sizeof(size_t));
	((scene *)c)->renderableNum = ((scene *)o)->renderableNum;
	((scene *)c)->renderableCapacity = ((scene *)o)->renderableCapacity;
	return 1;
}

void scnResetInterpolation(void *scn){
	//
}

unsigned char scnLoad(scene *scn){
	return 1;
}

unsigned char scnRenderableAdd(scene *scn, const size_t renderableID){
	if(scn->renderableNum >= scn->renderableCapacity){
		/* Allocate room for more renderable IDs. */
		/**if(scn->renderableCapacity == 0){
			scn->renderableCapacity = SCENE_START_CAPACITY;
		}**/
		size_t tempCapacity = scn->renderableCapacity*2;
		size_t *tempBuffer = realloc(scn->renderableIDs, tempCapacity*sizeof(size_t));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return 0;
		}
		scn->renderableCapacity = tempCapacity;
		scn->renderableIDs = tempBuffer;
	}
	scn->renderableIDs[scn->renderableNum] = renderableID;
	++scn->renderableNum;
	return 1;
}

void scnRenderableRemove(scene *scn, size_t pos){
	if(pos < scn->renderableNum){
		--scn->renderableNum;
		if(pos != scn->renderableNum){
			memmove(&scn->renderableIDs[pos], &scn->renderableIDs[pos+1], scn->renderableNum+1-pos);
		}
	}
}

void scnDelete(void *scn){
	if(((scene *)scn)->renderableIDs != NULL){
		free(((scene *)scn)->renderableIDs);
	}
}
