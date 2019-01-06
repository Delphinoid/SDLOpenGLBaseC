#include "vec4.h"
#include "helpersMath.h"
#include "inline.h"
#include <math.h>
#include <string.h>

__HINT_INLINE__ vec4 vec4New(const float x, const float y, const float z, const float w){
	vec4 v;
	v.x = x;
	v.y = y;
	v.z = z;
	v.w = w;
	return v;
}
__HINT_INLINE__ vec4 vec4NewS(const float s){
	vec4 v;
	v.x = s;
	v.y = s;
	v.z = s;
	v.w = s;
	return v;
}
__HINT_INLINE__ void vec4Set(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x = x; v->y = y; v->z = z; v->w = w;
}
__HINT_INLINE__ void vec4SetS(vec4 *const restrict v, const float s){
	v->x = s; v->y = s; v->z = s; v->w = s;
}
__HINT_INLINE__ void vec4Zero(vec4 *const restrict v){
	memset(v, 0, sizeof(vec4));
}

__HINT_INLINE__ vec4 vec4VAddV(const vec4 *const restrict v1, const vec4 *const restrict v2){
	vec4 r;
	r.x = v1->x + v2->x;
	r.y = v1->y + v2->y;
	r.z = v1->z + v2->z;
	r.w = v1->w + v2->w;
	return r;
}
__HINT_INLINE__ vec4 vec4VAddN(const vec4 *const restrict v, const float x, const float y, const float z, const float w){
	vec4 r;
	r.x = v->x + x;
	r.y = v->y + y;
	r.z = v->z + z;
	r.w = v->w + w;
	return r;
}
__HINT_INLINE__ vec4 vec4VAddS(const vec4 *const restrict v, const float s){
	vec4 r;
	r.x = v->x + s;
	r.y = v->y + s;
	r.z = v->z + s;
	r.w = v->w + s;
	return r;
}
__HINT_INLINE__ void vec4AddVToV(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x += v2->x; v1->y += v2->y; v1->z += v2->z; v1->w += v2->w;
}
__HINT_INLINE__ void vec4AddVToVR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x + v2->x; r->y = v1->y + v2->y; r->z = v1->z + v2->z; r->w = v1->w + v2->w;
}
__HINT_INLINE__ void vec4AddNToV(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x += x; v->y += y; v->z += z; v->w += w;
}
__HINT_INLINE__ void vec4AddSToV(vec4 *const restrict v, const float s){
	v->x += s; v->y += s; v->z += s; v->w += s;
}

__HINT_INLINE__ vec4 vec4VSubV(const vec4 *const restrict v1, const vec4 *const restrict v2){
	vec4 r;
	r.x = v1->x - v2->x;
	r.y = v1->y - v2->y;
	r.z = v1->z - v2->z;
	r.w = v1->w - v2->w;
	return r;
}
__HINT_INLINE__ vec4 vec4VSubN(const vec4 *const restrict v, const float x, const float y, const float z, const float w){
	vec4 r;
	r.x = v->x - x;
	r.y = v->y - y;
	r.z = v->z - z;
	r.w = v->w - w;
	return r;
}
__HINT_INLINE__ vec4 vec4VSubS(const vec4 *const restrict v, const float s){
	vec4 r;
	r.x = v->x - s;
	r.y = v->y - s;
	r.z = v->z - s;
	r.w = v->w - s;
	return r;
}
__HINT_INLINE__ vec4 vec4NSubV(const float x, const float y, const float z, const float w, const vec4 *const restrict v){
	vec4 r;
	r.x = x - v->x;
	r.y = y - v->y;
	r.z = z - v->z;
	r.w = w - v->w;
	return r;
}
__HINT_INLINE__ vec4 vec4SSubV(const float s, const vec4 *const restrict v){
	vec4 r;
	r.x = s - v->x;
	r.y = s - v->y;
	r.z = s - v->z;
	r.w = s - v->w;
	return r;
}
__HINT_INLINE__ void vec4SubVFromV1(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x -= v2->x; v1->y -= v2->y; v1->z -= v2->z; v1->w -= v2->w;
}
__HINT_INLINE__ void vec4SubVFromV2(const vec4 *const restrict v1, vec4 *const restrict v2){
	v2->x = v1->x - v2->x; v2->y = v1->y - v2->y; v2->z = v1->z - v2->z; v2->w = v1->w - v2->w;
}
__HINT_INLINE__ void vec4SubVFromVR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x - v2->x; r->y = v1->y - v2->y; r->z = v1->z - v2->z; r->w = v1->w - v2->w;
}
__HINT_INLINE__ void vec4SubNFromV(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x -= x; v->y -= y; v->z -= z; v->w -= w;
}
__HINT_INLINE__ void vec4SubSFromV(vec4 *const restrict v, const float s){
	v->x -= s; v->y -= s; v->z -= s; v->w -= s;
}
__HINT_INLINE__ void vec4SubVFromN(const float x, const float y, const float z, const float w, vec4 *const restrict v){
	v->x = x - v->x; v->y = y - v->y; v->z = z - v->z; v->w = w - v->w;
}
__HINT_INLINE__ void vec4SubVFromS(const float s, vec4 *const restrict v){
	v->x = s - v->x; v->y = s - v->y; v->z = s - v->z; v->w = s - v->w;
}

__HINT_INLINE__ vec4 vec4VMultV(const vec4 *const restrict v1, const vec4 *const restrict v2){
	vec4 r;
	r.x = v1->x * v2->x;
	r.y = v1->y * v2->y;
	r.z = v1->z * v2->z;
	r.w = v1->w * v2->w;
	return r;
}
__HINT_INLINE__ vec4 vec4VMultN(const vec4 *const restrict v, const float x, const float y, const float z, const float w){
	vec4 r;
	r.x = v->x * x;
	r.y = v->y * y;
	r.z = v->z * z;
	r.w = v->w * w;
	return r;
}
__HINT_INLINE__ vec4 vec4VMultS(const vec4 *const restrict v, const float s){
	vec4 r;
	r.x = v->x * s;
	r.y = v->y * s;
	r.z = v->z * s;
	r.w = v->w * s;
	return r;
}
__HINT_INLINE__ void vec4MultVByV(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x *= v2->x; v1->y *= v2->y; v1->z *= v2->z; v1->w *= v2->w;
}
__HINT_INLINE__ void vec4MultVByVR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
	r->x = v1->x * v2->x; r->y = v1->y * v2->y; r->z = v1->z * v2->z; r->w = v1->w * v2->w;
}
__HINT_INLINE__ void vec4MultVByN(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x *= x; v->y *= y; v->z *= z; v->w *= w;
}
__HINT_INLINE__ void vec4MultVByS(vec4 *const restrict v, const float s){
	v->x *= s; v->y *= s; v->z *= s; v->w *= s;
}

__HINT_INLINE__ vec4 vec4VDivV(const vec4 *const restrict v1, const vec4 *const restrict v2){
	vec4 r;
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
	if(v2->w != 0.f){
		r.w = v1->w / v2->w;
	}else{
		r.w = 0.f;
	}
	return r;
}
__HINT_INLINE__ vec4 vec4VDivN(const vec4 *const restrict v, const float x, const float y, const float z, const float w){
	vec4 r;
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
	if(w != 0.f){
		r.w = v->w / w;
	}else{
		r.w = 0.f;
	}
	return r;
}
__HINT_INLINE__ vec4 vec4VDivS(const vec4 *const restrict v, const float s){
	vec4 r;
	if(s != 0.f){
		const float invS = 1.f / s;
		r.x = v->x * invS;
		r.y = v->y * invS;
		r.z = v->z * invS;
		r.w = v->z * invS;
	}else{
		vec4Zero(&r);
	}
	return r;
}
__HINT_INLINE__ vec4 vec4NDivV(const float x, const float y, const float z, const float w, const vec4 *const restrict v){
	vec4 r;
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
	if(v->w != 0.f){
		r.w = w / v->w;
	}else{
		r.w = w;
	}
	return r;
}
__HINT_INLINE__ vec4 vec4SDivV(const float s, const vec4 *const restrict v){
	vec4 r;
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
	if(v->w != 0.f){
		r.w = s / v->w;
	}else{
		r.w = s;
	}
	return r;
}
__HINT_INLINE__ void vec4DivVByV1(vec4 *const restrict v1, const vec4 *const restrict v2){
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
	if(v2->w != 0.f){
		v1->w /= v2->w;
	}else{
		v1->w = 0.f;
	}
}
__HINT_INLINE__ void vec4DivVByV2(const vec4 *const restrict v1, vec4 *const restrict v2){
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
	if(v2->w != 0.f){
		v2->w = v1->w / v2->w;
	}else{
		v2->w = 0.f;
	}
}
__HINT_INLINE__ void vec4DivVByVR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r){
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
	if(v2->w != 0.f){
		r->w = v1->w / v2->w;
	}else{
		r->w = 0.f;
	}
}
__HINT_INLINE__ void vec4DivVByN(vec4 *const restrict v, const float x, const float y, const float z, const float w){
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
	if(w != 0.f){
		v->w /= w;
	}else{
		v->w = 0.f;
	}
}
__HINT_INLINE__ void vec4DivVByS(vec4 *const restrict v, const float s){
	if(s != 0.f){
		const float invS = 1.f / s;
		v->x *= invS; v->y *= invS; v->z *= invS; v->w *= invS;
	}else{
		v->x = 0.f; v->y = 0.f; v->z = 0.f; v->w = 0.f;
	}
}
__HINT_INLINE__ void vec4DivNByV(const float x, const float y, const float z, const float w, vec4 *const restrict v){
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
	if(v->w != 0.f){
		v->w = w / v->w;
	}else{
		v->w = 0.f;
	}
}
__HINT_INLINE__ void vec4DivSByV(const float s, vec4 *const restrict v){
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
	if(v->w != 0.f){
		v->w = s / v->w;
	}else{
		v->w = 0.f;
	}
}

__FORCE_INLINE__ float vec4Magnitude(const vec4 *const restrict v){
	return sqrtf(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
}
__FORCE_INLINE__ float vec4MagnitudeSquared(const vec4 *const restrict v){
	return v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
}
__FORCE_INLINE__ float vec4InverseMagnitude(const vec4 *const restrict v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
	return fastInvSqrt(magnitudeSquared);
}

__HINT_INLINE__ vec4 vec4GetUnit(const vec4 *const restrict v){
	const float magnitude = vec4Magnitude(v);
	if(magnitude != 0.f){
		return vec4VDivS(v, magnitude);
	}
	return *v;
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
	const float magnitude = vec4Magnitude(v);
	if(magnitude != 0.f){
		vec4DivVByS(v, magnitude);
	}
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

__FORCE_INLINE__ float vec4Dot(const vec4 *const restrict v1, const vec4 *const restrict v2){
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