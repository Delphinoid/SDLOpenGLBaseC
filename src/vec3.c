#include "vec3.h"
#include "math.h"

vec3 vec3New(float x, float y, float z){
	vec3 v = {.x = x, .y = y, .z = z};
	return v;
}
vec3 vec3NewS(float s){
	vec3 v = {.x = s, .y = s, .z = s};
	return v;
}
void vec3Set(vec3 *v, float x, float y, float z){
	v->x = x; v->y = y; v->z = z;
}
void vec3SetS(vec3 *v, float s){
	v->x = s; v->y = s; v->z = s;
}

vec3 vec3VAddV(vec3 v1, vec3 v2){
	vec3 r = {.x = v1.x + v2.x,
	          .y = v1.y + v2.y,
	          .z = v1.z + v2.z};
	return r;
}
vec3 vec3VAddN(vec3 v, float x, float y, float z){
	vec3 r = {.x = v.x + x,
	          .y = v.y + y,
	          .z = v.z + z};
	return r;
}
vec3 vec3VAddS(vec3 v, float s){
	vec3 r = {.x = v.x + s,
	          .y = v.y + s,
	          .z = v.z + s};
	return r;
}
void vec3AddVToV(vec3 *v1, vec3 v2){
	v1->x += v2.x; v1->y += v2.y; v1->z += v2.z;
}
void vec3AddNToV(vec3 *v, float x, float y, float z){
	v->x += x; v->y += y; v->z += z;
}
void vec3AddSToV(vec3 *v, float s){
	v->x += s; v->y += s; v->z += s;
}

vec3 vec3VSubV(vec3 v1, vec3 v2){
	vec3 r = {.x = v1.x - v2.x,
	          .y = v1.y - v2.y,
	          .z = v1.z - v2.z};
	return r;
}
vec3 vec3VSubN(vec3 v, float x, float y, float z){
	vec3 r = {.x = v.x - x,
	          .y = v.y - y,
	          .z = v.z - z};
	return r;
}
vec3 vec3VSubS(vec3 v, float s){
	vec3 r = {.x = v.x - s,
	          .y = v.y - s,
	          .z = v.z - s};
	return r;
}
vec3 vec3NSubV(float x, float y, float z, vec3 v){
	vec3 r = {.x = x - v.x,
	          .y = y - v.y,
	          .z = z - v.z};
	return r;
}
vec3 vec3SSubV(float s, vec3 v){
	vec3 r = {.x = s - v.x,
	          .y = s - v.y,
	          .z = s - v.z};
	return r;
}
void vec3SubVFromV1(vec3 *v1, vec3 v2){
	v1->x -= v2.x; v1->y -= v2.y; v1->z -= v2.z;
}
void vec3SubVFromV2(vec3 v1, vec3 *v2){
	v2->x = v1.x - v2->x; v2->y = v1.y - v2->y; v2->z = v1.z - v2->z;
}
void vec3SubNFromV(vec3 *v, float x, float y, float z){
	v->x -= x; v->y -= y; v->z -= z;
}
void vec3SubSFromV(vec3 *v, float s){
	v->x -= s; v->y -= s; v->z -= s;
}
void vec3SubVFromN(float x, float y, float z, vec3 *v){
	v->x = x - v->x; v->y = y - v->y; v->z = z - v->z;
}
void vec3SubVFromS(float s, vec3 *v){
	v->x = s - v->x; v->y = s - v->y; v->z = s - v->z;
}

vec3 vec3VMultV(vec3 v1, vec3 v2){
	vec3 r = {.x = v1.x * v2.x,
	          .y = v1.y * v2.y,
	          .z = v1.z * v2.z};
	return r;
}
vec3 vec3VMultN(vec3 v, float x, float y, float z){
	vec3 r = {.x = v.x * x,
	          .y = v.y * y,
	          .z = v.z * z};
	return r;
}
vec3 vec3VMultS(vec3 v, float s){
	vec3 r = {.x = v.x * s,
	          .y = v.y * s,
	          .z = v.z * s};
	return r;
}
void vec3MultVByV(vec3 *v1, vec3 v2){
	v1->x *= v2.x; v1->y *= v2.y; v1->z *= v2.z;
}
void vec3MultVByN(vec3 *v, float x, float y, float z){
	v->x *= x; v->y *= y; v->z *= z;
}
void vec3MultVByS(vec3 *v, float s){
	v->x *= s; v->y *= s; v->z *= s;
}

vec3 vec3VDivV(vec3 v1, vec3 v2){
	if(v2.x != 0 && v2.y != 0 && v2.z != 0){
		vec3 r = {.x = v1.x / v2.x,
		          .y = v1.y / v2.y,
		          .z = v1.z / v2.z};
		return r;
	}
	return v1;
}
vec3 vec3VDivN(vec3 v, float x, float y, float z){
	if(x != 0 && y != 0 && z != 0){
		vec3 r = {.x = v.x / x,
		          .y = v.y / y,
		          .z = v.z / z};
		return r;
	}
	return v;
}
vec3 vec3VDivS(vec3 v, float s){
	if(s != 0){
		vec3 r = {.x = v.x / s,
		          .y = v.y / s,
		          .z = v.z / s};
		return r;
	}
	return v;
}
vec3 vec3NDivV(float x, float y, float z, vec3 v){
	if(v.x != 0 && v.y != 0 && v.z != 0){
		vec3 r = {.x = x / v.x,
		          .y = y / v.y,
		          .z = z / v.z};
		return r;
	}
	return v;
}
vec3 vec3SDivV(float s, vec3 v){
	if(v.x != 0 && v.y != 0 && v.z != 0){
		vec3 r = {.x = s / v.x,
		          .y = s / v.y,
		          .z = s / v.z};
		return r;
	}
	return v;
}
void vec3DivVByV1(vec3 *v1, vec3 v2){
	if(v2.x != 0 && v2.y != 0 && v2.z != 0){
		v1->x /= v2.x; v1->y /= v2.y; v1->z /= v2.z;
	}
}
void vec3DivVByV2(vec3 v1, vec3 *v2){
	if(v1.x != 0 && v1.y != 0 && v1.z != 0){
		v2->x = v1.x / v2->x; v2->y = v1.y / v2->y; v2->z = v1.z / v2->z;
	}
}
void vec3DivVByN(vec3 *v, float x, float y, float z){
	if(x != 0 && y != 0 && z != 0){
		v->x /= x; v->y /= y; v->z /= z;
	}
}
void vec3DivVByS(vec3 *v, float s){
	if(s != 0){
		v->x /= s; v->y /= s; v->z /= s;
	}
}
void vec3DivNByV(float x, float y, float z, vec3 *v){
	if(v->x != 0 && v->y != 0 && v->z != 0){
		v->x = x / v->x; v->y = y / v->y; v->z = z / v->z;
	}
}
void vec3DivSByV(float s, vec3 *v){
	if(v->x != 0 && v->y != 0 && v->z != 0){
		v->x = s / v->x; v->y = s / v->y; v->z = s / v->z;
	}
}

float vec3GetMagnitude(vec3 v){
	return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

vec3 vec3GetUnit(vec3 v){
	float magnitude = vec3GetMagnitude(v);
	if(magnitude != 0){
		return vec3VDivN(v, magnitude, magnitude, magnitude);
	}
	return v;
}
void vec3Normalize(vec3 *v){
	float magnitude = vec3GetMagnitude(*v);
	if(magnitude != 0){
		vec3DivVByN(v, magnitude, magnitude, magnitude);
	}
}

float vec3Dot(vec3 v1, vec3 v2){
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
vec3 vec3Cross(vec3 v1, vec3 v2){
	vec3 r = {.x = v1.y * v2.z - v1.z * v2.y,
	          .y = v1.z * v2.x - v1.x * v2.z,
	          .z = v1.x * v2.y - v1.y * v2.x};
    return r;
}
