#ifndef QUAT_H
#define QUAT_H

#include "vec3.h"

typedef struct mat4 mat4;

typedef struct quat {
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

float quatGetMagnitude(quat q);

quat quatGetConjugate(quat q);
void quatConjugate(quat *q);

quat quatGetNegative(quat q);
void quatNegate(quat *q);

quat quatGetInverse(quat q);
void quatInvert(quat *q);

quat quatGetUnit(quat q);
void quatNormalize(quat *q);

void quatMat4(quat q, mat4 *m);
void quatAxisAngle(quat q, float *angle, float *axisX, float *axisY, float *axisZ);

#endif
