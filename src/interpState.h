#ifndef INTERPSTATE_H
#define INTERPSTATE_H

#include <stddef.h>
#include "vec3.h"
#include "quat.h"

/** Should these store currentValue (known as render now)? **/

/*
** typedef struct {
**     type previous;  // Stores the previous value of the variable.
**     type value;     // Stores the current value of the variable.
**     type render;    // Stores the value used for rendering. Used to tell when the
**                     // variable has been changed to potentially save time with
**                     // matrix calculations before rendering.
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
/** We definitely don't need render, try and remove it. **/

typedef struct {
	float previous;
	float value;
	float render;
} interpFloat;

void iFloatInit(interpFloat *iFloat, const float s);
void iFloatResetInterp(interpFloat *iFloat);
signed char iFloatUpdate(interpFloat *iFloat, const float interpT);


typedef struct {
	size_t previous;
	size_t value;
	size_t render;
} interpSizeT;

void iSizeTInit(interpSizeT *iSizeT, const size_t s);
void iSizeTResetInterp(interpSizeT *iSizeT);
signed char iSizeTUpdate(interpSizeT *iSizeT, const float interpT);


typedef struct {
	vec3 previous;
	vec3 value;
	vec3 render;
} interpVec3;

void iVec3Init(interpVec3 *iVec3, const float x, const float y, const float z);
void iVec3ResetInterp(interpVec3 *iVec3);
signed char iVec3Update(interpVec3 *iVec3, const float interpT);


typedef struct {
	quat previous;
	quat value;
	quat render;
} interpQuat;

void iQuatInit(interpQuat *iQuat);
void iQuatResetInterp(interpQuat *iQuat);
signed char iQuatUpdate(interpQuat *iQuat, const float interpT);

/*
typedef struct {
	float previous;
	float value;
} interpFloat;

void iFloatInit(interpFloat *iFloat, const float s);
void iFloatResetInterp(interpFloat *iFloat);
void iFloatInterpolate(const interpFloat *iFloat, const float interpT, float *r);


typedef struct {
	size_t previous;
	size_t value;
} interpSizeT;

void iSizeTInit(interpSizeT *iSizeT, const size_t s);
void iSizeTResetInterp(interpSizeT *iSizeT);
void iSizeTInterpolate(const interpSizeT *iSizeT, const float interpT, size_t *r);


typedef struct {
	vec3 previous;
	vec3 value;
} interpVec3;

void iVec3Init(interpVec3 *iVec3, const float x, const float y, const float z);
void iVec3ResetInterp(interpVec3 *iVec3);
void iVec3Interpolate(const interpVec3 *iVec3, const float interpT, vec3 *r);


typedef struct {
	quat previous;
	quat value;
} interpQuat;

void iQuatInit(interpQuat *iQuat);
void iQuatResetInterp(interpQuat *iQuat);
void iQuatInterpolate(const interpQuat *iQuat, const float interpT, quat *r);
*/

#endif
