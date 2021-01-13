#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "mat4.h"

// Represents an affine transformation.
typedef struct {
	vec3 position;
	quat orientation;
	vec3 scale;
} transform;

void tfInit(transform *const __RESTRICT__ tf);
transform tfIdentity();

mat4 tfMatrix(const transform tf);

///transform tfInverse(const transform tf);
///void tfInverseP(transform *const __RESTRICT__ tf);
///void tfInversePR(const transform *const __RESTRICT__ tf, transform *const __RESTRICT__ r);

transform tfInterpolate(const transform tf1, const transform tf2, const float t);
void tfInterpolateP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, const float t);
void tfInterpolateP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2, const float t);
void tfInterpolatePR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, const float t, transform *const __RESTRICT__ r);

vec3 tfTransform(const transform tf, const vec3 v);

transform tfInverse(const transform tf);
void tfInverseP(transform *const __RESTRICT__ tf);
void tfInversePR(const transform *const __RESTRICT__ tf, transform *const __RESTRICT__ r);

vec3 tfAppendPosition1(const transform tf1, const transform tf2);
vec3 tfAppendPosition2(const transform tf1, const transform tf2);
void tfAppendPositionP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2);
void tfAppendPositionP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2);
void tfAppendPositionPR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r);

vec3 tfAppendPositionVec(const transform tf, const float x, const float y, const float z);
void tfAppendPositionVecP(transform *const __RESTRICT__ tf, const float x, const float y, const float z);
void tfAppendPositionVecPR(const transform *const __RESTRICT__ tf, const float x, const float y, const float z, vec3 *const __RESTRICT__ r);

quat tfAppendOrientation(const transform tf1, const transform tf2);
void tfAppendOrientationP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2);
void tfAppendOrientationP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2);
void tfAppendOrientationPR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r);

vec3 tfAppendScale(const transform tf1, const transform tf2);
void tfAppendScaleP(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2);
void tfAppendScalePR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r);

transform tfAppend(const transform tf1, const transform tf2);
void tfAppendP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2);
void tfAppendP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2);
void tfAppendPR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r);

transform tfPrepend(const transform tf1, const transform tf2);

transform tfCombine(const transform tf1, const transform tf2);
void tfCombineP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2);
void tfCombineP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2);
void tfCombinePR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r);

#endif
