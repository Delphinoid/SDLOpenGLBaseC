#include "quat.h"
#include "helpersMath.h"
#include "constantsMath.h"
#include <math.h>
#include <float.h>

#define QUAT_LERP_ANGLE cos(RADIAN_RATIO)

quat quatNew(const float w, const float x, const float y, const float z){
	quat r = {.w = w, .v.x = x, .v.y = y, .v.z = z};
	return r;
}
quat quatNewS(const float s){
	quat r = {.w = s, .v.x = s, .v.y = s, .v.z = s};
	return r;
}
quat quatNewAxisAngle(const float angle, const float axisX, const float axisY, const float axisZ){
	float t = sinf(angle/2.f);
	quat r;
	r.w   = cosf(angle/2.f);
	r.v.x = axisX * t;
	r.v.y = axisY * t;
	r.v.z = axisZ * t;
	return r;
}
quat quatNewEuler(const float x, const float y, const float z){
	const float hx = x*0.5f;
	const float hy = y*0.5f;
	const float hz = z*0.5f;
	const float cx = cosf(hx);
	const float cy = cosf(hy);
	const float cz = cosf(hz);
	const float sx = sinf(hx);
	const float sy = sinf(hy);
	const float sz = sinf(hz);
	quat r;
	r.w   = cx*cy*cz+sx*sy*sz;
	r.v.x = sx*cy*cz-cx*sy*sz;
	r.v.y = cx*sy*cz+sx*cy*sz;
	r.v.z = cx*cy*sz-sx*sy*cz;
	return r;
}
void quatSet(quat *q, const float w, const float x, const float y, const float z){
	q->w = w; q->v.x = x; q->v.y = y; q->v.z = z;
}
void quatSetS(quat *q, const float s){
	q->w = s; q->v.x = s; q->v.y = s; q->v.z = s;
}
void quatSetAxisAngle(quat *q, const float angle, const float axisX, const float axisY, const float axisZ){
	const float t = sinf(angle/2.f);
	q->w = cosf(angle/2.f);
	q->v.x = axisX * t;
	q->v.y = axisY * t;
	q->v.z = axisZ * t;
}
void quatSetEuler(quat *q, const float x, const float y, const float z){
	const float hx = x*0.5f;
	const float hy = y*0.5f;
	const float hz = z*0.5f;
	const float cx = cosf(hx);
	const float cy = cosf(hy);
	const float cz = cosf(hz);
	const float sx = sinf(hx);
	const float sy = sinf(hy);
	const float sz = sinf(hz);
	q->w   = cx*cy*cz+sx*sy*sz;
	q->v.x = sx*cy*cz-cx*sy*sz;
	q->v.y = cx*sy*cz+sx*cy*sz;
	q->v.z = cx*cy*sz-sx*sy*cz;
}

quat quatQAddQ(const quat *q1, const quat *q2){
	quat r = {.w   = q1->w   + q2->w,
	          .v.x = q1->v.x + q2->v.x,
	          .v.y = q1->v.y + q2->v.y,
	          .v.z = q1->v.z + q2->v.z};
	return r;
}
quat quatQAddS(const quat *q, const float s){
	quat r = {.w   = q->w   + s,
	          .v.x = q->v.x + s,
	          .v.y = q->v.y + s,
	          .v.z = q->v.z + s};
	return r;
}
quat quatQAddW(const quat *q, const float w){
	quat r = {.w   = q->w + w,
	          .v.x = q->v.x,
	          .v.y = q->v.y,
	          .v.z = q->v.z};
	return r;
}
void quatAddQToQ(quat *q1, const quat *q2){
	q1->w += q2->w; q1->v.x += q2->v.x; q1->v.y += q2->v.y; q1->v.z += q2->v.z;
}
void quatAddQToQR(const quat *q1, const quat *q2, quat *r){
	r->w = q1->w + q2->w; r->v.x = q1->v.x + q2->v.x; r->v.y = q1->v.y + q2->v.y; r->v.z = q1->v.z + q2->v.z;
}
void quatAddSToQ(quat *q, const float s){
	q->w += s; q->v.x += s; q->v.y += s; q->v.z += s;
}
void quatAddWToQ(quat *q, const float w){
	q->w += w;
}

quat quatQSubQ(const quat *q1, const quat *q2){
	quat r = {.w   = q1->w   - q2->w,
	          .v.x = q1->v.x - q2->v.x,
	          .v.y = q1->v.y - q2->v.y,
	          .v.z = q1->v.z - q2->v.z};
	return r;
}
quat quatQSubS(const quat *q, const float s){
	quat r = {.w   = q->w   - s,
	          .v.x = q->v.x - s,
	          .v.y = q->v.y - s,
	          .v.z = q->v.z - s};
	return r;
}
quat quatQSubW(const quat *q, const float w){
	quat r = {.w   = q->w - w,
	          .v.x = q->v.x,
	          .v.y = q->v.y,
	          .v.z = q->v.z};
	return r;
}
void quatSubQFromQ1(quat *q1, const quat *q2){
	q1->w -= q2->w; q1->v.x -= q2->v.x; q1->v.y -= q2->v.y; q1->v.z -= q2->v.z;
}
void quatSubQFromQ2(const quat *q1, quat *q2){
	q2->w = q1->w - q2->w; q2->v.x = q1->v.x - q2->v.x; q2->v.y = q1->v.y - q2->v.y; q2->v.z = q1->v.z - q2->v.z;
}
void quatSubQFromQR(const quat *q1, const quat *q2, quat *r){
	r->w = q1->w - q2->w; r->v.x = q1->v.x - q2->v.x; r->v.y = q1->v.y - q2->v.y; r->v.z = q1->v.z - q2->v.z;
}
void quatSubSFromQ(quat *q, const float s){
	q->w -= s; q->v.x -= s; q->v.y -= s; q->v.z -= s;
}
void quatSubWFromQ(quat *q, const float w){
	q->w -= w;
}

quat quatQMultQ(const quat *q1, const quat *q2){
	/*float prodW = q1.w * q2.w - vec3Dot(q1.v, q2.v);
	vec3 prodV = vec3VMultS(q2.v, q1.w);
	vec3AddVToV(&prodV, vec3VMultS(q1.v, q2.w));
	vec3AddVToV(&prodV, vec3Cross(q1.v, q2.v));
	return quatNew(prodW, prodV.x, prodV.y, prodV.z);*/
	/*
	** Calculates the Grassmann product of two quaternions.
	*/
	quat r;
	r.w   = q1->w * q2->w   - q1->v.x * q2->v.x - q1->v.y * q2->v.y - q1->v.z * q2->v.z;
	r.v.x = q1->w * q2->v.x + q1->v.x * q2->w   + q1->v.y * q2->v.z - q1->v.z * q2->v.y;
	r.v.y = q1->w * q2->v.y + q1->v.y * q2->w   + q1->v.z * q2->v.x - q1->v.x * q2->v.z;
	r.v.z = q1->w * q2->v.z + q1->v.z * q2->w   + q1->v.x * q2->v.y - q1->v.y * q2->v.x;
	return r;
}
quat quatQMultS(const quat *q, const float s){
	quat r = {.w   = q->w   * s,
	          .v.x = q->v.x * s,
	          .v.y = q->v.y * s,
	          .v.z = q->v.z * s};
	return r;
}
void quatMultQByQ1(quat *q1, const quat *q2){
	/*float prodW = q1->w * q2.w - vec3Dot(q1->v, q2.v);
	vec3 prodV = vec3VMultS(q2.v, q1->w);
	vec3AddVToV(&prodV, vec3VMultS(q1->v, q2.w));
	vec3AddVToV(&prodV, vec3Cross(q1->v, q2.v));
	q1->w = prodW; q1->v = prodV;*/
	/*
	** Calculates the Grassmann product of two quaternions.
	*/
	quat r;
	r.w   = q1->w * q2->w   - q1->v.x * q2->v.x - q1->v.y * q2->v.y - q1->v.z * q2->v.z;
	r.v.x = q1->w * q2->v.x + q1->v.x * q2->w   + q1->v.y * q2->v.z - q1->v.z * q2->v.y;
	r.v.y = q1->w * q2->v.y + q1->v.y * q2->w   + q1->v.z * q2->v.x - q1->v.x * q2->v.z;
	r.v.z = q1->w * q2->v.z + q1->v.z * q2->w   + q1->v.x * q2->v.y - q1->v.y * q2->v.x;
	*q1 = r;
}
void quatMultQByQ2(const quat *q1, quat *q2){
	/*float prodW = q1.w * q2->w - vec3Dot(q1.v, q2->v);
	vec3 prodV = vec3VMultS(q2->v, q1.w);
	vec3AddVToV(&prodV, vec3VMultS(q1.v, q2->w));
	vec3AddVToV(&prodV, vec3Cross(q1.v, q2->v));
	q2->w = prodW; q2->v = prodV;*/
	/*
	** Calculates the Grassmann product of two quaternions.
	*/
	quat r;
	r.w   = q1->w * q2->w   - q1->v.x * q2->v.x - q1->v.y * q2->v.y - q1->v.z * q2->v.z;
	r.v.x = q1->w * q2->v.x + q1->v.x * q2->w   + q1->v.y * q2->v.z - q1->v.z * q2->v.y;
	r.v.y = q1->w * q2->v.y + q1->v.y * q2->w   + q1->v.z * q2->v.x - q1->v.x * q2->v.z;
	r.v.z = q1->w * q2->v.z + q1->v.z * q2->w   + q1->v.x * q2->v.y - q1->v.y * q2->v.x;
	*q2 = r;
}
void quatMultQByQR(const quat *q1, const quat *q2, quat *r){
	/*
	** Calculates the Grassmann product of two quaternions.
	*/
	r->w   = q1->w * q2->w   - q1->v.x * q2->v.x - q1->v.y * q2->v.y - q1->v.z * q2->v.z;
	r->v.x = q1->w * q2->v.x + q1->v.x * q2->w   + q1->v.y * q2->v.z - q1->v.z * q2->v.y;
	r->v.y = q1->w * q2->v.y + q1->v.y * q2->w   + q1->v.z * q2->v.x - q1->v.x * q2->v.z;
	r->v.z = q1->w * q2->v.z + q1->v.z * q2->w   + q1->v.x * q2->v.y - q1->v.y * q2->v.x;
}
void quatMultQByS(quat *q, const float s){
	q->w *= s;
	vec3MultVByS(&q->v, s);
}

quat quatQDivQ(const quat *q1, const quat *q2){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		quat r = {.w   = q1->w   / q2->w,
		          .v.x = q1->v.x / q2->v.x,
		          .v.y = q1->v.y / q2->v.y,
		          .v.z = q1->v.z / q2->v.z};
		return r;
	}
	return quatNewS(0.f);
}
quat quatQDivS(const quat *q, const float s){
	if(s != 0.f){
		const float invS = 1.f / s;
		quat r = {.w   = q->w   * invS,
		          .v.x = q->v.x * invS,
		          .v.y = q->v.y * invS,
		          .v.z = q->v.z * invS};
		return r;
	}
	return quatNewS(0.f);
}
void quatDivQByQ1(quat *q1, const quat *q2){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		q1->w   /= q2->w;
		q1->v.x /= q2->v.x;
		q1->v.y /= q2->v.y;
		q1->v.z /= q2->v.z;
	}else{
		quatSetS(q1, 0.f);
	}
}
void quatDivQByQ2(const quat *q1, quat *q2){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		q2->w   = q1->w   / q2->w;
		q2->v.x = q1->v.x / q2->v.x;
		q2->v.y = q1->v.y / q2->v.y;
		q2->v.z = q1->v.z / q2->v.z;
	}else{
		quatSetS(q2, 0.f);
	}
}
void quatDivQByQR(const quat *q1, const quat *q2, quat *r){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		r->w   = q1->w   / q2->w;
		r->v.x = q1->v.x / q2->v.x;
		r->v.y = q1->v.y / q2->v.y;
		r->v.z = q1->v.z / q2->v.z;
	}else{
		quatSetS(r, 0.f);
	}
}
void quatDivQByS(quat *q, const float s){
	if(s != 0.f){
		const float invS = 1.f / s;
		q->w   *= invS;
		q->v.x *= invS;
		q->v.y *= invS;
		q->v.z *= invS;
	}else{
		quatSetS(q, 0.f);
	}
}

float quatGetMagnitude(const quat *q){
	return sqrtf(q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z);
}

quat quatGetConjugate(const quat *q){
	return quatNew(q->w, -q->v.x, -q->v.y, -q->v.z);
}
quat quatGetConjugateFast(const quat *q){
	return quatNew(-q->w, q->v.x, q->v.y, q->v.z);
}
void quatConjugate(quat *q){
	q->v.x = -q->v.x;
	q->v.y = -q->v.y;
	q->v.z = -q->v.z;
}
void quatConjugateFast(quat *q){
	q->w = -q->w;
}
void quatConjugateR(const quat *q, quat *r){
	quatSet(r, q->w, -q->v.x, -q->v.y, -q->v.z);
}
void quatConjugateFastR(const quat *q, quat *r){
	quatSet(r, -q->w, q->v.x, q->v.y, q->v.z);
}

quat quatGetNegative(const quat *q){
	return quatNew(-q->w, q->v.x, q->v.y, q->v.z);
}
void quatNegate(quat *q){
	q->w = -q->w;
}
void quatNegateR(const quat *q, quat *r){
	quatSet(r, -q->w, q->v.x, q->v.y, q->v.z);
}

quat quatGetInverse(const quat *q){
	quat c;
	quatConjugateFastR(q, &c);
	return quatQMultQ(q, &c);
}
void quatInvert(quat *q){
	quat c;
	quatConjugateFastR(q, &c);
	quatMultQByQ1(q, &c);
}
void quatInvertR(const quat *q, quat *r){
	quatConjugateFastR(q, r);
	quatMultQByQ2(q, r);
}

quat quatGetDifference(const quat *q1, const quat *q2){
	quat r;
	quatInvertR(q1, &r);
	quatMultQByQ1(&r, q2);
	return r;
}
void quatDifference(const quat *q1, const quat *q2, quat *r){
	quatInvertR(q1, r);
	quatMultQByQ1(r, q2);
}

quat quatGetUnit(const quat *q){
	const float magnitude = quatGetMagnitude(q);
	if(magnitude != 0.f){
		return quatQDivS(q, magnitude);
	}
	return *q;
}
quat quatGetUnitFast(const quat *q){
	const float magnitudeSquared = q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	return quatQMultS(q, invSqrt);
}
void quatNormalize(quat *q){
	const float magnitude = quatGetMagnitude(q);
	if(magnitude != 0.f){
		quatDivQByS(q, magnitude);
	}
}
void quatNormalizeFast(quat *q){
	const float magnitudeSquared = q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	quatMultQByS(q, invSqrt);
}

quat quatIdentity(){
	quat r = {.w = 1.f, .v.x = 0.f, .v.y = 0.f, .v.z = 0.f};
	return r;
}
void quatSetIdentity(quat *q){
	q->w = 1.f; q->v.x = 0.f; q->v.y = 0.f; q->v.z = 0.f;
}

void quatAxisAngle(const quat *q, float *angle, float *axisX, float *axisY, float *axisZ){
	float scale = sqrtf(1.f-q->w*q->w);  /* Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1. */
	if(scale != 0.f){  /* We don't want to risk a potential divide-by-zero error. */
		scale = 1.f/scale;
		*angle = 2.f*acosf(q->w);
		*axisX = q->v.x*scale;
		*axisY = q->v.y*scale;
		*axisZ = q->v.z*scale;
	}
}
void quatAxisAngleFast(const quat *q, float *angle, float *axisX, float *axisY, float *axisZ){
	float scale = fastInvSqrt(1.f-q->w*q->w);  /* Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1. */
	*angle = 2.f*acosf(q->w);
	*axisX = q->v.x*scale;
	*axisY = q->v.y*scale;
	*axisZ = q->v.z*scale;
}

float quatDot(const quat *q1, const quat *q2){
	return q1->w   * q2->w +
	       q1->v.x * q2->v.x +
	       q1->v.y * q2->v.y +
	       q1->v.z * q2->v.z;
}

vec3 quatGetRotatedVec3(const quat *q, const vec3 *v){

	vec3 r;

	const float dotQV = vec3Dot(&q->v, v);
	const float dotQQ = vec3Dot(&q->v, &q->v);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3Cross(&q->v, v, &crossQV);

	r.x *= m;
	r.y *= m;
	r.z *= m;

	m = 2.f * dotQV;
	r.x += m * q->v.x;
	r.y += m * q->v.y;
	r.z += m * q->v.z;

	m = 2.f * q->w;
	r.x += m * crossQV.x;
	r.y += m * crossQV.y;
	r.z += m * crossQV.z;

	return r;

}
vec3 quatGetRotatedVec3Fast(const quat *q, const vec3 *v){

	vec3 r;
	vec3 crossQV, crossQQV;
	vec3Cross(&q->v, v, &crossQV);
	crossQV.x += q->w * v->x;
	crossQV.y += q->w * v->y;
	crossQV.z += q->w * v->z;
	vec3Cross(&q->v, &crossQV, &crossQQV);

	r.x = crossQQV.x + crossQQV.x + v->x;
	r.y = crossQQV.y + crossQQV.y + v->y;
	r.z = crossQQV.z + crossQQV.z + v->z;
	return r;

}
void quatRotateVec3(const quat *q, vec3 *v){

	const float dotQV = vec3Dot(&q->v, v);
	const float dotQQ = vec3Dot(&q->v, &q->v);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3Cross(&q->v, v, &crossQV);

	v->x *= m;
	v->y *= m;
	v->z *= m;

	m = 2.f * dotQV;
	v->x += m * q->v.x;
	v->y += m * q->v.y;
	v->z += m * q->v.z;

	m = 2.f * q->w;
	v->x += m * crossQV.x;
	v->y += m * crossQV.y;
	v->z += m * crossQV.z;

}
void quatRotateVec3R(const quat *q, const vec3 *v, vec3 *r){

	const float dotQV = vec3Dot(&q->v, v);
	const float dotQQ = vec3Dot(&q->v, &q->v);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3Cross(&q->v, v, &crossQV);

	r->x = v->x * m;
	r->y = v->y * m;
	r->z = v->z * m;

	m = 2.f * dotQV;
	r->x += m * q->v.x;
	r->y += m * q->v.y;
	r->z += m * q->v.z;

	m = 2.f * q->w;
	r->x += m * crossQV.x;
	r->y += m * crossQV.y;
	r->z += m * crossQV.z;

}
void quatRotateVec3Fast(const quat *q, vec3 *v){

	vec3 crossQV, crossQQV;
	vec3Cross(&q->v, v, &crossQV);
	crossQV.x += q->w * v->x;
	crossQV.y += q->w * v->y;
	crossQV.z += q->w * v->z;
	vec3Cross(&q->v, &crossQV, &crossQQV);

	v->x = crossQQV.x + crossQQV.x + v->x;
	v->y = crossQQV.y + crossQQV.y + v->y;
	v->z = crossQQV.z + crossQQV.z + v->z;

}
void quatRotateVec3FastR(const quat *q, const vec3 *v, vec3 *r){

	vec3 crossQV, crossQQV;
	vec3Cross(&q->v, v, &crossQV);
	crossQV.x += q->w * v->x;
	crossQV.y += q->w * v->y;
	crossQV.z += q->w * v->z;
	vec3Cross(&q->v, &crossQV, &crossQQV);

	r->x = crossQQV.x + crossQQV.x + v->x;
	r->y = crossQQV.y + crossQQV.y + v->y;
	r->z = crossQQV.z + crossQQV.z + v->z;

}

quat quatLookingAt(const vec3 *eye, const vec3 *target, const vec3 *up){

	quat r;

	const float dot = vec3Dot(eye, target);

	if(fabsf(dot + 1.f) < FLT_EPSILON){

		/* Eye and target point in opposite directions, */
		/* 180 degree rotation around up vector.        */
		r.w = M_PI;
		r.v = *up;

	}else if(fabsf(dot - 1.f) < FLT_EPSILON){

		/* Eye and target are pointing in the same direction. */
		quatSetIdentity(&r);

	}else{

		r.w = acosf(dot);
		vec3Cross(eye, target, &r.v);
		vec3NormalizeFast(&r.v);

	}

	return r;

}

void quatLookAt(quat *q, const vec3 *eye, const vec3 *target, const vec3 *up){

	const float dot = vec3Dot(eye, target);

	if(fabsf(dot + 1.f) < FLT_EPSILON){

		/* Eye and target point in opposite directions, */
		/* 180 degree rotation around up vector.        */
		q->w = M_PI;
		q->v = *up;

	}else if(fabsf(dot - 1.f) < FLT_EPSILON){

		/* Eye and target are pointing in the same direction. */
		quatSetIdentity(q);

	}else{

		q->w = acosf(dot);
		vec3Cross(eye, target, &q->v);
		vec3NormalizeFast(&q->v);

	}

}

quat quatGetLerp(const quat *q1, const quat *q2, const float t){
	/*
	**               ^
	** r = (q1 + (q2 - q1) * t)
	*/
	quat r;
	r.w   = q1->w   + (q2->w   - q1->w)   * t;
	r.v.x = q1->v.x + (q2->v.x - q1->v.x) * t;
	r.v.y = q1->v.y + (q2->v.y - q1->v.y) * t;
	r.v.z = q1->v.z + (q2->v.z - q1->v.z) * t;
	return r;
}
void quatLerp(const quat *q1, const quat *q2, const float t, quat *r){
	/*
	**               ^
	** r = (q1 + (q2 - q1) * t)
	*/
	r->w   = q1->w   + (q2->w   - q1->w)   * t;
	r->v.x = q1->v.x + (q2->v.x - q1->v.x) * t;
	r->v.y = q1->v.y + (q2->v.y - q1->v.y) * t;
	r->v.z = q1->v.z + (q2->v.z - q1->v.z) * t;
}
quat quatGetSlerp(const quat *q1, const quat *q2, const float t){

	quat r;

	/* Cosine of the angle between the two quaternions. */
	const float cosTheta = quatDot(q1, q2);
	const float cosThetaAbs = fabs(cosTheta);

	if(cosThetaAbs > QUAT_LERP_ANGLE){
		/* If the angle is small enough, we can just use linear interpolation. */
		quatLerp(q1, q2, t, &r);
	}else{

		/*
		** sin(x)^2 + cos(x)^2 = 1
		** sin(x)^2 = 1 - cos(x)^2
		** 1 / sin(x) = fastInvSqrt(1 - cos(x)^2)
		**
		** Calculating the reciprocal of sin(x) allows us to do
		** multiplications instead of divisions below, as the
		** following holds true:
		**
		** x * (1 / y) = x / y
		*/

		const float theta = acosf(cosThetaAbs);
		const float sinThetaInv = fastInvSqrt(1.f - cosThetaAbs * cosThetaAbs);
		const float sinThetaInvT = sinf(theta * (1.f - t)) * sinThetaInv;

		/* If q1 and q2 are > 90 degrees apart (cosTheta < 0), negate */
		/* sinThetaT so it doesn't go the long way around.            */
		float sinThetaT;
		if(cosTheta >= 0.f){
			sinThetaT = sinf(theta * t) * sinThetaInv;
		}else{
			sinThetaT = -(sinf(theta * t) * sinThetaInv);
		}

		r.w   = q1->w   * sinThetaInvT + q2->w   * sinThetaT;
		r.v.x = q1->v.x * sinThetaInvT + q2->v.x * sinThetaT;
		r.v.y = q1->v.y * sinThetaInvT + q2->v.y * sinThetaT;
		r.v.z = q1->v.z * sinThetaInvT + q2->v.z * sinThetaT;

	}

	quatNormalizeFast(&r);
	return r;

}
void quatSlerp(const quat *q1, const quat *q2, const float t, quat *r){

	/* Cosine of the angle between the two quaternions. */
	const float cosTheta = quatDot(q1, q2);
	const float cosThetaAbs = fabs(cosTheta);

	if(cosThetaAbs > QUAT_LERP_ANGLE){
		/* If the angle is small enough, we can just use linear interpolation. */
		quatLerp(q1, q2, t, r);
	}else{

		/*
		** sin(x)^2 + cos(x)^2 = 1
		** sin(x)^2 = 1 - cos(x)^2
		** 1 / sin(x) = fastInvSqrt(1 - cos(x)^2)
		**
		** Calculating the reciprocal of sin(x) allows us to do
		** multiplications instead of divisions below, as the
		** following holds true:
		**
		** x * (1 / y) = x / y
		*/

		const float theta = acosf(cosThetaAbs);
		const float sinThetaInv = fastInvSqrt(1.f - cosThetaAbs * cosThetaAbs);
		const float sinThetaInvT = sinf(theta * (1.f - t)) * sinThetaInv;

		/* If q1 and q2 are > 90 degrees apart (cosTheta < 0), negate */
		/* sinThetaT so it doesn't go the long way around.            */
		float sinThetaT;
		if(cosTheta >= 0.f){
			sinThetaT = sinf(theta * t) * sinThetaInv;
		}else{
			sinThetaT = -(sinf(theta * t) * sinThetaInv);
		}

		r->w   = q1->w   * sinThetaInvT + q2->w   * sinThetaT;
		r->v.x = q1->v.x * sinThetaInvT + q2->v.x * sinThetaT;
		r->v.y = q1->v.y * sinThetaInvT + q2->v.y * sinThetaT;
		r->v.z = q1->v.z * sinThetaInvT + q2->v.z * sinThetaT;

	}

	quatNormalizeFast(r);

}

void quatRotate(const quat *q1, const quat *q2, const float t, quat *r){
	quat temp;
	quatMultQByQR(q1, q2, &temp);
	/* *r = temp; */
	quatSlerp(q1, &temp, t, r);
}
