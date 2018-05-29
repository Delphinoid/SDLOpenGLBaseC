#ifndef SCENE_H
#define SCENE_H

#include <stdlib.h>

/** Add physics island arrays to scenes.           **/
/** Links both ways for object removal / deletion. **/

typedef struct {

	size_t objectNum;
	size_t objectCapacity;
	size_t *objectIDs;

} scene;

signed char scnInit(void *scn);
signed char scnStateCopy(void *o, void *c);
void scnResetInterpolation(void *scn);
signed char scnLoad(scene *scn);
signed char scnRenderableAdd(scene *scn, const size_t objectID);
void scnRenderableRemove(scene *scn, size_t pos);
void scnDelete(void *scn);

#endif
