#ifndef VEC3_H
#define VEC3_H

typedef struct {
	float x, y, z;
} vec3;

vec3 vec3New(float x, float y, float z);
vec3 vec3NewS(float s);
void vec3Set(vec3 *v, float x, float y, float z);
void vec3SetS(vec3 *v, float s);

vec3 vec3VAddV(vec3 v1, vec3 v2);
vec3 vec3VAddN(vec3 v, float x, float y, float z);
vec3 vec3VAddS(vec3 v, float s);
void vec3AddVToV(vec3 *v1, vec3 v2);
void vec3AddNToV(vec3 *v, float x, float y, float z);
void vec3AddSToV(vec3 *v, float s);

vec3 vec3VSubV(vec3 v1, vec3 v2);
vec3 vec3VSubN(vec3 v, float x, float y, float z);
vec3 vec3VSubS(vec3 v, float s);
vec3 vec3NSubV(float x, float y, float z, vec3 v);
vec3 vec3SSubV(float s, vec3 v);
void vec3SubVFromV1(vec3 *v1, vec3 v2);
void vec3SubVFromV2(vec3 v1, vec3 *v2);
void vec3SubNFromV(vec3 *v, float x, float y, float z);
void vec3SubSFromV(vec3 *v, float s);
void vec3SubVFromN(float x, float y, float z, vec3 *v);
void vec3SubVFromS(float s, vec3 *v);

vec3 vec3VMultV(vec3 v1, vec3 v2);
vec3 vec3VMultN(vec3 v, float x, float y, float z);
vec3 vec3VMultS(vec3 v, float s);
void vec3MultVByV(vec3 *v1, vec3 v2);
void vec3MultVByN(vec3 *v, float x, float y, float z);
void vec3MultVByS(vec3 *v, float s);

vec3 vec3VDivV(vec3 v1, vec3 v2);
vec3 vec3VDivN(vec3 v, float x, float y, float z);
vec3 vec3VDivS(vec3 v, float s);
vec3 vec3NDivV(float x, float y, float z, vec3 v);
vec3 vec3SDivV(float s, vec3 v);
void vec3DivVByV1(vec3 *v1, vec3 v2);
void vec3DivVByV2(vec3 v1, vec3 *v2);
void vec3DivVByN(vec3 *v, float x, float y, float z);
void vec3DivVByS(vec3 *v, float s);
void vec3DivNByV(float x, float y, float z, vec3 *v);
void vec3DivSByV(float s, vec3 *v);

float vec3GetMagnitude(vec3 v);

vec3 vec3GetUnit(vec3 v);
void vec3Normalize(vec3 *v);

float vec3Dot(vec3 v1, vec3 v2);
vec3 vec3Cross(vec3 v1, vec3 v2);

#endif
