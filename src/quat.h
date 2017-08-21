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
void quatSet(quat *q, const float w, const float x, const float y, const float z);
void quatSetS(quat *q, const float s);
void quatSetAxisAngle(quat *q, const float angle, const float axisX, const float axisY, const float axisZ);
void quatSetEuler(quat *q, const float x, const float y, const float z);

quat quatQAddQ(const quat q1, const quat q2);
quat quatQAddW(const quat q, const float w);
void quatAddQToQ(quat *q1, const quat q2);
void quatAddWToQ(quat *q, const float w);

quat quatQSubQ(const quat q1, const quat q2);
quat quatQSubW(const quat q, const float w);
void quatSubQFromQ1(quat *q1, const quat q2);
void quatSubQFromQ2(const quat q1, quat *q2);
void quatSubWFromQ(quat *q, const float w);

quat quatQMultQ(const quat q1, const quat q2);
quat quatQMultS(const quat q, const float s);
void quatMultQByQ1(quat *q1, const quat q2);
void quatMultQByQ2(const quat q1, quat *q2);
void quatMultQByS(quat *q, const float s);

quat quatQDivQ(const quat q1, const quat q2);
quat quatQDivS(const quat q, const float s);
void quatDivQByQ1(quat *q1, const quat q2);
void quatDivQByQ2(const quat q1, quat *q2);
void quatDivQByS(quat *q, const float s);

float quatGetMagnitude(const quat q);

quat quatGetConjugate(const quat q);
void quatConjugate(quat *q);

quat quatGetNegative(const quat q);
void quatNegate(quat *q);

quat quatGetInverse(const quat q);
void quatInvert(quat *q);

quat quatGetUnit(const quat q);
void quatNormalize(quat *q);

quat quatIdentity();
void quatSetIdentity(quat *q);

void quatAxisAngle(const quat q, float *angle, float *axisX, float *axisY, float *axisZ);

float quatDot(const quat q1, const quat q2);

vec3 quatGetRotatedVec3(const quat q, vec3 v);
void quatRotateVec3(const quat q, vec3 *v);

quat quatLookingAt(vec3 eye, vec3 target, vec3 up);
void quatLookAt(quat *q, vec3 eye, vec3 target, vec3 up);

quat quatLerp(quat q1, quat q2, const float t);
quat quatSlerp(quat q1, quat q2, const float t);

#endif
