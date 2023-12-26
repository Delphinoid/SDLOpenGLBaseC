#ifndef TRANSFORMRIGID_H
#define TRANSFORMRIGID_H

#include "mat3x4.h"

// Represents a rigid transformation.
typedef struct {
	vec3 position;
	quat orientation;
	vec3 scale;
} transformRigid;

extern transformRigid g_tfrIdentity;

void tfrIdentityP(transformRigid *const __RESTRICT__ tf);
mat3x4 tfrMatrix(const transformRigid tf);

vec3 tfrTransform(const transformRigid tf, const vec3 v);
void tfrTransformP(const transformRigid *const __RESTRICT__ tf, vec3 *const __RESTRICT__ v);
void tfrTransformPR(const transformRigid *const __RESTRICT__ tf, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r);

transformRigid tfrInterpolate(const transformRigid tf1, const transformRigid tf2, const float t);
void tfrInterpolateP1(transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, const float t);
void tfrInterpolateP2(const transformRigid *const __RESTRICT__ tf1, transformRigid *const __RESTRICT__ tf2, const float t);
void tfrInterpolatePR(const transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, const float t, transformRigid *const __RESTRICT__ r);

transformRigid tfrInverse(const transformRigid tf);
void tfrInverseP(transformRigid *const __RESTRICT__ tf);
void tfrInversePR(const transformRigid *const __RESTRICT__ tf, transformRigid *const __RESTRICT__ r);

vec3 tfrAppendPosition1(const transformRigid tf1, const transformRigid tf2);
vec3 tfrAppendPosition2(const transformRigid tf1, const transformRigid tf2);
void tfrAppendPositionP1(transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2);
void tfrAppendPositionP2(const transformRigid *const __RESTRICT__ tf1, transformRigid *const __RESTRICT__ tf2);
void tfrAppendPositionPR(const transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, transformRigid *const __RESTRICT__ r);

vec3 tfrAppendPositionVec(const transformRigid tf, const float x, const float y, const float z);
void tfrAppendPositionVecP(transformRigid *const __RESTRICT__ tf, const float x, const float y, const float z);
void tfrAppendPositionVecPR(const transformRigid *const __RESTRICT__ tf, const float x, const float y, const float z, vec3 *const __RESTRICT__ r);

quat tfrAppendOrientation(const transformRigid tf1, const transformRigid tf2);
void tfrAppendOrientationP1(transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2);
void tfrAppendOrientationP2(const transformRigid *const __RESTRICT__ tf1, transformRigid *const __RESTRICT__ tf2);
void tfrAppendOrientationPR(const transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, transformRigid *const __RESTRICT__ r);

vec3 tfrAppendScale(const transformRigid tf1, const transformRigid tf2);
void tfrAppendScaleP(transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2);
void tfrAppendScalePR(const transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, transformRigid *const __RESTRICT__ r);

transformRigid tfrAppend(const transformRigid tf1, const transformRigid tf2);
void tfrAppendP1(transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2);
void tfrAppendP2(const transformRigid *const __RESTRICT__ tf1, transformRigid *const __RESTRICT__ tf2);
void tfrAppendPR(const transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, transformRigid *const __RESTRICT__ r);

transformRigid tfrPrepend(const transformRigid tf1, const transformRigid tf2);

transformRigid tfrCombine(const transformRigid tf1, const transformRigid tf2);
void tfrCombineP1(transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2);
void tfrCombineP2(const transformRigid *const __RESTRICT__ tf1, transformRigid *const __RESTRICT__ tf2);
void tfrCombinePR(const transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, transformRigid *const __RESTRICT__ r);

#endif
