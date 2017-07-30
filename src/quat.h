#ifndef QUAT_H
#define QUAT_H

#include "vec3.h"

typedef struct {
	float w;
	vec3 v;
} quat;

quat quatNew(float w, float x, float y, float z);
quat quatNewS(float s);
quat quatNewAxisAngle(float angle, float axisX, float axisY, float axisZ);
quat quatNewEuler(float x, float y, float z);
void quatSet(quat *q, float w, float x, float y, float z);
void quatSetS(quat *q, float s);
void quatSetAxisAngle(quat *q, float angle, float axisX, float axisY, float axisZ);
void quatSetEuler(quat *q, float x, float y, float z);

quat quatQAddQ(quat q1, quat q2);
quat quatQAddW(quat q, float w);
void quatAddQToQ(quat *q1, quat q2);
void quatAddWToQ(quat *q, float w);

quat quatQSubQ(quat q1, quat q2);
quat quatQSubW(quat q, float w);
void quatSubQFromQ1(quat *q1, quat q2);
void quatSubQFromQ2(quat q1, quat *q2);
void quatSubWFromQ(quat *q, float w);

quat quatQMultQ(quat q1, quat q2);
quat quatQMultS(quat q, float s);
void quatMultQByQ1(quat *q1, quat q2);
void quatMultQByQ2(quat q1, quat *q2);
void quatMultQByS(quat *q, float s);

quat quatQDivQ(quat q1, quat q2);
quat quatQDivS(quat q, float s);
void quatDivQByQ1(quat *q1, quat q2);
void quatDivQByQ2(quat q1, quat *q2);
void quatDivQByS(quat *q, float s);

float quatGetMagnitude(quat q);

quat quatGetConjugate(quat q);
void quatConjugate(quat *q);

quat quatGetNegative(quat q);
void quatNegate(quat *q);

quat quatGetInverse(quat q);
void quatInvert(quat *q);

quat quatGetUnit(quat q);
void quatNormalize(quat *q);

quat quatIdentity();
void quatSetIdentity(quat *q);

void quatAxisAngle(quat q, float *angle, float *axisX, float *axisY, float *axisZ);

float quatDot(quat q1, quat q2);

vec3 quatGetRotatedVec3(quat q, vec3 v);
void quatRotateVec3(quat q, vec3 *v);

quat quatLookingAt(vec3 eye, vec3 target, vec3 up);
void quatLookAt(quat *q, vec3 eye, vec3 target, vec3 up);

quat quatLerp(quat q1, quat q2, float t);
quat quatSlerp(quat q1, quat q2, float t);

#endif
