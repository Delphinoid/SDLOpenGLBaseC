#include "quat.h"
#include "helpersMath.h"
#include "constantsMath.h"
#include "inline.h"
#include <math.h>
#include <float.h>
#include <string.h>

#define QUAT_LERP_ANGLE cos(RADIAN_RATIO)

__HINT_INLINE__ quat quatNew(const float w, const float x, const float y, const float z){
	const quat r = {.w = w, .v.x = x, .v.y = y, .v.z = z};
	return r;
}
__HINT_INLINE__ quat quatNewS(const float s){
	const quat r = {.w = s, .v.x = s, .v.y = s, .v.z = s};
	return r;
}
__HINT_INLINE__ quat quatNewAxisAngle(const float angle, const float axisX, const float axisY, const float axisZ){
	const float t = sinf(angle/2.f);
	const quat r = {
		.w   = cosf(angle/2.f),
		.v.x = axisX * t,
		.v.y = axisY * t,
		.v.z = axisZ * t
	};
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
	const quat r = {
		.w   = cx*cy*cz+sx*sy*sz,
		.v.x = sx*cy*cz-cx*sy*sz,
		.v.y = cx*sy*cz+sx*cy*sz,
		.v.z = cx*cy*sz-sx*sy*cz
	};
	return r;
}
quat quatZero(){
	const quat r = {.w = 0.f, .v.x = 0.f, .v.y = 0.f, .v.z = 0.f};
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
void quatZeroP(quat *const restrict q){
	memset(q, 0, sizeof(quat));
}

__HINT_INLINE__ quat quatQAddQ(const quat q1, const quat q2){
	const quat r = {.w   = q1.w   + q2.w,
	                .v.x = q1.v.x + q2.v.x,
	                .v.y = q1.v.y + q2.v.y,
	                .v.z = q1.v.z + q2.v.z};
	return r;
}
__HINT_INLINE__ quat quatQAddS(const quat q, const float s){
	const quat r = {.w   = q.w   + s,
	                .v.x = q.v.x + s,
	                .v.y = q.v.y + s,
	                .v.z = q.v.z + s};
	return r;
}
__HINT_INLINE__ quat quatQAddW(const quat q, const float w){
	const quat r = {.w   = q.w + w,
	                .v.x = q.v.x,
	                .v.y = q.v.y,
	                .v.z = q.v.z};
	return r;
}
__HINT_INLINE__ void quatQAddQP(quat *const restrict q1, const quat *const restrict q2){
	q1->w += q2->w; q1->v.x += q2->v.x; q1->v.y += q2->v.y; q1->v.z += q2->v.z;
}
__HINT_INLINE__ void quatQAddQPR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r){
	r->w = q1->w + q2->w; r->v.x = q1->v.x + q2->v.x; r->v.y = q1->v.y + q2->v.y; r->v.z = q1->v.z + q2->v.z;
}
__HINT_INLINE__ void quatQAddSP(quat *const restrict q, const float s){
	q->w += s; q->v.x += s; q->v.y += s; q->v.z += s;
}
__HINT_INLINE__ void quatQAddWP(quat *const restrict q, const float w){
	q->w += w;
}

__HINT_INLINE__ quat quatQSubQ(const quat q1, const quat q2){
	const quat r = {.w   = q1.w   - q2.w,
	                .v.x = q1.v.x - q2.v.x,
	                .v.y = q1.v.y - q2.v.y,
	                .v.z = q1.v.z - q2.v.z};
	return r;
}
__HINT_INLINE__ quat quatQSubS(const quat q, const float s){
	const quat r = {.w   = q.w   - s,
	                .v.x = q.v.x - s,
	                .v.y = q.v.y - s,
	                .v.z = q.v.z - s};
	return r;
}
__HINT_INLINE__ quat quatQSubW(const quat q, const float w){
	const quat r = {.w   = q.w - w,
	                .v.x = q.v.x,
	                .v.y = q.v.y,
	                .v.z = q.v.z};
	return r;
}
__HINT_INLINE__ void quatQSubQP1(quat *const restrict q1, const quat *const restrict q2){
	q1->w -= q2->w; q1->v.x -= q2->v.x; q1->v.y -= q2->v.y; q1->v.z -= q2->v.z;
}
__HINT_INLINE__ void quatQSubQP2(const quat *const restrict q1, quat *const restrict q2){
	q2->w = q1->w - q2->w; q2->v.x = q1->v.x - q2->v.x; q2->v.y = q1->v.y - q2->v.y; q2->v.z = q1->v.z - q2->v.z;
}
__HINT_INLINE__ void quatQSubQPR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r){
	r->w = q1->w - q2->w; r->v.x = q1->v.x - q2->v.x; r->v.y = q1->v.y - q2->v.y; r->v.z = q1->v.z - q2->v.z;
}
__HINT_INLINE__ void quatQSubSP(quat *const restrict q, const float s){
	q->w -= s; q->v.x -= s; q->v.y -= s; q->v.z -= s;
}
__HINT_INLINE__ void quatQSubWP(quat *const restrict q, const float w){
	q->w -= w;
}

__HINT_INLINE__ quat quatQMultQ(const quat q1, const quat q2){
	/*float prodW = q1.w * q2.w - vec3Dot(q1.v, q2.v);
	vec3 prodV = vec3VMultS(q2.v, q1.w);
	vec3AddVToV(&prodV, vec3VMultS(q1.v, q2.w));
	vec3AddVToV(&prodV, vec3CrossR(q1.v, q2.v));
	return quatNew(prodW, prodV.x, prodV.y, prodV.z);*/
	/*
	** Calculates the Grassmann product of two quaternions.
	*/
	const quat r = {.w   = q1.w * q2.w   - q1.v.x * q2.v.x - q1.v.y * q2.v.y - q1.v.z * q2.v.z,
	                .v.x = q1.w * q2.v.x + q1.v.x * q2.w   + q1.v.y * q2.v.z - q1.v.z * q2.v.y,
	                .v.y = q1.w * q2.v.y + q1.v.y * q2.w   + q1.v.z * q2.v.x - q1.v.x * q2.v.z,
	                .v.z = q1.w * q2.v.z + q1.v.z * q2.w   + q1.v.x * q2.v.y - q1.v.y * q2.v.x};
	return r;
}
__HINT_INLINE__ quat quatQMultS(const quat q, const float s){
	const quat r = {.w   = q.w   * s,
	                .v.x = q.v.x * s,
	                .v.y = q.v.y * s,
	                .v.z = q.v.z * s};
	return r;
}
__HINT_INLINE__ void quatQMultQP1(quat *const restrict q1, const quat *const restrict q2){
	/*
	** Calculates the Grassmann product of two quaternions.
	*/
	const quat r = {.w   = q1->w * q2->w   - q1->v.x * q2->v.x - q1->v.y * q2->v.y - q1->v.z * q2->v.z,
	                .v.x = q1->w * q2->v.x + q1->v.x * q2->w   + q1->v.y * q2->v.z - q1->v.z * q2->v.y,
	                .v.y = q1->w * q2->v.y + q1->v.y * q2->w   + q1->v.z * q2->v.x - q1->v.x * q2->v.z,
	                .v.z = q1->w * q2->v.z + q1->v.z * q2->w   + q1->v.x * q2->v.y - q1->v.y * q2->v.x};
	*q1 = r;
}
__HINT_INLINE__ void quatQMultQP2(const quat *const restrict q1, quat *const restrict q2){
	/*
	** Calculates the Grassmann product of two quaternions.
	*/
	const quat r = {.w   = q1->w * q2->w   - q1->v.x * q2->v.x - q1->v.y * q2->v.y - q1->v.z * q2->v.z,
	                .v.x = q1->w * q2->v.x + q1->v.x * q2->w   + q1->v.y * q2->v.z - q1->v.z * q2->v.y,
	                .v.y = q1->w * q2->v.y + q1->v.y * q2->w   + q1->v.z * q2->v.x - q1->v.x * q2->v.z,
	                .v.z = q1->w * q2->v.z + q1->v.z * q2->w   + q1->v.x * q2->v.y - q1->v.y * q2->v.x};
	*q2 = r;
}
__HINT_INLINE__ void quatQMultQPR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r){
	/*
	** Calculates the Grassmann product of two quaternions.
	*/
	r->w   = q1->w * q2->w   - q1->v.x * q2->v.x - q1->v.y * q2->v.y - q1->v.z * q2->v.z;
	r->v.x = q1->w * q2->v.x + q1->v.x * q2->w   + q1->v.y * q2->v.z - q1->v.z * q2->v.y;
	r->v.y = q1->w * q2->v.y + q1->v.y * q2->w   + q1->v.z * q2->v.x - q1->v.x * q2->v.z;
	r->v.z = q1->w * q2->v.z + q1->v.z * q2->w   + q1->v.x * q2->v.y - q1->v.y * q2->v.x;
}
__HINT_INLINE__ void quatQMultSP(quat *const restrict q, const float s){
	q->w *= s;
	vec3VMultSP(&q->v, s);
}

__HINT_INLINE__ quat quatQDivQ(const quat q1, const quat q2){
	if(q2.w != 0.f && q2.v.x != 0.f && q2.v.y != 0.f && q2.v.z != 0.f){
		const quat r = {.w   = q1.w   / q2.w,
		                .v.x = q1.v.x / q2.v.x,
		                .v.y = q1.v.y / q2.v.y,
		                .v.z = q1.v.z / q2.v.z};
		return r;
	}
	return quatNewS(0.f);
}
__HINT_INLINE__ quat quatQDivS(const quat q, const float s){
	if(s != 0.f){
		const float invS = 1.f / s;
		const quat r = {.w   = q.w   * invS,
		                .v.x = q.v.x * invS,
		                .v.y = q.v.y * invS,
		                .v.z = q.v.z * invS};
		return r;
	}
	return quatNewS(0.f);
}
__HINT_INLINE__ void quatQDivQP1(quat *const restrict q1, const quat *const restrict q2){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		q1->w   /= q2->w;
		q1->v.x /= q2->v.x;
		q1->v.y /= q2->v.y;
		q1->v.z /= q2->v.z;
	}else{
		quatZeroP(q1);
	}
}
__HINT_INLINE__ void quatQDivQP2(const quat *const restrict q1, quat *const restrict q2){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		q2->w   = q1->w   / q2->w;
		q2->v.x = q1->v.x / q2->v.x;
		q2->v.y = q1->v.y / q2->v.y;
		q2->v.z = q1->v.z / q2->v.z;
	}else{
		quatZeroP(q2);
	}
}
__HINT_INLINE__ void quatQDivQPR(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		r->w   = q1->w   / q2->w;
		r->v.x = q1->v.x / q2->v.x;
		r->v.y = q1->v.y / q2->v.y;
		r->v.z = q1->v.z / q2->v.z;
	}else{
		quatZeroP(r);
	}
}
__HINT_INLINE__ void quatQDivSP(quat *const restrict q, const float s){
	if(s != 0.f){
		const float invS = 1.f / s;
		q->w   *= invS;
		q->v.x *= invS;
		q->v.y *= invS;
		q->v.z *= invS;
	}else{
		quatZeroP(q);
	}
}

__HINT_INLINE__ float quatMagnitude(const quat q){
	return sqrtf(q.w*q.w + q.v.x*q.v.x + q.v.y*q.v.y + q.v.z*q.v.z);
}
__HINT_INLINE__ float quatMagnitudeP(const quat *const restrict q){
	return sqrtf(q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z);
}
__HINT_INLINE__ float quatMagnitudeSquared(const quat q){
	return q.w*q.w + q.v.x*q.v.x + q.v.y*q.v.y + q.v.z*q.v.z;
}
__HINT_INLINE__ float quatMagnitudeSquaredP(const quat *const restrict q){
	return q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z;
}
__HINT_INLINE__ float quatMagnitudeInverse(const quat q){
	return fastInvSqrt(q.w*q.w + q.v.x*q.v.x + q.v.y*q.v.y + q.v.z*q.v.z);
}
__HINT_INLINE__ float quatMagnitudeInverseP(const quat *const restrict q){
	return fastInvSqrt(q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z);
}

__HINT_INLINE__ quat quatConjugate(const quat q){
	return quatNew(q.w, -q.v.x, -q.v.y, -q.v.z);
}
__HINT_INLINE__ quat quatConjugateFast(const quat q){
	return quatNew(-q.w, q.v.x, q.v.y, q.v.z);
}
__HINT_INLINE__ void quatConjugateP(quat *const restrict q){
	q->v.x = -q->v.x;
	q->v.y = -q->v.y;
	q->v.z = -q->v.z;
}
__HINT_INLINE__ void quatConjugateFastP(quat *const restrict q){
	q->w = -q->w;
}
__HINT_INLINE__ void quatConjugatePR(const quat *const restrict q, quat *const restrict r){
	quatSet(r, q->w, -q->v.x, -q->v.y, -q->v.z);
}
__HINT_INLINE__ void quatConjugateFastPR(const quat *const restrict q, quat *const restrict r){
	quatSet(r, -q->w, q->v.x, q->v.y, q->v.z);
}

__HINT_INLINE__ quat quatNegate(const quat q){
	return quatNew(-q.w, q.v.x, q.v.y, q.v.z);
}
__HINT_INLINE__ void quatNegateP(quat *const restrict q){
	q->w = -q->w;
}
__HINT_INLINE__ void quatNegatePR(const quat *const restrict q, quat *const restrict r){
	quatSet(r, -q->w, q->v.x, q->v.y, q->v.z);
}

__HINT_INLINE__ quat quatInvert(const quat q){
	return quatQMultQ(q, quatConjugateFast(q));
}
__HINT_INLINE__ void quatInvertP(quat *const restrict q){
	quat c;
	quatConjugateFastPR(q, &c);
	quatQMultQP1(q, &c);
}
__HINT_INLINE__ void quatInvertPR(const quat *const restrict q, quat *const restrict r){
	quatConjugateFastPR(q, r);
	quatQMultQP2(q, r);
}

__HINT_INLINE__ quat quatDifference(const quat q1, const quat q2){
	return quatQMultQ(quatInvert(q1), q2);
}
__HINT_INLINE__ void quatDifferenceP(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict r){
	quatInvertPR(q1, r);
	quatQMultQP1(r, q2);
}

__HINT_INLINE__ quat quatNormalize(const quat q){
	const float magnitude = quatMagnitude(q);
	if(magnitude != 0.f){
		return quatQDivS(q, magnitude);
	}
	return q;
}
__HINT_INLINE__ quat quatNormalizeFast(const quat q){
	return quatQMultS(q, quatMagnitudeInverse(q));
}
__HINT_INLINE__ quat quatNormalizeFastAccurate(const quat q){
	return quatQMultS(q, fastInvSqrtAccurate(quatMagnitudeSquared(q)));
}
__HINT_INLINE__ void quatNormalizeP(quat *const restrict q){
	const float magnitude = quatMagnitudeP(q);
	if(magnitude != 0.f){
		quatQDivSP(q, magnitude);
	}
}
__HINT_INLINE__ void quatNormalizeFastP(quat *const restrict q){
	quatQMultSP(q, quatMagnitudeInverseP(q));
}
__HINT_INLINE__ void quatNormalizeFastAccurateP(quat *const restrict q){
	quatQMultSP(q, fastInvSqrtAccurate(quatMagnitudeSquaredP(q)));
}

__HINT_INLINE__ quat quatIdentity(){
	const quat r = {.w = 1.f, .v.x = 0.f, .v.y = 0.f, .v.z = 0.f};
	return r;
}
__HINT_INLINE__ void quatSetIdentity(quat *const restrict q){
	q->w = 1.f; q->v.x = 0.f; q->v.y = 0.f; q->v.z = 0.f;
}

__HINT_INLINE__ void quatAxisAngle(const quat q, float *angle, float *axisX, float *axisY, float *axisZ){
	if(q.w != 1.f){  // We don't want to risk a potential divide-by-zero error.
		const float scale = fastInvSqrt(1.f-q.w*q.w);  // Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
		*angle = 2.f*acosf(q.w);
		*axisX = q.v.x*scale;
		*axisY = q.v.y*scale;
		*axisZ = q.v.z*scale;
	}
}
__HINT_INLINE__ void quatAxisAngleFast(const quat q, float *angle, float *axisX, float *axisY, float *axisZ){
	const float scale = fastInvSqrt(1.f-q.w*q.w);  // Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
	*angle = 2.f*acosf(q.w);
	*axisX = q.v.x*scale;
	*axisY = q.v.y*scale;
	*axisZ = q.v.z*scale;
}
__HINT_INLINE__ void quatAxisAngleP(const quat *const restrict q, float *angle, float *axisX, float *axisY, float *axisZ){
	if(q->w != 1.f){  // We don't want to risk a potential divide-by-zero error.
		const float scale = fastInvSqrt(1.f-q->w*q->w);  // Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
		*angle = 2.f*acosf(q->w);
		*axisX = q->v.x*scale;
		*axisY = q->v.y*scale;
		*axisZ = q->v.z*scale;
	}
}
__HINT_INLINE__ void quatAxisAngleFastP(const quat *const restrict q, float *angle, float *axisX, float *axisY, float *axisZ){
	const float scale = fastInvSqrt(1.f-q->w*q->w);  // Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
	*angle = 2.f*acosf(q->w);
	*axisX = q->v.x*scale;
	*axisY = q->v.y*scale;
	*axisZ = q->v.z*scale;
}

__HINT_INLINE__ float quatDot(const quat q1, const quat q2){
	return q1.w   * q2.w +
	       q1.v.x * q2.v.x +
	       q1.v.y * q2.v.y +
	       q1.v.z * q2.v.z;
}
__HINT_INLINE__ float quatDotP(const quat *const restrict q1, const quat *const restrict q2){
	return q1->w   * q2->w +
	       q1->v.x * q2->v.x +
	       q1->v.y * q2->v.y +
	       q1->v.z * q2->v.z;
}

__HINT_INLINE__ vec3 quatRotateVec3(const quat q, const vec3 v){

	vec3 r, temp;

	const float dotQV = vec3Dot(q.v, v);
	const float dotQQ = vec3Dot(q.v, q.v);
	float m = q.w*q.w - dotQQ;
	const vec3 crossQV = vec3Cross(q.v, v);

	r = vec3VMultS(v, m);

	m = 2.f * dotQV;
	temp = vec3VMultS(q.v, m);
	r = vec3VAddV(r, temp);

	m = 2.f * q.w;
	temp = vec3VMultS(crossQV, m);
	r = vec3VAddV(r, temp);

	return r;

}
__HINT_INLINE__ vec3 quatRotateVec3Fast(const quat q, const vec3 v){
	const vec3 crossQQV = vec3Cross(q.v, vec3VAddV(vec3Cross(q.v, v), vec3VMultS(v, q.w)));
	return vec3VAddV(crossQQV, vec3VAddV(crossQQV, v));
}
__HINT_INLINE__ void quatRotateVec3P(const quat *const restrict q, vec3 *const restrict v){

	vec3 temp;

	const float dotQV = vec3DotP(&q->v, v);
	const float dotQQ = vec3DotP(&q->v, &q->v);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3CrossPR(&q->v, v, &crossQV);

	vec3VMultSP(v, m);

	m = 2.f * dotQV;
	vec3VMultSPR(&q->v, m, &temp);
	vec3VAddVP(v, &temp);

	m = 2.f * q->w;
	vec3VMultSPR(&crossQV, m, &temp);
	vec3VAddVP(v, &temp);

}
__HINT_INLINE__ void quatRotateVec3PR(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict r){

	vec3 temp;

	const float dotQV = vec3DotP(&q->v, v);
	const float dotQQ = vec3DotP(&q->v, &q->v);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3CrossPR(&q->v, v, &crossQV);

	vec3VMultSPR(v, m, r);

	m = 2.f * dotQV;
	vec3VMultSPR(&q->v, m, &temp);
	vec3VAddVP(r, &temp);

	m = 2.f * q->w;
	vec3VMultSPR(&crossQV, m, &temp);
	vec3VAddVP(r, &temp);

}
__HINT_INLINE__ void quatRotateVec3FastP(const quat *const restrict q, vec3 *const restrict v){

	vec3 crossQV, crossQQV;
	vec3CrossPR(&q->v, v, &crossQV);

	crossQV.x += q->w * v->x;
	crossQV.y += q->w * v->y;
	crossQV.z += q->w * v->z;
	vec3CrossPR(&q->v, &crossQV, &crossQQV);

	v->x = crossQQV.x + crossQQV.x + v->x;
	v->y = crossQQV.y + crossQQV.y + v->y;
	v->z = crossQQV.z + crossQQV.z + v->z;

}
__HINT_INLINE__ void quatRotateVec3FastPR(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict r){

	vec3 crossQV, crossQQV;
	vec3CrossPR(&q->v, v, &crossQV);

	crossQV.x += q->w * v->x;
	crossQV.y += q->w * v->y;
	crossQV.z += q->w * v->z;
	vec3CrossPR(&q->v, &crossQV, &crossQQV);

	r->x = crossQQV.x + crossQQV.x + v->x;
	r->y = crossQQV.y + crossQQV.y + v->y;
	r->z = crossQQV.z + crossQQV.z + v->z;

}

__HINT_INLINE__ quat quatLookAt(const vec3 eye, const vec3 target, const vec3 up){

	const float dot = vec3Dot(eye, target);

	if(fabsf(dot + 1.f) < FLT_EPSILON){

		// Eye and target point in opposite directions,
		// 180 degree rotation around up vector.
		const quat r = {.w = M_PI, .v = up};
		return r;

	}else if(fabsf(dot - 1.f) < FLT_EPSILON){

		// Eye and target are pointing in the same direction.
		return quatIdentity();

	}else{

		const quat r = {.w = acosf(dot), .v = vec3NormalizeFast(vec3Cross(eye, target))};
		return r;

	}

}
__HINT_INLINE__ void quatLookAtP(quat *const restrict q, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict up){

	const float dot = vec3DotP(eye, target);

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
		vec3CrossPR(eye, target, &q->v);
		vec3NormalizeFastP(&q->v);

	}

}

__HINT_INLINE__ quat quatLerp(const quat q1, const quat q2, const float t){
	/*
	**               ^
	** r = (q1 + (q2 - q1) * t)
	*/
	const quat r = {
		.w   = q1.w   + (q2.w   - q1.w)   * t,
		.v.x = q1.v.x + (q2.v.x - q1.v.x) * t,
		.v.y = q1.v.y + (q2.v.y - q1.v.y) * t,
		.v.z = q1.v.z + (q2.v.z - q1.v.z) * t
	};
	return r;
}
__HINT_INLINE__ void quatLerpP1(quat *const restrict q1, const quat *const restrict q2, const float t){
	/*
	**               ^
	** r = (q1 + (q2 - q1) * t)
	*/
	q1->w   += (q2->w   - q1->w)   * t;
	q1->v.x += (q2->v.x - q1->v.x) * t;
	q1->v.y += (q2->v.y - q1->v.y) * t;
	q1->v.z += (q2->v.z - q1->v.z) * t;
}
__HINT_INLINE__ void quatLerpP2(const quat *const restrict q1, quat *const restrict q2, const float t){
	/*
	**               ^
	** r = (q1 + (q2 - q1) * t)
	*/
	q2->w   = q1->w   + (q2->w   - q1->w)   * t;
	q2->v.x = q1->v.x + (q2->v.x - q1->v.x) * t;
	q2->v.y = q1->v.y + (q2->v.y - q1->v.y) * t;
	q2->v.z = q1->v.z + (q2->v.z - q1->v.z) * t;
}
__HINT_INLINE__ void quatLerpPR(const quat *const restrict q1, const quat *const restrict q2, const float t, quat *const restrict r){
	/*
	**               ^
	** r = (q1 + (q2 - q1) * t)
	*/
	r->w   = q1->w   + (q2->w   - q1->w)   * t;
	r->v.x = q1->v.x + (q2->v.x - q1->v.x) * t;
	r->v.y = q1->v.y + (q2->v.y - q1->v.y) * t;
	r->v.z = q1->v.z + (q2->v.z - q1->v.z) * t;
}

__HINT_INLINE__ quat quatSlerp(const quat q1, const quat q2, const float t){

	// Cosine of the angle between the two quaternions.
	const float cosTheta = quatDot(q1, q2);
	const float cosThetaAbs = fabs(cosTheta);

	if(cosThetaAbs > QUAT_LERP_ANGLE){
		// If the angle is small enough, we can just use linear interpolation.
		return quatNormalizeFast(quatLerp(q1, q2, t));
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

		return quatNormalizeFast(quatNew(
			q1.w   * sinThetaInvT + q2.w   * sinThetaT,
			q1.v.x * sinThetaInvT + q2.v.x * sinThetaT,
			q1.v.y * sinThetaInvT + q2.v.y * sinThetaT,
			q1.v.z * sinThetaInvT + q2.v.z * sinThetaT
		));

	}

}
__HINT_INLINE__ void quatSlerpP1(quat *const restrict q1, const quat *const restrict q2, const float t){

	// Cosine of the angle between the two quaternions.
	const float cosTheta = quatDotP(q1, q2);
	const float cosThetaAbs = fabs(cosTheta);

	if(cosThetaAbs > QUAT_LERP_ANGLE){
		// If the angle is small enough, we can just use linear interpolation.
		quatLerpP1(q1, q2, t);
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

	quatNormalizeFastP(q1);

}
__HINT_INLINE__ void quatSlerpP2(const quat *const restrict q1, quat *const restrict q2, const float t){

	// Cosine of the angle between the two quaternions.
	const float cosTheta = quatDotP(q1, q2);
	const float cosThetaAbs = fabs(cosTheta);

	if(cosThetaAbs > QUAT_LERP_ANGLE){
		// If the angle is small enough, we can just use linear interpolation.
		quatLerpP2(q1, q2, t);
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

	quatNormalizeFastP(q2);

}
__HINT_INLINE__ void quatSlerpPR(const quat *const restrict q1, const quat *const restrict q2, const float t, quat *const restrict r){

	// Cosine of the angle between the two quaternions.
	const float cosTheta = quatDotP(q1, q2);
	const float cosThetaAbs = fabs(cosTheta);

	if(cosThetaAbs > QUAT_LERP_ANGLE){
		// If the angle is small enough, we can just use linear interpolation.
		quatLerpPR(q1, q2, t, r);
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

	quatNormalizeFastP(r);

}

__HINT_INLINE__ quat quatDifferentiate(const quat q, const vec3 w){
	const quat r = {
		.w = 0.f,
		.v = vec3VMultS(w, 0.5f)
	};
	return quatQMultQ(r, q);
}
__HINT_INLINE__ void quatDifferentiateP(quat *const restrict q, const vec3 *const restrict w){
	quat r;
	quatSet(&r, 0.f, w->x * 0.5f, w->y * 0.5f, w->z * 0.5f);
	quatQMultQP2(&r, q);
}
__HINT_INLINE__ void quatDifferentiatePR(const quat *const restrict q, const vec3 *const restrict w, quat *const restrict r){
	quatSet(r, 0.f, w->x * 0.5f, w->y * 0.5f, w->z * 0.5f);
	quatQMultQP1(r, q);
}

__HINT_INLINE__ quat quatIntegrate(const quat q, const vec3 w, float dt){
	const quat r = {.w = 0.f, .v = w};
	return quatQAddQ(q, quatQMultS(quatQMultQ(r, q), dt*0.5f));
}
__HINT_INLINE__ void quatIntegrateP(quat *const restrict q, const vec3 *const restrict w, float dt){
	quat r = {.w = 0.f, .v = *w};
	quatQMultQP1(&r, q);
	dt *= 0.5f;
	q->w   += r.w   * dt;
	q->v.x += r.v.x * dt;
	q->v.y += r.v.y * dt;
	q->v.z += r.v.z * dt;
}
__HINT_INLINE__ void quatIntegratePR(const quat *const restrict q, const vec3 *const restrict w, float dt, quat *const restrict r){
	r->w = 0.f;
	r->v = *w;
	quatQMultQP1(r, q);
	dt *= 0.5f;
	r->w   = r->w   * dt + q->w;
	r->v.x = r->v.x * dt + q->v.x;
	r->v.y = r->v.y * dt + q->v.y;
	r->v.z = r->v.z * dt + q->v.z;
}

__HINT_INLINE__ quat quatRotate(const quat q1, const quat q2, const float t){
	return quatSlerp(q1, quatQMultQ(q1, q2), t);
}
__HINT_INLINE__ void quatRotatePR(const quat *const restrict q1, const quat *const restrict q2, const float t, quat *const restrict r){
	quat temp;
	quatQMultQPR(q1, q2, &temp);
	// *r = temp;
	quatSlerpPR(q1, &temp, t, r);
}
