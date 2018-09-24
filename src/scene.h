#ifndef SCENE_H
#define SCENE_H

#include "stateManager.h"
#include "physicsSolver.h"

/** Add physics island arrays to scenes.           **/
/** Links both ways for object removal / deletion. **/

typedef uint32_t objectIndex_t;

typedef struct {

	// Objects in the scene.
	objectIndex_t objectNum;
	objectIndex_t objectCapacity;
	objectIndex_t *objectIDs;

	// Solver for physically simulated objects.
	physicsSolver solver;

} scene;

return_t scnInit(void *scn);
return_t scnStateCopy(void *o, void *c);
void scnResetInterpolation(void *scn);
return_t scnLoad(scene *scn);

return_t scnObjectAdd(scene *scn, const objectIndex_t id);
void scnObjectRemove(scene *scn, stateManager *sm, const objectIndex_t id);

void scnUpdate(scene *scn, stateManager *sm, const float elapsedTime, const float dt);

void scnDelete(void *scn);

#endif
