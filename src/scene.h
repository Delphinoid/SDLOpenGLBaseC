#ifndef SCENE_H
#define SCENE_H

#include "renderable.h"

typedef struct {
	size_t renderableNum;
	renderable **renderables;
} scene;

void scnInit(scene *scn);
void scnDelete(scene *scn);

#endif
