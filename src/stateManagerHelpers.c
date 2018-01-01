#include "stateManagerHelpers.h"

inline renderable *rndrGetState(const stateManager *sm, const size_t objectID, const size_t stateID){
	return sm->objectType[SM_TYPE_RENDERABLE].instance[objectID].state[stateID];
}
inline scene *scnGetState(const stateManager *sm, const size_t objectID, const size_t stateID){
	return sm->objectType[SM_TYPE_SCENE].instance[objectID].state[stateID];
}
inline camera *camGetState(const stateManager *sm, const size_t objectID, const size_t stateID){
	return sm->objectType[SM_TYPE_CAMERA].instance[objectID].state[stateID];
}
