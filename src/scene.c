#include "scene.h"

#define SCENE_START_CAPACITY 1

void scnInit(scene *scn){
	scn->renderableNum = 0;
	scn->renderableCapacity = 0;
	scn->renderableIDs = NULL;
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

unsigned char scnStateCopy(const scene *o, scene *c){
	if(c->renderableCapacity != o->renderableCapacity){
		/*
		** We need to allocate more or less memory so that
		** the memory allocated for both scenes match.
		*/
		size_t *tempBuffer = malloc(o->renderableCapacity*sizeof(size_t));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return 0;
		}
		free(c->renderableIDs);
		c->renderableIDs = tempBuffer;
		c->renderableCapacity = o->renderableCapacity;
	}
	memcpy(c->renderableIDs, o->renderableIDs, o->renderableNum*sizeof(size_t));
	c->renderableNum = o->renderableNum;
	c->renderableCapacity = o->renderableCapacity;
	return 1;
}

void scnDelete(scene *scn){
	if(scn->renderableIDs != NULL){
		free(scn->renderableIDs);
	}
}
