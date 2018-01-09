#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <stdlib.h>

/** Remove "active" property from state types? **/

typedef struct {
	signed char active;  // Whether or not this slot is free.
	void **state;        // An array of pointers to objects. Objects that no longer exist have NULL entries.
} stateObject;

typedef struct {
	size_t size;            // The size of the object struct.
	size_t capacity;        // The size of stateObjectType.instance.
	stateObject *instance;  // Each instance of this type of object.
	signed char (*stateInit)(void*);
	signed char (*stateNew)(void*);
	signed char (*stateCopy)(void*, void*);
	void (*stateResetInterpolation)(void*);
	void (*stateDelete)(void*);
} stateObjectType;

typedef struct stateManager{

	size_t stateNum;        // The number of states being recorded.
	size_t currentStateID;  // The current state we are up to.

	size_t objectTypeNum;         // How many different types of objects are being recorded.
	stateObjectType *objectType;  // An array of object types being recorded.

} stateManager;

signed char smObjectTypeNew(stateManager *sm, signed char (*stateInit)(void*),
                            signed char (*stateNew)(void*), signed char (*stateCopy)(void*, void*),
                            void (*stateResetInterpolation)(void*), void (*stateDelete)(void*),
                            const size_t size, const size_t capacity);

signed char smObjectNew(stateManager *sm, const size_t objectTypeID, size_t *objectID);
void smObjectDelete(stateManager *sm, const size_t objectTypeID, const size_t objectID);

void smInit(stateManager *sm, const size_t stateNum);
signed char smPrepareNextState(stateManager *sm);
signed char smGenerateDeltaState(const stateManager *sm, const size_t stateID);
void smDelete(stateManager *sm);

#endif
