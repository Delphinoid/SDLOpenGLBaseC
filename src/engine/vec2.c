#include "vec2.h"
#include "helpersMath.h"
#include <string.h>

vec2 g_vec2Zero = {.x = 0.f, .y = 0.f};

__HINT_INLINE__ vec2 vec2New(const float x, const float y){
	const vec2 r = {.x = x, .y = y};
	return r;
}
__HINT_INLINE__ vec2 vec2NewS(const float s){
	const vec2 r = {.x = s, .y = s};
	return r;
}
__HINT_INLINE__ void vec2Set(vec2 *const __RESTRICT__ v, const float x, const float y){
	v->x = x;
	v->y = y;
}
__HINT_INLINE__ void vec2SetS(vec2 *const __RESTRICT__ v, const float s){
	v->x = s;
	v->y = s;
}
__HINT_INLINE__ void vec2ZeroP(vec2 *const __RESTRICT__ v){
	memset(v, 0, sizeof(vec2));
}

__HINT_INLINE__ vec2 vec2VAddV(const vec2 v1, const vec2 v2){
	const vec2 r = {.x = v1.x + v2.x,
	                .y = v1.y + v2.y};
	return r;
}
__HINT_INLINE__ vec2 vec2VAddN(const vec2 v, const float x, const float y){
	const vec2 r = {.x = v.x + x,
	                .y = v.y + y};
	return r;
}
__HINT_INLINE__ vec2 vec2VAddS(const vec2 v, const float s){
	const vec2 r = {.x = v.x + s,
	                .y = v.y + s};
	return r;
}
__HINT_INLINE__ void vec2VAddVP(vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2){
	v1->x += v2->x;
	v1->y += v2->y;
}
__HINT_INLINE__ void vec2VAddVPR(const vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2, vec2 *const __RESTRICT__ r){
	r->x = v1->x + v2->x;
	r->y = v1->y + v2->y;
}
__HINT_INLINE__ void vec2VAddNP(vec2 *const __RESTRICT__ v, const float x, const float y){
	v->x += x;
	v->y += y;
}
__HINT_INLINE__ void vec2VAddSP(vec2 *const __RESTRICT__ v, const float s){
	v->x += s;
	v->y += s;
}

__HINT_INLINE__ vec2 vec2VSubV(const vec2 v1, const vec2 v2){
	const vec2 r = {.x = v1.x - v2.x,
	                .y = v1.y - v2.y};
	return r;
}
__HINT_INLINE__ vec2 vec2VSubN(const vec2 v, const float x, const float y){
	const vec2 r = {.x = v.x - x,
	                .y = v.y - y};
	return r;
}
__HINT_INLINE__ vec2 vec2VSubS(const vec2 v, const float s){
	const vec2 r = {.x = v.x - s,
	                .y = v.y - s};
	return r;
}
__HINT_INLINE__ vec2 vec2NSubV(const float x, const float y, const vec2 v){
	const vec2 r = {.x = x - v.x,
	                .y = y - v.y};
	return r;
}
__HINT_INLINE__ vec2 vec2SSubV(const float s, const vec2 v){
	const vec2 r = {.x = s - v.x,
	                .y = s - v.y};
	return r;
}
__HINT_INLINE__ void vec2VSubVP1(vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2){
	v1->x -= v2->x;
	v1->y -= v2->y;
}
__HINT_INLINE__ void vec2VSubVP2(const vec2 *const __RESTRICT__ v1, vec2 *const __RESTRICT__ v2){
	v2->x = v1->x - v2->x;
	v2->y = v1->y - v2->y;
}
__HINT_INLINE__ void vec2VSubVPR(const vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2, vec2 *const __RESTRICT__ r){
	r->x = v1->x - v2->x;
	r->y = v1->y - v2->y;
}
__HINT_INLINE__ void vec2VSubNP(vec2 *const __RESTRICT__ v, const float x, const float y){
	v->x -= x;
	v->y -= y;
}
__HINT_INLINE__ void vec2VSubSP(vec2 *const __RESTRICT__ v, const float s){
	v->x -= s;
	v->y -= s;
}
__HINT_INLINE__ void vec2NSubVP(const float x, const float y, vec2 *const __RESTRICT__ v){
	v->x = x - v->x;
	v->y = y - v->y;
}
__HINT_INLINE__ void vec2SSubVP(const float s, vec2 *const __RESTRICT__ v){
	v->x = s - v->x;
	v->y = s - v->y;
}

__HINT_INLINE__ vec2 vec2VMultV(const vec2 v1, const vec2 v2){
	const vec2 r = {.x = v1.x * v2.x,
	                .y = v1.y * v2.y};
	return r;
}
__HINT_INLINE__ vec2 vec2VMultN(const vec2 v, const float x, const float y){
	const vec2 r = {.x = v.x * x,
	                .y = v.y * y};
	return r;
}
__HINT_INLINE__ vec2 vec2VMultS(const vec2 v, const float s){
	const vec2 r = {.x = v.x * s,
	                .y = v.y * s};
	return r;
}
__HINT_INLINE__ void vec2VMultVP(vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2){
	v1->x *= v2->x;
	v1->y *= v2->y;
}
__HINT_INLINE__ void vec2VMultVPR(const vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2, vec2 *const __RESTRICT__ r){
	r->x = v1->x * v2->x;
	r->y = v1->y * v2->y;
}
__HINT_INLINE__ void vec2VMultNP(vec2 *const __RESTRICT__ v, const float x, const float y){
	v->x *= x;
	v->y *= y;
}
__HINT_INLINE__ void vec2VMultSP(vec2 *const __RESTRICT__ v, const float s){
	v->x *= s;
	v->y *= s;
}

__HINT_INLINE__ vec2 vec2VDivV(const vec2 v1, const vec2 v2){
	const vec2 r = {.x = v1.x / v2.x,
	                .y = v1.y / v2.y};
	return r;
}
__HINT_INLINE__ vec2 vec2VDivN(const vec2 v, const float x, const float y){
	const vec2 r = {.x = v.x / x,
	                .y = v.y / y};
	return r;
}
__HINT_INLINE__ vec2 vec2VDivS(const vec2 v, const float s){
	const float invS = 1.f / s;
	const vec2 r = {.x = v.x * invS,
	                .y = v.y * invS};
	return r;
}
__HINT_INLINE__ vec2 vec2NDivV(const float x, const float y, const vec2 v){
	const vec2 r = {.x = x / v.x,
	                .y = y / v.y};
	return r;
}
__HINT_INLINE__ vec2 vec2SDivV(const float s, const vec2 v){
	const vec2 r = {.x = s / v.x,
	                .y = s / v.y};
	return r;
}
__HINT_INLINE__ void vec2VDivVP1(vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2){
	v1->x /= v2->x;
	v1->y /= v2->y;
}
__HINT_INLINE__ void vec2VDivVP2(const vec2 *const __RESTRICT__ v1, vec2 *const __RESTRICT__ v2){
	v2->x = v1->x / v2->x;
	v2->y = v1->y / v2->y;

}
__HINT_INLINE__ void vec2VDivVPR(const vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2, vec2 *const __RESTRICT__ r){
	r->x = v1->x / v2->x;
	r->y = v1->y / v2->y;

}
__HINT_INLINE__ void vec2VDivNP(vec2 *const __RESTRICT__ v, const float x, const float y){
	v->x /= x;
	v->y /= y;
}
__HINT_INLINE__ void vec2VDivSP(vec2 *const __RESTRICT__ v, const float s){
	const float invS = 1.f / s;
	v->x *= invS;
	v->y *= invS;
}
__HINT_INLINE__ void vec2NDivVP(const float x, const float y, vec2 *const __RESTRICT__ v){
	v->x = x / v->x;
	v->y = y / v->y;
}
__HINT_INLINE__ void vec2SDivVP(const float s, vec2 *const __RESTRICT__ v){
	v->x = s / v->x;
	v->y = s / v->y;
}

__HINT_INLINE__ vec2 vec2fmaf(const float x, const vec2 u, const vec2 v){
	const vec2 r = {
	#ifdef FP_FAST_FMAF
		.x = fmaf(x, u.x, v.x),
		.y = fmaf(x, u.y, v.y)
	#else
		.x = x*u.x + v.x,
		.y = x*u.y + v.y
	#endif
	};
	return r;
}

__HINT_INLINE__ void vec2fmafP(const float x, const vec2 *const __RESTRICT__ u, vec2 *const __RESTRICT__ v){
	#ifdef FP_FAST_FMAF
	v->x = fmaf(x, u->x, v->x);
	v->y = fmaf(x, u->y, v->y);
	#else
	v->x = x*u->x + v->x;
	v->y = x*u->y + v->y;
	#endif
}

__HINT_INLINE__ void vec2fmafPR(const float x, const vec2 *const __RESTRICT__ u, const vec2 *const __RESTRICT__ v, vec2 *const __RESTRICT__ r){
	#ifdef FP_FAST_FMAF
	r->x = fmaf(x, u->x, v->x);
	r->y = fmaf(x, u->y, v->y);
	#else
	r->x = x*u->x + v->x;
	r->y = x*u->y + v->y;
	#endif
}

__HINT_INLINE__ vec2 vec2Min(const vec2 v1, const vec2 v2){
	const vec2 r = {
		.x = floatMin(v1.x, v2.x),
		.y = floatMin(v1.y, v2.y)
	};
	return r;
}
__HINT_INLINE__ void vec2MinP(const vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2, vec2 *const __RESTRICT__ r){
	r->x = floatMin(v1->x, v2->x);
	r->y = floatMin(v1->y, v2->y);
}
__HINT_INLINE__ vec2 vec2Max(const vec2 v1, const vec2 v2){
	const vec2 r = {
		.x = floatMax(v1.x, v2.x),
		.y = floatMax(v1.y, v2.y)
	};
	return r;
}
__HINT_INLINE__ void vec2MaxP(const vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2, vec2 *const __RESTRICT__ r){
	r->x = floatMax(v1->x, v2->x);
	r->y = floatMax(v1->y, v2->y);
}

__HINT_INLINE__ vec2 vec2Negate(const vec2 v){
	const vec2 r = {
		.x = -v.x,
		.y = -v.y
	};
	return r;
}
__HINT_INLINE__ void vec2NegateP(vec2 *const __RESTRICT__ v){
	v->x = -v->x;
	v->y = -v->y;
}
__HINT_INLINE__ void vec2NegatePR(const vec2 *const __RESTRICT__ v, vec2 *const __RESTRICT__ r){
	r->x = -v->x;
	r->y = -v->y;
}

__HINT_INLINE__ float vec2Magnitude(const vec2 v){
	return sqrtf(v.x*v.x + v.y*v.y);
}
__HINT_INLINE__ float vec2MagnitudeP(const vec2 *const __RESTRICT__ v){
	return sqrtf(v->x*v->x + v->y*v->y);
}
__HINT_INLINE__ float vec2MagnitudeSquared(const vec2 v){
	return v.x*v.x + v.y*v.y;
}
__HINT_INLINE__ float vec2MagnitudeSquaredP(const vec2 *const __RESTRICT__ v){
	return v->x*v->x + v->y*v->y;
}
__HINT_INLINE__ float vec2MagnitudeInverse(const vec2 v){
	return 1.f/sqrtf(v.x*v.x + v.y*v.y);
}
__HINT_INLINE__ float vec2MagnitudeInverseP(const vec2 *const __RESTRICT__ v){
	return 1.f/sqrtf(v->x*v->x + v->y*v->y);
}
__HINT_INLINE__ float vec2MagnitudeInverseFast(const vec2 v){
	return rsqrt(v.x*v.x + v.y*v.y);
}
__HINT_INLINE__ float vec2MagnitudeInverseFastS(const float x, const float y){
	return rsqrt(x*x + y*y);
}
__HINT_INLINE__ float vec2MagnitudeInverseFastP(const vec2 *const __RESTRICT__ v){
	return rsqrt(v->x*v->x + v->y*v->y);
}
__HINT_INLINE__ float vec2MagnitudeInverseFastAccurate(const vec2 v){
	return rsqrtAccurate(v.x*v.x + v.y*v.y);
}
__HINT_INLINE__ float vec2MagnitudeInverseFastAccurateP(const vec2 *const __RESTRICT__ v){
	return rsqrtAccurate(v->x*v->x + v->y*v->y);
}

__HINT_INLINE__ vec2 vec2Normalize(const vec2 v){
	return vec2VMultS(v, vec2MagnitudeInverse(v));
}
__HINT_INLINE__ vec2 vec2NormalizeFast(const vec2 v){
	return vec2VMultS(v, vec2MagnitudeInverseFast(v));
}
__HINT_INLINE__ vec2 vec2NormalizeFastS(const float x, const float y){
	const float magnitude = vec2MagnitudeInverseFastS(x, y);
	const vec2 r = {.x = magnitude*x, .y = magnitude*y};
	return r;
}
__HINT_INLINE__ vec2 vec2NormalizeFastAccurate(const vec2 v){
	return vec2VMultS(v, vec2MagnitudeInverseFastAccurate(v));
}
__HINT_INLINE__ float vec2NormalizeP(vec2 *const __RESTRICT__ v){
	const float magnitude = vec2MagnitudeP(v);
	vec2VMultSP(v, 1.f/magnitude);
	return magnitude;
}
__HINT_INLINE__ void vec2NormalizeFastP(vec2 *const __RESTRICT__ v){
	vec2VMultSP(v, vec2MagnitudeInverseFastP(v));
}
__HINT_INLINE__ void vec2NormalizeFastAccurateP(vec2 *const __RESTRICT__ v){
	vec2VMultSP(v, vec2MagnitudeInverseFastAccurateP(v));
}

__HINT_INLINE__ float vec2Dot(const vec2 v1, const vec2 v2){
	return v1.x * v2.x + v1.y * v2.y;
}
__HINT_INLINE__ float vec2DotP(const vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2){
	return v1->x * v2->x + v1->y * v2->y;
}

__HINT_INLINE__ vec2 vec2Lerp(const vec2 v1, const vec2 v2, const float t){
	// r = v1 + (v2 - v1) * t
	const vec2 r = {
		.x = floatLerp(v1.x, v2.x, t),
		.y = floatLerp(v1.y, v2.y, t)
	};
	return r;
}
__HINT_INLINE__ void vec2LerpP1(vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2, const float t){
	// r = v1 + (v2 - v1) * t
	v1->x = floatLerp(v1->x, v2->x, t);
	v1->y = floatLerp(v1->y, v2->y, t);
}
__HINT_INLINE__ void vec2LerpP2(const vec2 *const __RESTRICT__ v1, vec2 *const __RESTRICT__ v2, const float t){
	// r = v1 + (v2 - v1) * t
	v2->x = floatLerp(v1->x, v2->x, t);
	v2->y = floatLerp(v1->y, v2->y, t);
}
__HINT_INLINE__ void vec2LerpPR(const vec2 *const __RESTRICT__ v1, const vec2 *const __RESTRICT__ v2, const float t, vec2 *const __RESTRICT__ r){
	// r = v1 + (v2 - v1) * t
	r->x = floatLerp(v1->x, v2->x, t);
	r->y = floatLerp(v1->y, v2->y, t);
}
