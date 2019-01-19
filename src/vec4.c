#include "vec4.h"
#include "helpersMath.h"
#include "inline.h"
#include <math.h>
#include <string.h>

__HINT_INLINE__ vec4 vec4New(const float x, const float y, const float z, const float w){
	const vec4 r = {.x = x,
	                .y = y,
	                .z = z,
	                .w = w};
	return r;
}
__HINT_INLINE__ vec4 vec4NewS(const float s){
	const vec4 r = {.x = s,
	                .y = s,
	                .z = s,
	                .w = s};
	return r;
}
__HINT_INLINE__ void vec4Set(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x = x;
	v->y = y;
	v->z = z;
	v->w = w;
}
__HINT_INLINE__ void vec4SetS(vec4 *const restrict v, const float s){
	v->x = s;
	v->y = s;
	v->z = s;
	v->w = s;
}
__HINT_INLINE__ void vec4Zero(vec4 *const restrict v){
	memset(v, 0, sizeof(vec4));
}

__HINT_INLINE__ vec4 vec4VAddV(const vec4 *const restrict v1, const vec4 *const restrict v2){
	const vec4 r = {.x = v1->x + v2->x,
	                .y = v1->y + v2->y,
	                .z = v1->z + v2->z,
	                .w = v1->w + v2->w};
	return r;
}
__HINT_INLINE__ vec4 vec4VAddN(const vec4 *const restrict v, const float x, const float y, const float z, const float w){
	const vec4 r = {.x = v->x + x,
	                .y = v->y + y,
	                .z = v->z + z,
	                .w = v->w + w};
	return r;
}
__HINT_INLINE__ vec4 vec4VAddS(const vec4 *const restrict v, const float s){
	const vec4 r = {.x = v->x + s,
	                .y = v->y + s,
	                .z = v->z + s,
	                .w = v->w + s};
	return r;
}
__HINT_INLINE__ void vec4AddVToV(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x += v2->x;
	v1->y += v2->y;
	v1->z += v2->z;
	v1->w += v2->w;
}
__HINT_INLINE__ void vec4AddVToVR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x + v2->x;
	r->y = v1->y + v2->y;
	r->z = v1->z + v2->z;
	r->w = v1->w + v2->w;
}
__HINT_INLINE__ void vec4AddNToV(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x += x;
	v->y += y;
	v->z += z;
	v->w += w;
}
__HINT_INLINE__ void vec4AddSToV(vec4 *const restrict v, const float s){
	v->x += s;
	v->y += s;
	v->z += s;
	v->w += s;
}

__HINT_INLINE__ vec4 vec4VSubV(const vec4 *const restrict v1, const vec4 *const restrict v2){
	const vec4 r = {.x = v1->x - v2->x,
	                .y = v1->y - v2->y,
	                .z = v1->z - v2->z,
	                .w = v1->w - v2->w};
	return r;
}
__HINT_INLINE__ vec4 vec4VSubN(const vec4 *const restrict v, const float x, const float y, const float z, const float w){
	const vec4 r = {.x = v->x - x,
	                .y = v->y - y,
	                .z = v->z - z,
	                .w = v->w - w};
	return r;
}
__HINT_INLINE__ vec4 vec4VSubS(const vec4 *const restrict v, const float s){
	const vec4 r = {.x = v->x - s,
	                .y = v->y - s,
	                .z = v->z - s,
	                .w = v->w - s};
	return r;
}
__HINT_INLINE__ vec4 vec4NSubV(const float x, const float y, const float z, const float w, const vec4 *const restrict v){
	const vec4 r = {.x = x - v->x,
	                .y = y - v->y,
	                .z = z - v->z,
	                .w = w - v->w};
	return r;
}
__HINT_INLINE__ vec4 vec4SSubV(const float s, const vec4 *const restrict v){
	const vec4 r = {.x = s - v->x,
	                .y = s - v->y,
	                .z = s - v->z,
	                .w = s - v->w};
	return r;
}
__HINT_INLINE__ void vec4SubVFromV1(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x -= v2->x;
	v1->y -= v2->y;
	v1->z -= v2->z;
	v1->w -= v2->w;
}
__HINT_INLINE__ void vec4SubVFromV2(const vec4 *const restrict v1, vec4 *const restrict v2){
	v2->x = v1->x - v2->x;
	v2->y = v1->y - v2->y;
	v2->z = v1->z - v2->z;
	v2->w = v1->w - v2->w;
}
__HINT_INLINE__ void vec4SubVFromVR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x - v2->x;
	r->y = v1->y - v2->y;
	r->z = v1->z - v2->z;
	r->w = v1->w - v2->w;
}
__HINT_INLINE__ void vec4SubNFromV(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x -= x;
	v->y -= y;
	v->z -= z;
	v->w -= w;
}
__HINT_INLINE__ void vec4SubSFromV(vec4 *const restrict v, const float s){
	v->x -= s;
	v->y -= s;
	v->z -= s;
	v->w -= s;
}
__HINT_INLINE__ void vec4SubVFromN(const float x, const float y, const float z, const float w, vec4 *const restrict v){
	v->x = x - v->x;
	v->y = y - v->y;
	v->z = z - v->z;
	v->w = w - v->w;
}
__HINT_INLINE__ void vec4SubVFromS(const float s, vec4 *const restrict v){
	v->x = s - v->x;
	v->y = s - v->y;
	v->z = s - v->z;
	v->w = s - v->w;
}

__HINT_INLINE__ vec4 vec4VMultV(const vec4 *const restrict v1, const vec4 *const restrict v2){
	const vec4 r = {.x = v1->x * v2->x,
	                .y = v1->y * v2->y,
	                .z = v1->z * v2->z,
	                .w = v1->w * v2->w};
	return r;
}
__HINT_INLINE__ vec4 vec4VMultN(const vec4 *const restrict v, const float x, const float y, const float z, const float w){
	const vec4 r = {.x = v->x * x,
	                .y = v->y * y,
	                .z = v->z * z,
	                .w = v->w * w};
	return r;
}
__HINT_INLINE__ vec4 vec4VMultS(const vec4 *const restrict v, const float s){
	const vec4 r = {.x = v->x * s,
	                .y = v->y * s,
	                .z = v->z * s,
	                .w = v->w * s};
	return r;
}
__HINT_INLINE__ void vec4MultVByV(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x *= v2->x;
	v1->y *= v2->y;
	v1->z *= v2->z;
	v1->w *= v2->w;
}
__HINT_INLINE__ void vec4MultVByVR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x * v2->x;
	r->y = v1->y * v2->y;
	r->z = v1->z * v2->z;
	r->w = v1->w * v2->w;
}
__HINT_INLINE__ void vec4MultVByN(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x *= x;
	v->y *= y;
	v->z *= z;
	v->w *= w;
}
__HINT_INLINE__ void vec4MultVByS(vec4 *const restrict v, const float s){
	v->x *= s;
	v->y *= s;
	v->z *= s;
	v->w *= s;
}

__HINT_INLINE__ vec4 vec4VDivV(const vec4 *const restrict v1, const vec4 *const restrict v2){
	const vec4 r = {.x = v1->x / v2->x,
	                .y = v1->y / v2->y,
	                .z = v1->z / v2->z,
	                .w = v1->w / v2->w};
	return r;
}
__HINT_INLINE__ vec4 vec4VDivN(const vec4 *const restrict v, const float x, const float y, const float z, const float w){
	const vec4 r = {.x = v->x / x,
	                .y = v->y / y,
	                .z = v->z / z,
	                .w = v->w / w};
	return r;
}
__HINT_INLINE__ vec4 vec4VDivS(const vec4 *const restrict v, const float s){
	const float invS = 1.f / s;
	const vec4 r = {.x = v->x * invS,
	                .y = v->y * invS,
	                .z = v->z * invS,
	                .w = v->w * invS};
	return r;
}
__HINT_INLINE__ vec4 vec4NDivV(const float x, const float y, const float z, const float w, const vec4 *const restrict v){
	const vec4 r = {.x = x / v->x,
	                .y = y / v->y,
	                .z = z / v->z,
	                .w = w / v->w};
	return r;
}
__HINT_INLINE__ vec4 vec4SDivV(const float s, const vec4 *const restrict v){
	const vec4 r = {.x = s / v->x,
	                .y = s / v->y,
	                .z = s / v->z,
	                .w = s / v->w};
	return r;
}
__HINT_INLINE__ void vec4DivVByV1(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x /= v2->x;
	v1->y /= v2->y;
	v1->z /= v2->z;
	v1->w /= v2->w;
}
__HINT_INLINE__ void vec4DivVByV2(const vec4 *const restrict v1, vec4 *const restrict v2){
	v2->x = v1->x / v2->x;
	v2->y = v1->y / v2->y;
	v2->z = v1->z / v2->z;
	v2->w = v1->w / v2->w;

}
__HINT_INLINE__ void vec4DivVByVR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x / v2->x;
	r->y = v1->y / v2->y;
	r->z = v1->z / v2->z;
	r->w = v1->w / v2->w;

}
__HINT_INLINE__ void vec4DivVByN(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x /= x;
	v->y /= y;
	v->z /= z;
	v->w /= w;
}
__HINT_INLINE__ void vec4DivVByS(vec4 *const restrict v, const float s){
	const float invS = 1.f / s;
	v->x *= invS;
	v->y *= invS;
	v->z *= invS;
	v->w *= invS;
}
__HINT_INLINE__ void vec4DivNByV(const float x, const float y, const float z, const float w, vec4 *const restrict v){
	v->x = x / v->x;
	v->y = y / v->y;
	v->z = z / v->z;
	v->w = w / v->w;
}
__HINT_INLINE__ void vec4DivSByV(const float s, vec4 *const restrict v){
	v->x = s / v->x;
	v->y = s / v->y;
	v->z = s / v->z;
	v->w = s / v->w;
}

__HINT_INLINE__ void vec4Min(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x <= v2->x ? v1->x : v2->x;
	r->y = v1->y <= v2->y ? v1->y : v2->y;
	r->z = v1->z <= v2->z ? v1->z : v2->z;
	r->w = v1->w <= v2->w ? v1->w : v2->w;
}
__HINT_INLINE__ void vec4Max(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x >= v2->x ? v1->x : v2->x;
	r->y = v1->y >= v2->y ? v1->y : v2->y;
	r->z = v1->z >= v2->z ? v1->z : v2->z;
	r->w = v1->w >= v2->w ? v1->w : v2->w;
}

__HINT_INLINE__ void vec4Negate(vec4 *const restrict v){
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
	v->w = -v->w;
}
__HINT_INLINE__ void vec4NegateR(const vec4 *const restrict v, vec4 *const restrict r){
	r->x = -v->x;
	r->y = -v->y;
	r->z = -v->z;
	r->w = -v->w;
}

__HINT_INLINE__ float vec4Magnitude(const vec4 *const restrict v){
	return sqrtf(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
}
__HINT_INLINE__ float vec4MagnitudeSquared(const vec4 *const restrict v){
	return v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
}
__HINT_INLINE__ float vec4InverseMagnitude(const vec4 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
	return fastInvSqrt(magnitudeSquared);
}

__HINT_INLINE__ vec4 vec4GetUnit(const vec4 *const restrict v){
	return vec4VMultS(v, 1.f/vec4Magnitude(v));
}
__HINT_INLINE__ vec4 vec4GetUnitFast(const vec4 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	return vec4VMultS(v, invSqrt);
}
__HINT_INLINE__ vec4 vec4GetUnitFastAccurate(const vec4 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
	const float invSqrt = fastInvSqrtAccurate(magnitudeSquared);
	return vec4VMultS(v, invSqrt);
}
__HINT_INLINE__ void vec4Normalize(vec4 *const restrict v){
	vec4MultVByS(v, 1.f/vec4Magnitude(v));
}
__HINT_INLINE__ void vec4NormalizeFast(vec4 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	vec4MultVByS(v, invSqrt);
}
__HINT_INLINE__ void vec4NormalizeFastAccurate(vec4 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
	const float invSqrt = fastInvSqrtAccurate(magnitudeSquared);
	vec4MultVByS(v, invSqrt);
}

__HINT_INLINE__ float vec4Dot(const vec4 *const restrict v1, const vec4 *const restrict v2){
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w;
}

__HINT_INLINE__ void vec4Lerp1(vec4 *const restrict v1, const vec4 *const restrict v2, const float t){
	/*
	** r = v1 + (v2 - v1) * t
	*/
	v1->x += (v2->x - v1->x) * t;
	v1->y += (v2->y - v1->y) * t;
	v1->z += (v2->z - v1->z) * t;
	v1->w += (v2->w - v1->w) * t;
}

__HINT_INLINE__ void vec4Lerp2(const vec4 *const restrict v1, vec4 *const restrict v2, const float t){
	/*
	** r = v1 + (v2 - v1) * t
	*/
	v2->x = v1->x + (v2->x - v1->x) * t;
	v2->y = v1->y + (v2->y - v1->y) * t;
	v2->z = v1->z + (v2->z - v1->z) * t;
	v2->w = v1->w + (v2->w - v1->w) * t;
}

__HINT_INLINE__ void vec4LerpR(const vec4 *const restrict v1, const vec4 *const restrict v2, const float t, vec4 *const restrict r){
	/*
	** r = v1 + (v2 - v1) * t
	*/
	r->x = v1->x + (v2->x - v1->x) * t;
	r->y = v1->y + (v2->y - v1->y) * t;
	r->z = v1->z + (v2->z - v1->z) * t;
	r->w = v1->w + (v2->w - v1->w) * t;
}