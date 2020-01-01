#include "helpersMath.h"
#include "inline.h"
#include <string.h>

__HINT_INLINE__ vec3 vec3New(const float x, const float y, const float z){
	const vec3 r = {.x = x, .y = y, .z = z};
	return r;
}
__HINT_INLINE__ vec3 vec3NewS(const float s){
	const vec3 r = {.x = s, .y = s, .z = s};
	return r;
}
__HINT_INLINE__ vec3 vec3Zero(){
	const vec3 r = {.x = 0.f,
	                .y = 0.f,
	                .z = 0.f};
	return r;
}
__HINT_INLINE__ void vec3Set(vec3 *const restrict v, const float x, const float y, const float z){
	v->x = x;
	v->y = y;
	v->z = z;
}
__HINT_INLINE__ void vec3SetS(vec3 *const restrict v, const float s){
	v->x = s;
	v->y = s;
	v->z = s;
}
__HINT_INLINE__ void vec3ZeroP(vec3 *const restrict v){
	memset(v, 0, sizeof(vec3));
}

__HINT_INLINE__ vec3 vec3VAddV(const vec3 v1, const vec3 v2){
	const vec3 r = {.x = v1.x + v2.x,
	                .y = v1.y + v2.y,
	                .z = v1.z + v2.z};
	return r;
}
__HINT_INLINE__ vec3 vec3VAddN(const vec3 v, const float x, const float y, const float z){
	const vec3 r = {.x = v.x + x,
	                .y = v.y + y,
	                .z = v.z + z};
	return r;
}
__HINT_INLINE__ vec3 vec3VAddS(const vec3 v, const float s){
	const vec3 r = {.x = v.x + s,
	                .y = v.y + s,
	                .z = v.z + s};
	return r;
}

__HINT_INLINE__ void vec3VAddVP(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x += v2->x;
	v1->y += v2->y;
	v1->z += v2->z;
}
__HINT_INLINE__ void vec3VAddNP(vec3 *const restrict v, const float x, const float y, const float z){
	v->x += x;
	v->y += y;
	v->z += z;
}
__HINT_INLINE__ void vec3VAddSP(vec3 *const restrict v, const float s){
	v->x += s;
	v->y += s;
	v->z += s;
}
__HINT_INLINE__ void vec3VAddVPR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r){
	r->x = v1->x + v2->x;
	r->y = v1->y + v2->y;
	r->z = v1->z + v2->z;
}
__HINT_INLINE__ void vec3VAddNPR(vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict r){
	r->x = v->x + x;
	r->y = v->y + y;
	r->z = v->z + z;
}
__HINT_INLINE__ void vec3VAddSPR(vec3 *const restrict v, const float s, vec3 *const restrict r){
	r->x = v->x + s;
	r->y = v->y + s;
	r->z = v->z + s;
}

__HINT_INLINE__ vec3 vec3VSubV(const vec3 v1, const vec3 v2){
	const vec3 r = {.x = v1.x - v2.x,
	                .y = v1.y - v2.y,
	                .z = v1.z - v2.z};
	return r;
}
__HINT_INLINE__ vec3 vec3VSubN(const vec3 v, const float x, const float y, const float z){
	const vec3 r = {.x = v.x - x,
	                .y = v.y - y,
	                .z = v.z - z};
	return r;
}
__HINT_INLINE__ vec3 vec3VSubS(const vec3 v, const float s){
	const vec3 r = {.x = v.x - s,
	                .y = v.y - s,
	                .z = v.z - s};
	return r;
}
__HINT_INLINE__ vec3 vec3NSubV(const float x, const float y, const float z, const vec3 v){
	const vec3 r = {.x = x - v.x,
	                .y = y - v.y,
	                .z = z - v.z};
	return r;
}
__HINT_INLINE__ vec3 vec3SSubV(const float s, const vec3 v){
	const vec3 r = {.x = s - v.x,
	                .y = s - v.y,
	                .z = s - v.z};
	return r;
}
__HINT_INLINE__ void vec3VSubVP1(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x -= v2->x;
	v1->y -= v2->y;
	v1->z -= v2->z;
}
__HINT_INLINE__ void vec3VSubVP2(const vec3 *const restrict v1, vec3 *const restrict v2){
	v2->x = v1->x - v2->x;
	v2->y = v1->y - v2->y;
	v2->z = v1->z - v2->z;
}
__HINT_INLINE__ void vec3VSubVPR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r){
	r->x = v1->x - v2->x;
	r->y = v1->y - v2->y;
	r->z = v1->z - v2->z;
}
__HINT_INLINE__ void vec3VSubNP(vec3 *const restrict v, const float x, const float y, const float z){
	v->x -= x;
	v->y -= y;
	v->z -= z;
}
__HINT_INLINE__ void vec3VSubSP(vec3 *const restrict v, const float s){
	v->x -= s;
	v->y -= s;
	v->z -= s;
}
__HINT_INLINE__ void vec3NSubVP(const float x, const float y, const float z, vec3 *const restrict v){
	v->x = x - v->x;
	v->y = y - v->y;
	v->z = z - v->z;
}
__HINT_INLINE__ void vec3SSubVP(const float s, vec3 *const restrict v){
	v->x = s - v->x;
	v->y = s - v->y;
	v->z = s - v->z;
}

__HINT_INLINE__ vec3 vec3VMultV(const vec3 v1, const vec3 v2){
	const vec3 r = {.x = v1.x * v2.x,
	                .y = v1.y * v2.y,
	                .z = v1.z * v2.z};
	return r;
}
__HINT_INLINE__ vec3 vec3VMultN(const vec3 v, const float x, const float y, const float z){
	const vec3 r = {.x = v.x * x,
	                .y = v.y * y,
	                .z = v.z * z};
	return r;
}
__HINT_INLINE__ vec3 vec3VMultS(const vec3 v, const float s){
	const vec3 r = {.x = v.x * s,
	                .y = v.y * s,
	                .z = v.z * s};
	return r;
}
__HINT_INLINE__ void vec3VMultVP(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x *= v2->x;
	v1->y *= v2->y;
	v1->z *= v2->z;
}
__HINT_INLINE__ void vec3VMultVPR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r){
	r->x = v1->x * v2->x;
	r->y = v1->y * v2->y;
	r->z = v1->z * v2->z;
}
__HINT_INLINE__ void vec3VMultNP(vec3 *const restrict v, const float x, const float y, const float z){
	v->x *= x;
	v->y *= y;
	v->z *= z;
}
__HINT_INLINE__ void vec3VMultSP(vec3 *const restrict v, const float s){
	v->x *= s;
	v->y *= s;
	v->z *= s;
}
__HINT_INLINE__ void vec3VMultSPR(const vec3 *const restrict v, const float s, vec3 *const restrict r){
	r->x = v->x * s;
	r->y = v->y * s;
	r->z = v->z * s;
}

__HINT_INLINE__ vec3 vec3VDivV(const vec3 v1, const vec3 v2){
	const vec3 r = {.x = v1.x / v2.x,
	                .y = v1.y / v2.y,
	                .z = v1.z / v2.z};
	return r;
}
__HINT_INLINE__ vec3 vec3VDivN(const vec3 v, const float x, const float y, const float z){
	const vec3 r = {.x = v.x / x,
	                .y = v.y / y,
	                .z = v.z / z};
	return r;
}
__HINT_INLINE__ vec3 vec3VDivS(const vec3 v, const float s){
	const float invS = 1.f / s;
	const vec3 r = {.x = v.x * invS,
	                .y = v.y * invS,
	                .z = v.z * invS};
	return r;
}
__HINT_INLINE__ vec3 vec3NDivV(const float x, const float y, const float z, const vec3 v){
	const vec3 r = {.x = x / v.x,
	                .y = y / v.y,
	                .z = z / v.z};
	return r;
}
__HINT_INLINE__ vec3 vec3SDivV(const float s, const vec3 v){
	const vec3 r = {.x = s / v.x,
	                .y = s / v.y,
	                .z = s / v.z};
	return r;
}
__HINT_INLINE__ void vec3VDivVP1(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x /= v2->x;
	v1->y /= v2->y;
	v1->z /= v2->z;
}
__HINT_INLINE__ void vec3VDivVP2(const vec3 *const restrict v1, vec3 *const restrict v2){
	v2->x = v1->x / v2->x;
	v2->y = v1->y / v2->y;
	v2->z = v1->z / v2->z;
}
__HINT_INLINE__ void vec3VDivVPR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r){
	r->x = v1->x / v2->x;
	r->y = v1->y / v2->y;
	r->z = v1->z / v2->z;
}
__HINT_INLINE__ void vec3VDivNP(vec3 *const restrict v, const float x, const float y, const float z){
	v->x /= x;
	v->y /= y;
	v->z /= z;
}
__HINT_INLINE__ void vec3VDivSP(vec3 *const restrict v, const float s){
	const float invS = 1.f / s;
	v->x *= invS;
	v->y *= invS;
	v->z *= invS;
}
__HINT_INLINE__ void vec3NDivVP(const float x, const float y, const float z, vec3 *const restrict v){
	v->x = x / v->x;
	v->y = y / v->y;
	v->z = z / v->z;
}
__HINT_INLINE__ void vec3SDivVP(const float s, vec3 *const restrict v){
	v->x = s / v->x;
	v->y = s / v->y;
	v->z = s / v->z;
}

__HINT_INLINE__ vec3 vec3Min(const vec3 v1, const vec3 v2){
	const vec3 r = {.x = v1.x <= v2.x ? v1.x : v2.x,
	                .y = v1.y <= v2.y ? v1.y : v2.y,
	                .z = v1.z <= v2.z ? v1.z : v2.z};
	return r;
}
__HINT_INLINE__ void vec3MinP(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r){
	r->x = v1->x <= v2->x ? v1->x : v2->x;
	r->y = v1->y <= v2->y ? v1->y : v2->y;
	r->z = v1->z <= v2->z ? v1->z : v2->z;
}
__HINT_INLINE__ vec3 vec3Max(const vec3 v1, const vec3 v2){
	const vec3 r = {.x = v1.x >= v2.x ? v1.x : v2.x,
	                .y = v1.y >= v2.y ? v1.y : v2.y,
	                .z = v1.z >= v2.z ? v1.z : v2.z};
	return r;
}
__HINT_INLINE__ void vec3MaxP(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r){
	r->x = v1->x >= v2->x ? v1->x : v2->x;
	r->y = v1->y >= v2->y ? v1->y : v2->y;
	r->z = v1->z >= v2->z ? v1->z : v2->z;
}

__HINT_INLINE__ vec3 vec3Negate(const vec3 v){
	const vec3 r = {.x = -v.x, .y = -v.y, .z = -v.z};
	return r;
}
__HINT_INLINE__ void vec3NegateP(vec3 *const restrict v){
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
}
__HINT_INLINE__ void vec3NegatePR(const vec3 *const restrict v, vec3 *const restrict r){
	r->x = -v->x;
	r->y = -v->y;
	r->z = -v->z;
}

__HINT_INLINE__ float vec3Magnitude(const vec3 v){
	return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}
__HINT_INLINE__ float vec3MagnitudeP(const vec3 *const restrict v){
	return sqrtf(v->x*v->x + v->y*v->y + v->z*v->z);
}
__HINT_INLINE__ float vec3MagnitudeSquared(const vec3 v){
	return v.x*v.x + v.y*v.y + v.z*v.z;
}
__HINT_INLINE__ float vec3MagnitudeSquaredP(const vec3 *const restrict v){
	return v->x*v->x + v->y*v->y + v->z*v->z;
}
__HINT_INLINE__ float vec3MagnitudeInverse(const vec3 v){
	return 1.f/sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}
__HINT_INLINE__ float vec3MagnitudeInverseP(const vec3 *const restrict v){
	return 1.f/sqrtf(v->x*v->x + v->y*v->y + v->z*v->z);
}
__HINT_INLINE__ float vec3MagnitudeInverseFast(const vec3 v){
	return rsqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}
__HINT_INLINE__ float vec3MagnitudeInverseFastP(const vec3 *const restrict v){
	return rsqrt(v->x*v->x + v->y*v->y + v->z*v->z);
}
__HINT_INLINE__ float vec3MagnitudeInverseFastAccurate(const vec3 v){
	return rsqrtAccurate(v.x*v.x + v.y*v.y + v.z*v.z);
}
__HINT_INLINE__ float vec3MagnitudeInverseFastAccurateP(const vec3 *const restrict v){
	return rsqrtAccurate(v->x*v->x + v->y*v->y + v->z*v->z);
}

__HINT_INLINE__ vec3 vec3Normalize(const vec3 v){
	return vec3VMultS(v, vec3MagnitudeInverse(v));
}
__HINT_INLINE__ vec3 vec3NormalizeFast(const vec3 v){
	return vec3VMultS(v, vec3MagnitudeInverseFast(v));
}
__HINT_INLINE__ vec3 vec3NormalizeFastAccurate(const vec3 v){
	return vec3VMultS(v, vec3MagnitudeInverseFastAccurate(v));
}
__HINT_INLINE__ void vec3NormalizeP(vec3 *const restrict v){
	vec3VMultSP(v, vec3MagnitudeInverseP(v));
}
__HINT_INLINE__ void vec3NormalizePR(const vec3 *const restrict v, vec3 *const restrict r){
	vec3VMultSPR(v, vec3MagnitudeInverseP(v), r);
}
__HINT_INLINE__ void vec3NormalizeFastP(vec3 *const restrict v){
	vec3VMultSP(v, vec3MagnitudeInverseFastP(v));
}
__HINT_INLINE__ void vec3NormalizeFastPR(const vec3 *const restrict v, vec3 *const restrict r){
	vec3VMultSPR(v, vec3MagnitudeInverseFastP(v), r);
}
__HINT_INLINE__ void vec3NormalizeFastAccurateP(vec3 *const restrict v){
	vec3VMultSP(v, vec3MagnitudeInverseFastAccurateP(v));
}
__HINT_INLINE__ void vec3NormalizeFastAccuratePR(const vec3 *const restrict v, vec3 *const restrict r){
	vec3VMultSPR(v, vec3MagnitudeInverseFastAccurateP(v), r);
}

__HINT_INLINE__ float vec3Dot(const vec3 v1, const vec3 v2){
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
__HINT_INLINE__ float vec3DotP(const vec3 *const restrict v1, const vec3 *const restrict v2){
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}
__HINT_INLINE__ vec3 vec3Cross(const vec3 v1, const vec3 v2){
	const vec3 r = {.x = v1.y * v2.z - v1.z * v2.y,
	                .y = v1.z * v2.x - v1.x * v2.z,
	                .z = v1.x * v2.y - v1.y * v2.x};
    return r;
}
__HINT_INLINE__ void vec3CrossP1(vec3 *const restrict v1, const vec3 *const restrict v2){
	const vec3 r = {.x = v1->y * v2->z - v1->z * v2->y,
	                .y = v1->z * v2->x - v1->x * v2->z,
	                .z = v1->x * v2->y - v1->y * v2->x};
	*v1 = r;
}
__HINT_INLINE__ void vec3CrossP2(const vec3 *const restrict v1, vec3 *const restrict v2){
	const vec3 r = {.x = v1->y * v2->z - v1->z * v2->y,
	                .y = v1->z * v2->x - v1->x * v2->z,
	                .z = v1->x * v2->y - v1->y * v2->x};
	*v2 = r;
}
__HINT_INLINE__ void vec3CrossPR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r){
	r->x = v1->y * v2->z - v1->z * v2->y;
	r->y = v1->z * v2->x - v1->x * v2->z;
	r->z = v1->x * v2->y - v1->y * v2->x;
}

__HINT_INLINE__ vec3 vec3Perpendicular(const vec3 v){
	if(fabsf(v.x) >= 0.5773502691896257){  // sqrtf(1.f / 3.f), 0x3F13CD3A
		return vec3New(v.y, -v.x, 0.f);
	}
	return vec3New(0.f, v.z, -v.y);
}
__HINT_INLINE__ void vec3PerpendicularP(vec3 *const restrict v){
	const float y = v->y;
	if(fabsf(v->x) >= 0.5773502691896257){  // sqrtf(1.f / 3.f), 0x3F13CD3A
		v->y = -v->x;
		v->x = y;
		v->z = 0.f;
	}
	v->y = v->z;
	v->x = 0.f;
	v->z = -y;
}
__HINT_INLINE__ void vec3PerpendicularPR(const vec3 *const restrict v, vec3 *const restrict r){
	if(fabsf(v->x) >= 0.5773502691896257){  // sqrtf(1.f / 3.f), 0x3F13CD3A
		r->x = v->y;
		r->y = -v->x;
		r->z = 0.f;
	}
	r->x = 0.f;
	r->y = v->z;
	r->z = -v->y;
}

__HINT_INLINE__ void vec3OrthonormalBasis(const vec3 v1, vec3 *const restrict v2, vec3 *const restrict v3){
	// Generates an orthonormal basis from v1.
	*v3 = vec3Cross(v1, (*v2 = vec3Perpendicular(v1)));
}
__HINT_INLINE__ void vec3OrthonormalBasisP(const vec3 *const restrict v1, vec3 *const restrict v2, vec3 *const restrict v3){
	// Generates an orthonormal basis from v1.
	vec3PerpendicularPR(v1, v2);
	vec3CrossPR(v1, v2, v3);
}

__HINT_INLINE__ vec3 vec3Lerp(const vec3 v1, const vec3 v2, const float t){
	// r = v1 + (v2 - v1) * t
	const vec3 r = {.x = floatLerp(v1.x, v2.x, t),
	                .y = floatLerp(v1.y, v2.y, t),
	                .z = floatLerp(v1.z, v2.z, t)};
	return r;
}
__HINT_INLINE__ void vec3LerpP1(vec3 *const restrict v1, const vec3 *const restrict v2, const float t){
	// r = v1 + (v2 - v1) * t
	v1->x = floatLerp(v1->x, v2->x, t);
	v1->y = floatLerp(v1->y, v2->y, t);
	v1->z = floatLerp(v1->z, v2->z, t);
}
__HINT_INLINE__ void vec3LerpP2(const vec3 *const restrict v1, vec3 *const restrict v2, const float t){
	// r = v1 + (v2 - v1) * t
	v2->x = floatLerp(v1->x, v2->x, t);
	v2->y = floatLerp(v1->y, v2->y, t);
	v2->z = floatLerp(v1->z, v2->z, t);
}
__HINT_INLINE__ void vec3LerpPR(const vec3 *const restrict v1, const vec3 *const restrict v2, const float t, vec3 *const restrict r){
	// r = v1 + (v2 - v1) * t
	r->x = floatLerp(v1->x, v2->x, t);
	r->y = floatLerp(v1->y, v2->y, t);
	r->z = floatLerp(v1->z, v2->z, t);
}

__HINT_INLINE__ vec3 vec3LinearCombination(const vec3 v1, const vec3 v2, const vec3 v3, const float a, const float b, const float c){
	// Linearly combine the vectors v1, v2 and v3 using the constants a, b and c.
	const vec3 r = {.x = v1.x * a + v2.x * b + v3.x * c,
	                .y = v1.y * a + v2.y * b + v3.y * c,
	                .z = v1.z * a + v2.z * b + v3.z * c};
	return r;
}
__HINT_INLINE__ void vec3LinearCombinationP(const vec3 *const restrict v1, const vec3 *const restrict v2, const vec3 *const restrict v3,
                                            const float a, const float b, const float c, vec3 *const restrict r){
	// Linearly combine the vectors v1, v2 and v3 using the constants a, b and c.
	r->x = v1->x * a + v2->x * b + v3->x * c;
	r->y = v1->y * a + v2->y * b + v3->y * c;
	r->z = v1->z * a + v2->z * b + v3->z * c;
}
