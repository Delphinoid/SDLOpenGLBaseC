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

quat quatQAddQ(const quat *const restrict q1, const quat *const restrict q2);
quat quatQAddS(const quat *const restrict q, const float s);
quat quatQAddW(const quat *const restrict q, const float w);
void quatAddQToQ(quat *const restrict q1, const quat *const restrict q2);
void quatAddQToQR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r);
void quatAddSToQ(quat *const restrict q, const float s);
void quatAddWToQ(quat *const restrict q, const float w);

quat quatQSubQ(const quat *const restrict q1, const quat *const restrict q2);
quat quatQSubS(const quat *const restrict q, const float s);
quat quatQSubW(const quat *const restrict q, const float w);
void quatSubQFromQ1(quat *const restrict q1, const quat *const restrict q2);
void quatSubQFromQ2(const quat *const restrict q1, quat *const restrict q2);
void quatSubQFromQR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r);
void quatSubSFromQ(quat *const restrict q, const float s);
void quatSubWFromQ(quat *const restrict q, const float w);

quat quatQMultQ(const quat *const restrict q1, const quat *const restrict q2);
quat quatQMultS(const quat *const restrict q, const float s);
void quatMultQByQ1(quat *const restrict q1, const quat *const restrict q2);
void quatMultQByQ2(const quat *const restrict q1, quat *const restrict q2);
void quatMultQByQR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r);
void quatMultQByS(quat *const restrict q, const float s);

quat quatQDivQ(const quat *const restrict q1, const quat *const restrict q2);
quat quatQDivS(const quat *const restrict q, const float s);
void quatDivQByQ1(quat *const restrict q1, const quat *const restrict q2);
void quatDivQByQ2(const quat *const restrict q1, quat *const restrict q2);
void quatDivQByQR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r);
void quatDivQByS(quat *const restrict q, const float s);

float quatGetMagnitude(const quat *const restrict q);

quat quatGetConjugate(const quat *const restrict q);
quat quatGetConjugateFast(const quat *const restrict q);
void quatConjugate(quat *const restrict q);
void quatConjugateFast(quat *const restrict q);
void quatConjugateR(const quat *const restrict q, quat *const restrict r);
void quatConjugateFastR(const quat *const restrict q, quat *const restrict r);

quat quatGetNegative(const quat *const restrict q);
void quatNegate(quat *const restrict q);
void quatNegateR(const quat *const restrict q, quat *const restrict r);

quat quatGetInverse(const quat *const restrict q);
void quatInvert(quat *const restrict q);
void quatInvertR(const quat *const restrict q, quat *const restrict r);

quat quatGetDifference(const quat *const restrict q1, const quat *const restrict q2);
void quatDifference(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r);

quat quatGetUnit(const quat *const restrict q);
quat quatGetUnitFast(const quat *const restrict q);
void quatNormalize(quat *const restrict q);
void quatNormalizeFast(quat *const restrict q);

quat quatIdentity();
void quatSetIdentity(quat *const restrict q);

void quatAxisAngle(const quat *const restrict q, float *angle, float *axisX, float *axisY, float *axisZ);
void quatAxisAngleFast(const quat *const restrict q, float *angle, float *axisX, float *axisY, float *axisZ);

float quatDot(const quat *const restrict q1, const quat *const restrict q2);

vec3 quatGetRotatedVec3(const quat *const restrict q, const vec3 *const restrict v);
vec3 quatGetRotatedVec3Fast(const quat *const restrict q, const vec3 *const restrict v);
void quatRotateVec3(const quat *const restrict q, vec3 *const restrict v);
void quatRotateVec3R(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict r);
void quatRotateVec3Fast(const quat *const restrict q, vec3 *const restrict v);
void quatRotateVec3FastR(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict r);

quat quatLookingAt(const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict up);
void quatLookAt(quat *const restrict q, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict up);

quat quatGetLerp(const quat *const restrict q1, const quat *const restrict q2, const float t);
void quatLerp1(quat *const restrict q1, const quat *const restrict q2, const float t);
void quatLerp2(const quat *const restrict q1, quat *const restrict q2, const float t);
void quatLerpR(const quat *const restrict q1, const quat *const restrict q2, const float t, quat *const restrict r);

quat quatGetSlerp(const quat *const restrict q1, const quat *const restrict q2, const float t);
void quatSlerp1(quat *const restrict q1, const quat *const restrict q2, const float t);
void quatSlerp2(const quat *const restrict q1, quat *const restrict q2, const float t);
void quatSlerpR(const quat *const restrict q1, const quat *const restrict q2, const float t, quat *const restrict r);

void quatDifferentiate(quat *const restrict q, const vec3 *const restrict w);
void quatDifferentiateR(const quat *const restrict q, const vec3 *const restrict w, quat *const restrict r);
void quatIntegrate(quat *const restrict q, const vec3 *const restrict w, float dt);
void quatIntegrateR(const quat *const restrict q, const vec3 *const restrict w, float dt, quat *const restrict r);

void quatRotateR(const quat *const restrict q1, const quat *const restrict q2, const float t, quat *const restrict r);

#endif
