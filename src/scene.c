#include "scene.h"

void scnInit(scene *scn){
	scn->renderableNum = 0;
	scn->renderableIDs = NULL;
}

unsigned char scnLoad(scene *scn){
	return 1;
}

unsigned char scnStateCopy(const scene *o, scene *c){
	size_t i;
	c->renderableIDs = malloc(o->renderableNum * sizeof(size_t));
	if(c->renderableIDs == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	for(i = 0; i < o->renderableNum; ++i){
		c->renderableIDs[i] = o->renderableIDs[i];
	}
	c->renderableNum = o->renderableNum;
	return 1;
}

void scnDelete(scene *scn){
	if(scn->renderableIDs == NULL){
		free(scn->renderableIDs);
	}
}
