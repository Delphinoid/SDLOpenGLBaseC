#ifndef INTERPSTATE_H
#define INTERPSTATE_H

#include <stdlib.h>
#include "vec3.h"
#include "quat.h"

/** Should these store currentValue (known as render now)? **/

/*
** typedef struct {
**     type *previous;  // Stores an array of previous values, from most recent to least recent.
**     type *value;     // Stores an array of current values, from most recent to least recent.
**     type render;     // Stores the latest value used for rendering. Used to tell when the
**                      // variable has been changed to potentially save time with matrix
**                      // calculations before rendering.
** } interpType;
**
** Generally, clients will only need to store one previous value and one current value. It becomes
** much more useful for servers, which may need to know what the values of certain variables were
** many states earlier.
**
*/

/** Remove the ResetInterp() functions. **/
/** Merge previous and update functions. **/
/** Do we actually need previous? **/

typedef struct {
	float *previous;
	float *value;
	float render;
} interpFloat;

unsigned char iFloatInit(interpFloat *iFloat, const size_t stateNum, const float s);
unsigned char iFloatResize(interpFloat *iFloat, const size_t oldStateNum, const size_t newStateNum);
void iFloatSkipCurrentInterp(interpFloat *iFloat);
void iFloatResetInterp(interpFloat *iFloat, const size_t stateNum);
unsigned char iFloatUpdate(interpFloat *iFloat, const size_t state, const float interpT);
void iFloatDelete(interpFloat *iFloat);


typedef struct {
	size_t *previous;
	size_t *value;
	size_t render;
} interpSizeT;

unsigned char iSizeTInit(interpSizeT *iSizeT, const size_t stateNum, const size_t s);
unsigned char iSizeTResize(interpSizeT *iSizeT, const size_t oldStateNum, const size_t newStateNum);
void iSizeTSkipCurrentInterp(interpSizeT *iSizeT);
void iSizeTResetInterp(interpSizeT *iSizeT, const size_t stateNum);
unsigned char iSizeTUpdate(interpSizeT *iSizeT, const size_t state, const float interpT);
void iSizeTDelete(interpSizeT *iSizeT);


typedef struct {
	vec3 *previous;
	vec3 *value;
	vec3 render;
} interpVec3;

unsigned char iVec3Init(interpVec3 *iVec3, const size_t stateNum, const float x, const float y, const float z);
unsigned char iVec3Resize(interpVec3 *iVec3, const size_t oldStateNum, const size_t newStateNum);
void iVec3SkipCurrentInterp(interpVec3 *iVec3);
void iVec3ResetInterp(interpVec3 *iVec3, const size_t stateNum);
unsigned char iVec3Update(interpVec3 *iVec3, const size_t state, const float interpT);
void iVec3Delete(interpVec3 *iVec3);


typedef struct {
	quat *previous;
	quat *value;
	quat render;
} interpQuat;

unsigned char iQuatInit(interpQuat *iQuat, const size_t stateNum);
unsigned char iQuatResize(interpQuat *iQuat, const size_t oldStateNum, const size_t newStateNum);
void iQuatSkipCurrentInterp(interpQuat *iQuat);
void iQuatResetInterp(interpQuat *iQuat, const size_t stateNum);
unsigned char iQuatUpdate(interpQuat *iQuat, const size_t state, const float interpT);
void iQuatDelete(interpQuat *iQuat);

#endif
