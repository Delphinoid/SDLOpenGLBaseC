#include "scene.h"

void scnInit(scene *scn){
	scn->renderableNum = 0;
	scn->renderables = NULL;
}

void scnDelete(scene *scn){
	if(scn->renderables == NULL){
		free(scn->renderables);
	}
}
