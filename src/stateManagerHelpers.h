#ifndef STATEMANAGERHELPERS_H
#define STATEMANAGERHELPERS_H

#include "stateManager.h"
#include "camera.h"
#include "object.h"

#define SM_TYPE_SCENE      0
#define SM_TYPE_CAMERA     1
#define SM_TYPE_OBJECT     2

scene *scnGetState(const stateManager *sm, const size_t objectID, const size_t stateID);
camera *camGetState(const stateManager *sm, const size_t objectID, const size_t stateID);
objInstance *objGetState(const stateManager *sm, const size_t objectID, const size_t stateID);

#endif
