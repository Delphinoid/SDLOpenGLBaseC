#ifndef INTERPSTATE_H
#define INTERPSTATE_H

#include "vec3.h"
#include "quat.h"

/** Should these store currentValue (known as render now)? **/

typedef struct {
	float previous;
	float render;
	float value;
} interpFloat;

void iFloatInit(interpFloat *iFloat, const float v);
void iFloatResetInterp(interpFloat *iFloat);
unsigned char iFloatUpdate(interpFloat *iFloat, const float interpT);


typedef struct {
	vec3 previous;
	vec3 render;
	vec3 value;
} interpVec3;

void iVec3Init(interpVec3 *iVec3, const float x, const float y, const float z);
void iVec3ResetInterp(interpVec3 *iVec3);
unsigned char iVec3Update(interpVec3 *iVec3, const float interpT);


typedef struct {
	quat previous;
	quat render;
	quat value;
} interpQuat;

void iQuatInit(interpQuat *iQuat);
void iQuatResetInterp(interpQuat *iQuat);
unsigned char iQuatUpdate(interpQuat *iQuat, const float interpT);

#endif
