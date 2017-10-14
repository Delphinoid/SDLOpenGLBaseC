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

typedef struct {
	float *previous;
	float *value;
	float render;
} interpFloat;

unsigned char iFloatInit(interpFloat *iFloat, const float s, size_t stateNum);
unsigned char iFloatResize(interpFloat *iFloat, const size_t oldStateNum, size_t newStateNum);
void iFloatSkipCurrentInterp(interpFloat *iFloat);
void iFloatResetInterp(interpFloat *iFloat, const size_t stateNum);
void iFloatPreviousState(interpFloat *iFloat, float *value, const size_t state, const float interpT);
unsigned char iFloatUpdate(interpFloat *iFloat, const float interpT);


typedef struct {
	vec3 *previous;
	vec3 *value;
	vec3 render;
} interpVec3;

unsigned char iVec3Init(interpVec3 *iVec3, const float x, const float y, const float z, size_t stateNum);
unsigned char iVec3Resize(interpVec3 *iVec3, const size_t oldStateNum, size_t newStateNum);
void iVec3SkipCurrentInterp(interpVec3 *iVec3);
void iVec3ResetInterp(interpVec3 *iVec3, const size_t stateNum);
void iVec3PreviousState(interpVec3 *iVec3, vec3 *value, const size_t state, const float interpT);
unsigned char iVec3Update(interpVec3 *iVec3, const float interpT);


typedef struct {
	quat *previous;
	quat *value;
	quat render;
} interpQuat;

unsigned char iQuatInit(interpQuat *iQuat, size_t stateNum);
unsigned char iQuatResize(interpQuat *iQuat, const size_t oldStateNum, size_t newStateNum);
void iQuatSkipCurrentInterp(interpQuat *iQuat);
void iQuatResetInterp(interpQuat *iQuat, const size_t stateNum);
void iQuatPreviousState(interpQuat *iQuat, quat *value, const size_t state, const float interpT);
unsigned char iQuatUpdate(interpQuat *iQuat, const float interpT);

#endif
