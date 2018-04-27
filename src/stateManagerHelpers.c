#include "stateManagerHelpers.h"

inline scene *scnGetState(const stateManager *sm, const size_t objectID, const size_t stateID){
	return sm->objectType[SM_TYPE_SCENE].instance[objectID].state[stateID];
}
inline camera *camGetState(const stateManager *sm, const size_t objectID, const size_t stateID){
	return sm->objectType[SM_TYPE_CAMERA].instance[objectID].state[stateID];
}
inline objInstance *objGetState(const stateManager *sm, const size_t objectID, const size_t stateID){
	return sm->objectType[SM_TYPE_OBJECT].instance[objectID].state[stateID];
}
