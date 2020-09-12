#ifndef QUAT_H
#define QUAT_H

#include "vec3.h"

typedef struct {
	vec3 v;
	float w;
} quat;

quat quatNew(const float w, const float x, const float y, const float z);
quat quatNewS(const float s);
quat quatNewAxisAngle(const float angle, const float axisX, const float axisY, const float axisZ);
quat quatNewEuler(const float x, const float y, const float z);
quat quatNewRotation(const vec3 v1, const vec3 v2);
quat quatZero();
void quatSet(quat *const __RESTRICT__ q, const float w, const float x, const float y, const float z);
void quatSetS(quat *const __RESTRICT__ q, const float s);
void quatSetAxisAngle(quat *const __RESTRICT__ q, const float angle, const float axisX, const float axisY, const float axisZ);
void quatSetEuler(quat *const __RESTRICT__ q, const float x, const float y, const float z);
void quatZeroP(quat *const __RESTRICT__ q);

quat quatQAddQ(const quat q1, const quat q2);
quat quatQAddS(const quat q, const float s);
quat quatQAddW(const quat q, const float w);
void quatQAddQP(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2);
void quatQAddQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r);
void quatQAddSP(quat *const __RESTRICT__ q, const float s);
void quatQAddWP(quat *const __RESTRICT__ q, const float w);

quat quatQSubQ(const quat q1, const quat q2);
quat quatQSubS(const quat q, const float s);
quat quatQSubW(const quat q, const float w);
void quatQSubQP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2);
void quatQSubQP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2);
void quatQSubQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r);
void quatQSubSP(quat *const __RESTRICT__ q, const float s);
void quatQSubWP(quat *const __RESTRICT__ q, const float w);

quat quatQMultQ(const quat q1, const quat q2);
quat quatQMultS(const quat q, const float s);
void quatQMultQP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2);
void quatQMultQP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2);
void quatQMultQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r);
void quatQMultSP(quat *const __RESTRICT__ q, const float s);

quat quatQDivQ(const quat q1, const quat q2);
quat quatQDivS(const quat q, const float s);
void quatQDivQP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2);
void quatQDivQP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2);
void quatQDivQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r);
void quatQDivSP(quat *const __RESTRICT__ q, const float s);

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

quat quatIdentity();
void quatSetIdentity(quat *const __RESTRICT__ q);

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
