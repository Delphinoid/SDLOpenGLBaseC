#ifndef SCENE_H
#define SCENE_H

#include "object.h"
#include "memoryList.h"

/** Add physics island arrays to scenes.           **/
/** Links both ways for object removal / deletion. **/

//typedef uint_least32_t objectIndex_t;

typedef struct {

	// Objects in the scene.
	memoryList objects;  // Contains objInstance pointers.
	size_t objectNum;    // The number of objects in each region.

	// Zone bounding box.
	// Used by cameras to check which
	// zone they should render.
	hbAABB bounds;

} zone;

/** Rename to zone. **/
typedef struct {

	// Objects in the scene.
	/** Replace __ObjectInstanceResourceArray? **/
	memoryList objects;  // Contains objInstance pointers.
	size_t objectNum;    // The number of objects in each region.

	/*objectIndex_t objectNum;
	objectIndex_t objectCapacity;
	objectIndex_t *objectIDs;*/

	/** Create scene portals? **/

	// Solver for physically simulated objects.
	physicsSolver solver;

} scene;

/*return_t scnInit(void *scn);
return_t scnStateCopy(void *o, void *c);
void scnResetInterpolation(void *scn);
return_t scnLoad(scene *scn);

return_t scnObjectAdd(scene *scn, const objectIndex_t id);
void scnObjectRemove(scene *scn, stateManager *sm, const objectIndex_t id);*/

return_t scnInit(scene *scn, const size_t objectNum);

objInstance **scnAllocate(scene *scn);
void scnFree(scene *scn, objInstance **obji);

void scnUpdate(scene *scn, const float elapsedTime, const float dt);

void scnReset(scene *scn);
void scnDelete(scene *scn);

//void scnDelete(void *scn);

#endif
