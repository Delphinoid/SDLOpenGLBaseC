#ifndef SCENE_H
#define SCENE_H

#include "renderable.h"

typedef struct {
	size_t renderableNum;
	size_t renderableCapacity;
	size_t *renderableIDs;
} scene;

void scnInit(scene *scn);
unsigned char scnLoad(scene *scn);
unsigned char scnStateCopy(const scene *o, scene *c);
void scnDelete(scene *scn);

#endif
