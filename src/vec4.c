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
__HINT_INLINE__ vec4 vec4Zero(){
	const vec4 r = {.x = 0.f,
	                .y = 0.f,
	                .z = 0.f,
	                .w = 0.f};
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
__HINT_INLINE__ void vec4ZeroP(vec4 *const restrict v){
	memset(v, 0, sizeof(vec4));
}

__HINT_INLINE__ vec4 vec4VAddV(const vec4 v1, const vec4 v2){
	const vec4 r = {.x = v1.x + v2.x,
	                .y = v1.y + v2.y,
	                .z = v1.z + v2.z,
	                .w = v1.w + v2.w};
	return r;
}
__HINT_INLINE__ vec4 vec4VAddN(const vec4 v, const float x, const float y, const float z, const float w){
	const vec4 r = {.x = v.x + x,
	                .y = v.y + y,
	                .z = v.z + z,
	                .w = v.w + w};
	return r;
}
__HINT_INLINE__ vec4 vec4VAddS(const vec4 v, const float s){
	const vec4 r = {.x = v.x + s,
	                .y = v.y + s,
	                .z = v.z + s,
	                .w = v.w + s};
	return r;
}
__HINT_INLINE__ void vec4VAddVP(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x += v2->x;
	v1->y += v2->y;
	v1->z += v2->z;
	v1->w += v2->w;
}
__HINT_INLINE__ void vec4VAddVPR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x + v2->x;
	r->y = v1->y + v2->y;
	r->z = v1->z + v2->z;
	r->w = v1->w + v2->w;
}
__HINT_INLINE__ void vec4VAddNP(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x += x;
	v->y += y;
	v->z += z;
	v->w += w;
}
__HINT_INLINE__ void vec4VAddSP(vec4 *const restrict v, const float s){
	v->x += s;
	v->y += s;
	v->z += s;
	v->w += s;
}


__HINT_INLINE__ vec4 vec4VSubV(const vec4 v1, const vec4 v2){
	const vec4 r = {.x = v1.x - v2.x,
	                .y = v1.y - v2.y,
	                .z = v1.z - v2.z,
	                .w = v1.w - v2.w};
	return r;
}
__HINT_INLINE__ vec4 vec4VSubN(const vec4 v, const float x, const float y, const float z, const float w){
	const vec4 r = {.x = v.x - x,
	                .y = v.y - y,
	                .z = v.z - z,
	                .w = v.w - w};
	return r;
}
__HINT_INLINE__ vec4 vec4VSubS(const vec4 v, const float s){
	const vec4 r = {.x = v.x - s,
	                .y = v.y - s,
	                .z = v.z - s,
	                .w = v.w - s};
	return r;
}
__HINT_INLINE__ vec4 vec4NSubV(const float x, const float y, const float z, const float w, const vec4 v){
	const vec4 r = {.x = x - v.x,
	                .y = y - v.y,
	                .z = z - v.z,
	                .w = w - v.w};
	return r;
}
__HINT_INLINE__ vec4 vec4SSubV(const float s, const vec4 v){
	const vec4 r = {.x = s - v.x,
	                .y = s - v.y,
	                .z = s - v.z,
	                .w = s - v.w};
	return r;
}
__HINT_INLINE__ void vec4VSubVP1(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x -= v2->x;
	v1->y -= v2->y;
	v1->z -= v2->z;
	v1->w -= v2->w;
}
__HINT_INLINE__ void vec4VSubVP2(const vec4 *const restrict v1, vec4 *const restrict v2){
	v2->x = v1->x - v2->x;
	v2->y = v1->y - v2->y;
	v2->z = v1->z - v2->z;
	v2->w = v1->w - v2->w;
}
__HINT_INLINE__ void vec4VSubVPR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x - v2->x;
	r->y = v1->y - v2->y;
	r->z = v1->z - v2->z;
	r->w = v1->w - v2->w;
}
__HINT_INLINE__ void vec4VSubNP(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x -= x;
	v->y -= y;
	v->z -= z;
	v->w -= w;
}
__HINT_INLINE__ void vec4VSubSP(vec4 *const restrict v, const float s){
	v->x -= s;
	v->y -= s;
	v->z -= s;
	v->w -= s;
}
__HINT_INLINE__ void vec4VSubVP(const float x, const float y, const float z, const float w, vec4 *const restrict v){
	v->x = x - v->x;
	v->y = y - v->y;
	v->z = z - v->z;
	v->w = w - v->w;
}
__HINT_INLINE__ void vec4SSubVP(const float s, vec4 *const restrict v){
	v->x = s - v->x;
	v->y = s - v->y;
	v->z = s - v->z;
	v->w = s - v->w;
}

__HINT_INLINE__ vec4 vec4VMultV(const vec4 v1, const vec4 v2){
	const vec4 r = {.x = v1.x * v2.x,
	                .y = v1.y * v2.y,
	                .z = v1.z * v2.z,
	                .w = v1.w * v2.w};
	return r;
}
__HINT_INLINE__ vec4 vec4VMultN(const vec4 v, const float x, const float y, const float z, const float w){
	const vec4 r = {.x = v.x * x,
	                .y = v.y * y,
	                .z = v.z * z,
	                .w = v.w * w};
	return r;
}
__HINT_INLINE__ vec4 vec4VMultS(const vec4 v, const float s){
	const vec4 r = {.x = v.x * s,
	                .y = v.y * s,
	                .z = v.z * s,
	                .w = v.w * s};
	return r;
}
__HINT_INLINE__ void vec4VMultVP(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x *= v2->x;
	v1->y *= v2->y;
	v1->z *= v2->z;
	v1->w *= v2->w;
}
__HINT_INLINE__ void vec4VMultVPR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x * v2->x;
	r->y = v1->y * v2->y;
	r->z = v1->z * v2->z;
	r->w = v1->w * v2->w;
}
__HINT_INLINE__ void vec4VMultNP(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x *= x;
	v->y *= y;
	v->z *= z;
	v->w *= w;
}
__HINT_INLINE__ void vec4VMultSP(vec4 *const restrict v, const float s){
	v->x *= s;
	v->y *= s;
	v->z *= s;
	v->w *= s;
}

__HINT_INLINE__ vec4 vec4VDivV(const vec4 v1, const vec4 v2){
	const vec4 r = {.x = v1.x / v2.x,
	                .y = v1.y / v2.y,
	                .z = v1.z / v2.z,
	                .w = v1.w / v2.w};
	return r;
}
__HINT_INLINE__ vec4 vec4VDivN(const vec4 v, const float x, const float y, const float z, const float w){
	const vec4 r = {.x = v.x / x,
	                .y = v.y / y,
	                .z = v.z / z,
	                .w = v.w / w};
	return r;
}
__HINT_INLINE__ vec4 vec4VDivS(const vec4 v, const float s){
	const float invS = 1.f / s;
	const vec4 r = {.x = v.x * invS,
	                .y = v.y * invS,
	                .z = v.z * invS,
	                .w = v.w * invS};
	return r;
}
__HINT_INLINE__ vec4 vec4NDivV(const float x, const float y, const float z, const float w, const vec4 v){
	const vec4 r = {.x = x / v.x,
	                .y = y / v.y,
	                .z = z / v.z,
	                .w = w / v.w};
	return r;
}
__HINT_INLINE__ vec4 vec4SDivV(const float s, const vec4 v){
	const vec4 r = {.x = s / v.x,
	                .y = s / v.y,
	                .z = s / v.z,
	                .w = s / v.w};
	return r;
}
__HINT_INLINE__ void vec4VDivVP1(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x /= v2->x;
	v1->y /= v2->y;
	v1->z /= v2->z;
	v1->w /= v2->w;
}
__HINT_INLINE__ void vec4VDivVP2(const vec4 *const restrict v1, vec4 *const restrict v2){
	v2->x = v1->x / v2->x;
	v2->y = v1->y / v2->y;
	v2->z = v1->z / v2->z;
	v2->w = v1->w / v2->w;

}
__HINT_INLINE__ void vec4VDivVPR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x / v2->x;
	r->y = v1->y / v2->y;
	r->z = v1->z / v2->z;
	r->w = v1->w / v2->w;

}
__HINT_INLINE__ void vec4VDivPN(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x /= x;
	v->y /= y;
	v->z /= z;
	v->w /= w;
}
__HINT_INLINE__ void vec4VDivSP(vec4 *const restrict v, const float s){
	const float invS = 1.f / s;
	v->x *= invS;
	v->y *= invS;
	v->z *= invS;
	v->w *= invS;
}
__HINT_INLINE__ void vec4NDivVP(const float x, const float y, const float z, const float w, vec4 *const restrict v){
	v->x = x / v->x;
	v->y = y / v->y;
	v->z = z / v->z;
	v->w = w / v->w;
}
__HINT_INLINE__ void vec4SDivVP(const float s, vec4 *const restrict v){
	v->x = s / v->x;
	v->y = s / v->y;
	v->z = s / v->z;
	v->w = s / v->w;
}

__HINT_INLINE__ vec4 vec4Min(const vec4 v1, const vec4 v2){
	const vec4 r = {.x = v1.x <= v2.x ? v1.x : v2.x,
	                .y = v1.y <= v2.y ? v1.y : v2.y,
	                .z = v1.z <= v2.z ? v1.z : v2.z,
					.w = v1.w <= v2.w ? v1.w : v2.w};
	return r;
}
__HINT_INLINE__ void vec4MinP(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x <= v2->x ? v1->x : v2->x;
	r->y = v1->y <= v2->y ? v1->y : v2->y;
	r->z = v1->z <= v2->z ? v1->z : v2->z;
	r->w = v1->w <= v2->w ? v1->w : v2->w;
}
__HINT_INLINE__ vec4 vec4Max(const vec4 v1, const vec4 v2){
	const vec4 r = {.x = v1.x >= v2.x ? v1.x : v2.x,
	                .y = v1.y >= v2.y ? v1.y : v2.y,
	                .z = v1.z >= v2.z ? v1.z : v2.z,
					.w = v1.w >= v2.w ? v1.w : v2.w};
	return r;
}
__HINT_INLINE__ void vec4MaxP(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x >= v2->x ? v1->x : v2->x;
	r->y = v1->y >= v2->y ? v1->y : v2->y;
	r->z = v1->z >= v2->z ? v1->z : v2->z;
	r->w = v1->w >= v2->w ? v1->w : v2->w;
}

__HINT_INLINE__ vec4 vec4Negate(const vec4 v){
	const vec4 r = {.x = -v.x,
	                .y = -v.y,
	                .z = -v.z,
					.w = -v.w};
	return r;
}
__HINT_INLINE__ void vec4NegateP(vec4 *const restrict v){
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
	v->w = -v->w;
}
__HINT_INLINE__ void vec4NegatePR(const vec4 *const restrict v, vec4 *const restrict r){
	r->x = -v->x;
	r->y = -v->y;
	r->z = -v->z;
	r->w = -v->w;
}

__HINT_INLINE__ float vec4Magnitude(const vec4 v){
	return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}
__HINT_INLINE__ float vec4MagnitudeP(const vec4 *const restrict v){
	return sqrtf(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
}
__HINT_INLINE__ float vec4MagnitudeSquared(const vec4 v){
	return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
}
__HINT_INLINE__ float vec4MagnitudeSquaredP(const vec4 *const restrict v){
	return v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
}
__HINT_INLINE__ float vec4MagnitudeInverse(const vec4 v){
	return 1.f/sqrtf(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}
__HINT_INLINE__ float vec4MagnitudeInverseP(const vec4 *const restrict v){
	return 1.f/sqrtf(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
}
__HINT_INLINE__ float vec4MagnitudeInverseFast(const vec4 v){
	return rsqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}
__HINT_INLINE__ float vec4MagnitudeInverseFastP(const vec4 *const restrict v){
	return rsqrt(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
}
__HINT_INLINE__ float vec4MagnitudeInverseFastAccurate(const vec4 v){
	return rsqrtAccurate(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}
__HINT_INLINE__ float vec4MagnitudeInverseFastAccurateP(const vec4 *const restrict v){
	return rsqrtAccurate(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
}

__HINT_INLINE__ vec4 vec4Normalize(const vec4 v){
	return vec4VMultS(v, vec4MagnitudeInverse(v));
}
__HINT_INLINE__ vec4 vec4NormalizeFast(const vec4 v){
	return vec4VMultS(v, vec4MagnitudeInverseFast(v));
}
__HINT_INLINE__ vec4 vec4NormalizeFastAccurate(const vec4 v){
	return vec4VMultS(v, vec4MagnitudeInverseFastAccurate(v));
}
__HINT_INLINE__ void vec4NormalizeP(vec4 *const restrict v){
	vec4VMultSP(v, vec4MagnitudeInverseP(v));
}
__HINT_INLINE__ void vec4NormalizeFastP(vec4 *const restrict v){
	vec4VMultSP(v, vec4MagnitudeInverseFastP(v));
}
__HINT_INLINE__ void vec4NormalizeFastAccurateP(vec4 *const restrict v){
	vec4VMultSP(v, vec4MagnitudeInverseFastAccurateP(v));
}

__HINT_INLINE__ float vec4Dot(const vec4 v1, const vec4 v2){
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}
__HINT_INLINE__ float vec4DotP(const vec4 *const restrict v1, const vec4 *const restrict v2){
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w;
}

__HINT_INLINE__ vec4 vec4Lerp(const vec4 v1, const vec4 v2, const float t){
	// r = v1 + (v2 - v1) * t
	const vec4 r = {.x = v1.x + (v2.x - v1.x) * t,
	                .y = v1.y + (v2.y - v1.y) * t,
	                .z = v1.z + (v2.z - v1.z) * t,
	                .w = v1.w + (v2.w - v1.w) * t};
	return r;
}
__HINT_INLINE__ void vec4LerpP1(vec4 *const restrict v1, const vec4 *const restrict v2, const float t){
	// r = v1 + (v2 - v1) * t
	v1->x += (v2->x - v1->x) * t;
	v1->y += (v2->y - v1->y) * t;
	v1->z += (v2->z - v1->z) * t;
	v1->w += (v2->w - v1->w) * t;
}
__HINT_INLINE__ void vec4LerpP2(const vec4 *const restrict v1, vec4 *const restrict v2, const float t){
	// r = v1 + (v2 - v1) * t
	v2->x = v1->x + (v2->x - v1->x) * t;
	v2->y = v1->y + (v2->y - v1->y) * t;
	v2->z = v1->z + (v2->z - v1->z) * t;
	v2->w = v1->w + (v2->w - v1->w) * t;
}
__HINT_INLINE__ void vec4LerpPR(const vec4 *const restrict v1, const vec4 *const restrict v2, const float t, vec4 *const restrict r){
	// r = v1 + (v2 - v1) * t
	r->x = v1->x + (v2->x - v1->x) * t;
	r->y = v1->y + (v2->y - v1->y) * t;
	r->z = v1->z + (v2->z - v1->z) * t;
	r->w = v1->w + (v2->w - v1->w) * t;
}