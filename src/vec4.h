#ifndef VEC4_H
#define VEC4_H

#define vec3Extend(v, w) vec4New(v.x, v.y, v.z, w)

typedef struct vec3 vec3;

typedef struct vec4 {
	float x, y, z, w;
} vec4;

vec4 vec4New(float x, float y, float z, float w);
vec4 vec4NewS(float s);
void vec4Set(vec4 *v, float x, float y, float z, float w);
void vec4SetS(vec4 *v, float s);

vec4 vec4VAddV(vec4 v1, vec4 v2);
vec4 vec4VAddN(vec4 v, float x, float y, float z, float w);
vec4 vec4VAddS(vec4 v, float s);
void vec4AddVToV(vec4 *v1, vec4 v2);
void vec4AddNToV(vec4 *v, float x, float y, float z, float w);
void vec4AddSToV(vec4 *v, float s);

vec4 vec4VSubV(vec4 v1, vec4 v2);
vec4 vec4VSubN(vec4 v, float x, float y, float z, float w);
vec4 vec4VSubS(vec4 v, float s);
vec4 vec4NSubV(float x, float y, float z, float w, vec4 v);
vec4 vec4SSubV(float s, vec4 v);
void vec4SubVFromV1(vec4 *v1, vec4 v2);
void vec4SubVFromV2(vec4 v1, vec4 *v2);
void vec4SubNFromV(vec4 *v, float x, float y, float z, float w);
void vec4SubSFromV(vec4 *v, float s);
void vec4SubVFromN(float x, float y, float z, float w, vec4 *v);
void vec4SubVFromS(float s, vec4 *v);

vec4 vec4VMultV(vec4 v1, vec4 v2);
vec4 vec4VMultN(vec4 v, float x, float y, float z, float w);
vec4 vec4VMultS(vec4 v, float s);
void vec4MultVByV(vec4 *v1, vec4 v2);
void vec4MultVByN(vec4 *v, float x, float y, float z, float w);
void vec4MultVByS(vec4 *v, float s);

vec4 vec4VDivV(vec4 v1, vec4 v2);
vec4 vec4VDivN(vec4 v, float x, float y, float z, float w);
vec4 vec4VDivS(vec4 v, float s);
vec4 vec4NDivV(float x, float y, float z, float w, vec4 v);
vec4 vec4SDivV(float s, vec4 v);
void vec4DivVByV1(vec4 *v1, vec4 v2);
void vec4DivVByV2(vec4 v1, vec4 *v2);
void vec4DivVByN(vec4 *v, float x, float y, float z, float w);
void vec4DivVByS(vec4 *v, float s);
void vec4DivNByV(float x, float y, float z, float w, vec4 *v);
void vec4DivSByV(float s, vec4 *v);

float vec4GetMagnitude(vec4 v);

vec4 vec4GetUnit(vec4 v);
void vec4Normalize(vec4 *v);

#endif
