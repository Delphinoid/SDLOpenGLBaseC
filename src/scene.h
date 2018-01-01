#ifndef SCENE_H
#define SCENE_H

#include "renderable.h"

typedef struct {
	size_t renderableNum;
	size_t renderableCapacity;
	size_t *renderableIDs;
} scene;

unsigned char scnInit(void *scn);
unsigned char scnStateCopy(const void *o, void *c);
void scnResetInterpolation(void *scn);
unsigned char scnLoad(scene *scn);
unsigned char scnRenderableAdd(scene *scn, const size_t renderableID);
void scnRenderableRemove(scene *scn, size_t pos);
void scnDelete(void *scn);

#endif
