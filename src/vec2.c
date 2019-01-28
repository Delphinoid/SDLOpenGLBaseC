#include "vec2.h"
#include "helpersMath.h"
#include "inline.h"
#include <math.h>
#include <string.h>

__HINT_INLINE__ vec2 vec2New(const float x, const float y){
	const vec2 r = {.x = x, .y = y};
	return r;
}
__HINT_INLINE__ vec2 vec2NewS(const float s){
	const vec2 r = {.x = s, .y = s};
	return r;
}
__HINT_INLINE__ void vec2Set(vec2 *const restrict v, const float x, const float y){
	v->x = x;
	v->y = y;
}
__HINT_INLINE__ void vec2SetS(vec2 *const restrict v, const float s){
	v->x = s;
	v->y = s;
}
__HINT_INLINE__ void vec2Zero(vec2 *const restrict v){
	memset(v, 0, sizeof(vec2));
}

__HINT_INLINE__ vec2 vec2VAddV(const vec2 *const restrict v1, const vec2 *const restrict v2){
	const vec2 r = {.x = v1->x + v2->x,
	                .y = v1->y + v2->y};
	return r;
}
__HINT_INLINE__ vec2 vec2VAddN(const vec2 *const restrict v, const float x, const float y){
	const vec2 r = {.x = v->x + x,
	                .y = v->y + y};
	return r;
}
__HINT_INLINE__ vec2 vec2VAddS(const vec2 *const restrict v, const float s){
	const vec2 r = {.x = v->x + s,
	                .y = v->y + s};
	return r;
}
__HINT_INLINE__ void vec2AddVToV(vec2 *const restrict v1, const vec2 *const restrict v2){
	v1->x += v2->x;
	v1->y += v2->y;
}
__HINT_INLINE__ void vec2AddVToVR(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r){
	r->x = v1->x + v2->x;
	r->y = v1->y + v2->y;
}
__HINT_INLINE__ void vec2AddNToV(vec2 *const restrict v, const float x, const float y){
	v->x += x;
	v->y += y;
}
__HINT_INLINE__ void vec2AddSToV(vec2 *const restrict v, const float s){
	v->x += s;
	v->y += s;
}

__HINT_INLINE__ vec2 vec2VSubV(const vec2 *const restrict v1, const vec2 *const restrict v2){
	const vec2 r = {.x = v1->x - v2->x,
	                .y = v1->y - v2->y};
	return r;
}
__HINT_INLINE__ vec2 vec2VSubN(const vec2 *const restrict v, const float x, const float y){
	const vec2 r = {.x = v->x - x,
	                .y = v->y - y};
	return r;
}
__HINT_INLINE__ vec2 vec2VSubS(const vec2 *const restrict v, const float s){
	const vec2 r = {.x = v->x - s,
	                .y = v->y - s};
	return r;
}
__HINT_INLINE__ vec2 vec2NSubV(const float x, const float y, const vec2 *const restrict v){
	const vec2 r = {.x = x - v->x,
	                .y = y - v->y};
	return r;
}
__HINT_INLINE__ vec2 vec2SSubV(const float s, const vec2 *const restrict v){
	const vec2 r = {.x = s - v->x,
	                .y = s - v->y};
	return r;
}
__HINT_INLINE__ void vec2SubVFromV1(vec2 *const restrict v1, const vec2 *const restrict v2){
	v1->x -= v2->x;
	v1->y -= v2->y;
}
__HINT_INLINE__ void vec2SubVFromV2(const vec2 *const restrict v1, vec2 *const restrict v2){
	v2->x = v1->x - v2->x;
	v2->y = v1->y - v2->y;
}
__HINT_INLINE__ void vec2SubVFromVR(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r){
	r->x = v1->x - v2->x;
	r->y = v1->y - v2->y;
}
__HINT_INLINE__ void vec2SubNFromV(vec2 *const restrict v, const float x, const float y){
	v->x -= x;
	v->y -= y;
}
__HINT_INLINE__ void vec2SubSFromV(vec2 *const restrict v, const float s){
	v->x -= s;
	v->y -= s;
}
__HINT_INLINE__ void vec2SubVFromN(const float x, const float y, vec2 *const restrict v){
	v->x = x - v->x;
	v->y = y - v->y;
}
__HINT_INLINE__ void vec2SubVFromS(const float s, vec2 *const restrict v){
	v->x = s - v->x;
	v->y = s - v->y;
}

__HINT_INLINE__ vec2 vec2VMultV(const vec2 *const restrict v1, const vec2 *const restrict v2){
	const vec2 r = {.x = v1->x * v2->x,
	                .y = v1->y * v2->y};
	return r;
}
__HINT_INLINE__ vec2 vec2VMultN(const vec2 *const restrict v, const float x, const float y){
	const vec2 r = {.x = v->x * x,
	                .y = v->y * y};
	return r;
}
__HINT_INLINE__ vec2 vec2VMultS(const vec2 *const restrict v, const float s){
	const vec2 r = {.x = v->x * s,
	                .y = v->y * s};
	return r;
}
__HINT_INLINE__ void vec2MultVByV(vec2 *const restrict v1, const vec2 *const restrict v2){
	v1->x *= v2->x;
	v1->y *= v2->y;
}
__HINT_INLINE__ void vec2MultVByVR(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r){
	r->x = v1->x * v2->x;
	r->y = v1->y * v2->y;
}
__HINT_INLINE__ void vec2MultVByN(vec2 *const restrict v, const float x, const float y){
	v->x *= x;
	v->y *= y;
}
__HINT_INLINE__ void vec2MultVByS(vec2 *const restrict v, const float s){
	v->x *= s;
	v->y *= s;
}

__HINT_INLINE__ vec2 vec2VDivV(const vec2 *const restrict v1, const vec2 *const restrict v2){
	const vec2 r = {.x = v1->x / v2->x,
	                .y = v1->y / v2->y};
	return r;
}
__HINT_INLINE__ vec2 vec2VDivN(const vec2 *const restrict v, const float x, const float y){
	const vec2 r = {.x = v->x / x,
	                .y = v->y / y};
	return r;
}
__HINT_INLINE__ vec2 vec2VDivS(const vec2 *const restrict v, const float s){
	const float invS = 1.f / s;
	const vec2 r = {.x = v->x * invS,
	                .y = v->y * invS};
	return r;
}
__HINT_INLINE__ vec2 vec2NDivV(const float x, const float y, const vec2 *const restrict v){
	const vec2 r = {.x = x / v->x,
	                .y = y / v->y};
	return r;
}
__HINT_INLINE__ vec2 vec2SDivV(const float s, const vec2 *const restrict v){
	const vec2 r = {.x = s / v->x,
	                .y = s / v->y};
	return r;
}
__HINT_INLINE__ void vec2DivVByV1(vec2 *const restrict v1, const vec2 *const restrict v2){
	v1->x /= v2->x;
	v1->y /= v2->y;
}
__HINT_INLINE__ void vec2DivVByV2(const vec2 *const restrict v1, vec2 *const restrict v2){
	v2->x = v1->x / v2->x;
	v2->y = v1->y / v2->y;

}
__HINT_INLINE__ void vec2DivVByVR(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r){
	r->x = v1->x / v2->x;
	r->y = v1->y / v2->y;

}
__HINT_INLINE__ void vec2DivVByN(vec2 *const restrict v, const float x, const float y){
	v->x /= x;
	v->y /= y;
}
__HINT_INLINE__ void vec2DivVByS(vec2 *const restrict v, const float s){
	const float invS = 1.f / s;
	v->x *= invS;
	v->y *= invS;
}
__HINT_INLINE__ void vec2DivNByV(const float x, const float y, vec2 *const restrict v){
	v->x = x / v->x;
	v->y = y / v->y;
}
__HINT_INLINE__ void vec2DivSByV(const float s, vec2 *const restrict v){
	v->x = s / v->x;
	v->y = s / v->y;
}

__HINT_INLINE__ void vec2Min(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r){
	r->x = v1->x <= v2->x ? v1->x : v2->x;
	r->y = v1->y <= v2->y ? v1->y : v2->y;
}
__HINT_INLINE__ void vec2Max(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r){
	r->x = v1->x >= v2->x ? v1->x : v2->x;
	r->y = v1->y >= v2->y ? v1->y : v2->y;
}

__HINT_INLINE__ void vec2Negate(vec2 *const restrict v){
	v->x = -v->x;
	v->y = -v->y;
}
__HINT_INLINE__ void vec2NegateR(const vec2 *const restrict v, vec2 *const restrict r){
	r->x = -v->x;
	r->y = -v->y;
}

__HINT_INLINE__ float vec2Magnitude(const vec2 *const restrict v){
	return sqrtf(v->x*v->x + v->y*v->y);
}
__HINT_INLINE__ float vec2MagnitudeSquared(const vec2 *const restrict v){
	return v->x*v->x + v->y*v->y;
}
__HINT_INLINE__ float vec2InverseMagnitude(const vec2 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y;
	return fastInvSqrt(magnitudeSquared);
}

__HINT_INLINE__ vec2 vec2GetUnit(const vec2 *const restrict v){
	return vec2VMultS(v, 1.f/vec2Magnitude(v));
}
__HINT_INLINE__ vec2 vec2GetUnitFast(const vec2 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	return vec2VMultS(v, invSqrt);
}
__HINT_INLINE__ vec2 vec2GetUnitFastAccurate(const vec2 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y;
	const float invSqrt = fastInvSqrtAccurate(magnitudeSquared);
	return vec2VMultS(v, invSqrt);
}
__HINT_INLINE__ void vec2Normalize(vec2 *const restrict v){
	vec2MultVByS(v, 1.f/vec2Magnitude(v));
}
__HINT_INLINE__ void vec2NormalizeFast(vec2 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	vec2MultVByS(v, invSqrt);
}
__HINT_INLINE__ void vec2NormalizeFastAccurate(vec2 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y;
	const float invSqrt = fastInvSqrtAccurate(magnitudeSquared);
	vec2MultVByS(v, invSqrt);
}

__HINT_INLINE__ float vec2Dot(const vec2 *const restrict v1, const vec2 *const restrict v2){
	return v1->x * v2->x + v1->y * v2->y;
}

__HINT_INLINE__ void vec2Lerp1(vec2 *const restrict v1, const vec2 *const restrict v2, const float t){
	/*
	** r = v1 + (v2 - v1) * t
	*/
	v1->x += (v2->x - v1->x) * t;
	v1->y += (v2->y - v1->y) * t;
}

__HINT_INLINE__ void vec2Lerp2(const vec2 *const restrict v1, vec2 *const restrict v2, const float t){
	/*
	** r = v1 + (v2 - v1) * t
	*/
	v2->x = v1->x + (v2->x - v1->x) * t;
	v2->y = v1->y + (v2->y - v1->y) * t;
}

__HINT_INLINE__ void vec2LerpR(const vec2 *const restrict v1, const vec2 *const restrict v2, const float t, vec2 *const restrict r){
	/*
	** r = v1 + (v2 - v1) * t
	*/
	r->x = v1->x + (v2->x - v1->x) * t;
	r->y = v1->y + (v2->y - v1->y) * t;
}