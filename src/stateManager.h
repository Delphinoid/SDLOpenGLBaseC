#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <stdlib.h>

//#define SM_TYPE_NUM 3

/**
*** Redo parts of this later, as it should only
*** usually be needed for netcode in very
*** specific instances. Try and incorporate it
*** into the general resource manager.
**/

/** Remove "active" property from state types? **/

/** Don't like using an array of pointers. **/
typedef struct {
	void **state;        // An array of pointers to objects. Objects that no longer exist have NULL entries.
	signed char active;  // Whether or not this slot is free.
} stateObject;

typedef struct {
	size_t size;            // The size of the object struct.
	size_t stateNum;        // The number of states being recorded.
	size_t capacity;        // The size of stateObjectType.instance.
	stateObject *instance;  // Each instance of this type of object.
	signed char (*stateInit)(void*);
	signed char (*stateCopy)(void*, void*);
	void (*stateResetInterpolation)(void*);
	void (*stateDelete)(void*);
} stateObjectType;

typedef struct stateManager{

	size_t currentStateID;  // The current state we are up to.

	size_t objectTypeNum;         // How many different types of objects are being recorded.
	stateObjectType *objectType;  // An array of object types being recorded.

} stateManager;

signed char stateObjectTypeInit(stateObjectType *objType,
                                signed char (*stateInit)(void*), signed char (*stateCopy)(void*, void*),
                                void (*stateResetInterpolation)(void*), void (*stateDelete)(void*),
                                const size_t size, const size_t capacity, const size_t stateNum);
signed char stateObjectTypeUpdate(stateObjectType *objType);
void stateObjectTypeDelete(stateObjectType *objType);

signed char smObjectTypeNew(stateManager *sm,
                            signed char (*stateInit)(void*), signed char (*stateCopy)(void*, void*),
                            void (*stateResetInterpolation)(void*), void (*stateDelete)(void*),
                            const size_t size, const size_t capacity, const size_t stateNum);
signed char smObjectNew(stateManager *sm, const size_t objectTypeID, size_t *objectID);
void smObjectDelete(stateManager *sm, const size_t objectTypeID, const size_t objectID);

void smInit(stateManager *sm);
signed char smPrepareNextState(stateManager *sm);
signed char smGenerateDeltaState(const stateManager *sm, const size_t stateID);
void smDelete(stateManager *sm);

#endif
