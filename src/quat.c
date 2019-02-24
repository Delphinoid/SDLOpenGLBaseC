#include "quat.h"
#include "helpersMath.h"
#include "constantsMath.h"
#include "inline.h"
#include <math.h>
#include <float.h>
#include <string.h>

#define QUAT_LERP_ANGLE cos(RADIAN_RATIO)

__HINT_INLINE__ quat quatNew(const float w, const float x, const float y, const float z){
	quat r = {.w = w, .v.x = x, .v.y = y, .v.z = z};
	return r;
}
__HINT_INLINE__ quat quatNewS(const float s){
	quat r = {.w = s, .v.x = s, .v.y = s, .v.z = s};
	return r;
}
__HINT_INLINE__ quat quatNewAxisAngle(const float angle, const float axisX, const float axisY, const float axisZ){
	float t = sinf(angle/2.f);
	quat r;
	r.w   = cosf(angle/2.f);
	r.v.x = axisX * t;
	r.v.y = axisY * t;
	r.v.z = axisZ * t;
	return r;
}
__HINT_INLINE__ quat quatNewEuler(const float x, const float y, const float z){
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
__HINT_INLINE__ void quatSet(quat *const restrict q, const float w, const float x, const float y, const float z){
	q->w = w; q->v.x = x; q->v.y = y; q->v.z = z;
}
__HINT_INLINE__ void quatSetS(quat *const restrict q, const float s){
	q->w = s; q->v.x = s; q->v.y = s; q->v.z = s;
}
__HINT_INLINE__ void quatSetAxisAngle(quat *const restrict q, const float angle, const float axisX, const float axisY, const float axisZ){
	const float t = sinf(angle/2.f);
	q->w = cosf(angle/2.f);
	q->v.x = axisX * t;
	q->v.y = axisY * t;
	q->v.z = axisZ * t;
}
__HINT_INLINE__ void quatSetEuler(quat *const restrict q, const float x, const float y, const float z){
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
void quatZero(quat *const restrict q){
	memset(q, 0, sizeof(quat));
}

__HINT_INLINE__ quat quatQAddQ(const quat *const restrict q1, const quat *const restrict q2){
	quat r = {.w   = q1->w   + q2->w,
	          .v.x = q1->v.x + q2->v.x,
	          .v.y = q1->v.y + q2->v.y,
	          .v.z = q1->v.z + q2->v.z};
	return r;
}
__HINT_INLINE__ quat quatQAddS(const quat *const restrict q, const float s){
	quat r = {.w   = q->w   + s,
	          .v.x = q->v.x + s,
	          .v.y = q->v.y + s,
	          .v.z = q->v.z + s};
	return r;
}
__HINT_INLINE__ quat quatQAddW(const quat *const restrict q, const float w){
	quat r = {.w   = q->w + w,
	          .v.x = q->v.x,
	          .v.y = q->v.y,
	          .v.z = q->v.z};
	return r;
}
__HINT_INLINE__ void quatAddQToQ(quat *const restrict q1, const quat *const restrict q2){
	q1->w += q2->w; q1->v.x += q2->v.x; q1->v.y += q2->v.y; q1->v.z += q2->v.z;
}
__HINT_INLINE__ void quatAddQToQR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r){
	r->w = q1->w + q2->w; r->v.x = q1->v.x + q2->v.x; r->v.y = q1->v.y + q2->v.y; r->v.z = q1->v.z + q2->v.z;
}
__HINT_INLINE__ void quatAddSToQ(quat *const restrict q, const float s){
	q->w += s; q->v.x += s; q->v.y += s; q->v.z += s;
}
__HINT_INLINE__ void quatAddWToQ(quat *const restrict q, const float w){
	q->w += w;
}

__HINT_INLINE__ quat quatQSubQ(const quat *const restrict q1, const quat *const restrict q2){
	quat r = {.w   = q1->w   - q2->w,
	          .v.x = q1->v.x - q2->v.x,
	          .v.y = q1->v.y - q2->v.y,
	          .v.z = q1->v.z - q2->v.z};
	return r;
}
__HINT_INLINE__ quat quatQSubS(const quat *const restrict q, const float s){
	quat r = {.w   = q->w   - s,
	          .v.x = q->v.x - s,
	          .v.y = q->v.y - s,
	          .v.z = q->v.z - s};
	return r;
}
__HINT_INLINE__ quat quatQSubW(const quat *const restrict q, const float w){
	quat r = {.w   = q->w - w,
	          .v.x = q->v.x,
	          .v.y = q->v.y,
	          .v.z = q->v.z};
	return r;
}
__HINT_INLINE__ void quatSubQFromQ1(quat *const restrict q1, const quat *const restrict q2){
	q1->w -= q2->w; q1->v.x -= q2->v.x; q1->v.y -= q2->v.y; q1->v.z -= q2->v.z;
}
__HINT_INLINE__ void quatSubQFromQ2(const quat *const restrict q1, quat *const restrict q2){
	q2->w = q1->w - q2->w; q2->v.x = q1->v.x - q2->v.x; q2->v.y = q1->v.y - q2->v.y; q2->v.z = q1->v.z - q2->v.z;
}
__HINT_INLINE__ void quatSubQFromQR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r){
	r->w = q1->w - q2->w; r->v.x = q1->v.x - q2->v.x; r->v.y = q1->v.y - q2->v.y; r->v.z = q1->v.z - q2->v.z;
}
__HINT_INLINE__ void quatSubSFromQ(quat *const restrict q, const float s){
	q->w -= s; q->v.x -= s; q->v.y -= s; q->v.z -= s;
}
__HINT_INLINE__ void quatSubWFromQ(quat *const restrict q, const float w){
	q->w -= w;
}

__HINT_INLINE__ quat quatQMultQ(const quat *const restrict q1, const quat *const restrict q2){
	/*float prodW = q1.w * q2.w - vec3Dot(q1.v, q2.v);
	vec3 prodV = vec3VMultS(q2.v, q1.w);
	vec3AddVToV(&prodV, vec3VMultS(q1.v, q2.w));
	vec3AddVToV(&prodV, vec3CrossR(q1.v, q2.v));
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
__HINT_INLINE__ quat quatQMultS(const quat *const restrict q, const float s){
	quat r = {.w   = q->w   * s,
	          .v.x = q->v.x * s,
	          .v.y = q->v.y * s,
	          .v.z = q->v.z * s};
	return r;
}
__HINT_INLINE__ void quatMultQByQ1(quat *const restrict q1, const quat *const restrict q2){
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
__HINT_INLINE__ void quatMultQByQ2(const quat *const restrict q1, quat *const restrict q2){
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
__HINT_INLINE__ void quatMultQByQR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r){
	/*
	** Calculates the Grassmann product of two quaternions.
	*/
	r->w   = q1->w * q2->w   - q1->v.x * q2->v.x - q1->v.y * q2->v.y - q1->v.z * q2->v.z;
	r->v.x = q1->w * q2->v.x + q1->v.x * q2->w   + q1->v.y * q2->v.z - q1->v.z * q2->v.y;
	r->v.y = q1->w * q2->v.y + q1->v.y * q2->w   + q1->v.z * q2->v.x - q1->v.x * q2->v.z;
	r->v.z = q1->w * q2->v.z + q1->v.z * q2->w   + q1->v.x * q2->v.y - q1->v.y * q2->v.x;
}
__HINT_INLINE__ void quatMultQByS(quat *const restrict q, const float s){
	q->w *= s;
	vec3MultVByS(&q->v, s);
}

__HINT_INLINE__ quat quatQDivQ(const quat *const restrict q1, const quat *const restrict q2){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		quat r = {.w   = q1->w   / q2->w,
		          .v.x = q1->v.x / q2->v.x,
		          .v.y = q1->v.y / q2->v.y,
		          .v.z = q1->v.z / q2->v.z};
		return r;
	}
	return quatNewS(0.f);
}
__HINT_INLINE__ quat quatQDivS(const quat *const restrict q, const float s){
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
__HINT_INLINE__ void quatDivQByQ1(quat *const restrict q1, const quat *const restrict q2){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		q1->w   /= q2->w;
		q1->v.x /= q2->v.x;
		q1->v.y /= q2->v.y;
		q1->v.z /= q2->v.z;
	}else{
		quatZero(q1);
	}
}
__HINT_INLINE__ void quatDivQByQ2(const quat *const restrict q1, quat *const restrict q2){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		q2->w   = q1->w   / q2->w;
		q2->v.x = q1->v.x / q2->v.x;
		q2->v.y = q1->v.y / q2->v.y;
		q2->v.z = q1->v.z / q2->v.z;
	}else{
		quatZero(q2);
	}
}
__HINT_INLINE__ void quatDivQByQR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		r->w   = q1->w   / q2->w;
		r->v.x = q1->v.x / q2->v.x;
		r->v.y = q1->v.y / q2->v.y;
		r->v.z = q1->v.z / q2->v.z;
	}else{
		quatZero(r);
	}
}
__HINT_INLINE__ void quatDivQByS(quat *const restrict q, const float s){
	if(s != 0.f){
		const float invS = 1.f / s;
		q->w   *= invS;
		q->v.x *= invS;
		q->v.y *= invS;
		q->v.z *= invS;
	}else{
		quatZero(q);
	}
}

__HINT_INLINE__ float quatGetMagnitude(const quat *const restrict q){
	return sqrtf(q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z);
}

__HINT_INLINE__ quat quatGetConjugate(const quat *const restrict q){
	return quatNew(q->w, -q->v.x, -q->v.y, -q->v.z);
}
__HINT_INLINE__ quat quatGetConjugateFast(const quat *const restrict q){
	return quatNew(-q->w, q->v.x, q->v.y, q->v.z);
}
__HINT_INLINE__ void quatConjugate(quat *const restrict q){
	q->v.x = -q->v.x;
	q->v.y = -q->v.y;
	q->v.z = -q->v.z;
}
__HINT_INLINE__ void quatConjugateFast(quat *const restrict q){
	q->w = -q->w;
}
__HINT_INLINE__ void quatConjugateR(const quat *const restrict q, quat *const restrict r){
	quatSet(r, q->w, -q->v.x, -q->v.y, -q->v.z);
}
__HINT_INLINE__ void quatConjugateFastR(const quat *const restrict q, quat *const restrict r){
	quatSet(r, -q->w, q->v.x, q->v.y, q->v.z);
}

__HINT_INLINE__ quat quatGetNegative(const quat *const restrict q){
	return quatNew(-q->w, q->v.x, q->v.y, q->v.z);
}
__HINT_INLINE__ void quatNegate(quat *const restrict q){
	q->w = -q->w;
}
__HINT_INLINE__ void quatNegateR(const quat *const restrict q, quat *const restrict r){
	quatSet(r, -q->w, q->v.x, q->v.y, q->v.z);
}

__HINT_INLINE__ quat quatGetInverse(const quat *const restrict q){
	quat c;
	quatConjugateFastR(q, &c);
	return quatQMultQ(q, &c);
}
__HINT_INLINE__ void quatInvert(quat *const restrict q){
	quat c;
	quatConjugateFastR(q, &c);
	quatMultQByQ1(q, &c);
}
__HINT_INLINE__ void quatInvertR(const quat *const restrict q, quat *const restrict r){
	quatConjugateFastR(q, r);
	quatMultQByQ2(q, r);
}

__HINT_INLINE__ quat quatGetDifference(const quat *const restrict q1, const quat *const restrict q2){
	quat r;
	quatInvertR(q1, &r);
	quatMultQByQ1(&r, q2);
	return r;
}
__HINT_INLINE__ void quatDifference(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r){
	quatInvertR(q1, r);
	quatMultQByQ1(r, q2);
}

__HINT_INLINE__ quat quatGetUnit(const quat *const restrict q){
	const float magnitude = quatGetMagnitude(q);
	if(magnitude != 0.f){
		return quatQDivS(q, magnitude);
	}
	return *q;
}
__HINT_INLINE__ quat quatGetUnitFast(const quat *const restrict q){
	const float magnitudeSquared = q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	return quatQMultS(q, invSqrt);
}
__HINT_INLINE__ void quatNormalize(quat *const restrict q){
	const float magnitude = quatGetMagnitude(q);
	if(magnitude != 0.f){
		quatDivQByS(q, magnitude);
	}
}
__HINT_INLINE__ void quatNormalizeFast(quat *const restrict q){
	const float magnitudeSquared = q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	quatMultQByS(q, invSqrt);
}

__HINT_INLINE__ quat quatIdentity(){
	quat r = {.w = 1.f, .v.x = 0.f, .v.y = 0.f, .v.z = 0.f};
	return r;
}
__HINT_INLINE__ void quatSetIdentity(quat *const restrict q){
	q->w = 1.f; q->v.x = 0.f; q->v.y = 0.f; q->v.z = 0.f;
}

__HINT_INLINE__ void quatAxisAngle(const quat *const restrict q, float *angle, float *axisX, float *axisY, float *axisZ){
	float scale = sqrtf(1.f-q->w*q->w);  // Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
	if(scale != 0.f){  // We don't want to risk a potential divide-by-zero error.
		scale = 1.f/scale;
		*angle = 2.f*acosf(q->w);
		*axisX = q->v.x*scale;
		*axisY = q->v.y*scale;
		*axisZ = q->v.z*scale;
	}
}
__HINT_INLINE__ void quatAxisAngleFast(const quat *const restrict q, float *angle, float *axisX, float *axisY, float *axisZ){
	float scale = fastInvSqrt(1.f-q->w*q->w);  // Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
	*angle = 2.f*acosf(q->w);
	*axisX = q->v.x*scale;
	*axisY = q->v.y*scale;
	*axisZ = q->v.z*scale;
}

__HINT_INLINE__ float quatDot(const quat *const restrict q1, const quat *const restrict q2){
	return q1->w   * q2->w +
	       q1->v.x * q2->v.x +
	       q1->v.y * q2->v.y +
	       q1->v.z * q2->v.z;
}

__HINT_INLINE__ vec3 quatGetRotatedVec3(const quat *const restrict q, const vec3 *const restrict v){

	vec3 r, temp;

	const float dotQV = vec3Dot(&q->v, v);
	const float dotQQ = vec3Dot(&q->v, &q->v);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3CrossR(&q->v, v, &crossQV);

	vec3MultVBySR(v, m, &r);

	m = 2.f * dotQV;
	vec3MultVBySR(&q->v, m, &temp);
	vec3AddVToV(&r, &temp);

	m = 2.f * q->w;
	vec3MultVBySR(&crossQV, m, &temp);
	vec3AddVToV(&r, &temp);

	return r;

}
__HINT_INLINE__ vec3 quatGetRotatedVec3Fast(const quat *const restrict q, const vec3 *const restrict v){

	vec3 r;
	vec3 crossQV, crossQQV;

	vec3CrossR(&q->v, v, &crossQV);
	crossQV.x += q->w * v->x;
	crossQV.y += q->w * v->y;
	crossQV.z += q->w * v->z;
	vec3CrossR(&q->v, &crossQV, &crossQQV);

	r.x = crossQQV.x + crossQQV.x + v->x;
	r.y = crossQQV.y + crossQQV.y + v->y;
	r.z = crossQQV.z + crossQQV.z + v->z;

	return r;

}
__HINT_INLINE__ void quatRotateVec3(const quat *const restrict q, vec3 *const restrict v){

	vec3 temp;

	const float dotQV = vec3Dot(&q->v, v);
	const float dotQQ = vec3Dot(&q->v, &q->v);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3CrossR(&q->v, v, &crossQV);

	vec3MultVByS(v, m);

	m = 2.f * dotQV;
	vec3MultVBySR(&q->v, m, &temp);
	vec3AddVToV(v, &temp);

	m = 2.f * q->w;
	vec3MultVBySR(&crossQV, m, &temp);
	vec3AddVToV(v, &temp);

}
__HINT_INLINE__ void quatRotateVec3R(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict r){

	vec3 temp;

	const float dotQV = vec3Dot(&q->v, v);
	const float dotQQ = vec3Dot(&q->v, &q->v);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3CrossR(&q->v, v, &crossQV);

	vec3MultVBySR(v, m, r);

	m = 2.f * dotQV;
	vec3MultVBySR(&q->v, m, &temp);
	vec3AddVToV(r, &temp);

	m = 2.f * q->w;
	vec3MultVBySR(&crossQV, m, &temp);
	vec3AddVToV(r, &temp);

}
__HINT_INLINE__ void quatRotateVec3Fast(const quat *const restrict q, vec3 *const restrict v){

	vec3 crossQV, crossQQV;
	vec3CrossR(&q->v, v, &crossQV);

	crossQV.x += q->w * v->x;
	crossQV.y += q->w * v->y;
	crossQV.z += q->w * v->z;
	vec3CrossR(&q->v, &crossQV, &crossQQV);

	v->x = crossQQV.x + crossQQV.x + v->x;
	v->y = crossQQV.y + crossQQV.y + v->y;
	v->z = crossQQV.z + crossQQV.z + v->z;

}
__HINT_INLINE__ void quatRotateVec3FastR(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict r){

	vec3 crossQV, crossQQV;
	vec3CrossR(&q->v, v, &crossQV);

	crossQV.x += q->w * v->x;
	crossQV.y += q->w * v->y;
	crossQV.z += q->w * v->z;
	vec3CrossR(&q->v, &crossQV, &crossQQV);

	r->x = crossQQV.x + crossQQV.x + v->x;
	r->y = crossQQV.y + crossQQV.y + v->y;
	r->z = crossQQV.z + crossQQV.z + v->z;

}

__HINT_INLINE__ quat quatLookingAt(const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict up){

	quat r;

	const float dot = vec3Dot(eye, target);

	if(fabsf(dot + 1.f) < FLT_EPSILON){

		// Eye and target point in opposite directions,
		// 180 degree rotation around up vector.
		r.w = M_PI;
		r.v = *up;

	}else if(fabsf(dot - 1.f) < FLT_EPSILON){

		// Eye and target are pointing in the same direction.
		quatSetIdentity(&r);

	}else{

		r.w = acosf(dot);
		vec3CrossR(eye, target, &r.v);
		vec3NormalizeFast(&r.v);

	}

	return r;

}

__HINT_INLINE__ void quatLookAt(quat *const restrict q, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict up){

	const float dot = vec3Dot(eye, target);

	if(fabsf(dot + 1.f) < FLT_EPSILON){

		// Eye and target point in opposite directions,
		// 180 degree rotation around up vector.
		q->w = M_PI;
		q->v = *up;

	}else if(fabsf(dot - 1.f) < FLT_EPSILON){

		// Eye and target are pointing in the same direction.
		quatSetIdentity(q);

	}else{

		q->w = acosf(dot);
		vec3CrossR(eye, target, &q->v);
		vec3NormalizeFast(&q->v);

	}

}

__HINT_INLINE__ quat quatGetLerp(const quat *const restrict q1, const quat *const restrict q2, const float t){
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
__HINT_INLINE__ void quatLerp1(quat *const restrict q1, const quat *const restrict q2, const float t){
	/*
	**               ^
	** r = (q1 + (q2 - q1) * t)
	*/
	q1->w   += (q2->w   - q1->w)   * t;
	q1->v.x += (q2->v.x - q1->v.x) * t;
	q1->v.y += (q2->v.y - q1->v.y) * t;
	q1->v.z += (q2->v.z - q1->v.z) * t;
}
__HINT_INLINE__ void quatLerp2(const quat *const restrict q1, quat *const restrict q2, const float t){
	/*
	**               ^
	** r = (q1 + (q2 - q1) * t)
	*/
	q2->w   = q1->w   + (q2->w   - q1->w)   * t;
	q2->v.x = q1->v.x + (q2->v.x - q1->v.x) * t;
	q2->v.y = q1->v.y + (q2->v.y - q1->v.y) * t;
	q2->v.z = q1->v.z + (q2->v.z - q1->v.z) * t;
}
__HINT_INLINE__ void quatLerpR(const quat *const restrict q1, const quat *const restrict q2, const float t, quat *const restrict r){
	/*
	**               ^
	** r = (q1 + (q2 - q1) * t)
	*/
	r->w   = q1->w   + (q2->w   - q1->w)   * t;
	r->v.x = q1->v.x + (q2->v.x - q1->v.x) * t;
	r->v.y = q1->v.y + (q2->v.y - q1->v.y) * t;
	r->v.z = q1->v.z + (q2->v.z - q1->v.z) * t;
}

__HINT_INLINE__ quat quatGetSlerp(const quat *const restrict q1, const quat *const restrict q2, const float t){

	quat r;

	// Cosine of the angle between the two quaternions.
	const float cosTheta = quatDot(q1, q2);
	const float cosThetaAbs = fabs(cosTheta);

	if(cosThetaAbs > QUAT_LERP_ANGLE){
		// If the angle is small enough, we can just use linear interpolation.
		quatLerpR(q1, q2, t, &r);
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

		// If q1 and q2 are > 90 degrees apart (cosTheta < 0), negate
		// sinThetaT so it doesn't go the long way around.
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
__HINT_INLINE__ void quatSlerp1(quat *const restrict q1, const quat *const restrict q2, const float t){

	// Cosine of the angle between the two quaternions.
	const float cosTheta = quatDot(q1, q2);
	const float cosThetaAbs = fabs(cosTheta);

	if(cosThetaAbs > QUAT_LERP_ANGLE){
		// If the angle is small enough, we can just use linear interpolation.
		quatLerp1(q1, q2, t);
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

		// If q1 and q2 are > 90 degrees apart (cosTheta < 0), negate
		// sinThetaT so it doesn't go the long way around.
		float sinThetaT;
		if(cosTheta >= 0.f){
			sinThetaT = sinf(theta * t) * sinThetaInv;
		}else{
			sinThetaT = -(sinf(theta * t) * sinThetaInv);
		}

		q1->w   = q1->w   * sinThetaInvT + q2->w   * sinThetaT;
		q1->v.x = q1->v.x * sinThetaInvT + q2->v.x * sinThetaT;
		q1->v.y = q1->v.y * sinThetaInvT + q2->v.y * sinThetaT;
		q1->v.z = q1->v.z * sinThetaInvT + q2->v.z * sinThetaT;

	}

	quatNormalizeFast(q1);

}
__HINT_INLINE__ void quatSlerp2(const quat *const restrict q1, quat *const restrict q2, const float t){

	// Cosine of the angle between the two quaternions.
	const float cosTheta = quatDot(q1, q2);
	const float cosThetaAbs = fabs(cosTheta);

	if(cosThetaAbs > QUAT_LERP_ANGLE){
		// If the angle is small enough, we can just use linear interpolation.
		quatLerp2(q1, q2, t);
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

		// If q1 and q2 are > 90 degrees apart (cosTheta < 0), negate
		// sinThetaT so it doesn't go the long way around.
		float sinThetaT;
		if(cosTheta >= 0.f){
			sinThetaT = sinf(theta * t) * sinThetaInv;
		}else{
			sinThetaT = -(sinf(theta * t) * sinThetaInv);
		}

		q2->w   = q1->w   * sinThetaInvT + q2->w   * sinThetaT;
		q2->v.x = q1->v.x * sinThetaInvT + q2->v.x * sinThetaT;
		q2->v.y = q1->v.y * sinThetaInvT + q2->v.y * sinThetaT;
		q2->v.z = q1->v.z * sinThetaInvT + q2->v.z * sinThetaT;

	}

	quatNormalizeFast(q2);

}
__HINT_INLINE__ void quatSlerpR(const quat *const restrict q1, const quat *const restrict q2, const float t, quat *const restrict r){

	// Cosine of the angle between the two quaternions.
	const float cosTheta = quatDot(q1, q2);
	const float cosThetaAbs = fabs(cosTheta);

	if(cosThetaAbs > QUAT_LERP_ANGLE){
		// If the angle is small enough, we can just use linear interpolation.
		quatLerpR(q1, q2, t, r);
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

		// If q1 and q2 are > 90 degrees apart (cosTheta < 0), negate
		// sinThetaT so it doesn't go the long way around.
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

__HINT_INLINE__ void quatDifferentiate(quat *const restrict q, const vec3 *const restrict w){
	quat r;
	quatSet(&r, 0.f, w->x * 0.5f, w->y * 0.5f, w->z * 0.5f);
	quatMultQByQ2(&r, q);
}

__HINT_INLINE__ void quatDifferentiateR(const quat *const restrict q, const vec3 *const restrict w, quat *const restrict r){
	quatSet(r, 0.f, w->x * 0.5f, w->y * 0.5f, w->z * 0.5f);
	quatMultQByQ1(r, q);
}

__HINT_INLINE__ void quatIntegrate(quat *const restrict q, const vec3 *const restrict w, float dt){
	quat r = {.w = 0.f, .v = *w};
	quatMultQByQ1(&r, q);
	dt *= 0.5f;
	q->w   += r.w   * dt;
	q->v.x += r.v.x * dt;
	q->v.y += r.v.y * dt;
	q->v.z += r.v.z * dt;
}

__HINT_INLINE__ void quatIntegrateR(const quat *const restrict q, const vec3 *const restrict w, float dt, quat *const restrict r){
	r->w = 0.f;
	r->v = *w;
	quatMultQByQ1(r, q);
	dt *= 0.5f;
	r->w   = r->w   * dt + q->w;
	r->v.x = r->v.x * dt + q->v.x;
	r->v.y = r->v.y * dt + q->v.y;
	r->v.z = r->v.z * dt + q->v.z;
}

__HINT_INLINE__ void quatRotateR(const quat *const restrict q1, const quat *const restrict q2, const float t, quat *const restrict r){
	quat temp;
	quatMultQByQR(q1, q2, &temp);
	// *r = temp;
	quatSlerpR(q1, &temp, t, r);
}
