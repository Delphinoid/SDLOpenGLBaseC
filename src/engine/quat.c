#include "quat.h"
#include "helpersMath.h"
#include "constantsMath.h"
#include <float.h>
#include <string.h>

// Antiparallel threshold for rotating between two vectors.
#define QUAT_SINGULARITY_THRESHOLD 0.0001f

// The LERP threshold is the cosine of 1 radian.
// Try 1/2 radians if spherical lerp results are bad: 0.99996192306417128873735516482698
#define QUAT_LERP_THRESHOLD 0.99984769515639123915701155881391  // cos(RADIAN_RATIO)

// These constants are used by David Eberly's slerp function.
static const float u[8] = {
    1.f/(1.f * 3.f),
    1.f/(2.f * 5.f),
    1.f/(3.f * 7.f),
    1.f/(4.f * 9.f),
    1.f/(5.f * 11.f),
    1.f/(6.f * 13.f),
    1.f/(7.f * 15.f),
    1.90110745351730037f/(8.f * 17.f)
};
static const float v[8] = {
    1.f/3.f,
    2.f/5.f,
    3.f/7.f,
    4.f/9.f,
    5.f/11.f,
    6.f/13.f,
    7.f/15.f,
    1.90110745351730037f*(8.f/17.f)
};

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
	// Order of rotations: XYZ
	const float hx = x * 0.5f;
	const float hy = y * 0.5f;
	const float hz = z * 0.5f;
	const float cx = cosf(hx);
	const float cy = cosf(hy);
	const float cz = cosf(hz);
	const float sx = sinf(hx);
	const float sy = sinf(hy);
	const float sz = sinf(hz);
	const float czcy = cz*cy;
	const float szsy = sz*sy;
	const float czsy = cz*sy;
	const float szcy = sz*cy;
	const quat r = {
		.w   = czcy * cx + szsy * sx,
		.v.x = czcy * sx - szsy * cx,
		.v.y = czsy * cx + szcy * sx,
		.v.z = szcy * cx - czsy * sx
	};
	return r;
}
__HINT_INLINE__ quat quatNewRotation(const vec3 v1, const vec3 v2){
	// Rotation from v1 to v2.
	const float d = vec3Dot(v1, v2);
	// Handle the case where the vectors are
	// antiparallel to avoid singularities.
	if(d < -1.f + QUAT_SINGULARITY_THRESHOLD){
		const quat q = {
			.w = 0.f,
			.v = vec3Orthonormal(v1)
		};
		return q;
	}else{
		const float s = sqrtf(2.f * (1.f + d));
		const float rs = 1.f/s;
		const vec3 axis = vec3Cross(v1, v2);
		return quatNew(s*0.5f, rs*axis.x, rs*axis.y, rs*axis.z);
	}
}
__HINT_INLINE__ quat quatNewRotationFast(const vec3 v1, const vec3 v2){
	// Rotation from v1 to v2.
	// Doesn't check antiparallel vectors.
	const quat r = {
		.w = 1.f + vec3Dot(v1, v2),
		.v = vec3Cross(v1, v2)
	};
	return quatNormalizeFastAccurate(r);
}
quat quatZero(){
	const quat r = {.w = 0.f, .v.x = 0.f, .v.y = 0.f, .v.z = 0.f};
	return r;
}
__HINT_INLINE__ void quatSet(quat *const __RESTRICT__ q, const float w, const float x, const float y, const float z){
	q->w = w; q->v.x = x; q->v.y = y; q->v.z = z;
}
__HINT_INLINE__ void quatSetS(quat *const __RESTRICT__ q, const float s){
	q->w = s; q->v.x = s; q->v.y = s; q->v.z = s;
}
__HINT_INLINE__ void quatSetAxisAngle(quat *const __RESTRICT__ q, const float angle, const float axisX, const float axisY, const float axisZ){
	const float t = sinf(angle/2.f);
	q->w = cosf(angle/2.f);
	q->v.x = axisX * t;
	q->v.y = axisY * t;
	q->v.z = axisZ * t;
}
__HINT_INLINE__ void quatSetEuler(quat *const __RESTRICT__ q, const float x, const float y, const float z){
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
void quatZeroP(quat *const __RESTRICT__ q){
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
__HINT_INLINE__ void quatQAddQP(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2){
	q1->w += q2->w; q1->v.x += q2->v.x; q1->v.y += q2->v.y; q1->v.z += q2->v.z;
}
__HINT_INLINE__ void quatQAddQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r){
	r->w = q1->w + q2->w; r->v.x = q1->v.x + q2->v.x; r->v.y = q1->v.y + q2->v.y; r->v.z = q1->v.z + q2->v.z;
}
__HINT_INLINE__ void quatQAddSP(quat *const __RESTRICT__ q, const float s){
	q->w += s; q->v.x += s; q->v.y += s; q->v.z += s;
}
__HINT_INLINE__ void quatQAddWP(quat *const __RESTRICT__ q, const float w){
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
__HINT_INLINE__ void quatQSubQP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2){
	q1->w -= q2->w; q1->v.x -= q2->v.x; q1->v.y -= q2->v.y; q1->v.z -= q2->v.z;
}
__HINT_INLINE__ void quatQSubQP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2){
	q2->w = q1->w - q2->w; q2->v.x = q1->v.x - q2->v.x; q2->v.y = q1->v.y - q2->v.y; q2->v.z = q1->v.z - q2->v.z;
}
__HINT_INLINE__ void quatQSubQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r){
	r->w = q1->w - q2->w; r->v.x = q1->v.x - q2->v.x; r->v.y = q1->v.y - q2->v.y; r->v.z = q1->v.z - q2->v.z;
}
__HINT_INLINE__ void quatQSubSP(quat *const __RESTRICT__ q, const float s){
	q->w -= s; q->v.x -= s; q->v.y -= s; q->v.z -= s;
}
__HINT_INLINE__ void quatQSubWP(quat *const __RESTRICT__ q, const float w){
	q->w -= w;
}

__HINT_INLINE__ quat quatQMultQ(const quat q1, const quat q2){
	// Calculates the Grassmann product of two quaternions.
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
__HINT_INLINE__ void quatQMultQP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2){
	// Calculates the Grassmann product of two quaternions.
	const quat r = {.w   = q1->w * q2->w   - q1->v.x * q2->v.x - q1->v.y * q2->v.y - q1->v.z * q2->v.z,
	                .v.x = q1->w * q2->v.x + q1->v.x * q2->w   + q1->v.y * q2->v.z - q1->v.z * q2->v.y,
	                .v.y = q1->w * q2->v.y + q1->v.y * q2->w   + q1->v.z * q2->v.x - q1->v.x * q2->v.z,
	                .v.z = q1->w * q2->v.z + q1->v.z * q2->w   + q1->v.x * q2->v.y - q1->v.y * q2->v.x};
	*q1 = r;
}
__HINT_INLINE__ void quatQMultQP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2){
	// Calculates the Grassmann product of two quaternions.
	const quat r = {.w   = q1->w * q2->w   - q1->v.x * q2->v.x - q1->v.y * q2->v.y - q1->v.z * q2->v.z,
	                .v.x = q1->w * q2->v.x + q1->v.x * q2->w   + q1->v.y * q2->v.z - q1->v.z * q2->v.y,
	                .v.y = q1->w * q2->v.y + q1->v.y * q2->w   + q1->v.z * q2->v.x - q1->v.x * q2->v.z,
	                .v.z = q1->w * q2->v.z + q1->v.z * q2->w   + q1->v.x * q2->v.y - q1->v.y * q2->v.x};
	*q2 = r;
}
__HINT_INLINE__ void quatQMultQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r){
	// Calculates the Grassmann product of two quaternions.
	r->w   = q1->w * q2->w   - q1->v.x * q2->v.x - q1->v.y * q2->v.y - q1->v.z * q2->v.z;
	r->v.x = q1->w * q2->v.x + q1->v.x * q2->w   + q1->v.y * q2->v.z - q1->v.z * q2->v.y;
	r->v.y = q1->w * q2->v.y + q1->v.y * q2->w   + q1->v.z * q2->v.x - q1->v.x * q2->v.z;
	r->v.z = q1->w * q2->v.z + q1->v.z * q2->w   + q1->v.x * q2->v.y - q1->v.y * q2->v.x;
}
__HINT_INLINE__ void quatQMultSP(quat *const __RESTRICT__ q, const float s){
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
__HINT_INLINE__ void quatQDivQP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		q1->w   /= q2->w;
		q1->v.x /= q2->v.x;
		q1->v.y /= q2->v.y;
		q1->v.z /= q2->v.z;
	}else{
		quatZeroP(q1);
	}
}
__HINT_INLINE__ void quatQDivQP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		q2->w   = q1->w   / q2->w;
		q2->v.x = q1->v.x / q2->v.x;
		q2->v.y = q1->v.y / q2->v.y;
		q2->v.z = q1->v.z / q2->v.z;
	}else{
		quatZeroP(q2);
	}
}
__HINT_INLINE__ void quatQDivQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r){
	if(q2->w != 0.f && q2->v.x != 0.f && q2->v.y != 0.f && q2->v.z != 0.f){
		r->w   = q1->w   / q2->w;
		r->v.x = q1->v.x / q2->v.x;
		r->v.y = q1->v.y / q2->v.y;
		r->v.z = q1->v.z / q2->v.z;
	}else{
		quatZeroP(r);
	}
}
__HINT_INLINE__ void quatQDivSP(quat *const __RESTRICT__ q, const float s){
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
__HINT_INLINE__ float quatMagnitudeP(const quat *const __RESTRICT__ q){
	return sqrtf(q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z);
}
__HINT_INLINE__ float quatMagnitudeSquared(const quat q){
	return q.w*q.w + q.v.x*q.v.x + q.v.y*q.v.y + q.v.z*q.v.z;
}
__HINT_INLINE__ float quatMagnitudeSquaredP(const quat *const __RESTRICT__ q){
	return q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z;
}
__HINT_INLINE__ float quatMagnitudeInverse(const quat q){
	return 1.f/sqrtf(q.w*q.w + q.v.x*q.v.x + q.v.y*q.v.y + q.v.z*q.v.z);
}
__HINT_INLINE__ float quatMagnitudeInverseP(const quat *const __RESTRICT__ q){
	return 1.f/sqrtf(q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z);
}
__HINT_INLINE__ float quatMagnitudeInverseFast(const quat q){
	return rsqrt(q.w*q.w + q.v.x*q.v.x + q.v.y*q.v.y + q.v.z*q.v.z);
}
__HINT_INLINE__ float quatMagnitudeInverseFastP(const quat *const __RESTRICT__ q){
	return rsqrt(q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z);
}
__HINT_INLINE__ float quatMagnitudeInverseFastAccurate(const quat q){
	return rsqrtAccurate(q.w*q.w + q.v.x*q.v.x + q.v.y*q.v.y + q.v.z*q.v.z);
}
__HINT_INLINE__ float quatMagnitudeInverseFastAccurateP(const quat *const __RESTRICT__ q){
	return rsqrtAccurate(q->w*q->w + q->v.x*q->v.x + q->v.y*q->v.y + q->v.z*q->v.z);
}

__HINT_INLINE__ quat quatConjugate(const quat q){
	return quatNew(q.w, -q.v.x, -q.v.y, -q.v.z);
}
__HINT_INLINE__ quat quatConjugateFast(const quat q){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	return quatNew(-q.w, q.v.x, q.v.y, q.v.z);
}
__HINT_INLINE__ void quatConjugateP(quat *const __RESTRICT__ q){
	q->v.x = -q->v.x;
	q->v.y = -q->v.y;
	q->v.z = -q->v.z;
}
__HINT_INLINE__ void quatConjugateFastP(quat *const __RESTRICT__ q){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	q->w = -q->w;
}
__HINT_INLINE__ void quatConjugatePR(const quat *const __RESTRICT__ q, quat *const __RESTRICT__ r){
	r->w = q->w;
	r->v.x = -q->v.x;
	r->v.y = -q->v.y;
	r->v.z = -q->v.z;
}
__HINT_INLINE__ void quatConjugateFastPR(const quat *const __RESTRICT__ q, quat *const __RESTRICT__ r){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	r->w = -q->w;
	r->v.x = q->v.x;
	r->v.y = q->v.y;
	r->v.z = q->v.z;
}

__HINT_INLINE__ quat quatNegate(const quat q){
	return quatNew(-q.w, -q.v.x, -q.v.y, -q.v.z);
}
__HINT_INLINE__ void quatNegateP(quat *const __RESTRICT__ q){
	q->w = -q->w;
	q->v.x = -q->v.x;
	q->v.y = -q->v.y;
	q->v.z = -q->v.z;
}
__HINT_INLINE__ void quatNegatePR(const quat *const __RESTRICT__ q, quat *const __RESTRICT__ r){
	r->w = -q->w;
	r->v.x = -q->v.x;
	r->v.y = -q->v.y;
	r->v.z = -q->v.z;
}

__HINT_INLINE__ quat quatDifference(const quat q1, const quat q2){
	return quatQMultQ(quatInverseFast(q1), q2);
}
__HINT_INLINE__ void quatDifferenceP(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r){
	quatInverseFastPR(q1, r);
	quatQMultQP1(r, q2);
}

__HINT_INLINE__ quat quatNormalize(const quat q){
	return quatQMultS(q, quatMagnitudeInverse(q));
}
__HINT_INLINE__ quat quatNormalizeFast(const quat q){
	return quatQMultS(q, quatMagnitudeInverseFast(q));
}
__HINT_INLINE__ quat quatNormalizeFastAccurate(const quat q){
	return quatQMultS(q, quatMagnitudeInverseFastAccurate(q));
}
__HINT_INLINE__ void quatNormalizeP(quat *const __RESTRICT__ q){
	quatQMultSP(q, quatMagnitudeInverseP(q));
}
__HINT_INLINE__ void quatNormalizeFastP(quat *const __RESTRICT__ q){
	quatQMultSP(q, quatMagnitudeInverseFastP(q));
}
__HINT_INLINE__ void quatNormalizeFastAccurateP(quat *const __RESTRICT__ q){
	quatQMultSP(q, quatMagnitudeInverseFastAccurateP(q));
}

__HINT_INLINE__ quat quatIdentity(){
	const quat r = {.w = 1.f, .v.x = 0.f, .v.y = 0.f, .v.z = 0.f};
	return r;
}
__HINT_INLINE__ void quatSetIdentity(quat *const __RESTRICT__ q){
	q->w = 1.f; q->v.x = 0.f; q->v.y = 0.f; q->v.z = 0.f;
}

__HINT_INLINE__ void quatAxisAngle(const quat q, float *angle, float *axisX, float *axisY, float *axisZ){
	if(q.w != 1.f){  // We don't want to risk a potential divide-by-zero error.
		const float scale = rsqrt(1.f-q.w*q.w);  // Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
		*angle = 2.f*acosf(q.w);
		*axisX = q.v.x*scale;
		*axisY = q.v.y*scale;
		*axisZ = q.v.z*scale;
	}
}
__HINT_INLINE__ void quatAxisAngleUnsafe(const quat q, float *angle, float *axisX, float *axisY, float *axisZ){
	const float scale = rsqrt(1.f-q.w*q.w);  // Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
	*angle = 2.f*acosf(q.w);
	*axisX = q.v.x*scale;
	*axisY = q.v.y*scale;
	*axisZ = q.v.z*scale;
}
__HINT_INLINE__ void quatAxisAngleP(const quat *const __RESTRICT__ q, float *angle, float *axisX, float *axisY, float *axisZ){
	if(q->w != 1.f){  // We don't want to risk a potential divide-by-zero error.
		const float scale = rsqrt(1.f-q->w*q->w);  // Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
		*angle = 2.f*acosf(q->w);
		*axisX = q->v.x*scale;
		*axisY = q->v.y*scale;
		*axisZ = q->v.z*scale;
	}
}
__HINT_INLINE__ void quatAxisAngleUnsafeP(const quat *const __RESTRICT__ q, float *angle, float *axisX, float *axisY, float *axisZ){
	const float scale = rsqrt(1.f-q->w*q->w);  // Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
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
__HINT_INLINE__ float quatDotP(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2){
	return q1->w   * q2->w +
	       q1->v.x * q2->v.x +
	       q1->v.y * q2->v.y +
	       q1->v.z * q2->v.z;
}

__HINT_INLINE__ vec3 quatRotateVec3(const quat q, const vec3 v){
	const float dotQV = vec3Dot(q.v, v);
	return vec3VAddV(
		vec3VMultS(v, q.w*q.w - vec3Dot(q.v, q.v)),
		vec3VAddV(
			vec3VMultS(q.v, dotQV + dotQV),
			vec3VMultS(vec3Cross(q.v, v), q.w + q.w)
		)
	);
}
__HINT_INLINE__ vec3 quatRotateVec3FastApproximate(const quat q, const vec3 v){
	const vec3 crossQQV = vec3Cross(q.v, vec3VAddV(vec3Cross(q.v, v), vec3VMultS(v, q.w)));
	return vec3VAddV(crossQQV, vec3VAddV(crossQQV, v));
}
__HINT_INLINE__ void quatRotateVec3P(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ v){

	vec3 temp;

	const float dotQV = vec3DotP(&q->v, v);
	const float dotQQ = vec3DotP(&q->v, &q->v);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3CrossPR(&q->v, v, &crossQV);

	vec3VMultSP(v, m);

	m = dotQV + dotQV;
	vec3VMultSPR(&q->v, m, &temp);
	vec3VAddVP(v, &temp);

	m = q->w + q->w;
	vec3VMultSPR(&crossQV, m, &temp);
	vec3VAddVP(v, &temp);

}
__HINT_INLINE__ void quatRotateVec3PR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){

	vec3 temp;

	const float dotQV = vec3DotP(&q->v, v);
	const float dotQQ = vec3DotP(&q->v, &q->v);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3CrossPR(&q->v, v, &crossQV);

	vec3VMultSPR(v, m, r);

	m = dotQV + dotQV;
	vec3VMultSPR(&q->v, m, &temp);
	vec3VAddVP(r, &temp);

	m = q->w + q->w;
	vec3VMultSPR(&crossQV, m, &temp);
	vec3VAddVP(r, &temp);

}
__HINT_INLINE__ void quatRotateVec3FastApproximateP(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ v){

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
__HINT_INLINE__ void quatRotateVec3FastApproximatePR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){

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
__HINT_INLINE__ void quatLookAtP(quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ eye, const vec3 *const __RESTRICT__ target, const vec3 *const __RESTRICT__ up){

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
	//               ^
	// r = (q1 + (q2 - q1) * t)
	const quat r = {
		.w   = floatLerp(q1.w, q2.w, t),
		.v.x = floatLerp(q1.v.x, q2.v.x, t),
		.v.y = floatLerp(q1.v.y, q2.v.y, t),
		.v.z = floatLerp(q1.v.z, q2.v.z, t)
	};
	return r;
}
__HINT_INLINE__ void quatLerpP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t){
	//               ^
	// r = (q1 + (q2 - q1) * t)
	q1->w   = floatLerp(q1->w, q2->w, t);
	q1->v.x = floatLerp(q1->v.x, q2->v.x, t);
	q1->v.y = floatLerp(q1->v.y, q2->v.y, t);
	q1->v.z = floatLerp(q1->v.z, q2->v.z, t);
}
__HINT_INLINE__ void quatLerpP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2, const float t){
	//               ^
	// r = (q1 + (q2 - q1) * t)
	q2->w   = floatLerp(q1->w, q2->w, t);
	q2->v.x = floatLerp(q1->v.x, q2->v.x, t);
	q2->v.y = floatLerp(q1->v.y, q2->v.y, t);
	q2->v.z = floatLerp(q1->v.z, q2->v.z, t);
}
__HINT_INLINE__ void quatLerpPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t, quat *const __RESTRICT__ r){
	//               ^
	// r = (q1 + (q2 - q1) * t)
	r->w   = floatLerp(q1->w, q2->w, t);
	r->v.x = floatLerp(q1->v.x, q2->v.x, t);
	r->v.y = floatLerp(q1->v.y, q2->v.y, t);
	r->v.z = floatLerp(q1->v.z, q2->v.z, t);
}

quat quatSlerp(const quat q1, const quat q2, const float t){

	// Cosine of the angle between the two quaternions.
	float cosTheta = quatDot(q1, q2);
	quat q2Corrected;

	// If q1 and q2 are > 90 degrees apart (cosTheta < 0),
	// negate q2 so it doesn't go the long way around.
	if(cosTheta < 0.f){
        cosTheta = -cosTheta;
        q2Corrected = quatNegate(q2);
    }else{
		q2Corrected = q2;
    }

	if(cosTheta > QUAT_LERP_THRESHOLD){
		// If the angle is small enough, we can just use linear interpolation.
		return quatNormalizeFast(quatLerp(q1, q2Corrected, t));
	}else{

		// sin(x)^2 + cos(x)^2 = 1
		// sin(x)^2 = 1 - cos(x)^2
		// 1 / sin(x) = rsqrt(1 - cos(x)^2)
		//
		// Calculating the reciprocal of sin(x) allows us to do
		// multiplications instead of divisions below, as the
		// following holds:
		//
		// x * (1 / y) = x / y

		const float theta = acosf(cosTheta);
		const float sinThetaInv = rsqrt(1.f - cosTheta * cosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - t)) * sinThetaInv;
		const float sinThetaT = sinf(theta * t) * sinThetaInv;

		return quatNormalizeFast(quatNew(
			q1.w   * sinThetaInvT + q2Corrected.w   * sinThetaT,
			q1.v.x * sinThetaInvT + q2Corrected.v.x * sinThetaT,
			q1.v.y * sinThetaInvT + q2Corrected.v.y * sinThetaT,
			q1.v.z * sinThetaInvT + q2Corrected.v.z * sinThetaT
		));

	}

}
void quatSlerpP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t){

	// Cosine of the angle between the two quaternions.
	float cosTheta = quatDotP(q1, q2);
	quat q2Corrected = *q2;

	// If q1 and q2 are > 90 degrees apart (cosTheta < 0),
	// negate q2 so it doesn't go the long way around.
	if(cosTheta < 0.f){
        cosTheta = -cosTheta;
        quatNegateP(&q2Corrected);
    }

	if(cosTheta > QUAT_LERP_THRESHOLD){
		// If the angle is small enough, we can just use linear interpolation.
		quatLerpP1(q1, &q2Corrected, t);
	}else{

		// sin(x)^2 + cos(x)^2 = 1
		// sin(x)^2 = 1 - cos(x)^2
		// 1 / sin(x) = rsqrt(1 - cos(x)^2)
		//
		// Calculating the reciprocal of sin(x) allows us to do
		// multiplications instead of divisions below, as the
		// following holds:
		//
		// x * (1 / y) = x / y

		const float theta = acosf(cosTheta);
		const float sinThetaInv = rsqrt(1.f - cosTheta * cosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - t)) * sinThetaInv;
		const float sinThetaT = sinf(theta * t) * sinThetaInv;

		q1->w   = q1->w   * sinThetaInvT + q2Corrected.w   * sinThetaT;
		q1->v.x = q1->v.x * sinThetaInvT + q2Corrected.v.x * sinThetaT;
		q1->v.y = q1->v.y * sinThetaInvT + q2Corrected.v.y * sinThetaT;
		q1->v.z = q1->v.z * sinThetaInvT + q2Corrected.v.z * sinThetaT;

	}

	quatNormalizeFastP(q1);

}
void quatSlerpP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2, const float t){

	// Cosine of the angle between the two quaternions.
	float cosTheta = quatDotP(q1, q2);

	// If q1 and q2 are > 90 degrees apart (cosTheta < 0),
	// negate q2 so it doesn't go the long way around.
	if(cosTheta < 0.f){
        cosTheta = -cosTheta;
        quatNegateP(q2);
    }

	if(cosTheta > QUAT_LERP_THRESHOLD){
		// If the angle is small enough, we can just use linear interpolation.
		quatLerpP2(q1, q2, t);
	}else{

		// sin(x)^2 + cos(x)^2 = 1
		// sin(x)^2 = 1 - cos(x)^2
		// 1 / sin(x) = rsqrt(1 - cos(x)^2)
		//
		// Calculating the reciprocal of sin(x) allows us to do
		// multiplications instead of divisions below, as the
		// following holds:
		//
		// x * (1 / y) = x / y

		const float theta = acosf(cosTheta);
		const float sinThetaInv = rsqrt(1.f - cosTheta * cosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - t)) * sinThetaInv;
		const float sinThetaT = sinf(theta * t) * sinThetaInv;

		q2->w   = q1->w   * sinThetaInvT + q2->w   * sinThetaT;
		q2->v.x = q1->v.x * sinThetaInvT + q2->v.x * sinThetaT;
		q2->v.y = q1->v.y * sinThetaInvT + q2->v.y * sinThetaT;
		q2->v.z = q1->v.z * sinThetaInvT + q2->v.z * sinThetaT;

	}

	quatNormalizeFastP(q2);

}
void quatSlerpPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t, quat *const __RESTRICT__ r){

	// Cosine of the angle between the two quaternions.
	float cosTheta = quatDotP(q1, q2);
	quat q2Corrected;

	// If q1 and q2 are > 90 degrees apart (cosTheta < 0),
	// negate q2 so it doesn't go the long way around.
	if(cosTheta < 0.f){
        cosTheta = -cosTheta;
        quatNegatePR(q2, &q2Corrected);
    }else{
    	q2Corrected = *q2;
    }

	if(cosTheta > QUAT_LERP_THRESHOLD){
		// If the angle is small enough, we can just use linear interpolation.
		quatLerpPR(q1, q2, t, r);
	}else{

		// sin(x)^2 + cos(x)^2 = 1
		// sin(x)^2 = 1 - cos(x)^2
		// 1 / sin(x) = rsqrt(1 - cos(x)^2)
		//
		// Calculating the reciprocal of sin(x) allows us to do
		// multiplications instead of divisions below, as the
		// following holds:
		//
		// x * (1 / y) = x / y

		const float theta = acosf(cosTheta);
		const float sinThetaInv = rsqrt(1.f - cosTheta * cosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - t)) * sinThetaInv;
		const float sinThetaT = sinf(theta * t) * sinThetaInv;

		r->w   = q1->w   * sinThetaInvT + q2Corrected.w   * sinThetaT;
		r->v.x = q1->v.x * sinThetaInvT + q2Corrected.v.x * sinThetaT;
		r->v.y = q1->v.y * sinThetaInvT + q2Corrected.v.y * sinThetaT;
		r->v.z = q1->v.z * sinThetaInvT + q2Corrected.v.z * sinThetaT;

	}

	quatNormalizeFastP(r);

}

quat quatSlerpFast(const quat q1, const quat q2, const float t){

	// Fast quaternion slerp function, as described in David Eberly's
	// paper "A Fast and Accurate Algorithm for Computing SLERP",
	// published in the Journal of Graphics, GPU, and Game Tools 15.3,
	// 161-176 (2011). Uses only additions and multiplications, with no
	// transcendental (trigonometric) function calls or branching.

	float x = quatDot(q1, q2);
	const float sign = (x >= 0.f ? 1.f : (x = -x, -1.f));

	const float xm1 = x - 1.f;
	const float d = 1.f - t;
	const float sqrD = d * d;
	const float sqrT = t * t;

	const float bD[8] = {
		(u[0] * sqrD - v[0]) * xm1,
		(u[1] * sqrD - v[1]) * xm1,
		(u[2] * sqrD - v[2]) * xm1,
		(u[3] * sqrD - v[3]) * xm1,
		(u[4] * sqrD - v[4]) * xm1,
		(u[5] * sqrD - v[5]) * xm1,
		(u[6] * sqrD - v[6]) * xm1,
		(u[7] * sqrD - v[7]) * xm1
	};
	const float bT[8] = {
		(u[0] * sqrT - v[0]) * xm1,
		(u[1] * sqrT - v[1]) * xm1,
		(u[2] * sqrT - v[2]) * xm1,
		(u[3] * sqrT - v[3]) * xm1,
		(u[4] * sqrT - v[4]) * xm1,
		(u[5] * sqrT - v[5]) * xm1,
		(u[6] * sqrT - v[6]) * xm1,
		(u[7] * sqrT - v[7]) * xm1
	};

	const float cD = d * (
		1.0 + bD[0] * (1.0 + bD[1] * (1.0 + bD[2] * (1.0 + bD[3] * (
		1.0 + bD[4] * (1.0 + bD[5] * (1.0 + bD[6] * (1.0 + bD[7])))))))
	);
	const float cT = sign * t * (
		1.0 + bT[0] * (1.0 + bT[1] * (1.0 + bT[2] * (1.0 + bT[3] * (
		1.0 + bT[4] * (1.0 + bT[5] * (1.0 + bT[6] * (1.0 + bT[7])))))))
	);

	return quatNormalizeFast(quatNew(
		q1.w   * cD + q2.w   * cT,
		q1.v.x * cD + q2.v.x * cT,
		q1.v.y * cD + q2.v.y * cT,
		q1.v.z * cD + q2.v.z * cT
	));

}
void quatSlerpFastP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t){

	// Fast quaternion slerp function, as described in David Eberly's
	// paper "A Fast and Accurate Algorithm for Computing SLERP",
	// published in the Journal of Graphics, GPU, and Game Tools 15.3,
	// 161-176 (2011). Uses only additions and multiplications, with no
	// transcendental (trigonometric) function calls or branching.

	float x = quatDotP(q1, q2);
	const float sign = (x >= 0.f ? 1.f : (x = -x, -1.f));

	const float xm1 = x - 1.f;
	const float d = 1.f - t;
	const float sqrD = d * d;
	const float sqrT = t * t;

	const float bD[8] = {
		(u[0] * sqrD - v[0]) * xm1,
		(u[1] * sqrD - v[1]) * xm1,
		(u[2] * sqrD - v[2]) * xm1,
		(u[3] * sqrD - v[3]) * xm1,
		(u[4] * sqrD - v[4]) * xm1,
		(u[5] * sqrD - v[5]) * xm1,
		(u[6] * sqrD - v[6]) * xm1,
		(u[7] * sqrD - v[7]) * xm1
	};
	const float bT[8] = {
		(u[0] * sqrT - v[0]) * xm1,
		(u[1] * sqrT - v[1]) * xm1,
		(u[2] * sqrT - v[2]) * xm1,
		(u[3] * sqrT - v[3]) * xm1,
		(u[4] * sqrT - v[4]) * xm1,
		(u[5] * sqrT - v[5]) * xm1,
		(u[6] * sqrT - v[6]) * xm1,
		(u[7] * sqrT - v[7]) * xm1
	};

	const float cD = d * (
		1.0 + bD[0] * (1.0 + bD[1] * (1.0 + bD[2] * (1.0 + bD[3] * (
		1.0 + bD[4] * (1.0 + bD[5] * (1.0 + bD[6] * (1.0 + bD[7])))))))
	);
	const float cT = sign * t * (
		1.0 + bT[0] * (1.0 + bT[1] * (1.0 + bT[2] * (1.0 + bT[3] * (
		1.0 + bT[4] * (1.0 + bT[5] * (1.0 + bT[6] * (1.0 + bT[7])))))))
	);

	q1->w   = q1->w   * cD + q2->w   * cT;
	q1->v.x = q1->v.x * cD + q2->v.x * cT;
	q1->v.y = q1->v.y * cD + q2->v.y * cT;
	q1->v.z = q1->v.z * cD + q2->v.z * cT;

	quatNormalizeFastP(q1);

}
void quatSlerpFastP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2, const float t){

	// Fast quaternion slerp function, as described in David Eberly's
	// paper "A Fast and Accurate Algorithm for Computing SLERP",
	// published in the Journal of Graphics, GPU, and Game Tools 15.3,
	// 161-176 (2011). Uses only additions and multiplications, with no
	// transcendental (trigonometric) function calls or branching.

	float x = quatDotP(q1, q2);
	const float sign = (x >= 0.f ? 1.f : (x = -x, -1.f));

	const float xm1 = x - 1.f;
	const float d = 1.f - t;
	const float sqrD = d * d;
	const float sqrT = t * t;

	const float bD[8] = {
		(u[0] * sqrD - v[0]) * xm1,
		(u[1] * sqrD - v[1]) * xm1,
		(u[2] * sqrD - v[2]) * xm1,
		(u[3] * sqrD - v[3]) * xm1,
		(u[4] * sqrD - v[4]) * xm1,
		(u[5] * sqrD - v[5]) * xm1,
		(u[6] * sqrD - v[6]) * xm1,
		(u[7] * sqrD - v[7]) * xm1
	};
	const float bT[8] = {
		(u[0] * sqrT - v[0]) * xm1,
		(u[1] * sqrT - v[1]) * xm1,
		(u[2] * sqrT - v[2]) * xm1,
		(u[3] * sqrT - v[3]) * xm1,
		(u[4] * sqrT - v[4]) * xm1,
		(u[5] * sqrT - v[5]) * xm1,
		(u[6] * sqrT - v[6]) * xm1,
		(u[7] * sqrT - v[7]) * xm1
	};

	const float cD = d * (
		1.0 + bD[0] * (1.0 + bD[1] * (1.0 + bD[2] * (1.0 + bD[3] * (
		1.0 + bD[4] * (1.0 + bD[5] * (1.0 + bD[6] * (1.0 + bD[7])))))))
	);
	const float cT = sign * t * (
		1.0 + bT[0] * (1.0 + bT[1] * (1.0 + bT[2] * (1.0 + bT[3] * (
		1.0 + bT[4] * (1.0 + bT[5] * (1.0 + bT[6] * (1.0 + bT[7])))))))
	);

	q2->w   = q1->w   * cD + q2->w   * cT;
	q2->v.x = q1->v.x * cD + q2->v.x * cT;
	q2->v.y = q1->v.y * cD + q2->v.y * cT;
	q2->v.z = q1->v.z * cD + q2->v.z * cT;

	quatNormalizeFastP(q2);

}
void quatSlerpFastPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t, quat *const __RESTRICT__ r){

	// Fast quaternion slerp function, as described in David Eberly's
	// paper "A Fast and Accurate Algorithm for Computing SLERP",
	// published in the Journal of Graphics, GPU, and Game Tools 15.3,
	// 161-176 (2011). Uses only additions and multiplications, with no
	// transcendental (trigonometric) function calls or branching.

	float x = quatDotP(q1, q2);
	const float sign = (x >= 0.f ? 1.f : (x = -x, -1.f));

	const float xm1 = x - 1.f;
	const float d = 1.f - t;
	const float sqrD = d * d;
	const float sqrT = t * t;

	const float bD[8] = {
		(u[0] * sqrD - v[0]) * xm1,
		(u[1] * sqrD - v[1]) * xm1,
		(u[2] * sqrD - v[2]) * xm1,
		(u[3] * sqrD - v[3]) * xm1,
		(u[4] * sqrD - v[4]) * xm1,
		(u[5] * sqrD - v[5]) * xm1,
		(u[6] * sqrD - v[6]) * xm1,
		(u[7] * sqrD - v[7]) * xm1
	};
	const float bT[8] = {
		(u[0] * sqrT - v[0]) * xm1,
		(u[1] * sqrT - v[1]) * xm1,
		(u[2] * sqrT - v[2]) * xm1,
		(u[3] * sqrT - v[3]) * xm1,
		(u[4] * sqrT - v[4]) * xm1,
		(u[5] * sqrT - v[5]) * xm1,
		(u[6] * sqrT - v[6]) * xm1,
		(u[7] * sqrT - v[7]) * xm1
	};

	const float cD = d * (
		1.0 + bD[0] * (1.0 + bD[1] * (1.0 + bD[2] * (1.0 + bD[3] * (
		1.0 + bD[4] * (1.0 + bD[5] * (1.0 + bD[6] * (1.0 + bD[7])))))))
	);
	const float cT = sign * t * (
		1.0 + bT[0] * (1.0 + bT[1] * (1.0 + bT[2] * (1.0 + bT[3] * (
		1.0 + bT[4] * (1.0 + bT[5] * (1.0 + bT[6] * (1.0 + bT[7])))))))
	);

	r->w   = q1->w   * cD + q2->w   * cT;
	r->v.x = q1->v.x * cD + q2->v.x * cT;
	r->v.y = q1->v.y * cD + q2->v.y * cT;
	r->v.z = q1->v.z * cD + q2->v.z * cT;

	quatNormalizeFastP(r);

}

__HINT_INLINE__ quat quatDifferentiate(const quat q, const vec3 w){
	const quat r = {
		.w = 0.f,
		.v = vec3VMultS(w, 0.5f)
	};
	return quatQMultQ(r, q);
}
__HINT_INLINE__ void quatDifferentiateP(quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ w){
	quat r;
	quatSet(&r, 0.f, w->x * 0.5f, w->y * 0.5f, w->z * 0.5f);
	quatQMultQP2(&r, q);
}
__HINT_INLINE__ void quatDifferentiatePR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ w, quat *const __RESTRICT__ r){
	quatSet(r, 0.f, w->x * 0.5f, w->y * 0.5f, w->z * 0.5f);
	quatQMultQP1(r, q);
}

__HINT_INLINE__ quat quatIntegrate(const quat q, const vec3 w, float dt){
	const quat r = {.w = 0.f, .v = w};
	return quatQAddQ(q, quatQMultS(quatQMultQ(r, q), dt*0.5f));
}
__HINT_INLINE__ void quatIntegrateP(quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ w, float dt){
	quat r = {.w = 0.f, .v = *w};
	quatQMultQP1(&r, q);
	dt *= 0.5f;
	q->w   = floatMA(q->w, r.w, dt);
	q->v.x = floatMA(q->v.x, r.v.x, dt);
	q->v.y = floatMA(q->v.y, r.v.y, dt);
	q->v.z = floatMA(q->v.z, r.v.z, dt);
}
__HINT_INLINE__ void quatIntegratePR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ w, float dt, quat *const __RESTRICT__ r){
	r->w = 0.f;
	r->v = *w;
	quatQMultQP1(r, q);
	dt *= 0.5f;
	r->w   = floatMA(q->w, r->w, dt);
	r->v.x = floatMA(q->v.x, r->v.x, dt);
	r->v.y = floatMA(q->v.y, r->v.y, dt);
	r->v.z = floatMA(q->v.z, r->v.z, dt);
}

__HINT_INLINE__ quat quatRotate(const quat q1, const quat q2, const float t){
	return quatSlerpFast(q1, quatQMultQ(q1, q2), t);
}
__HINT_INLINE__ void quatRotatePR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t, quat *const __RESTRICT__ r){
	quat temp;
	quatQMultQPR(q1, q2, &temp);
	// *r = temp;
	quatSlerpFastPR(q1, &temp, t, r);
}

/*__HINT_INLINE__ quat quatMat4(const mat4 m){
    quat q;
    float t;
    if(m.m[2][2] < 0.f){
        if(m.m[0][0] > m.m[1][1]){
            t = m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.f;
            q = quatNew(m.m[1][2] - m.m[2][1], t, m.m[0][1] + m.m[1][0], m.m[2][0] + m.m[0][2]);
        }else{
            t = -m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.f;
            q = quatNew(m.m[2][0] - m.m[0][2], m.m[0][1] + m.m[1][0], t, m.m[1][2] + m.m[2][1]);
        }
    }else{
        if(m.m[0][0] < -m.m[1][1]){
            t = -m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.f;
            q = quatNew(m.m[0][1] - m.m[1][0], m.m[2][0] + m.m[0][2], m.m[1][2] + m.m[2][1], t);
        }else{
            t = m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.f;
            q = quatNew(t, m.m[1][2] - m.m[2][1], m.m[2][0] - m.m[0][2], m.m[0][1] - m.m[1][0]);
        }
    }
    return quatQMultS(q, 0.5f*rsqrt(t));
}
__HINT_INLINE__ quat quatMat4Alt(const mat4 m){
    const quat q = {
        .w = 0.5f * sqrtf(m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.f),
        .v.x = copySignZero(0.5f * sqrtf( m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.f), m.m[1][2] - m.m[2][1]),
        .v.y = copySignZero(0.5f * sqrtf(-m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.f), m.m[2][0] - m.m[0][2]),
        .v.z = copySignZero(0.5f * sqrtf(-m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.f), m.m[0][1] - m.m[1][0])
    };
    return q;
}*/
