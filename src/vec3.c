#include "vec3.h"
#include <math.h>

float fastInvSqrt(float x);

vec3 vec3New(const float x, const float y, const float z){
	const vec3 v = {.x = x, .y = y, .z = z};
	return v;
}
vec3 vec3NewS(const float s){
	const vec3 v = {.x = s, .y = s, .z = s};
	return v;
}
void vec3Set(vec3 *v, const float x, const float y, const float z){
	v->x = x; v->y = y; v->z = z;
}
void vec3SetS(vec3 *v, const float s){
	v->x = s; v->y = s; v->z = s;
}

vec3 vec3VAddV(const vec3 *v1, const vec3 *v2){
	vec3 r = {.x = v1->x + v2->x,
	          .y = v1->y + v2->y,
	          .z = v1->z + v2->z};
	return r;
}
vec3 vec3VAddN(const vec3 *v, const float x, const float y, const float z){
	vec3 r = {.x = v->x + x,
	          .y = v->y + y,
	          .z = v->z + z};
	return r;
}
vec3 vec3VAddS(const vec3 *v, const float s){
	vec3 r = {.x = v->x + s,
	          .y = v->y + s,
	          .z = v->z + s};
	return r;
}
void vec3AddVToV(vec3 *v1, const vec3 *v2){
	v1->x += v2->x; v1->y += v2->y; v1->z += v2->z;
}
void vec3AddVToVR(const vec3 *v1, const vec3 *v2, vec3 *r){
	r->x = v1->x + v2->x; r->y = v1->y + v2->y; r->z = v1->z + v2->z;
}
void vec3AddNToV(vec3 *v, const float x, const float y, const float z){
	v->x += x; v->y += y; v->z += z;
}
void vec3AddSToV(vec3 *v, const float s){
	v->x += s; v->y += s; v->z += s;
}

vec3 vec3VSubV(const vec3 *v1, const vec3 *v2){
	vec3 r = {.x = v1->x - v2->x,
	          .y = v1->y - v2->y,
	          .z = v1->z - v2->z};
	return r;
}
vec3 vec3VSubN(const vec3 *v, const float x, const float y, const float z){
	vec3 r = {.x = v->x - x,
	          .y = v->y - y,
	          .z = v->z - z};
	return r;
}
vec3 vec3VSubS(const vec3 *v, const float s){
	vec3 r = {.x = v->x - s,
	          .y = v->y - s,
	          .z = v->z - s};
	return r;
}
vec3 vec3NSubV(const float x, const float y, const float z, const vec3 *v){
	vec3 r = {.x = x - v->x,
	          .y = y - v->y,
	          .z = z - v->z};
	return r;
}
vec3 vec3SSubV(const float s, const vec3 *v){
	vec3 r = {.x = s - v->x,
	          .y = s - v->y,
	          .z = s - v->z};
	return r;
}
void vec3SubVFromV1(vec3 *v1, const vec3 *v2){
	v1->x -= v2->x; v1->y -= v2->y; v1->z -= v2->z;
}
void vec3SubVFromV2(const vec3 *v1, vec3 *v2){
	v2->x = v1->x - v2->x; v2->y = v1->y - v2->y; v2->z = v1->z - v2->z;
}
void vec3SubVFromVR(const vec3 *v1, const vec3 *v2, vec3 *r){
	r->x = v1->x - v2->x; r->y = v1->y - v2->y; r->z = v1->z - v2->z;
}
void vec3SubNFromV(vec3 *v, const float x, const float y, const float z){
	v->x -= x; v->y -= y; v->z -= z;
}
void vec3SubSFromV(vec3 *v, const float s){
	v->x -= s; v->y -= s; v->z -= s;
}
void vec3SubVFromN(const float x, const float y, const float z, vec3 *v){
	v->x = x - v->x; v->y = y - v->y; v->z = z - v->z;
}
void vec3SubVFromS(const float s, vec3 *v){
	v->x = s - v->x; v->y = s - v->y; v->z = s - v->z;
}

vec3 vec3VMultV(const vec3 *v1, const vec3 *v2){
	vec3 r = {.x = v1->x * v2->x,
	          .y = v1->y * v2->y,
	          .z = v1->z * v2->z};
	return r;
}
vec3 vec3VMultN(const vec3 *v, const float x, const float y, const float z){
	vec3 r = {.x = v->x * x,
	          .y = v->y * y,
	          .z = v->z * z};
	return r;
}
vec3 vec3VMultS(const vec3 *v, const float s){
	vec3 r = {.x = v->x * s,
	          .y = v->y * s,
	          .z = v->z * s};
	return r;
}
void vec3MultVByV(vec3 *v1, const vec3 *v2){
	v1->x *= v2->x; v1->y *= v2->y; v1->z *= v2->z;
}
void vec3MultVByVR(const vec3 *v1, const vec3 *v2, vec3 *r){
	r->x = v1->x * v2->x; r->y = v1->y * v2->y; r->z = v1->z * v2->z;
}
void vec3MultVByN(vec3 *v, const float x, const float y, const float z){
	v->x *= x; v->y *= y; v->z *= z;
}
void vec3MultVByS(vec3 *v, const float s){
	v->x *= s; v->y *= s; v->z *= s;
}

vec3 vec3VDivV(const vec3 *v1, const vec3 *v2){
	if(v2->x != 0.f && v2->y != 0.f && v2->z != 0.f){
		vec3 r = {.x = v1->x / v2->x,
		          .y = v1->y / v2->y,
		          .z = v1->z / v2->z};
		return r;
	}
	return *v1;
}
vec3 vec3VDivN(const vec3 *v, const float x, const float y, const float z){
	if(x != 0.f && y != 0.f && z != 0.f){
		vec3 r = {.x = v->x / x,
		          .y = v->y / y,
		          .z = v->z / z};
		return r;
	}
	return *v;
}
vec3 vec3VDivS(const vec3 *v, const float s){
	if(s != 0.f){
		vec3 r = {.x = v->x / s,
		          .y = v->y / s,
		          .z = v->z / s};
		return r;
	}
	return *v;
}
vec3 vec3NDivV(const float x, const float y, const float z, const vec3 *v){
	if(v->x != 0.f && v->y != 0.f && v->z != 0.f){
		vec3 r = {.x = x / v->x,
		          .y = y / v->y,
		          .z = z / v->z};
		return r;
	}
	return *v;
}
vec3 vec3SDivV(const float s, const vec3 *v){
	if(v->x != 0.f && v->y != 0.f && v->z != 0.f){
		vec3 r = {.x = s / v->x,
		          .y = s / v->y,
		          .z = s / v->z};
		return r;
	}
	return *v;
}
void vec3DivVByV1(vec3 *v1, const vec3 *v2){
	if(v2->x != 0.f && v2->y != 0.f && v2->z != 0.f){
		v1->x /= v2->x; v1->y /= v2->y; v1->z /= v2->z;
	}
}
void vec3DivVByV2(const vec3 *v1, vec3 *v2){
	if(v2->x != 0.f && v2->y != 0.f && v2->z != 0.f){
		v2->x = v1->x / v2->x; v2->y = v1->y / v2->y; v2->z = v1->z / v2->z;
	}
}
void vec3DivVByVR(const vec3 *v1, const vec3 *v2, vec3 *r){
	if(v2->x != 0.f && v2->y != 0.f && v2->z != 0.f){
		r->x = v1->x / v2->x; r->y = v1->y / v2->y; r->z = v1->z / v2->z;
	}
}
void vec3DivVByN(vec3 *v, const float x, const float y, const float z){
	if(x != 0.f && y != 0.f && z != 0.f){
		v->x /= x; v->y /= y; v->z /= z;
	}
}
void vec3DivVByS(vec3 *v, const float s){
	if(s != 0.f){
		v->x /= s; v->y /= s; v->z /= s;
	}
}
void vec3DivNByV(const float x, const float y, const float z, vec3 *v){
	if(v->x != 0.f && v->y != 0.f && v->z != 0.f){
		v->x = x / v->x; v->y = y / v->y; v->z = z / v->z;
	}
}
void vec3DivSByV(const float s, vec3 *v){
	if(v->x != 0.f && v->y != 0.f && v->z != 0.f){
		v->x = s / v->x; v->y = s / v->y; v->z = s / v->z;
	}
}

float vec3GetMagnitude(const vec3 *v){
	return sqrtf(v->x*v->x + v->y*v->y + v->z*v->z);
}

vec3 vec3GetUnit(const vec3 *v){
	const float magnitude = vec3GetMagnitude(v);
	if(magnitude != 0.f){
		return vec3VDivS(v, magnitude);
	}
	return *v;
}
vec3 vec3GetUnitFast(const vec3 *v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	return vec3VMultS(v, invSqrt);
}
void vec3Normalize(vec3 *v){
	const float magnitude = vec3GetMagnitude(v);
	if(magnitude != 0.f){
		vec3DivVByS(v, magnitude);
	}
}
void vec3NormalizeFast(vec3 *v){
	const float magnitudeSquared = v->x*v->x + v->y*v->y + v->z*v->z;
	const float invSqrt = fastInvSqrt(magnitudeSquared);
	vec3MultVByS(v, invSqrt);
}

float vec3Dot(const vec3 *v1, const vec3 *v2){
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}
vec3 vec3GetCross(const vec3 *v1, const vec3 *v2){
	vec3 r = {.x = v1->y * v2->z - v1->z * v2->y,
	          .y = v1->z * v2->x - v1->x * v2->z,
	          .z = v1->x * v2->y - v1->y * v2->x};
    return r;
}
void vec3Cross(const vec3 *v1, const vec3 *v2, vec3 *r){
	r->x = v1->y * v2->z - v1->z * v2->y;
	r->y = v1->z * v2->x - v1->x * v2->z;
	r->z = v1->x * v2->y - v1->y * v2->x;
}

void vec3Lerp(const vec3 *v1, const vec3 *v2, const float t, vec3 *r){
	/*
	** r = v1 + (v2 - v1) * t
	*/
	vec3SubVFromVR(v2, v1, r);
	vec3MultVByS(r, t);
	vec3AddVToV(r, v1);
}
