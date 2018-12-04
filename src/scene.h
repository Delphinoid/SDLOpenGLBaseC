#ifndef SCENE_H
#define SCENE_H

#include "object.h"
#include "memoryPool.h"

#ifndef SCENE_ZONE_DEFAULT_OBJECT_NUM
	#define SCENE_ZONE_DEFAULT_OBJECT_NUM 1024
#endif

#define SCENE_ZONE_SLEEP 0x01

/** Add physics island arrays to scenes.           **/
/** Links both ways for object removal / deletion. **/

//typedef uint_least32_t objectIndex_t;

typedef struct scnZone scnZone;
typedef struct {

	// Maximum render distance.
	float distance;

	// Portal bounding box.
	cAABB bounds;

	// Zone pointer.
	scnZone *zone;

} scnPortal;

typedef struct scnZone {

	// Objects in the zone.
	memoryPool objects;  // Contains objInstance pointers.
	size_t objectNum;    // The number of objects in each region.
	                     // Allows preallocations when rendering.

	// Zone portals.
	// Used to mark the areas between
	// this zone and other zones.
	scnPortal *portals;  // Contains scnPortals.
	size_t portalNum;

	// Zone collider.
	// Used to find which zone the
	// camera is in, if any.
	colliderArray bounds;

	// Flags for the zone.
	flags_t flags;

} scnZone;

typedef struct {

	/**
	*** Eventually, the object pool should
	*** replace __ObjectInstanceResourceArray.
	**/

	// Objects in the zone.
	memoryPool objects;  // Contains objInstance pointers.
	size_t objectNum;    // The number of objects in each region.
	                     // Allows preallocations when rendering.

	// Solver for physically simulated objects.
	physicsSolver solver;

} scene;

return_t scnInit(scene *scn, size_t zoneNum);

objInstance **scnAllocate(scene *scn);
void scnFree(scene *scn, objInstance **obji);

void scnUpdate(scene *scn, const float elapsedTime, const float dt);

void scnReset(scene *scn);
void scnDelete(scene *scn);

//void scnDelete(void *scn);

#endif
