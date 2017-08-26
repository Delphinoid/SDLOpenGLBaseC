#ifndef VEC3_H
#define VEC3_H

typedef struct {
	float x, y, z;
} vec3;

vec3 vec3New(const float x, const float y, const float z);
vec3 vec3NewS(const float s);
void vec3Set(vec3 *v, const float x, const float y, const float z);
void vec3SetS(vec3 *v, const float s);

vec3 vec3VAddV(const vec3 v1, const vec3 v2);
vec3 vec3VAddN(const vec3 v, const float x, const float y, const float z);
vec3 vec3VAddS(const vec3 v, const float s);
void vec3AddVToV(vec3 *v1, const vec3 v2);
void vec3AddNToV(vec3 *v, const float x, const float y, const float z);
void vec3AddSToV(vec3 *v, const float s);

vec3 vec3VSubV(const vec3 v1, const vec3 v2);
vec3 vec3VSubN(const vec3 v, const float x, const float y, const float z);
vec3 vec3VSubS(const vec3 v, const float s);
vec3 vec3NSubV(const float x, const float y, const float z, const vec3 v);
vec3 vec3SSubV(const float s, const vec3 v);
void vec3SubVFromV1(vec3 *v1, const vec3 v2);
void vec3SubVFromV2(const vec3 v1, vec3 *v2);
void vec3SubNFromV(vec3 *v, const float x, const float y, const float z);
void vec3SubSFromV(vec3 *v, const float s);
void vec3SubVFromN(const float x, const float y, const float z, vec3 *v);
void vec3SubVFromS(const float s, vec3 *v);

vec3 vec3VMultV(const vec3 v1, const vec3 v2);
vec3 vec3VMultN(const vec3 v, const float x, const float y, const float z);
vec3 vec3VMultS(const vec3 v, const float s);
void vec3MultVByV(vec3 *v1, const vec3 v2);
void vec3MultVByN(vec3 *v, const float x, const float y, const float z);
void vec3MultVByS(vec3 *v, const float s);

vec3 vec3VDivV(const vec3 v1, const vec3 v2);
vec3 vec3VDivN(const vec3 v, const float x, const float y, const float z);
vec3 vec3VDivS(const vec3 v, const float s);
vec3 vec3NDivV(const float x, const float y, const float z, const vec3 v);
vec3 vec3SDivV(const float s, vec3 v);
void vec3DivVByV1(vec3 *v1, const vec3 v2);
void vec3DivVByV2(const vec3 v1, vec3 *v2);
void vec3DivVByN(vec3 *v, const float x, const float y, const float z);
void vec3DivVByS(vec3 *v, const float s);
void vec3DivNByV(const float x, const float y, const float z, vec3 *v);
void vec3DivSByV(const float s, vec3 *v);

float vec3GetMagnitude(const vec3 v);

vec3 vec3GetUnit(const vec3 v);
void vec3Normalize(vec3 *v);

float vec3Dot(const vec3 v1, const vec3 v2);
vec3 vec3GetCross(const vec3 v1, const vec3 v2);
void vec3Cross(const vec3 v1, const vec3 v2, vec3 *r);

#endif
