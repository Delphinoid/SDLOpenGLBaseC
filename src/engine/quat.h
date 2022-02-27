#ifndef QUAT_H
#define QUAT_H

#include "vec3.h"

// WARNING: Both quaternions and matrices often violate
// strict-aliasing rules. That is, it is common for the
// engine to cast adjacent floats in quaternion and matrix
// structures to vectors, for instance. While this is not
// an issue on most sensible compilers and platforms, it
// is still technically undefined behaviour.

typedef struct {
	float x;
	float y;
	float z;
	float w;
} quat;

extern quat g_quatIdentity;

void quatIdentityP(quat *const __RESTRICT__ q);
quat quatNew(const float w, const float x, const float y, const float z);
quat quatNewAxisAngle(const float angle, const float axisX, const float axisY, const float axisZ);
quat quatNewEuler(const float x, const float y, const float z);
quat quatNewRotation(const vec3 v1, const vec3 v2);
quat quatNewRotationFast(const vec3 v1, const vec3 v2);
void quatSet(quat *const __RESTRICT__ q, const float w, const float x, const float y, const float z);
void quatSetAxisAngle(quat *const __RESTRICT__ q, const float angle, const float axisX, const float axisY, const float axisZ);
void quatSetEuler(quat *const __RESTRICT__ q, const float x, const float y, const float z);

quat quatQAddQ(const quat q1, const quat q2);
void quatQAddQP(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2);
void quatQAddQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r);

quat quatQSubQ(const quat q1, const quat q2);
void quatQSubQP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2);
void quatQSubQP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2);
void quatQSubQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r);

quat quatQMultQ(const quat q1, const quat q2);
quat quatQMultS(const quat q, const float s);
void quatQMultQP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2);
void quatQMultQP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2);
void quatQMultQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r);
void quatQMultSP(quat *const __RESTRICT__ q, const float s);

quat quatQConjugateMultQ(const quat q1, const quat q2);
void quatQConjugateMultQP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2);
void quatQConjugateMultQP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2);
void quatQConjugateMultQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r);
quat quatQMultQConjugate(const quat q1, const quat q2);
void quatQMultQConjugateP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2);
void quatQMultQConjugateP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2);
void quatQMultQConjugatePR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r);

float quatMagnitude(const quat q);
float quatMagnitudeP(const quat *const __RESTRICT__ q);
float quatMagnitudeSquared(const quat q);
float quatMagnitudeSquaredP(const quat *const __RESTRICT__ q);
float quatMagnitudeInverse(const quat q);
float quatMagnitudeInverseP(const quat *const __RESTRICT__ q);
float quatMagnitudeInverseFast(const quat q);
float quatMagnitudeInverseFastP(const quat *const __RESTRICT__ q);
float quatMagnitudeInverseFastAccurate(const quat q);
float quatMagnitudeInverseFastAccurateP(const quat *const __RESTRICT__ q);

quat quatConjugate(const quat q);
quat quatConjugateFast(const quat q);
void quatConjugateP(quat *const __RESTRICT__ q);
void quatConjugateFastP(quat *const __RESTRICT__ q);
void quatConjugatePR(const quat *const __RESTRICT__ q, quat *const __RESTRICT__ r);
void quatConjugateFastPR(const quat *const __RESTRICT__ q, quat *const __RESTRICT__ r);

quat quatNegate(const quat q);
void quatNegateP(quat *const __RESTRICT__ q);
void quatNegatePR(const quat *const __RESTRICT__ q, quat *const __RESTRICT__ r);

#define quatInverse(q)          quatConjugate(q)
#define quatInverseFast(q)      quatConjugateFast(q)
#define quatInverseP(q)         quatConjugateP(q)
#define quatInverseFastP(q)     quatConjugateFastP(q)
#define quatInversePR(q, r)     quatConjugatePR(q, r)
#define quatInverseFastPR(q, r) quatConjugateFastPR(q, r)

quat quatDifference(const quat q1, const quat q2);
void quatDifferenceP(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r);

quat quatNormalize(const quat q);
quat quatNormalizeFast(const quat q);
quat quatNormalizeFastAccurate(const quat q);
void quatNormalizeP(quat *const __RESTRICT__ q);
void quatNormalizeFastP(quat *const __RESTRICT__ q);
void quatNormalizeFastAccurateP(quat *const __RESTRICT__ q);

void quatAxisAngle(const quat q, float *angle, float *axisX, float *axisY, float *axisZ);
void quatAxisAngleUnsafe(const quat q, float *angle, float *axisX, float *axisY, float *axisZ);
void quatAxisAngleP(const quat *const __RESTRICT__ q, float *angle, float *axisX, float *axisY, float *axisZ);
void quatAxisAngleUnsafeP(const quat *const __RESTRICT__ q, float *angle, float *axisX, float *axisY, float *axisZ);

float quatDot(const quat q1, const quat q2);
float quatDotP(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2);

vec3 quatRotateVec3(const quat q, const vec3 v);
vec3 quatRotateVec3FastApproximate(const quat q, const vec3 v);
void quatRotateVec3P(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ v);
void quatRotateVec3PR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r);
void quatRotateVec3FastApproximateP(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ v);
void quatRotateVec3FastApproximatePR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r);

vec3 quatConjugateRotateVec3(const quat q, const vec3 v);
vec3 quatConjugateRotateVec3FastApproximate(const quat q, const vec3 v);
void quatConjugateRotateVec3P(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ v);
void quatConjugateRotateVec3PR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r);
void quatConjugateRotateVec3FastApproximateP(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ v);
void quatConjugateRotateVec3FastApproximatePR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r);

quat quatLookAt(const vec3 eye, const vec3 target, const vec3 up);
void quatLookAtP(quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ eye, const vec3 *const __RESTRICT__ target, const vec3 *const __RESTRICT__ up);

quat quatLerp(const quat q1, const quat q2, const float t);
void quatLerpP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t);
void quatLerpP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2, const float t);
void quatLerpPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t, quat *const __RESTRICT__ r);

quat quatSlerp(const quat q1, const quat q2, const float t);
void quatSlerpP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t);
void quatSlerpP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2, const float t);
void quatSlerpPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t, quat *const __RESTRICT__ r);

quat quatSlerpFast(const quat q1, const quat q2, const float t);
void quatSlerpFastP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t);
void quatSlerpFastP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2, const float t);
void quatSlerpFastPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t, quat *const __RESTRICT__ r);

quat quatDifferentiate(const quat q, const vec3 w);
void quatDifferentiateP(quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ w);
void quatDifferentiatePR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ w, quat *const __RESTRICT__ r);

quat quatIntegrate(const quat q, const vec3 w, float dt);
void quatIntegrateP(quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ w, float dt);
void quatIntegratePR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ w, float dt, quat *const __RESTRICT__ r);

quat quatRotate(const quat q1, const quat q2, const float t);
void quatRotatePR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t, quat *const __RESTRICT__ r);

#endif
