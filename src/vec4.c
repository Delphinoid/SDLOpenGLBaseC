#include "vec4.h"
#include "vec3.h"
#include "math.h"

vec4 vec4New(float x, float y, float z, float w){
	vec4 v = {.x = x, .y = y, .z = z, .w = w};
	return v;
}
vec4 vec4NewS(float s){
	vec4 v = {.x = s, .y = s, .z = s, .w = s};
	return v;
}
void vec4Set(vec4 *v, float x, float y, float z, float w){
	v->x = x; v->y = y; v->z = z; v->w = w;
}
void vec4SetS(vec4 *v, float s){
	v->x = s; v->y = s; v->z = s; v->w = s;
}

vec4 vec4VAddV(vec4 v1, vec4 v2){
	vec4 r = {.x = v1.x + v2.x,
	          .y = v1.y + v2.y,
	          .z = v1.z + v2.z,
	          .w = v1.w + v2.w};
	return r;
}
vec4 vec4VAddN(vec4 v, float x, float y, float z, float w){
	vec4 r = {.x = v.x + x,
	          .y = v.y + y,
	          .z = v.z + z,
	          .w = v.w + w};
	return r;
}
vec4 vec4VAddS(vec4 v, float s){
	vec4 r = {.x = v.x + s,
	          .y = v.y + s,
	          .z = v.z + s,
	          .w = v.w + s};
	return r;
}
void vec4AddVToV(vec4 *v1, vec4 v2){
	v1->x += v2.x; v1->y += v2.y; v1->z += v2.z; v1->w += v2.w;
}
void vec4AddNToV(vec4 *v, float x, float y, float z, float w){
	v->x += x; v->y += y; v->z += z; v->w += w;
}
void vec4AddSToV(vec4 *v, float s){
	v->x += s; v->y += s; v->z += s; v->w += s;
}

vec4 vec4VSubV(vec4 v1, vec4 v2){
	vec4 r = {.x = v1.x - v2.x,
	          .y = v1.y - v2.y,
	          .z = v1.z - v2.z,
	          .w = v1.w - v2.w};
	return r;
}
vec4 vec4VSubN(vec4 v, float x, float y, float z, float w){
	vec4 r = {.x = v.x - x,
	          .y = v.y - y,
	          .z = v.z - z,
	          .w = v.w - w};
	return r;
}
vec4 vec4VSubS(vec4 v, float s){
	vec4 r = {.x = v.x - s,
	          .y = v.y - s,
	          .z = v.z - s,
	          .w = v.w - s};
	return r;
}
vec4 vec4NSubV(float x, float y, float z, float w, vec4 v){
	vec4 r = {.x = x - v.x,
	          .y = y - v.y,
	          .z = z - v.z,
	          .w = w - v.w};
	return r;
}
vec4 vec4SSubV(float s, vec4 v){
	vec4 r = {.x = s - v.x,
	          .y = s - v.y,
	          .z = s - v.z,
	          .w = s - v.w};
	return r;
}
void vec4SubVFromV1(vec4 *v1, vec4 v2){
	v1->x -= v2.x; v1->y -= v2.y; v1->z -= v2.z; v1->w -= v2.w;
}
void vec4SubVFromV2(vec4 v1, vec4 *v2){
	v2->x = v1.x - v2->x; v2->y = v1.y - v2->y; v2->z = v1.z - v2->z; v2->w = v1.w - v2->w;
}
void vec4SubNFromV(vec4 *v, float x, float y, float z, float w){
	v->x -= x; v->y -= y; v->z -= z; v->w -= w;
}
void vec4SubSFromV(vec4 *v, float s){
	v->x -= s; v->y -= s; v->z -= s; v->w -= s;
}
void vec4SubVFromN(float x, float y, float z, float w, vec4 *v){
	v->x = x - v->x; v->y = y - v->y; v->z = z - v->z; v->w = w - v->w;
}
void vec4SubVFromS(float s, vec4 *v){
	v->x = s - v->x; v->y = s - v->y; v->z = s - v->z; v->w = s - v->w;
}

vec4 vec4VMultV(vec4 v1, vec4 v2){
	vec4 r = {.x = v1.x * v2.x,
	          .y = v1.y * v2.y,
	          .z = v1.z * v2.z,
	          .w = v1.w * v2.w};
	return r;
}
vec4 vec4VMultN(vec4 v, float x, float y, float z, float w){
	vec4 r = {.x = v.x * x,
	          .y = v.y * y,
	          .z = v.z * z,
	          .w = v.w * w};
	return r;
}
vec4 vec4VMultS(vec4 v, float s){
	vec4 r = {.x = v.x * s,
	          .y = v.y * s,
	          .z = v.z * s,
	          .w = v.w * s};
	return r;
}
void vec4MultVByV(vec4 *v1, vec4 v2){
	v1->x *= v2.x; v1->y *= v2.y; v1->z *= v2.z; v1->w *= v2.w;
}
void vec4MultVByN(vec4 *v, float x, float y, float z, float w){
	v->x *= x; v->y *= y; v->z *= z; v->w *= w;
}
void vec4MultVByS(vec4 *v, float s){
	v->x *= s; v->y *= s; v->z *= s; v->w *= s;
}

vec4 vec4VDivV(vec4 v1, vec4 v2){
	if(v2.x != 0 && v2.y != 0 && v2.z != 0 && v2.w != 0){
		vec4 r = {.x = v1.x / v2.x,
		          .y = v1.y / v2.y,
		          .z = v1.z / v2.z,
		          .w = v1.w / v2.w};
		return r;
	}
	return v1;
}
vec4 vec4VDivN(vec4 v, float x, float y, float z, float w){
	if(x != 0 && y != 0 && z != 0 && w != 0){
		vec4 r = {.x = v.x / x,
		          .y = v.y / y,
		          .z = v.z / z,
		          .w = v.w / w};
		return r;
	}
	return v;
}
vec4 vec4VDivS(vec4 v, float s){
	if(s != 0){
		vec4 r = {.x = v.x / s,
		          .y = v.y / s,
		          .z = v.z / s,
		          .w = v.w / s};
		return r;
	}
	return v;
}
vec4 vec4NDivV(float x, float y, float z, float w, vec4 v){
	if(v.x != 0 && v.y != 0 && v.z != 0 && v.w != 0){
		vec4 r = {.x = x / v.x,
		          .y = y / v.y,
		          .z = z / v.z,
		          .w = w / v.w};
		return r;
	}
	return v;
}
vec4 vec4SDivV(float s, vec4 v){
	if(v.x != 0 && v.y != 0 && v.z != 0 && v.w != 0){
		vec4 r = {.x = s / v.x,
		          .y = s / v.y,
		          .z = s / v.z,
		          .w = s / v.w};
		return r;
	}
	return v;
}
void vec4DivVByV1(vec4 *v1, vec4 v2){
	if(v2.x != 0 && v2.y != 0 && v2.z != 0 && v2.w != 0){
		v1->x /= v2.x; v1->y /= v2.y; v1->z /= v2.z; v1->w /= v2.w;
	}
}
void vec4DivVByV2(vec4 v1, vec4 *v2){
	if(v1.x != 0 && v1.y != 0 && v1.z != 0 && v1.w != 0){
		v2->x = v1.x / v2->x; v2->y = v1.y / v2->y; v2->z = v1.z / v2->z; v2->w = v1.w / v2->w;
	}
}
void vec4DivVByN(vec4 *v, float x, float y, float z, float w){
	if(x != 0 && y != 0 && z != 0 && w != 0){
		v->x /= x; v->y /= y; v->z /= z; v->w /= w;
	}
}
void vec4DivVByS(vec4 *v, float s){
	if(s != 0){
		v->x /= s; v->y /= s; v->z /= s; v->w /= s;
	}
}
void vec4DivNByV(float x, float y, float z, float w, vec4 *v){
	if(v->x != 0 && v->y != 0 && v->z != 0 && v->w != 0){
		v->x = x / v->x; v->y = y / v->y; v->z = z / v->z; v->w = w / v->w;
	}
}
void vec4DivSByV(float s, vec4 *v){
	if(v->x != 0 && v->y != 0 && v->z != 0 && v->w != 0){
		v->x = s / v->x; v->y = s / v->y; v->z = s / v->z; v->w = s / v->w;
	}
}

float vec4GetMagnitude(vec4 v){
	return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}

vec4 vec4GetUnit(vec4 v){
	float magnitude = vec4GetMagnitude(v);
	if(magnitude != 0){
		return vec4VDivN(v, magnitude, magnitude, magnitude, magnitude);
	}
	return v;
}
void vec4Normalize(vec4 *v){
	float magnitude = vec4GetMagnitude(*v);
	if(magnitude != 0){
		vec4DivVByN(v, magnitude, magnitude, magnitude, magnitude);
	}
}
