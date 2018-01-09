#ifndef SCENE_H
#define SCENE_H

#include "renderable.h"

typedef struct {
	size_t renderableNum;
	size_t renderableCapacity;
	size_t *renderableIDs;
} scene;

signed char scnInit(void *scn);
signed char scnStateCopy(void *o, void *c);
void scnResetInterpolation(void *scn);
signed char scnLoad(scene *scn);
signed char scnRenderableAdd(scene *scn, const size_t renderableID);
void scnRenderableRemove(scene *scn, size_t pos);
void scnDelete(void *scn);

#endif
