#include "quat.h"
#include "helpersMath.h"
#include "constantsMath.h"
#include <float.h>
#include <string.h>

// Antiparallel threshold for rotating between two vectors.
#define QUAT_SINGULARITY_THRESHOLD 0.0001f
#define QUAT_SINGULARITY_THRESHOLD_SQUARED (QUAT_SINGULARITY_THRESHOLD*QUAT_SINGULARITY_THRESHOLD)

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

quat g_quatIdentity = {.x = 0.f, .y = 0.f, .z = 0.f, .w = 1.f};

__HINT_INLINE__ void quatIdentityP(quat *const __RESTRICT__ q){
	*q = g_quatIdentity;
}
__HINT_INLINE__ quat quatNew(const float w, const float x, const float y, const float z){
	const quat r = {.x = x, .y = y, .z = z, .w = w};
	return r;
}
__HINT_INLINE__ quat quatNewAxisAngle(const float angle, const float axisX, const float axisY, const float axisZ){
	const float t = sinf(0.5f*angle);
	const quat r = {
		.x = axisX * t,
		.y = axisY * t,
		.z = axisZ * t,
		.w = cosf(0.5f*angle)
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
		.w = czcy * cx + szsy * sx,
		.x = czcy * sx - szsy * cx,
		.y = czsy * cx + szcy * sx,
		.z = szcy * cx - czsy * sx
	};
	return r;
}
__HINT_INLINE__ quat quatNewRotation(const vec3 v1, const vec3 v2){
	// Rotation from v1 to v2.
	const float d = vec3Dot(v1, v2);
	// Handle the case where the vectors are
	// antiparallel to avoid singularities.
	if(d < -1.f + QUAT_SINGULARITY_THRESHOLD){
		const vec3 v = vec3Orthonormal(v1);
		const quat q = {
			.x = v.x,
			.y = v.y,
			.z = v.z,
			.w = 0
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
	const vec3 v = vec3Cross(v1, v2);
	const quat r = {
		.x = v.x,
		.y = v.y,
		.z = v.z,
		.w = 1.f + vec3Dot(v1, v2)
	};
	return quatNormalizeFastAccurate(r);
}
__HINT_INLINE__ void quatSet(quat *const __RESTRICT__ q, const float w, const float x, const float y, const float z){
	q->x = x; q->y = y; q->z = z; q->w = w;
}
__HINT_INLINE__ void quatSetAxisAngle(quat *const __RESTRICT__ q, const float angle, const float axisX, const float axisY, const float axisZ){
	const float t = sinf(0.5f*angle);
	q->x = axisX * t;
	q->y = axisY * t;
	q->z = axisZ * t;
	q->w = cosf(0.5f*angle);
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
	q->x = sx*cy*cz-cx*sy*sz;
	q->y = cx*sy*cz+sx*cy*sz;
	q->z = cx*cy*sz-sx*sy*cz;
	q->w = cx*cy*cz+sx*sy*sz;
}

__HINT_INLINE__ quat quatQAddQ(const quat q1, const quat q2){
	const quat r = {
		.x = q1.x + q2.x,
		.y = q1.y + q2.y,
		.z = q1.z + q2.z,
		.w = q1.w + q2.w
	};
	return r;
}
__HINT_INLINE__ void quatQAddQP(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2){
	q1->x += q2->x; q1->y += q2->y; q1->z += q2->z; q1->w += q2->w;
}
__HINT_INLINE__ void quatQAddQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r){
	r->x = q1->x + q2->x; r->y = q1->y + q2->y; r->z = q1->z + q2->z; r->w = q1->w + q2->w;
}

__HINT_INLINE__ quat quatQSubQ(const quat q1, const quat q2){
	const quat r = {
		.x = q1.x - q2.x,
		.y = q1.y - q2.y,
		.z = q1.z - q2.z,
		.w = q1.w - q2.w
	};
	return r;
}
__HINT_INLINE__ void quatQSubQP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2){
	q1->x -= q2->x; q1->y -= q2->y; q1->z -= q2->z; q1->w -= q2->w;
}
__HINT_INLINE__ void quatQSubQP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2){
	q2->x = q1->x - q2->x; q2->y = q1->y - q2->y; q2->z = q1->z - q2->z; q2->w = q1->w - q2->w;
}
__HINT_INLINE__ void quatQSubQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r){
	r->x = q1->x - q2->x; r->y = q1->y - q2->y; r->z = q1->z - q2->z; r->w = q1->w - q2->w;
}

__HINT_INLINE__ quat quatQMultQ(const quat q1, const quat q2){
	// Calculates the Grassmann product of two quaternions.
	const quat r = {
		.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		.y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		.z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
	};
	return r;
}
__HINT_INLINE__ quat quatQMultS(const quat q, const float s){
	const quat r = {
		.x = q.x * s,
		.y = q.y * s,
		.z = q.z * s,
		.w = q.w * s
	};
	return r;
}
__HINT_INLINE__ void quatQMultQP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2){
	// Calculates the Grassmann product of two quaternions.
	const quat r = {
		.x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y,
		.y = q1->w * q2->y + q1->y * q2->w + q1->z * q2->x - q1->x * q2->z,
		.z = q1->w * q2->z + q1->z * q2->w + q1->x * q2->y - q1->y * q2->x,
		.w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z
	};
	*q1 = r;
}
__HINT_INLINE__ void quatQMultQP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2){
	// Calculates the Grassmann product of two quaternions.
	const quat r = {
		.x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y,
		.y = q1->w * q2->y + q1->y * q2->w + q1->z * q2->x - q1->x * q2->z,
		.z = q1->w * q2->z + q1->z * q2->w + q1->x * q2->y - q1->y * q2->x,
		.w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z
	};
	*q2 = r;
}
__HINT_INLINE__ void quatQMultQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r){
	// Calculates the Grassmann product of two quaternions.
	r->x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y;
	r->y = q1->w * q2->y + q1->y * q2->w + q1->z * q2->x - q1->x * q2->z;
	r->z = q1->w * q2->z + q1->z * q2->w + q1->x * q2->y - q1->y * q2->x;
	r->w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z;
}
__HINT_INLINE__ void quatQMultSP(quat *const __RESTRICT__ q, const float s){
	q->x *= s;
	q->y *= s;
	q->z *= s;
	q->w *= s;
}

__HINT_INLINE__ quat quatQConjugateMultQ(const quat q1, const quat q2){
	// Calculates the Grassmann product of two quaternions.
	const quat r = {
		.x = q1.w * q2.x - q1.x * q2.w - q1.y * q2.z + q1.z * q2.y,
		.y = q1.w * q2.y - q1.y * q2.w - q1.z * q2.x + q1.x * q2.z,
		.z = q1.w * q2.z - q1.z * q2.w - q1.x * q2.y + q1.y * q2.x,
		.w = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z
	};
	return r;
}
__HINT_INLINE__ void quatQConjugateMultQP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2){
	// Calculates the Grassmann product of two quaternions.
	const quat r = {
		.x = q1->w * q2->x - q1->x * q2->w - q1->y * q2->z + q1->z * q2->y,
		.y = q1->w * q2->y - q1->y * q2->w - q1->z * q2->x + q1->x * q2->z,
		.z = q1->w * q2->z - q1->z * q2->w - q1->x * q2->y + q1->y * q2->x,
		.w = q1->w * q2->w + q1->x * q2->x + q1->y * q2->y + q1->z * q2->z
	};
	*q1 = r;
}
__HINT_INLINE__ void quatQConjugateMultQP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2){
	// Calculates the Grassmann product of two quaternions.
	const quat r = {
		.x = q1->w * q2->x - q1->x * q2->w - q1->y * q2->z + q1->z * q2->y,
		.y = q1->w * q2->y - q1->y * q2->w - q1->z * q2->x + q1->x * q2->z,
		.z = q1->w * q2->z - q1->z * q2->w - q1->x * q2->y + q1->y * q2->x,
		.w = q1->w * q2->w + q1->x * q2->x + q1->y * q2->y + q1->z * q2->z
	};
	*q2 = r;
}
__HINT_INLINE__ void quatQConjugateMultQPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r){
	// Calculates the Grassmann product of two quaternions.
	r->x = q1->w * q2->x - q1->x * q2->w - q1->y * q2->z + q1->z * q2->y;
	r->y = q1->w * q2->y - q1->y * q2->w - q1->z * q2->x + q1->x * q2->z;
	r->z = q1->w * q2->z - q1->z * q2->w - q1->x * q2->y + q1->y * q2->x;
	r->w = q1->w * q2->w + q1->x * q2->x + q1->y * q2->y + q1->z * q2->z;
}

__HINT_INLINE__ quat quatQMultQConjugate(const quat q1, const quat q2){
	// Calculates the Grassmann product of two quaternions.
	const quat r = {
		.x = -q1.w * q2.x + q1.x * q2.w - q1.y * q2.z + q1.z * q2.y,
		.y = -q1.w * q2.y + q1.y * q2.w - q1.z * q2.x + q1.x * q2.z,
		.z = -q1.w * q2.z + q1.z * q2.w - q1.x * q2.y + q1.y * q2.x,
		.w =  q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z
	};
	return r;
}
__HINT_INLINE__ void quatQMultQConjugateP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2){
	// Calculates the Grassmann product of two quaternions.
	const quat r = {
		.x = -q1->w * q2->x + q1->x * q2->w - q1->y * q2->z + q1->z * q2->y,
		.y = -q1->w * q2->y + q1->y * q2->w - q1->z * q2->x + q1->x * q2->z,
		.z = -q1->w * q2->z + q1->z * q2->w - q1->x * q2->y + q1->y * q2->x,
		.w =  q1->w * q2->w + q1->x * q2->x + q1->y * q2->y + q1->z * q2->z
	};
	*q1 = r;
}
__HINT_INLINE__ void quatQMultQConjugateP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2){
	// Calculates the Grassmann product of two quaternions.
	const quat r = {
		.x = -q1->w * q2->x + q1->x * q2->w - q1->y * q2->z + q1->z * q2->y,
		.y = -q1->w * q2->y + q1->y * q2->w - q1->z * q2->x + q1->x * q2->z,
		.z = -q1->w * q2->z + q1->z * q2->w - q1->x * q2->y + q1->y * q2->x,
		.w =  q1->w * q2->w + q1->x * q2->x + q1->y * q2->y + q1->z * q2->z
	};
	*q2 = r;
}
__HINT_INLINE__ void quatQMultQConjugatePR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, quat *const __RESTRICT__ r){
	// Calculates the Grassmann product of two quaternions.
	r->x = -q1->w * q2->x + q1->x * q2->w - q1->y * q2->z + q1->z * q2->y;
	r->y = -q1->w * q2->y + q1->y * q2->w - q1->z * q2->x + q1->x * q2->z;
	r->z = -q1->w * q2->z + q1->z * q2->w - q1->x * q2->y + q1->y * q2->x;
	r->w =  q1->w * q2->w + q1->x * q2->x + q1->y * q2->y + q1->z * q2->z;
}

__HINT_INLINE__ float quatMagnitude(const quat q){
	return sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}
__HINT_INLINE__ float quatMagnitudeP(const quat *const __RESTRICT__ q){
	return sqrtf(q->x*q->x + q->y*q->y + q->z*q->z + q->w*q->w);
}
__HINT_INLINE__ float quatMagnitudeSquared(const quat q){
	return q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
}
__HINT_INLINE__ float quatMagnitudeSquaredP(const quat *const __RESTRICT__ q){
	return q->x*q->x + q->y*q->y + q->z*q->z + q->w*q->w;
}
__HINT_INLINE__ float quatMagnitudeInverse(const quat q){
	return 1.f/sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}
__HINT_INLINE__ float quatMagnitudeInverseP(const quat *const __RESTRICT__ q){
	return 1.f/sqrtf(q->x*q->x + q->y*q->y + q->z*q->z + q->w*q->w);
}
__HINT_INLINE__ float quatMagnitudeInverseFast(const quat q){
	return rsqrt(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}
__HINT_INLINE__ float quatMagnitudeInverseFastP(const quat *const __RESTRICT__ q){
	return rsqrt(q->x*q->x + q->y*q->y + q->z*q->z + q->w*q->w);
}
__HINT_INLINE__ float quatMagnitudeInverseFastAccurate(const quat q){
	return rsqrtAccurate(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}
__HINT_INLINE__ float quatMagnitudeInverseFastAccurateP(const quat *const __RESTRICT__ q){
	return rsqrtAccurate(q->x*q->x + q->y*q->y + q->z*q->z + q->w*q->w);
}

__HINT_INLINE__ quat quatConjugate(const quat q){
	return quatNew(q.w, -q.x, -q.y, -q.z);
}
__HINT_INLINE__ quat quatConjugateFast(const quat q){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	return quatNew(-q.w, q.x, q.y, q.z);
}
__HINT_INLINE__ void quatConjugateP(quat *const __RESTRICT__ q){
	q->x = -q->x;
	q->y = -q->y;
	q->z = -q->z;
}
__HINT_INLINE__ void quatConjugateFastP(quat *const __RESTRICT__ q){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	q->w = -q->w;
}
__HINT_INLINE__ void quatConjugatePR(const quat *const __RESTRICT__ q, quat *const __RESTRICT__ r){
	r->x = -q->x;
	r->y = -q->y;
	r->z = -q->z;
	r->w =  q->w;
}
__HINT_INLINE__ void quatConjugateFastPR(const quat *const __RESTRICT__ q, quat *const __RESTRICT__ r){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	r->x =  q->x;
	r->y =  q->y;
	r->z =  q->z;
	r->w = -q->w;
}

__HINT_INLINE__ quat quatNegate(const quat q){
	return quatNew(-q.w, -q.x, -q.y, -q.z);
}
__HINT_INLINE__ void quatNegateP(quat *const __RESTRICT__ q){
	q->x = -q->x;
	q->y = -q->y;
	q->z = -q->z;
	q->w = -q->w;
}
__HINT_INLINE__ void quatNegatePR(const quat *const __RESTRICT__ q, quat *const __RESTRICT__ r){
	r->x = -q->x;
	r->y = -q->y;
	r->z = -q->z;
	r->w = -q->w;
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

__HINT_INLINE__ vec3 quatAxis(const quat q){
	// We don't want to risk a potential divide-by-zero error.
	if(q.w == 1.f){
		return g_vec3Zero;
	}else{
		// Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
		const float scale = rsqrt(1.f-q.w*q.w);
		const vec3 r = {
			.x = q.x*scale,
			.y = q.y*scale,
			.z = q.z*scale
		};
		return r;
	}
}
__HINT_INLINE__ void quatAxisPR(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ r){
	// We don't want to risk a potential divide-by-zero error.
	if(q->w == 1.f){
		vec3ZeroP(r);
	}else{
		// Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
		const float scale = rsqrt(1.f-q->w*q->w);
		r->x = q->x*scale;
		r->y = q->y*scale;
		r->z = q->z*scale;
	}
}
__HINT_INLINE__ vec3 quatAxisUnsafe(const quat q){
	// Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
	const float scale = rsqrt(1.f-q.w*q.w);
	const vec3 r = {
		.x = q.x*scale,
		.y = q.y*scale,
		.z = q.z*scale
	};
	return r;
}
__HINT_INLINE__ void quatAxisUnsafePR(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ r){
	// Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1.
	const float scale = rsqrt(1.f-q->w*q->w);
	r->x = q->x*scale;
	r->y = q->y*scale;
	r->z = q->z*scale;
}
__HINT_INLINE__ float quatAngle(const quat q){
	return 2.f*acosf(q.w);
}
__HINT_INLINE__ float quatAngleP(const quat *const __RESTRICT__ q){
	return 2.f*acosf(q->w);
}

__HINT_INLINE__ float quatDot(const quat q1, const quat q2){
	return q1.x * q2.x +
	       q1.y * q2.y +
	       q1.z * q2.z +
	       q1.w * q2.w;
}
__HINT_INLINE__ float quatDotP(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2){
	return q1->x * q2->x +
	       q1->y * q2->y +
	       q1->z * q2->z +
	       q1->w * q2->w;
}

__HINT_INLINE__ vec3 quatRotateVec3(const quat q, const vec3 v){
	const float dotQV = vec3Dot(*((vec3 *)&q), v);
	return vec3VAddV(
		vec3VMultS(v, q.w*q.w - vec3Dot(*((vec3 *)&q), *((vec3 *)&q))),
		vec3VAddV(
			vec3VMultS(*((vec3 *)&q), 2.f*dotQV),
			vec3VMultS(vec3Cross(*((vec3 *)&q), v), 2.f*q.w)
		)
	);
}
__HINT_INLINE__ vec3 quatRotateVec3FastApproximate(const quat q, const vec3 v){
	const vec3 crossQQV = vec3Cross(*((vec3 *)&q), vec3VAddV(vec3Cross(*((vec3 *)&q), v), vec3VMultS(v, q.w)));
	return vec3VAddV(crossQQV, vec3VAddV(crossQQV, v));
}
__HINT_INLINE__ void quatRotateVec3P(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ v){

	vec3 temp;

	const float dotQV = vec3DotP((vec3 *)q, v);
	const float dotQQ = vec3DotP((vec3 *)q, (vec3 *)q);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3CrossPR((vec3 *)q, v, &crossQV);

	vec3VMultSP(v, m);

	m = 2.f*dotQV;
	vec3VMultSPR((vec3 *)q, m, &temp);
	vec3VAddVP(v, &temp);

	m = 2.f*q->w;
	vec3VMultSPR(&crossQV, m, &temp);
	vec3VAddVP(v, &temp);

}
__HINT_INLINE__ void quatRotateVec3PR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){

	vec3 temp;

	const float dotQV = vec3DotP((vec3 *)q, v);
	const float dotQQ = vec3DotP((vec3 *)q, (vec3 *)q);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3CrossPR((vec3 *)q, v, &crossQV);

	vec3VMultSPR(v, m, r);

	m = 2.f*dotQV;
	vec3VMultSPR((vec3 *)q, m, &temp);
	vec3VAddVP(r, &temp);

	m = 2.f*q->w;
	vec3VMultSPR(&crossQV, m, &temp);
	vec3VAddVP(r, &temp);

}
__HINT_INLINE__ void quatRotateVec3FastApproximateP(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ v){

	vec3 crossQV, crossQQV;
	vec3CrossPR((vec3 *)q, v, &crossQV);

	crossQV.x += q->w * v->x;
	crossQV.y += q->w * v->y;
	crossQV.z += q->w * v->z;
	vec3CrossPR((vec3 *)q, &crossQV, &crossQQV);

	v->x += 2.f*crossQQV.x;
	v->y += 2.f*crossQQV.y;
	v->z += 2.f*crossQQV.z;

}
__HINT_INLINE__ void quatRotateVec3FastApproximatePR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){

	vec3 crossQV, crossQQV;
	vec3CrossPR((vec3 *)q, v, &crossQV);

	crossQV.x += q->w * v->x;
	crossQV.y += q->w * v->y;
	crossQV.z += q->w * v->z;
	vec3CrossPR((vec3 *)q, &crossQV, &crossQQV);

	r->x += 2.f*crossQQV.x;
	r->y += 2.f*crossQQV.y;
	r->z += 2.f*crossQQV.z;

}

__HINT_INLINE__ vec3 quatConjugateRotateVec3(const quat q, const vec3 v){
	const float dotQV = vec3Dot(*((vec3 *)&q), v);
	return vec3VAddV(
		vec3VMultS(v, q.w*q.w - vec3Dot(*((vec3 *)&q), *((vec3 *)&q))),
		vec3VAddV(
			vec3VMultS(*((vec3 *)&q), 2.f*dotQV),
			vec3VMultS(vec3Cross(*((vec3 *)&q), v), -2.f*q.w)
		)
	);
}
__HINT_INLINE__ vec3 quatConjugateRotateVec3FastApproximate(const quat q, const vec3 v){
	const vec3 crossQQV = vec3Cross(*((vec3 *)&q), vec3VAddV(vec3Cross(*((vec3 *)&q), v), vec3VMultS(v, -q.w)));
	return vec3VAddV(crossQQV, vec3VAddV(crossQQV, v));
}
__HINT_INLINE__ void quatConjugateRotateVec3P(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ v){

	vec3 temp;

	const float dotQV = vec3DotP((vec3 *)q, v);
	const float dotQQ = vec3DotP((vec3 *)q, (vec3 *)q);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3CrossPR((vec3 *)q, v, &crossQV);

	vec3VMultSP(v, m);

	m = 2.f*dotQV;
	vec3VMultSPR((vec3 *)q, m, &temp);
	vec3VAddVP(v, &temp);

	m = -2.f*q->w;
	vec3VMultSPR(&crossQV, m, &temp);
	vec3VAddVP(v, &temp);

}
__HINT_INLINE__ void quatConjugateRotateVec3PR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){

	vec3 temp;

	const float dotQV = vec3DotP((vec3 *)q, v);
	const float dotQQ = vec3DotP((vec3 *)q, (vec3 *)q);
	float m = q->w*q->w - dotQQ;
	vec3 crossQV;
	vec3CrossPR((vec3 *)q, v, &crossQV);

	vec3VMultSPR(v, m, r);

	m = 2.f*dotQV;
	vec3VMultSPR((vec3 *)q, m, &temp);
	vec3VAddVP(r, &temp);

	m = -2.f*q->w;
	vec3VMultSPR(&crossQV, m, &temp);
	vec3VAddVP(r, &temp);

}
__HINT_INLINE__ void quatConjugateRotateVec3FastApproximateP(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ v){

	vec3 crossQV, crossQQV;
	vec3CrossPR((vec3 *)q, v, &crossQV);

	crossQV.x -= q->w * v->x;
	crossQV.y -= q->w * v->y;
	crossQV.z -= q->w * v->z;
	vec3CrossPR((vec3 *)q, &crossQV, &crossQQV);

	v->x += 2.f*crossQQV.x;
	v->y += 2.f*crossQQV.y;
	v->z += 2.f*crossQQV.z;

}
__HINT_INLINE__ void quatConjugateRotateVec3FastApproximatePR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){

	vec3 crossQV, crossQQV;
	vec3CrossPR((vec3 *)q, v, &crossQV);

	crossQV.x -= q->w * v->x;
	crossQV.y -= q->w * v->y;
	crossQV.z -= q->w * v->z;
	vec3CrossPR((vec3 *)q, &crossQV, &crossQQV);

	r->x += 2.f*crossQQV.x;
	r->y += 2.f*crossQQV.y;
	r->z += 2.f*crossQQV.z;

}

__HINT_INLINE__ quat quatLookAt(const vec3 eye, const vec3 target, const vec3 up){

	const float dot = vec3Dot(eye, target);

	if(fabsf(dot + 1.f) < FLT_EPSILON){

		// Eye and target point in opposite directions,
		// 180 degree rotation around up vector.
		const quat r = {.x = up.x, .y = up.y, .z = up.z, .w = M_PI};
		return r;

	}else if(fabsf(dot - 1.f) < FLT_EPSILON){

		// Eye and target are pointing in the same direction.
		return g_quatIdentity;

	}else{

		const vec3 v = vec3NormalizeFast(vec3Cross(eye, target));
		const quat r = {.x = v.x, .y = v.y, .z = v.z, .w = acosf(dot)};
		return r;

	}

}
__HINT_INLINE__ void quatLookAtP(quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ eye, const vec3 *const __RESTRICT__ target, const vec3 *const __RESTRICT__ up){

	const float dot = vec3DotP(eye, target);

	if(fabsf(dot + 1.f) < FLT_EPSILON){

		// Eye and target point in opposite directions,
		// 180 degree rotation around up vector.
		*((vec3 *)q) = *up;
		q->w = M_PI;

	}else if(fabsf(dot - 1.f) < FLT_EPSILON){

		// Eye and target are pointing in the same direction.
		quatIdentityP(q);

	}else{

		vec3CrossPR(eye, target, (vec3 *)q);
		vec3NormalizeFastP((vec3 *)q);
		q->w = acosf(dot);

	}

}

__HINT_INLINE__ quat quatLerp(const quat q1, const quat q2, const float t){
	//               ^
	// r = (q1 + (q2 - q1) * t)
	const quat r = {
		.x = floatLerpFast(q1.x, q2.x, t),
		.y = floatLerpFast(q1.y, q2.y, t),
		.z = floatLerpFast(q1.z, q2.z, t),
		.w = floatLerpFast(q1.w, q2.w, t)
	};
	return r;
}
__HINT_INLINE__ void quatLerpP1(quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t){
	//               ^
	// r = (q1 + (q2 - q1) * t)
	q1->x = floatLerpFast(q1->x, q2->x, t);
	q1->y = floatLerpFast(q1->y, q2->y, t);
	q1->z = floatLerpFast(q1->z, q2->z, t);
	q1->w = floatLerpFast(q1->w, q2->w, t);
}
__HINT_INLINE__ void quatLerpP2(const quat *const __RESTRICT__ q1, quat *const __RESTRICT__ q2, const float t){
	//               ^
	// r = (q1 + (q2 - q1) * t)
	q2->x = floatLerpFast(q1->x, q2->x, t);
	q2->y = floatLerpFast(q1->y, q2->y, t);
	q2->z = floatLerpFast(q1->z, q2->z, t);
	q2->w = floatLerpFast(q1->w, q2->w, t);
}
__HINT_INLINE__ void quatLerpPR(const quat *const __RESTRICT__ q1, const quat *const __RESTRICT__ q2, const float t, quat *const __RESTRICT__ r){
	//               ^
	// r = (q1 + (q2 - q1) * t)
	r->x = floatLerpFast(q1->x, q2->x, t);
	r->y = floatLerpFast(q1->y, q2->y, t);
	r->z = floatLerpFast(q1->z, q2->z, t);
	r->w = floatLerpFast(q1->w, q2->w, t);
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
		///
		// Calculating the reciprocal of sin(x) allows us to do
		// multiplications instead of divisions below, as the
		// following holds:
		///
		// x * (1 / y) = x / y

		const float theta = acosf(cosTheta);
		const float sinThetaInv = rsqrt(1.f - cosTheta * cosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - t)) * sinThetaInv;
		const float sinThetaT = sinf(theta * t) * sinThetaInv;

		return quatNormalizeFast(quatNew(
			q1.w * sinThetaInvT + q2Corrected.w * sinThetaT,
			q1.x * sinThetaInvT + q2Corrected.x * sinThetaT,
			q1.y * sinThetaInvT + q2Corrected.y * sinThetaT,
			q1.z * sinThetaInvT + q2Corrected.z * sinThetaT
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
		///
		// Calculating the reciprocal of sin(x) allows us to do
		// multiplications instead of divisions below, as the
		// following holds:
		///
		// x * (1 / y) = x / y

		const float theta = acosf(cosTheta);
		const float sinThetaInv = rsqrt(1.f - cosTheta * cosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - t)) * sinThetaInv;
		const float sinThetaT = sinf(theta * t) * sinThetaInv;

		q1->x = q1->x * sinThetaInvT + q2Corrected.x * sinThetaT;
		q1->y = q1->y * sinThetaInvT + q2Corrected.y * sinThetaT;
		q1->z = q1->z * sinThetaInvT + q2Corrected.z * sinThetaT;
		q1->w = q1->w * sinThetaInvT + q2Corrected.w * sinThetaT;

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
		///
		// Calculating the reciprocal of sin(x) allows us to do
		// multiplications instead of divisions below, as the
		// following holds:
		///
		// x * (1 / y) = x / y

		const float theta = acosf(cosTheta);
		const float sinThetaInv = rsqrt(1.f - cosTheta * cosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - t)) * sinThetaInv;
		const float sinThetaT = sinf(theta * t) * sinThetaInv;

		q2->x = q1->x * sinThetaInvT + q2->x * sinThetaT;
		q2->y = q1->y * sinThetaInvT + q2->y * sinThetaT;
		q2->z = q1->z * sinThetaInvT + q2->z * sinThetaT;
		q2->w = q1->w * sinThetaInvT + q2->w * sinThetaT;

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
		///
		// Calculating the reciprocal of sin(x) allows us to do
		// multiplications instead of divisions below, as the
		// following holds:
		///
		// x * (1 / y) = x / y

		const float theta = acosf(cosTheta);
		const float sinThetaInv = rsqrt(1.f - cosTheta * cosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - t)) * sinThetaInv;
		const float sinThetaT = sinf(theta * t) * sinThetaInv;

		r->x = q1->x * sinThetaInvT + q2Corrected.x * sinThetaT;
		r->y = q1->y * sinThetaInvT + q2Corrected.y * sinThetaT;
		r->z = q1->z * sinThetaInvT + q2Corrected.z * sinThetaT;
		r->w = q1->w * sinThetaInvT + q2Corrected.w * sinThetaT;

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
		q1.w * cD + q2.w * cT,
		q1.x * cD + q2.x * cT,
		q1.y * cD + q2.y * cT,
		q1.z * cD + q2.z * cT
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

	q1->x = q1->x * cD + q2->x * cT;
	q1->y = q1->y * cD + q2->y * cT;
	q1->z = q1->z * cD + q2->z * cT;
	q1->w = q1->w * cD + q2->w * cT;

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

	q2->x = q1->x * cD + q2->x * cT;
	q2->y = q1->y * cD + q2->y * cT;
	q2->z = q1->z * cD + q2->z * cT;
	q2->w = q1->w * cD + q2->w * cT;

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

	r->x = q1->x * cD + q2->x * cT;
	r->y = q1->y * cD + q2->y * cT;
	r->z = q1->z * cD + q2->z * cT;
	r->w = q1->w * cD + q2->w * cT;

	quatNormalizeFastP(r);

}

__HINT_INLINE__ quat quatDifferentiate(const quat q, const vec3 w){
	const vec3 v = vec3VMultS(w, 0.5f);
	const quat r = {
		.x = v.x,
		.y = v.y,
		.z = v.z,
		.w = 0.f
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
	const quat r = {.x = w.x, .y = w.y, .z = w.z, .w = 0.f};
	return quatQAddQ(q, quatQMultS(quatQMultQ(r, q), dt*0.5f));
}
__HINT_INLINE__ void quatIntegrateP(quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ w, float dt){
	quat r = {.x = w->x, .y = w->y, .z = w->z, .w = 0.f};
	quatQMultQP1(&r, q);
	dt *= 0.5f;
	q->x = floatMA(q->x, r.x, dt);
	q->y = floatMA(q->y, r.y, dt);
	q->z = floatMA(q->z, r.z, dt);
	q->w = floatMA(q->w, r.w, dt);
}
__HINT_INLINE__ void quatIntegratePR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ w, float dt, quat *const __RESTRICT__ r){
	*((vec3 *)r) = *w;
	r->w = 0.f;
	quatQMultQP1(r, q);
	dt *= 0.5f;
	r->x = floatMA(q->x, r->x, dt);
	r->y = floatMA(q->y, r->y, dt);
	r->z = floatMA(q->z, r->z, dt);
	r->w = floatMA(q->w, r->w, dt);
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

vec3 quatBasisX(const quat q){
	// Rotate a (1, 0, 0)^T vector by the quaternion.
	const float x2 = 2.f * q.x;
	const float w2 = 2.f * q.w;
	const vec3 r = {
		.x = q.x * x2 + q.w * w2 - 1.f,
		.y = q.y * x2 + q.z * w2,
		.z = q.z * x2 - q.y * w2
	};
	return r;
}
void quatBasisXPR(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ r){
	// Rotate a (1, 0, 0)^T vector by the quaternion.
	const float x2 = 2.f * q->x;
	const float w2 = 2.f * q->w;
	r->x = q->x * x2 + q->w * w2 - 1.f;
	r->y = q->y * x2 + q->z * w2;
	r->z = q->z * x2 - q->y * w2;
}

vec3 quatBasisY(const quat q){
	// Rotate a (0, 1, 0)^T vector by the quaternion.
	const float y2 = 2.f * q.y;
	const float w2 = 2.f * q.w;
	const vec3 r = {
		.x = q.x * y2 - q.z * w2,
		.y = q.y * y2 + q.w * w2 - 1.f,
		.z = q.z * y2 + q.x * w2
	};
	return r;
}
void quatBasisYPR(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ r){
	// Rotate a (0, 1, 0)^T vector by the quaternion.
	const float y2 = 2.f * q->y;
	const float w2 = 2.f * q->w;
	r->x = q->x * y2 - q->z * w2;
	r->y = q->y * y2 + q->w * w2 - 1.f;
	r->z = q->z * y2 + q->x * w2;
}

vec3 quatBasisZ(const quat q){
	// Rotate a (0, 0, 1)^T vector by the quaternion.
	const float z2 = 2.f * q.z;
	const float w2 = 2.f * q.w;
	const vec3 r = {
		.x = q.x * z2 + q.y * w2,
		.y = q.y * z2 - q.x * w2,
		.z = q.z * z2 + q.w * w2 - 1.f
	};
	return r;
}
void quatBasisZPR(const quat *const __RESTRICT__ q, vec3 *const __RESTRICT__ r){
	// Rotate a (0, 0, 1)^T vector by the quaternion.
	const float z2 = 2.f * q->z;
	const float w2 = 2.f * q->w;
	r->x = q->x * z2 + q->y * w2;
	r->y = q->y * z2 - q->x * w2;
	r->z = q->z * z2 + q->w * w2 - 1.f;
}

quat quatTwist(const quat q, const vec3 v){

	// Decompose a quaternion into its twist component.
	///
	// This implementation negates twist when the dot product is
	// negative to ensure that it points in the same direction as "v".
	// We also check for and handle the singularity.
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	quat t;

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3Dot(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	float l = q.w*q.w + u*u;

	// Handle the singularity at twist rotations close to pi.
	if(l < QUAT_SINGULARITY_THRESHOLD_SQUARED){
		t = g_quatIdentity;
	}else{
		l = fastInvSqrt(l);
		u *= l;
		t.w = q.w*l;
		t.x = v.x*u;
		t.y = v.y*u;
		t.z = v.z*u;
	}

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	///if(u < 0.f){
		///quatNegateP(t);
	///}
	t.w = copySign(t.w, u);
	t.x = copySign(t.x, u);
	t.y = copySign(t.y, u);
	t.z = copySign(t.z, u);

	return t;

}
void quatTwistP(const quat q, const vec3 v, quat *const __RESTRICT__ t){

	// Decompose a quaternion into its twist component.
	///
	// This implementation negates twist when the dot product is
	// negative to ensure that it points in the same direction as "v".
	// We also check for and handle the singularity.
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3Dot(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	float l = q.w*q.w + u*u;

	// Handle the singularity at twist rotations close to pi.
	if(l < QUAT_SINGULARITY_THRESHOLD_SQUARED){
		*t = g_quatIdentity;
	}else{
		l = fastInvSqrt(l);
		u *= l;
		quatSet(t, q.w*l, v.x*u, v.y*u, v.z*u);
	}

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	///if(u < 0.f){
		///quatNegateP(t);
	///}
	t->w = copySign(t->w, u);
	t->x = copySign(t->x, u);
	t->y = copySign(t->y, u);
	t->z = copySign(t->z, u);

}
void quatTwistPR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, quat *const __RESTRICT__ t){

	// Decompose a quaternion into its twist component.
	///
	// This implementation negates twist when the dot product is
	// negative to ensure that it points in the same direction as "v".
	// We also check for and handle the singularity.
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotP(v, (const vec3 *)&q->x);
	// Quickly compute the inverse magnitude of the projection.
	float l = q->w*q->w + u*u;

	// Handle the singularity at twist rotations close to pi.
	if(l < QUAT_SINGULARITY_THRESHOLD_SQUARED){
		quatIdentityP(t);
	}else{
		l = fastInvSqrt(l);
		u *= l;
		quatSet(t, q->w*l, v->x*u, v->y*u, v->z*u);
	}

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	///if(u < 0.f){
		///quatNegateP(t);
	///}
	t->w = copySign(t->w, u);
	t->x = copySign(t->x, u);
	t->y = copySign(t->y, u);
	t->z = copySign(t->z, u);

}
quat quatTwistFast(const quat q, const vec3 v){

	// Decompose a quaternion into its twist component.
	///
	// This implementation negates twist when the dot product is
	// negative to ensure that it points in the same direction as "v".
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	quat t;

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3Dot(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	const float l = fastInvSqrt(q.w*q.w + u*u);
	u *= l;

	// Set the twist quaternion.
	t.w = q.w*l;
	t.x = v.x*u;
	t.y = v.y*u;
	t.z = v.z*u;

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	///if(u < 0.f){
		///quatNegateP(t);
	///}
	t.w = copySign(t.w, u);
	t.x = copySign(t.x, u);
	t.y = copySign(t.y, u);
	t.z = copySign(t.z, u);

	return t;

}
void quatTwistFastP(const quat q, const vec3 v, quat *const __RESTRICT__ t){

	// Decompose a quaternion into its twist component.
	///
	// This implementation negates twist when the dot product is
	// negative to ensure that it points in the same direction as "v".
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3Dot(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	const float l = fastInvSqrt(q.w*q.w + u*u);
	u *= l;

	// Set the twist quaternion.
	quatSet(t, q.w*l, v.x*u, v.y*u, v.z*u);

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	///if(u < 0.f){
		///quatNegateP(t);
	///}
	t->w = copySign(t->w, u);
	t->x = copySign(t->x, u);
	t->y = copySign(t->y, u);
	t->z = copySign(t->z, u);

}
void quatTwistFastPR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, quat *const __RESTRICT__ t){

	// Decompose a quaternion into its twist component.
	///
	// This implementation negates twist when the dot product is
	// negative to ensure that it points in the same direction as "v".
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotP(v, (const vec3 *)&q->x);
	// Quickly compute the inverse magnitude of the projection.
	const float l = fastInvSqrt(q->w*q->w + u*u);
	u *= l;

	// Set the twist quaternion.
	quatSet(t, q->w*l, v->x*u, v->y*u, v->z*u);

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	///if(u < 0.f){
		///quatNegateP(t);
	///}
	t->w = copySign(t->w, u);
	t->x = copySign(t->x, u);
	t->y = copySign(t->y, u);
	t->z = copySign(t->z, u);

}
quat quatTwistFaster(const quat q, const vec3 v){

	// Decompose a quaternion into its twist component.
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	quat t;

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3Dot(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	const float l = fastInvSqrt(q.w*q.w + u*u);
	u *= l;

	// Set the twist quaternion.
	t.w = q.w*l;
	t.x = v.x*u;
	t.y = v.y*u;
	t.z = v.z*u;

	return t;

}
void quatTwistFasterP(const quat q, const vec3 v, quat *const __RESTRICT__ t){

	// Decompose a quaternion into its twist component.
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3Dot(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	const float l = fastInvSqrt(q.w*q.w + u*u);
	u *= l;

	// Set the twist quaternion.
	quatSet(t, q.w*l, v.x*u, v.y*u, v.z*u);

}
void quatTwistFasterPR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, quat *const __RESTRICT__ t){

	// Decompose a quaternion into its twist component.
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotP(v, (const vec3 *)&q->x);
	// Quickly compute the inverse magnitude of the projection.
	const float l = fastInvSqrt(q->w*q->w + u*u);
	u *= l;

	// Set the twist quaternion.
	quatSet(t, q->w*l, v->x*u, v->y*u, v->z*u);

}

void quatSwingTwist(const quat q, const vec3 v, quat *const __RESTRICT__ t, quat *const __RESTRICT__ s){

	// Decompose a quaternion into its swing and twist components.
	// This results in a swing quaternion "s" and twist quaternion
	// "t" such that q = s*t. We also assume that the twist axis
	// "v" has been normalized.
	///
	// This implementation negates twist when the dot product is
	// negative to ensure that it points in the same direction as "v".
	// We also check for and handle the singularity.
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3Dot(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	float l = q.w*q.w + u*u;

	// Handle the singularity at twist rotations close to pi.
	if(l < QUAT_SINGULARITY_THRESHOLD_SQUARED){
		*t = g_quatIdentity;
	}else{
		l = fastInvSqrt(l);
		u *= l;
		quatSet(t, q.w*l, v.x*u, v.y*u, v.z*u);
	}

	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	*s = quatQMultQConjugate(q, *t);

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	///if(u < 0.f){
		///quatNegateP(t);
	///}
	t->w = copySign(t->w, u);
	t->x = copySign(t->x, u);
	t->y = copySign(t->y, u);
	t->z = copySign(t->z, u);

}
void quatSwingTwistPR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, quat *const __RESTRICT__ t, quat *const __RESTRICT__ s){

	// Decompose a quaternion into its swing and twist components.
	// This results in a swing quaternion "s" and twist quaternion
	// "t" such that q = s*t. We also assume that the twist axis
	// "v" has been normalized.
	///
	// This implementation negates twist when the dot product is
	// negative to ensure that it points in the same direction as "v".
	// We also check for and handle the singularity.
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotP(v, (const vec3 *)&q->x);
	// Quickly compute the inverse magnitude of the projection.
	float l = q->w*q->w + u*u;

	// Handle the singularity at twist rotations close to pi.
	if(l < QUAT_SINGULARITY_THRESHOLD_SQUARED){
		quatIdentityP(t);
	}else{
		l = fastInvSqrt(l);
		u *= l;
		quatSet(t, q->w*l, v->x*u, v->y*u, v->z*u);
	}

	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	quatQMultQConjugatePR(q, t, s);

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	///if(u < 0.f){
		///quatNegateP(t);
	///}
	t->w = copySign(t->w, u);
	t->x = copySign(t->x, u);
	t->y = copySign(t->y, u);
	t->z = copySign(t->z, u);

}
void quatSwingTwistFast(const quat q, const vec3 v, quat *const __RESTRICT__ t, quat *const __RESTRICT__ s){

	// Decompose a quaternion into its swing and twist components.
	// This results in a swing quaternion "s" and twist quaternion
	// "t" such that q = s*t. We also assume that the twist axis
	// "v" has been normalized.
	///
	// This implementation negates twist when the dot product is
	// negative to ensure that it points in the same direction as "v".
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3Dot(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	const float l = fastInvSqrt(q.w*q.w + u*u);
	u *= l;

	// Set the twist quaternion.
	quatSet(t, q.w*l, v.x*u, v.y*u, v.z*u);
	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	*s = quatQMultQConjugate(q, *t);

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	///if(u < 0.f){
		///quatNegateP(t);
	///}
	t->w = copySign(t->w, u);
	t->x = copySign(t->x, u);
	t->y = copySign(t->y, u);
	t->z = copySign(t->z, u);

}
void quatSwingTwistFastPR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, quat *const __RESTRICT__ t, quat *const __RESTRICT__ s){

	// Decompose a quaternion into its swing and twist components.
	// This results in a swing quaternion "s" and twist quaternion
	// "t" such that q = s*t. We also assume that the twist axis
	// "v" has been normalized.
	///
	// This implementation negates twist when the dot product is
	// negative to ensure that it points in the same direction as "v".
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotP(v, (const vec3 *)&q->x);
	// Quickly compute the inverse magnitude of the projection.
	const float l = fastInvSqrt(q->w*q->w + u*u);
	u *= l;

	// Set the twist quaternion.
	quatSet(t, q->w*l, v->x*u, v->y*u, v->z*u);
	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	quatQMultQConjugatePR(q, t, s);

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	///if(u < 0.f){
		///quatNegateP(t);
	///}
	t->w = copySign(t->w, u);
	t->x = copySign(t->x, u);
	t->y = copySign(t->y, u);
	t->z = copySign(t->z, u);

}
void quatSwingTwistFaster(const quat q, const vec3 v, quat *const __RESTRICT__ t, quat *const __RESTRICT__ s){

	// Decompose a quaternion into its swing and twist components.
	// This results in a swing quaternion "s" and twist quaternion
	// "t" such that q = s*t. We also assume that the twist axis
	// "v" has been normalized.
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3Dot(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	const float l = fastInvSqrt(q.w*q.w + u*u);
	u *= l;

	// Set the twist quaternion.
	quatSet(t, q.w*l, v.x*u, v.y*u, v.z*u);
	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	*s = quatQMultQConjugate(q, *t);

}
void quatSwingTwistFasterPR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ v, quat *const __RESTRICT__ t, quat *const __RESTRICT__ s){

	// Decompose a quaternion into its swing and twist components.
	// This results in a swing quaternion "s" and twist quaternion
	// "t" such that q = s*t. We also assume that the twist axis
	// "v" has been normalized.
	///
	// Based off Przemyslaw Dobrowolski's implementation given
	// in Swing-Twist Decomposition in Clifford Algebra (2015).

	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotP(v, (const vec3 *)&q->x);
	// Quickly compute the inverse magnitude of the projection.
	const float l = fastInvSqrt(q->w*q->w + u*u);
	u *= l;

	// Set the twist quaternion.
	quatSet(t, q->w*l, v->x*u, v->y*u, v->z*u);
	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	quatQMultQConjugatePR(q, t, s);

}

/**__HINT_INLINE__ quat quatMat4(const mat4 m){
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
}**/
