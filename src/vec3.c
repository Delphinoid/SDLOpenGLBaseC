#include "helpersMath.h"
#include "inline.h"
#include <math.h>
#include <string.h>

__HINT_INLINE__ vec3 vec3New(const float x, const float y, const float z){
	const vec3 v = {.x = x, .y = y, .z = z};
	return v;
}
__HINT_INLINE__ vec3 vec3NewS(const float s){
	const vec3 v = {.x = s, .y = s, .z = s};
	return v;
}
__HINT_INLINE__ void vec3Set(vec3 *const restrict v, const float x, const float y, const float z){
	v->x = x; v->y = y; v->z = z;
}
__HINT_INLINE__ void vec3SetS(vec3 *const restrict v, const float s){
	v->x = s; v->y = s; v->z = s;
}

__HINT_INLINE__ vec3 vec3VAddV(const vec3 *const restrict v1, const vec3 *const restrict v2){
	vec3 r = {.x = v1->x + v2->x,
	          .y = v1->y + v2->y,
	          .z = v1->z + v2->z};
	return r;
}
__HINT_INLINE__ vec3 vec3VAddN(const vec3 *const restrict v, const float x, const float y, const float z){
	vec3 r = {.x = v->x + x,
	          .y = v->y + y,
	          .z = v->z + z};
	return r;
}
__HINT_INLINE__ vec3 vec3VAddS(const vec3 *const restrict v, const float s){
	vec3 r = {.x = v->x + s,
	          .y = v->y + s,
	          .z = v->z + s};
	return r;
}
__HINT_INLINE__ void vec3Zero(vec3 *const restrict v){
	memset(v, 0, sizeof(vec3));
}

__HINT_INLINE__ void vec3AddVToV(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x += v2->x; v1->y += v2->y; v1->z += v2->z;
}
__HINT_INLINE__ void vec3AddVToVR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r){
	r->x = v1->x + v2->x; r->y = v1->y + v2->y; r->z = v1->z + v2->z;
}
__HINT_INLINE__ void vec3AddNToV(vec3 *const restrict v, const float x, const float y, const float z){
	v->x += x; v->y += y; v->z += z;
}
__HINT_INLINE__ void vec3AddSToV(vec3 *const restrict v, const float s){
	v->x += s; v->y += s; v->z += s;
}

__HINT_INLINE__ vec3 vec3VSubV(const vec3 *const restrict v1, const vec3 *const restrict v2){
	vec3 r = {.x = v1->x - v2->x,
	          .y = v1->y - v2->y,
	          .z = v1->z - v2->z};
	return r;
}
__HINT_INLINE__ vec3 vec3VSubN(const vec3 *const restrict v, const float x, const float y, const float z){
	vec3 r = {.x = v->x - x,
	          .y = v->y - y,
	          .z = v->z - z};
	return r;
}
__HINT_INLINE__ vec3 vec3VSubS(const vec3 *const restrict v, const float s){
	vec3 r = {.x = v->x - s,
	          .y = v->y - s,
	          .z = v->z - s};
	return r;
}
__HINT_INLINE__ vec3 vec3NSubV(const float x, const float y, const float z, const vec3 *const restrict v){
	vec3 r = {.x = x - v->x,
	          .y = y - v->y,
	          .z = z - v->z};
	return r;
}
__HINT_INLINE__ vec3 vec3SSubV(const float s, const vec3 *const restrict v){
	vec3 r = {.x = s - v->x,
	          .y = s - v->y,
	          .z = s - v->z};
	return r;
}
__HINT_INLINE__ void vec3SubVFromV1(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x -= v2->x; v1->y -= v2->y; v1->z -= v2->z;
}
__HINT_INLINE__ void vec3SubVFromV2(const vec3 *const restrict v1, vec3 *const restrict v2){
	v2->x = v1->x - v2->x; v2->y = v1->y - v2->y; v2->z = v1->z - v2->z;
}
__HINT_INLINE__ void vec3SubVFromVR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r){
	r->x = v1->x - v2->x; r->y = v1->y - v2->y; r->z = v1->z - v2->z;
}
__HINT_INLINE__ void vec3SubNFromV(vec3 *const restrict v, const float x, const float y, const float z){
	v->x -= x; v->y -= y; v->z -= z;
}
__HINT_INLINE__ void vec3SubSFromV(vec3 *const restrict v, const float s){
	v->x -= s; v->y -= s; v->z -= s;
}
__HINT_INLINE__ void vec3SubVFromN(const float x, const float y, const float z, vec3 *const restrict v){
	v->x = x - v->x; v->y = y - v->y; v->z = z - v->z;
}
__HINT_INLINE__ void vec3SubVFromS(const float s, vec3 *const restrict v){
	v->x = s - v->x; v->y = s - v->y; v->z = s - v->z;
}

__HINT_INLINE__ vec3 vec3VMultV(const vec3 *const restrict v1, const vec3 *const restrict v2){
	vec3 r = {.x = v1->x * v2->x,
	          .y = v1->y * v2->y,
	          .z = v1->z * v2->z};
	return r;
}
__HINT_INLINE__ vec3 vec3VMultN(const vec3 *const restrict v, const float x, const float y, const float z){
	vec3 r = {.x = v->x * x,
	          .y = v->y * y,
	          .z = v->z * z};
	return r;
}
__HINT_INLINE__ vec3 vec3VMultS(const vec3 *const restrict v, const float s){
	vec3 r = {.x = v->x * s,
	          .y = v->y * s,
	          .z = v->z * s};
	return r;
}
__HINT_INLINE__ void vec3MultVByV(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x *= v2->x; v1->y *= v2->y; v1->z *= v2->z;
}
__HINT_INLINE__ void vec3MultVByVR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r){
	r->x = v1->x * v2->x; r->y = v1->y * v2->y; r->z = v1->z * v2->z;
}
__HINT_INLINE__ void vec3MultVByN(vec3 *const restrict v, const float x, const float y, const float z){
	v->x *= x; v->y *= y; v->z *= z;
}
__HINT_INLINE__ void vec3MultVByS(vec3 *const restrict v, const float s){
	v->x *= s; v->y *= s; v->z *= s;
}
__HINT_INLINE__ void vec3MultVBySR(const vec3 *const restrict v, const float s, vec3 *const restrict r){
	r->x = v->x * s; r->y = v->y * s; r->z = v->z * s;
}

__HINT_INLINE__ vec3 vec3VDivV(const vec3 *const restrict v1, const vec3 *const restrict v2){
	vec3 r;
	if(v2->x != 0.f){
		r.x = v1->x / v2->x;
	}else{
		r.x = 0.f;
	}
	if(v2->y != 0.f){
		r.y = v1->y / v2->y;
	}else{
		r.y = 0.f;
	}
	if(v2->z != 0.f){
		r.z = v1->z / v2->z;
	}else{
		r.z = 0.f;
	}
	return r;
}
__HINT_INLINE__ vec3 vec3VDivN(const vec3 *const restrict v, const float x, const float y, const float z){
	vec3 r;
	if(x != 0.f){
		r.x = v->x / x;
	}else{
		r.x = 0.f;
	}
	if(y != 0.f){
		r.y = v->y / y;
	}else{
		r.y = 0.f;
	}
	if(z != 0.f){
		r.z = v->z / z;
	}else{
		r.z = 0.f;
	}
	return r;
}
__HINT_INLINE__ vec3 vec3VDivS(const vec3 *const restrict v, const float s){
	if(s != 0.f){
		const float invS = 1.f / s;
		vec3 r = {.x = v->x * invS,
		          .y = v->y * invS,
		          .z = v->z * invS};
		return r;
	}
	return vec3NewS(0.f);
}
__HINT_INLINE__ vec3 vec3NDivV(const float x, const float y, const float z, const vec3 *const restrict v){
	vec3 r;
	if(v->x != 0.f){
		r.x = x / v->x;
	}else{
		r.x = x;
	}
	if(v->y != 0.f){
		r.y = y / v->y;
	}else{
		r.y = y;
	}
	if(v->z != 0.f){
		r.z = z / v->z;
	}else{
		r.z = z;
	}
	return r;
}
__HINT_INLINE__ vec3 vec3SDivV(const float s, const vec3 *const restrict v){
	vec3 r;
	if(v->x != 0.f){
		r.x = s / v->x;
	}else{
		r.x = s;
	}
	if(v->y != 0.f){
		r.y = s / v->y;
	}else{
		r.y = s;
	}
	if(v->z != 0.f){
		r.z = s / v->z;
	}else{
		r.z = s;
	}
	return r;
}
__HINT_INLINE__ void vec3DivVByV1(vec3 *const restrict v1, const vec3 *const restrict v2){
	if(v2->x != 0.f){
		v1->x /= v2->x;
	}else{
		v1->x = 0.f;
	}
	if(v2->y != 0.f){
		v1->y /= v2->y;
	}else{
		v1->y = 0.f;
	}
	if(v2->z != 0.f){
		v1->z /= v2->z;
	}else{
		v1->z = 0.f;
	}
}
__HINT_INLINE__ void vec3DivVByV2(const vec3 *const restrict v1, vec3 *const restrict v2){
	if(v2->x != 0.f){
		v2->x = v1->x / v2->x;
	}else{
		v2->x = 0.f;
	}
	if(v2->y != 0.f){
		v2->y = v1->y / v2->y;
	}else{
		v2->y = 0.f;
	}
	if(v2->z != 0.f){
		v2->z = v1->z / v2->z;
	}else{
		v2->z = 0.f;
	}
}
__HINT_INLINE__ void vec3DivVByVR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r){
	if(v2->x != 0.f){
		r->x = v1->x / v2->x;
	}else{
		r->x = 0.f;
	}
	if(v2->y != 0.f){
		r->y = v1->y / v2->y;
	}else{
		r->y = 0.f;
	}
	if(v2->z != 0.f){
		r->z = v1->z / v2->z;
	}else{
		r->z = 0.f;
	}
}
__HINT_INLINE__ void vec3DivVByN(vec3 *const restrict v, const float x, const float y, const float z){
	if(x != 0.f){
		v->x /= x;
	}else{
		v->x = 0.f;
	}
	if(y != 0.f){
		v->y /= y;
	}else{
		v->y = 0.f;
	}
	if(z != 0.f){
		v->z /= z;
	}else{
		v->z = 0.f;
	}
}
__HINT_INLINE__ void vec3DivVByS(vec3 *const restrict v, const float s){
	if(s != 0.f){
		const float invS = 1.f / s;
		v->x *= invS; v->y *= invS; v->z *= invS;
	}else{
		v->x = 0.f; v->y = 0.f; v->z = 0.f;
	}
}
__HINT_INLINE__ void vec3DivNByV(const float x, const float y, const float z, vec3 *const restrict v){
	if(v->x != 0.f){
		v->x = x / v->x;
	}else{
		v->x = 0.f;
	}
	if(v->y != 0.f){
		v->y = y / v->y;
	}else{
		v->y = 0.f;
	}
	if(v->z != 0.f){
		v->z = z / v->z;
	}else{
		v->z = 0.f;
	}
}
__HINT_INLINE__ void vec3DivSByV(const float s, vec3 *const restrict v){
	if(v->x != 0.f){
		v->x = s / v->x;
	}else{
		v->x = 0.f;
	}
	if(v->y != 0.f){
		v->y = s / v->y;
	}else{
		v->y = 0.f;
	}
	if(v->z != 0.f){
		v->z = s / v->z;
	}else{
		v->z = 0.f;
	}
}

__FORCE_INLINE__ float vec3Magnitude(const vec3 *const restrict v){
	return sqrtf(v->x*v->x + v->y*v->y + v->z*v->z);
}
__FORCE_INLINE__ float vec3MagnitudeSquared(const vec3 *const restrict v){
	return v->x*v->x + v->y*v->y + v->z*v->z;
}
__FORCE_INLINE__ float vec3InverseMagnitude(const vec3 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z;
	return fastInvSqrt(magnitudeSquared);
}

__HINT_INLINE__ vec3 vec3GetUnit(const vec3 *const restrict v){
	const float magnitude = vec3Magnitude(v);
	if(magnitude != 0.f){
		return vec3VMultS(v, 1.f/magnitude);
	}
	return *v;
}
__HINT_INLINE__ vec3 vec3GetUnitFast(const vec3 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	return vec3VMultS(v, invSqrt);
}
__HINT_INLINE__ vec3 vec3GetUnitFastAccurate(const vec3 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z;
	const float invSqrt = fastInvSqrtAccurate(magnitudeSquared);
	return vec3VMultS(v, invSqrt);
}
__HINT_INLINE__ void vec3Normalize(vec3 *const restrict v){
	const float magnitude = vec3Magnitude(v);
	if(magnitude != 0.f){
		vec3MultVByS(v, 1.f/magnitude);
	}
}
__HINT_INLINE__ void vec3NormalizeFast(vec3 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	vec3MultVByS(v, invSqrt);
}
__HINT_INLINE__ void vec3NormalizeFastAccurate(vec3 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z;
	const float invSqrt = fastInvSqrtAccurate(magnitudeSquared);
	vec3MultVByS(v, invSqrt);
}

__FORCE_INLINE__ float vec3Dot(const vec3 *const restrict v1, const vec3 *const restrict v2){
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}
__HINT_INLINE__ vec3 vec3GetCross(const vec3 *const restrict v1, const vec3 *const restrict v2){
	vec3 r = {.x = v1->y * v2->z - v1->z * v2->y,
	          .y = v1->z * v2->x - v1->x * v2->z,
	          .z = v1->x * v2->y - v1->y * v2->x};
    return r;
}
__HINT_INLINE__ void vec3Cross1(vec3 *const restrict v1, const vec3 *const restrict v2){
	vec3 r;
	r.x = v1->y * v2->z - v1->z * v2->y;
	r.y = v1->z * v2->x - v1->x * v2->z;
	r.z = v1->x * v2->y - v1->y * v2->x;
	*v1 = r;
}
__HINT_INLINE__ void vec3Cross2(const vec3 *const restrict v1, vec3 *const restrict v2){
	vec3 r;
	r.x = v1->y * v2->z - v1->z * v2->y;
	r.y = v1->z * v2->x - v1->x * v2->z;
	r.z = v1->x * v2->y - v1->y * v2->x;
	*v2 = r;
}
__HINT_INLINE__ void vec3CrossR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r){
	r->x = v1->y * v2->z - v1->z * v2->y;
	r->y = v1->z * v2->x - v1->x * v2->z;
	r->z = v1->x * v2->y - v1->y * v2->x;
}

__HINT_INLINE__ void vec3Lerp1(vec3 *const restrict v1, const vec3 *const restrict v2, const float t){
	/*
	** r = v1 + (v2 - v1) * t
	*/
	v1->x += (v2->x - v1->x) * t;
	v1->y += (v2->y - v1->y) * t;
	v1->z += (v2->z - v1->z) * t;
}

__HINT_INLINE__ void vec3Lerp2(const vec3 *const restrict v1, vec3 *const restrict v2, const float t){
	/*
	** r = v1 + (v2 - v1) * t
	*/
	v2->x = v1->x + (v2->x - v1->x) * t;
	v2->y = v1->y + (v2->y - v1->y) * t;
	v2->z = v1->z + (v2->z - v1->z) * t;
}

__HINT_INLINE__ void vec3LerpR(const vec3 *const restrict v1, const vec3 *const restrict v2, const float t, vec3 *const restrict r){
	/*
	** r = v1 + (v2 - v1) * t
	*/
	r->x = v1->x + (v2->x - v1->x) * t;
	r->y = v1->y + (v2->y - v1->y) * t;
	r->z = v1->z + (v2->z - v1->z) * t;
}

__HINT_INLINE__ void vec3CombineLinear(const vec3 *const restrict v1, const vec3 *const restrict v2, const vec3 *const restrict v3,
                       const float a, const float b, const float c, vec3 *const restrict r){
	/*
	** Linearly combine the vectors v1, v2 and v3 using the constants a, b and c.
	*/
	r->x = v1->x * a + v2->x * b + v3->x * c;
	r->y = v1->y * a + v2->y * b + v3->y * c;
	r->z = v1->z * a + v2->z * b + v3->z * c;
}
