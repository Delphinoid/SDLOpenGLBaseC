#include "vec4.h"
#include "engineMath.h"
#include <math.h>

vec4 vec4New(const float x, const float y, const float z, const float w){
	vec4 v = {.x = x, .y = y, .z = z, .w = w};
	return v;
}
vec4 vec4NewS(const float s){
	vec4 v = {.x = s, .y = s, .z = s, .w = s};
	return v;
}
void vec4Set(vec4 *v, const float x, const float y, const float z, const float w){
	v->x = x; v->y = y; v->z = z; v->w = w;
}
void vec4SetS(vec4 *v, const float s){
	v->x = s; v->y = s; v->z = s; v->w = s;
}

vec4 vec4VAddV(const vec4 *v1, const vec4 *v2){
	vec4 r = {.x = v1->x + v2->x,
	          .y = v1->y + v2->y,
	          .z = v1->z + v2->z,
	          .w = v1->w + v2->w};
	return r;
}
vec4 vec4VAddN(const vec4 *v, const float x, const float y, const float z, const float w){
	vec4 r = {.x = v->x + x,
	          .y = v->y + y,
	          .z = v->z + z,
	          .w = v->w + w};
	return r;
}
vec4 vec4VAddS(const vec4 *v, const float s){
	vec4 r = {.x = v->x + s,
	          .y = v->y + s,
	          .z = v->z + s,
	          .w = v->w + s};
	return r;
}
void vec4AddVToV(vec4 *v1, const vec4 *v2){
	v1->x += v2->x; v1->y += v2->y; v1->z += v2->z; v1->w += v2->w;
}
void vec4AddVToVR(const vec4 *v1, const vec4 *v2, vec4 *r){
	r->x = v1->x + v2->x; r->y = v1->y + v2->y; r->z = v1->z + v2->z; r->w = v1->w + v2->w;
}
void vec4AddNToV(vec4 *v, const float x, const float y, const float z, const float w){
	v->x += x; v->y += y; v->z += z; v->w += w;
}
void vec4AddSToV(vec4 *v, const float s){
	v->x += s; v->y += s; v->z += s; v->w += s;
}

vec4 vec4VSubV(const vec4 *v1, const vec4 *v2){
	vec4 r = {.x = v1->x - v2->x,
	          .y = v1->y - v2->y,
	          .z = v1->z - v2->z,
	          .w = v1->w - v2->w};
	return r;
}
vec4 vec4VSubN(const vec4 *v, const float x, const float y, const float z, const float w){
	vec4 r = {.x = v->x - x,
	          .y = v->y - y,
	          .z = v->z - z,
	          .w = v->w - w};
	return r;
}
vec4 vec4VSubS(const vec4 *v, const float s){
	vec4 r = {.x = v->x - s,
	          .y = v->y - s,
	          .z = v->z - s,
	          .w = v->w - s};
	return r;
}
vec4 vec4NSubV(const float x, const float y, const float z, const float w, const vec4 *v){
	vec4 r = {.x = x - v->x,
	          .y = y - v->y,
	          .z = z - v->z,
	          .w = w - v->w};
	return r;
}
vec4 vec4SSubV(const float s, const vec4 *v){
	vec4 r = {.x = s - v->x,
	          .y = s - v->y,
	          .z = s - v->z,
	          .w = s - v->w};
	return r;
}
void vec4SubVFromV1(vec4 *v1, const vec4 *v2){
	v1->x -= v2->x; v1->y -= v2->y; v1->z -= v2->z; v1->w -= v2->w;
}
void vec4SubVFromV2(const vec4 *v1, vec4 *v2){
	v2->x = v1->x - v2->x; v2->y = v1->y - v2->y; v2->z = v1->z - v2->z; v2->w = v1->w - v2->w;
}
void vec4SubVFromVR(const vec4 *v1, const vec4 *v2, vec4 *r){
	r->x = v1->x - v2->x; r->y = v1->y - v2->y; r->z = v1->z - v2->z; r->w = v1->w - v2->w;
}
void vec4SubNFromV(vec4 *v, const float x, const float y, const float z, const float w){
	v->x -= x; v->y -= y; v->z -= z; v->w -= w;
}
void vec4SubSFromV(vec4 *v, const float s){
	v->x -= s; v->y -= s; v->z -= s; v->w -= s;
}
void vec4SubVFromN(const float x, const float y, const float z, const float w, vec4 *v){
	v->x = x - v->x; v->y = y - v->y; v->z = z - v->z; v->w = w - v->w;
}
void vec4SubVFromS(const float s, vec4 *v){
	v->x = s - v->x; v->y = s - v->y; v->z = s - v->z; v->w = s - v->w;
}

vec4 vec4VMultV(const vec4 *v1, const vec4 *v2){
	vec4 r = {.x = v1->x * v2->x,
	          .y = v1->y * v2->y,
	          .z = v1->z * v2->z,
	          .w = v1->w * v2->w};
	return r;
}
vec4 vec4VMultN(const vec4 *v, const float x, const float y, const float z, const float w){
	vec4 r = {.x = v->x * x,
	          .y = v->y * y,
	          .z = v->z * z,
	          .w = v->w * w};
	return r;
}
vec4 vec4VMultS(const vec4 *v, const float s){
	vec4 r = {.x = v->x * s,
	          .y = v->y * s,
	          .z = v->z * s,
	          .w = v->w * s};
	return r;
}
void vec4MultVByV(vec4 *v1, const vec4 *v2){
	v1->x *= v2->x; v1->y *= v2->y; v1->z *= v2->z; v1->w *= v2->w;
}
void vec4MultVByVR(const vec4 *v1, const vec4 *v2, vec4 *r){
	r->x = v1->x * v2->x; r->y = v1->y * v2->y; r->z = v1->z * v2->z; r->w = v1->w * v2->w;
}
void vec4MultVByN(vec4 *v, const float x, const float y, const float z, const float w){
	v->x *= x; v->y *= y; v->z *= z; v->w *= w;
}
void vec4MultVByS(vec4 *v, const float s){
	v->x *= s; v->y *= s; v->z *= s; v->w *= s;
}

vec4 vec4VDivV(const vec4 *v1, const vec4 *v2){
	if(v2->x != 0.f && v2->y != 0.f && v2->z != 0.f && v2->w != 0.f){
		vec4 r = {.x = v1->x / v2->x,
		          .y = v1->y / v2->y,
		          .z = v1->z / v2->z,
		          .w = v1->w / v2->w};
		return r;
	}
	return *v1;
}
vec4 vec4VDivN(const vec4 *v, const float x, const float y, const float z, const float w){
	if(x != 0.f && y != 0.f && z != 0.f && w != 0.f){
		vec4 r = {.x = v->x / x,
		          .y = v->y / y,
		          .z = v->z / z,
		          .w = v->w / w};
		return r;
	}
	return *v;
}
vec4 vec4VDivS(const vec4 *v, const float s){
	if(s != 0.f){
		vec4 r = {.x = v->x / s,
		          .y = v->y / s,
		          .z = v->z / s,
		          .w = v->w / s};
		return r;
	}
	return *v;
}
vec4 vec4NDivV(const float x, const float y, const float z, const float w, const vec4 *v){
	if(v->x != 0.f && v->y != 0.f && v->z != 0.f && v->w != 0.f){
		vec4 r = {.x = x / v->x,
		          .y = y / v->y,
		          .z = z / v->z,
		          .w = w / v->w};
		return r;
	}
	return *v;
}
vec4 vec4SDivV(const float s, const vec4 *v){
	if(v->x != 0.f && v->y != 0.f && v->z != 0.f && v->w != 0.f){
		vec4 r = {.x = s / v->x,
		          .y = s / v->y,
		          .z = s / v->z,
		          .w = s / v->w};
		return r;
	}
	return *v;
}
void vec4DivVByV1(vec4 *v1, const vec4 *v2){
	if(v2->x != 0.f && v2->y != 0.f && v2->z != 0.f && v2->w != 0.f){
		v1->x /= v2->x; v1->y /= v2->y; v1->z /= v2->z; v1->w /= v2->w;
	}
}
void vec4DivVByV2(const vec4 *v1, vec4 *v2){
	if(v2->x != 0.f && v2->y != 0.f && v2->z != 0.f && v2->w != 0.f){
		v2->x = v1->x / v2->x; v2->y = v1->y / v2->y; v2->z = v1->z / v2->z; v2->w = v1->w / v2->w;
	}
}
void vec4DivVByVR(const vec4 *v1, const vec4 *v2, vec4 *r){
	if(v2->x != 0.f && v2->y != 0.f && v2->z != 0.f && v2->w != 0.f){
		r->x = v1->x / v2->x; r->y = v1->y / v2->y; r->z = v1->z / v2->z; r->w = v1->w / v2->w;
	}
}
void vec4DivVByN(vec4 *v, const float x, const float y, const float z, const float w){
	if(x != 0.f && y != 0.f && z != 0.f && w != 0.f){
		v->x /= x; v->y /= y; v->z /= z; v->w /= w;
	}
}
void vec4DivVByS(vec4 *v, const float s){
	if(s != 0.f){
		v->x /= s; v->y /= s; v->z /= s; v->w /= s;
	}
}
void vec4DivNByV(const float x, const float y, const float z, const float w, vec4 *v){
	if(v->x != 0.f && v->y != 0.f && v->z != 0.f && v->w != 0.f){
		v->x = x / v->x; v->y = y / v->y; v->z = z / v->z; v->w = w / v->w;
	}
}
void vec4DivSByV(const float s, vec4 *v){
	if(v->x != 0.f && v->y != 0.f && v->z != 0.f && v->w != 0.f){
		v->x = s / v->x; v->y = s / v->y; v->z = s / v->z; v->w = s / v->w;
	}
}

float vec4GetMagnitude(const vec4 *v){
	return sqrtf(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
}

vec4 vec4GetUnit(const vec4 *v){
	const float magnitude = vec4GetMagnitude(v);
	if(magnitude != 0.f){
		return vec4VDivS(v, magnitude);
	}
	return *v;
}
vec4 vec4GetUnitFast(const vec4 *v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	return vec4VMultS(v, invSqrt);
}
void vec4Normalize(vec4 *v){
	const float magnitude = vec4GetMagnitude(v);
	if(magnitude != 0.f){
		vec4DivVByS(v, magnitude);
	}
}
void vec4NormalizeFast(vec4 *v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	vec4MultVByS(v, invSqrt);
}

float vec4Dot(const vec4 *v1, const vec4 *v2){
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w;
}

void vec4Lerp(const vec4 *v1, const vec4 *v2, const float t, vec4 *r){
	/*
	** r = v1 + (v2 - v1) * t
	*/
	vec4SubVFromVR(v2, v1, r);
	vec4MultVByS(r, t);
	vec4AddVToV(r, v1);
}
