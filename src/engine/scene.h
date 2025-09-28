#ifndef SCENE_H
#define SCENE_H

#include "settingsScene.h"
#include "collider.h"
#include "physicsIsland.h"
#include "memoryDLink.h"

#ifndef SCENE_OBJECT_REGION_SIZE
	#define SCENE_OBJECT_REGION_SIZE 1024
#endif

#define SCENE_ZONE_SLEEP 0x01

/** Add physics island arrays to scenes.           **/
/** Links both ways for object removal / deletion. **/
/** Scene / zone update rate? **/
/** Scene transformation (translation, scale, rotation for updating)? **/

//typedef uint_least32_t objectIndex_t;

typedef struct object object;
typedef struct physRigidBody physRigidBody;
typedef struct physJoint physJoint;

///typedef struct scnZone scnZone;
typedef struct {

	// Maximum render distance.
	float distance;

	// Portal bounding box.
	cAABB bounds;

	// Zone pointer.
	///scnZone *zone;

} scnPortal;

typedef struct scnZone {

	/**
	*** Eventually, the object pool should
	*** replace __g_ObjectResourceArray.
	*** Or just use an SLink, depending on
	*** performance.
	**/

	// Zone portals.
	// Used to mark the areas between
	// this zone and other zones.
	scnPortal *portals;  // Contains scnPortals.
	size_t portalNum;

	// Zone collider.
	// Used to find which zone the
	// camera is in, if any.
	collider bounds;

	// Flags for the zone.
	flags_t flags;

} scnZone;

typedef struct scene {

	/**
	*** Eventually, the object pool should
	*** replace __g_ObjectResourceArray.
	*** Or just use an SLink, depending on
	*** performance.
	**/

	// Objects in the scene.
	// We store the number of objects to allow
	// easier preallocations when rendering.
	object *objects;   // Contains object pointers (DLink).
	size_t objectNum;  // The number of objects in the scene.

	/**
	*** Particle systems should use an SLink
	*** similarly to physics islands. That is,
	*** the scene / module uses an SLink, and
	*** objects storing arrays of particle
	*** systems update it so that the particles
	*** in their own arrays are all adjacent.
	**/

	// SLink of scene zones.
	///scnZone *zones;

	// The scene's physics island.
	physIsland island;

} scene;

void scnInit(scene *const __RESTRICT__ scn);

void scnInsertJoint(scene *const __RESTRICT__ scn, physJoint *const joint);
void scnRemoveJoint(scene *const __RESTRICT__ scn, physJoint *const joint);

void scnInsertRigidBody(scene *const __RESTRICT__ scn, physRigidBody *const body);
void scnRemoveRigidBody(scene *const __RESTRICT__ scn, physRigidBody *const body);

void scnInsertObject(scene *const __RESTRICT__ scn, object *const obj);
void scnRemoveObject(scene *const __RESTRICT__ scn, object *const obj);

#ifdef PHYSICS_CONTACT_STABILIZER_BAUMGARTE
return_t scnTick(scene *const __RESTRICT__ scn, const float dt_ms, const float dt_s, const float frequency);
#else
return_t scnTick(scene *const __RESTRICT__ scn, const float dt_ms, const float dt_s);
#endif

void scnReset(scene *const __RESTRICT__ scn);
void scnDelete(scene *const __RESTRICT__ scn);

#endif
