#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <stdlib.h>

/** Remove "active" property from state types? **/

typedef struct {
	unsigned char active;  // Whether or not this slot is free.
	void **state;          // An array of pointers to objects. Objects that no longer exist have NULL entries.
} stateObject;

typedef struct {
	unsigned char (*stateInit)(void*);
	unsigned char (*stateNew)(void*);
	unsigned char (*stateCopy)(void*, void*);
	void (*stateResetInterpolation)(void*);
	void (*stateDelete)(void*);
	size_t size;
	size_t capacity;
	stateObject *instance;
} stateObjectType;

typedef struct stateManager{

	size_t stateNum;
	size_t currentStateID;

	size_t objectTypeNum;
	stateObjectType *objectType;

} stateManager;

unsigned char smObjectTypeNew(stateManager *sm, unsigned char (*stateInit)(void*),
                              unsigned char (*stateNew)(void*), unsigned char (*stateCopy)(void*, void*),
                              void (*stateResetInterpolation)(void*), void (*stateDelete)(void*),
                              const size_t size, const size_t capacity);

unsigned char smObjectNew(stateManager *sm, const size_t objectTypeID, size_t *objectID);
void smObjectDelete(stateManager *sm, const size_t objectTypeID, const size_t objectID);

void smInit(stateManager *sm, const size_t stateNum);
unsigned char smPrepareNextState(stateManager *sm);
unsigned char smGenerateDeltaState(const stateManager *sm, const size_t stateID);
void smDelete(stateManager *sm);

#endif
