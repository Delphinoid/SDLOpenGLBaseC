#ifndef STATEMANAGERHELPERS_H
#define STATEMANAGERHELPERS_H

#include "stateManager.h"
#include "camera.h"

#define SM_TYPE_RENDERABLE 0
#define SM_TYPE_SCENE      1
#define SM_TYPE_CAMERA     2

renderable *rndrGetState(const stateManager *sm, const size_t objectID, const size_t stateID);
scene *scnGetState(const stateManager *sm, const size_t objectID, const size_t stateID);
camera *camGetState(const stateManager *sm, const size_t objectID, const size_t stateID);

#endif
