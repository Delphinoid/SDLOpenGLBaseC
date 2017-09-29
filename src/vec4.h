#ifndef VEC4_H
#define VEC4_H

#include "vec3.h"

typedef struct {
	float x, y, z, w;
} vec4;

vec4 vec4New(const float x, const float y, const float z, const float w);
vec4 vec4NewS(const float s);
void vec4Set(vec4 *v, const float x, const float y, const float z, const float w);
void vec4SetS(vec4 *v, const float s);

vec4 vec4VAddV(const vec4 *v1, const vec4 *v2);
vec4 vec4VAddN(const vec4 *v, const float x, const float y, const float z, const float w);
vec4 vec4VAddS(const vec4 *v, const float s);
void vec4AddVToV(vec4 *v1, const vec4 *v2);
void vec4AddNToV(vec4 *v, const float x, const float y, const float z, const float w);
void vec4AddSToV(vec4 *v, const float s);

vec4 vec4VSubV(const vec4 *v1, const vec4 *v2);
vec4 vec4VSubN(const vec4 *v, const float x, const float y, const float z, const float w);
vec4 vec4VSubS(const vec4 *v, const float s);
vec4 vec4NSubV(const float x, const float y, const float z, const float w, const vec4 *v);
vec4 vec4SSubV(const float s, const vec4 *v);
void vec4SubVFromV1(vec4 *v1, const vec4 *v2);
void vec4SubVFromV2(const vec4 *v1, vec4 *v2);
void vec4SubNFromV(vec4 *v, const float x, const float y, const float z, const float w);
void vec4SubSFromV(vec4 *v, const float s);
void vec4SubVFromN(const float x, const float y, const float z, const float w, vec4 *v);
void vec4SubVFromS(const float s, vec4 *v);

vec4 vec4VMultV(const vec4 *v1, const vec4 *v2);
vec4 vec4VMultN(const vec4 *v, const float x, const float y, const float z, const float w);
vec4 vec4VMultS(const vec4 *v, const float s);
void vec4MultVByV(vec4 *v1, const vec4 *v2);
void vec4MultVByN(vec4 *v, const float x, const float y, const float z, const float w);
void vec4MultVByS(vec4 *v, const float s);

vec4 vec4VDivV(const vec4 *v1, const vec4 *v2);
vec4 vec4VDivN(const vec4 *v, const float x, const float y, const float z, const float w);
vec4 vec4VDivS(const vec4 *v, const float s);
vec4 vec4NDivV(const float x, const float y, const float z, const float w, const vec4 *v);
vec4 vec4SDivV(const float s, const vec4 *v);
void vec4DivVByV1(vec4 *v1, const vec4 *v2);
void vec4DivVByV2(const vec4 *v1, vec4 *v2);
void vec4DivVByN(vec4 *v, const float x, const float y, const float z, const float w);
void vec4DivVByS(vec4 *v, const float s);
void vec4DivNByV(const float x, const float y, const float z, const float w, vec4 *v);
void vec4DivSByV(const float s, vec4 *v);

float vec4GetMagnitude(const vec4 *v);

vec4 vec4GetUnit(const vec4 *v);
vec4 vec4GetUnitFast(const vec4 *v);
void vec4Normalize(vec4 *v);
void vec4NormalizeFast(vec4 *v);

float vec4Dot(const vec4 *v1, const vec4 *v2);

#endif
