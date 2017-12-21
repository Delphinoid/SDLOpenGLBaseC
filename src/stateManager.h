#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include "camera.h"

/** Remove "active" property from state types. Also consider merging them? **/

/* Contains each state of a specific renderable. */
typedef struct {
	unsigned char active;  // Whether or not this slot is free.
	renderable **state;   // An array of pointers to renderables. Renderables that no longer exist have NULL entries.
} stateRenderable;

/* Contains each state of a specific scene. */
typedef struct {
	unsigned char active;  // Whether or not this slot is free.
	scene **state;        // An array of pointers to scenes. Scenes that no longer exist have NULL entries.
} stateScene;

/* Contains each state of a specific camera. */
typedef struct {
	unsigned char active;  // Whether or not this slot is free.
	camera **state;       // An array of pointers to cameras. Cameras that no longer exist have NULL entries.
} stateCamera;


typedef struct stateManager{

	size_t stateNum;
	size_t currentStateID;

	size_t renderableCapacity;
	stateRenderable *renderables;

	size_t sceneCapacity;
	stateScene *scenes;

	size_t cameraCapacity;
	stateCamera *cameras;

} stateManager;

unsigned char smInit(stateManager *sm, const size_t renderableCapacity, const size_t sceneCapacity, const size_t cameraCapacity, const size_t stateNum);

/** The six functions below, as well as the three respective update functions, are practically identical. Maybe they can be combined? **/
unsigned char smRenderableNew(stateManager *sm, size_t *renderableID);
void smRenderableDelete(stateManager *sm, const size_t renderableID);
unsigned char smSceneNew(stateManager *sm, size_t *sceneID);
void smSceneDelete(stateManager *sm, const size_t sceneID);
unsigned char smCameraNew(stateManager *sm, size_t *cameraID);
void smCameraDelete(stateManager *sm, const size_t cameraID);

unsigned char smPrepareNextState(stateManager *sm);
unsigned char smGenerateDeltaState(const stateManager *sm, const size_t stateID);
void smDelete(stateManager *sm);

/* A structure describing the state of the game at a certain point in time. */
/*typedef struct {

	size_t renderableNum;
	size_t renderableCapacity;
	//size_t *renderableIDs;    // A numerical ID for each renderable in renderables. Stores (size_t)-1 for deleted values.
	renderable *renderables;  // An array of pointers to every renderable in the current game state.

	size_t sceneNum;
	size_t sceneCapacity;
	scene *scenes;            // A copy of every scene in the current game state.

	size_t cameraNum;
	size_t cameraCapacity;
	camera *cameras;          // A copy of every camera in the current game state. Contains pointers to elements in sceneArray.

} state;

typedef struct {
	size_t stateNum;
	state *stateArray;  // An array of game states. Size is stateNum. More recent states come first.
	size_t currentStateID;
} stateManager;*/

#endif
