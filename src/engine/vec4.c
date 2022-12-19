#include "vec4.h"
#include "helpersMath.h"
#include <string.h>

vec4 g_vec4Zero = {.x = 0.f, .y = 0.f, .z = 0.f, .w = 0.f};

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
__HINT_INLINE__ void vec4Set(vec4 *const __RESTRICT__ v, const float x, const float y, const float z, const float w){
	v->x = x;
	v->y = y;
	v->z = z;
	v->w = w;
}
__HINT_INLINE__ void vec4SetS(vec4 *const __RESTRICT__ v, const float s){
	v->x = s;
	v->y = s;
	v->z = s;
	v->w = s;
}
__HINT_INLINE__ void vec4ZeroP(vec4 *const __RESTRICT__ v){
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
__HINT_INLINE__ void vec4VAddVP(vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2){
	v1->x += v2->x;
	v1->y += v2->y;
	v1->z += v2->z;
	v1->w += v2->w;
}
__HINT_INLINE__ void vec4VAddVPR(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, vec4 *const __RESTRICT__ r){
	r->x = v1->x + v2->x;
	r->y = v1->y + v2->y;
	r->z = v1->z + v2->z;
	r->w = v1->w + v2->w;
}
__HINT_INLINE__ void vec4VAddNP(vec4 *const __RESTRICT__ v, const float x, const float y, const float z, const float w){
	v->x += x;
	v->y += y;
	v->z += z;
	v->w += w;
}
__HINT_INLINE__ void vec4VAddSP(vec4 *const __RESTRICT__ v, const float s){
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
__HINT_INLINE__ void vec4VSubVP1(vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2){
	v1->x -= v2->x;
	v1->y -= v2->y;
	v1->z -= v2->z;
	v1->w -= v2->w;
}
__HINT_INLINE__ void vec4VSubVP2(const vec4 *const __RESTRICT__ v1, vec4 *const __RESTRICT__ v2){
	v2->x = v1->x - v2->x;
	v2->y = v1->y - v2->y;
	v2->z = v1->z - v2->z;
	v2->w = v1->w - v2->w;
}
__HINT_INLINE__ void vec4VSubVPR(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, vec4 *const __RESTRICT__ r){
	r->x = v1->x - v2->x;
	r->y = v1->y - v2->y;
	r->z = v1->z - v2->z;
	r->w = v1->w - v2->w;
}
__HINT_INLINE__ void vec4VSubNP(vec4 *const __RESTRICT__ v, const float x, const float y, const float z, const float w){
	v->x -= x;
	v->y -= y;
	v->z -= z;
	v->w -= w;
}
__HINT_INLINE__ void vec4VSubSP(vec4 *const __RESTRICT__ v, const float s){
	v->x -= s;
	v->y -= s;
	v->z -= s;
	v->w -= s;
}
__HINT_INLINE__ void vec4VSubVP(const float x, const float y, const float z, const float w, vec4 *const __RESTRICT__ v){
	v->x = x - v->x;
	v->y = y - v->y;
	v->z = z - v->z;
	v->w = w - v->w;
}
__HINT_INLINE__ void vec4SSubVP(const float s, vec4 *const __RESTRICT__ v){
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
__HINT_INLINE__ void vec4VMultVP(vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2){
	v1->x *= v2->x;
	v1->y *= v2->y;
	v1->z *= v2->z;
	v1->w *= v2->w;
}
__HINT_INLINE__ void vec4VMultVPR(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, vec4 *const __RESTRICT__ r){
	r->x = v1->x * v2->x;
	r->y = v1->y * v2->y;
	r->z = v1->z * v2->z;
	r->w = v1->w * v2->w;
}
__HINT_INLINE__ void vec4VMultNP(vec4 *const __RESTRICT__ v, const float x, const float y, const float z, const float w){
	v->x *= x;
	v->y *= y;
	v->z *= z;
	v->w *= w;
}
__HINT_INLINE__ void vec4VMultSP(vec4 *const __RESTRICT__ v, const float s){
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
__HINT_INLINE__ void vec4VDivVP1(vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2){
	v1->x /= v2->x;
	v1->y /= v2->y;
	v1->z /= v2->z;
	v1->w /= v2->w;
}
__HINT_INLINE__ void vec4VDivVP2(const vec4 *const __RESTRICT__ v1, vec4 *const __RESTRICT__ v2){
	v2->x = v1->x / v2->x;
	v2->y = v1->y / v2->y;
	v2->z = v1->z / v2->z;
	v2->w = v1->w / v2->w;

}
__HINT_INLINE__ void vec4VDivVPR(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, vec4 *const __RESTRICT__ r){
	r->x = v1->x / v2->x;
	r->y = v1->y / v2->y;
	r->z = v1->z / v2->z;
	r->w = v1->w / v2->w;

}
__HINT_INLINE__ void vec4VDivPN(vec4 *const __RESTRICT__ v, const float x, const float y, const float z, const float w){
	v->x /= x;
	v->y /= y;
	v->z /= z;
	v->w /= w;
}
__HINT_INLINE__ void vec4VDivSP(vec4 *const __RESTRICT__ v, const float s){
	const float invS = 1.f / s;
	v->x *= invS;
	v->y *= invS;
	v->z *= invS;
	v->w *= invS;
}
__HINT_INLINE__ void vec4NDivVP(const float x, const float y, const float z, const float w, vec4 *const __RESTRICT__ v){
	v->x = x / v->x;
	v->y = y / v->y;
	v->z = z / v->z;
	v->w = w / v->w;
}
__HINT_INLINE__ void vec4SDivVP(const float s, vec4 *const __RESTRICT__ v){
	v->x = s / v->x;
	v->y = s / v->y;
	v->z = s / v->z;
	v->w = s / v->w;
}

__HINT_INLINE__ vec4 vec4fmaf(const float x, const vec4 u, const vec4 v){
	const vec4 r = {
	#ifdef FP_FAST_FMAF
		.x = fmaf(x, u.x, v.x),
		.y = fmaf(x, u.y, v.y),
		.z = fmaf(x, u.z, v.z),
		.w = fmaf(x, u.w, v.w)
	#else
		.x = x*u.x + v.x,
		.y = x*u.y + v.y,
		.z = x*u.z + v.z,
		.w = x*u.w + v.w
	#endif
	};
	return r;
}

__HINT_INLINE__ void vec4fmafP(const float x, const vec4 *const __RESTRICT__ u, vec4 *const __RESTRICT__ v){
	#ifdef FP_FAST_FMAF
	v->x = fmaf(x, u->x, v->x);
	v->y = fmaf(x, u->y, v->y);
	v->z = fmaf(x, u->z, v->z);
	v->w = fmaf(x, u->w, v->w);
	#else
	v->x = x*u->x + v->x;
	v->y = x*u->y + v->y;
	v->z = x*u->z + v->z;
	v->w = x*u->w + v->w;
	#endif
}

__HINT_INLINE__ void vec4fmafPR(const float x, const vec4 *const __RESTRICT__ u, const vec4 *const __RESTRICT__ v, vec4 *const __RESTRICT__ r){
	#ifdef FP_FAST_FMAF
	r->x = fmaf(x, u->x, v->x);
	r->y = fmaf(x, u->y, v->y);
	r->z = fmaf(x, u->z, v->z);
	r->w = fmaf(x, u->w, v->w);
	#else
	r->x = x*u->x + v->x;
	r->y = x*u->y + v->y;
	r->z = x*u->z + v->z;
	r->w = x*u->w + v->w;
	#endif
}

__HINT_INLINE__ vec4 vec4Min(const vec4 v1, const vec4 v2){
	const vec4 r = {
		.x = floatMin(v1.x, v2.x),
		.y = floatMin(v1.y, v2.y),
		.z = floatMin(v1.z, v2.z),
		.w = floatMin(v1.w, v2.w)
	};
	return r;
}
__HINT_INLINE__ void vec4MinP(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, vec4 *const __RESTRICT__ r){
	r->x = floatMin(v1->x, v2->x);
	r->y = floatMin(v1->y, v2->y);
	r->z = floatMin(v1->z, v2->z);
	r->w = floatMin(v1->z, v2->z);
}
__HINT_INLINE__ vec4 vec4Max(const vec4 v1, const vec4 v2){
	const vec4 r = {
		.x = floatMax(v1.x, v2.x),
		.y = floatMax(v1.y, v2.y),
		.z = floatMax(v1.z, v2.z),
		.w = floatMax(v1.w, v2.w)
	};
	return r;
}
__HINT_INLINE__ void vec4MaxP(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, vec4 *const __RESTRICT__ r){
	r->x = floatMax(v1->x, v2->x);
	r->y = floatMax(v1->y, v2->y);
	r->z = floatMax(v1->z, v2->z);
	r->w = floatMax(v1->z, v2->z);
}

__HINT_INLINE__ vec4 vec4Negate(const vec4 v){
	const vec4 r = {.x = -v.x,
	                .y = -v.y,
	                .z = -v.z,
					.w = -v.w};
	return r;
}
__HINT_INLINE__ void vec4NegateP(vec4 *const __RESTRICT__ v){
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
	v->w = -v->w;
}
__HINT_INLINE__ void vec4NegatePR(const vec4 *const __RESTRICT__ v, vec4 *const __RESTRICT__ r){
	r->x = -v->x;
	r->y = -v->y;
	r->z = -v->z;
	r->w = -v->w;
}

__HINT_INLINE__ float vec4Magnitude(const vec4 v){
	return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}
__HINT_INLINE__ float vec4MagnitudeP(const vec4 *const __RESTRICT__ v){
	return sqrtf(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
}
__HINT_INLINE__ float vec4MagnitudeSquared(const vec4 v){
	return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
}
__HINT_INLINE__ float vec4MagnitudeSquaredP(const vec4 *const __RESTRICT__ v){
	return v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
}
__HINT_INLINE__ float vec4MagnitudeInverse(const vec4 v){
	return 1.f/sqrtf(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}
__HINT_INLINE__ float vec4MagnitudeInverseP(const vec4 *const __RESTRICT__ v){
	return 1.f/sqrtf(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
}
__HINT_INLINE__ float vec4MagnitudeInverseFast(const vec4 v){
	return rsqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}
__HINT_INLINE__ float vec4MagnitudeInverseFastP(const vec4 *const __RESTRICT__ v){
	return rsqrt(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
}
__HINT_INLINE__ float vec4MagnitudeInverseFastAccurate(const vec4 v){
	return rsqrtAccurate(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}
__HINT_INLINE__ float vec4MagnitudeInverseFastAccurateP(const vec4 *const __RESTRICT__ v){
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
__HINT_INLINE__ float vec4NormalizeP(vec4 *const __RESTRICT__ v){
	const float magnitude = vec4MagnitudeP(v);
	vec4VMultSP(v, 1.f/magnitude);
	return magnitude;
}
__HINT_INLINE__ void vec4NormalizeFastP(vec4 *const __RESTRICT__ v){
	vec4VMultSP(v, vec4MagnitudeInverseFastP(v));
}
__HINT_INLINE__ void vec4NormalizeFastAccurateP(vec4 *const __RESTRICT__ v){
	vec4VMultSP(v, vec4MagnitudeInverseFastAccurateP(v));
}

__HINT_INLINE__ float vec4Dot(const vec4 v1, const vec4 v2){
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}
__HINT_INLINE__ float vec4DotP(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2){
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w;
}

__HINT_INLINE__ vec4 vec4Lerp(const vec4 v1, const vec4 v2, const float t){
	// r = v1 + (v2 - v1) * t
	const vec4 r = {.x = floatLerp(v1.x, v2.x, t),
	                .y = floatLerp(v1.y, v2.y, t),
	                .z = floatLerp(v1.z, v2.z, t),
	                .w = floatLerp(v1.w, v2.w, t)};
	return r;
}
__HINT_INLINE__ void vec4LerpP1(vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, const float t){
	// r = v1 + (v2 - v1) * t
	v1->x = floatLerp(v1->x, v2->x, t);
	v1->y = floatLerp(v1->y, v2->y, t);
	v1->z = floatLerp(v1->z, v2->z, t);
	v1->w = floatLerp(v1->w, v2->w, t);
}
__HINT_INLINE__ void vec4LerpP2(const vec4 *const __RESTRICT__ v1, vec4 *const __RESTRICT__ v2, const float t){
	// r = v1 + (v2 - v1) * t
	v2->x = floatLerp(v1->x, v2->x, t);
	v2->y = floatLerp(v1->y, v2->y, t);
	v2->z = floatLerp(v1->z, v2->z, t);
	v2->w = floatLerp(v1->w, v2->w, t);
}
__HINT_INLINE__ void vec4LerpPR(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, const float t, vec4 *const __RESTRICT__ r){
	// r = v1 + (v2 - v1) * t
	r->x = floatLerp(v1->x, v2->x, t);
	r->y = floatLerp(v1->y, v2->y, t);
	r->z = floatLerp(v1->z, v2->z, t);
	r->w = floatLerp(v1->w, v2->w, t);
}