#ifndef QUAT_H
#define QUAT_H

#include "vec3.h"

typedef struct {
	float w;
	vec3 v;
} quat;

quat quatNew(const float w, const float x, const float y, const float z);
quat quatNewS(const float s);
quat quatNewAxisAngle(const float angle, const float axisX, const float axisY, const float axisZ);
quat quatNewEuler(const float x, const float y, const float z);
void quatSet(quat *const restrict q, const float w, const float x, const float y, const float z);
void quatSetS(quat *const restrict q, const float s);
void quatSetAxisAngle(quat *const restrict q, const float angle, const float axisX, const float axisY, const float axisZ);
void quatSetEuler(quat *const restrict q, const float x, const float y, const float z);
void quatZero(quat *const restrict q);

quat quatQAddQ(const quat q1, const quat q2);
quat quatQAddS(const quat q, const float s);
quat quatQAddW(const quat q, const float w);
void quatQAddQP(quat *const restrict q1, const quat *const restrict q2);
void quatQAddQPR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r);
void quatQAddSP(quat *const restrict q, const float s);
void quatQAddWP(quat *const restrict q, const float w);

quat quatQSubQ(const quat q1, const quat q2);
quat quatQSubS(const quat q, const float s);
quat quatQSubW(const quat q, const float w);
void quatQSubQP1(quat *const restrict q1, const quat *const restrict q2);
void quatQSubQP2(const quat *const restrict q1, quat *const restrict q2);
void quatQSubQPR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r);
void quatQSubSP(quat *const restrict q, const float s);
void quatQSubWP(quat *const restrict q, const float w);

quat quatQMultQ(const quat q1, const quat q2);
quat quatQMultS(const quat q, const float s);
void quatQMultQP1(quat *const restrict q1, const quat *const restrict q2);
void quatQMultQP2(const quat *const restrict q1, quat *const restrict q2);
void quatQMultQPR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r);
void quatQMultSP(quat *const restrict q, const float s);

quat quatQDivQ(const quat q1, const quat q2);
quat quatQDivS(const quat q, const float s);
void quatQDivQP1(quat *const restrict q1, const quat *const restrict q2);
void quatQDivQP2(const quat *const restrict q1, quat *const restrict q2);
void quatQDivQPR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r);
void quatQDivSP(quat *const restrict q, const float s);

float quatMagnitude(const quat q);
float quatMagnitudeP(const quat *const restrict q);
float quatMagnitudeSquared(const quat q);
float quatMagnitudeSquaredP(const quat *const restrict q);
float quatMagnitudeInverse(const quat q);
float quatMagnitudeInverseP(const quat *const restrict q);

quat quatConjugate(const quat q);
quat quatConjugateFast(const quat q);
void quatConjugateP(quat *const restrict q);
void quatConjugateFastP(quat *const restrict q);
void quatConjugatePR(const quat *const restrict q, quat *const restrict r);
void quatConjugateFastPR(const quat *const restrict q, quat *const restrict r);

quat quatNegate(const quat q);
void quatNegateP(quat *const restrict q);
void quatNegatePR(const quat *const restrict q, quat *const restrict r);

quat quatInvert(const quat q);
void quatInvertP(quat *const restrict q);
void quatInvertPR(const quat *const restrict q, quat *const restrict r);

quat quatDifference(const quat q1, const quat q2);
void quatDifferenceP(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r);

quat quatNormalize(const quat q);
quat quatNormalizeFast(const quat q);
quat quatNormalizeFastAccurate(const quat q);
void quatNormalizeP(quat *const restrict q);
void quatNormalizeFastP(quat *const restrict q);
void quatNormalizeFastAccurateP(quat *const restrict q);

quat quatIdentity();
void quatSetIdentity(quat *const restrict q);

void quatAxisAngle(const quat q, float *angle, float *axisX, float *axisY, float *axisZ);
void quatAxisAngleFast(const quat q, float *angle, float *axisX, float *axisY, float *axisZ);
void quatAxisAngleP(const quat *const restrict q, float *angle, float *axisX, float *axisY, float *axisZ);
void quatAxisAngleFastP(const quat *const restrict q, float *angle, float *axisX, float *axisY, float *axisZ);

float quatDot(const quat q1, const quat q2);
float quatDotP(const quat *const restrict q1, const quat *const restrict q2);

vec3 quatRotateVec3(const quat q, const vec3 v);
vec3 quatRotateVec3Fast(const quat q, const vec3 v);
void quatRotateVec3P(const quat *const restrict q, vec3 *const restrict v);
void quatRotateVec3PR(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict r);
void quatRotateVec3FastP(const quat *const restrict q, vec3 *const restrict v);
void quatRotateVec3FastPR(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict r);

quat quatLookAt(const vec3 eye, const vec3 target, const vec3 up);
void quatLookAtP(quat *const restrict q, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict up);

quat quatLerp(const quat q1, const quat q2, const float t);
void quatLerpP1(quat *const restrict q1, const quat *const restrict q2, const float t);
void quatLerpP2(const quat *const restrict q1, quat *const restrict q2, const float t);
void quatLerpPR(const quat *const restrict q1, const quat *const restrict q2, const float t, quat *const restrict r);

quat quatSlerp(const quat q1, const quat q2, const float t);
void quatSlerpP1(quat *const restrict q1, const quat *const restrict q2, const float t);
void quatSlerpP2(const quat *const restrict q1, quat *const restrict q2, const float t);
void quatSlerpPR(const quat *const restrict q1, const quat *const restrict q2, const float t, quat *const restrict r);

quat quatDifferentiate(const quat q, const vec3 w);
void quatDifferentiateP(quat *const restrict q, const vec3 *const restrict w);
void quatDifferentiatePR(const quat *const restrict q, const vec3 *const restrict w, quat *const restrict r);

quat quatIntegrate(const quat q, const vec3 w, float dt);
void quatIntegrateP(quat *const restrict q, const vec3 *const restrict w, float dt);
void quatIntegratePR(const quat *const restrict q, const vec3 *const restrict w, float dt, quat *const restrict r);

quat quatRotate(const quat q1, const quat q2, const float t);
void quatRotatePR(const quat *const restrict q1, const quat *const restrict q2, const float t, quat *const restrict r);

#endif
