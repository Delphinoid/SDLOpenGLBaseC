#ifndef VEC3_H
#define VEC3_H

typedef struct {
	float x, y, z;
} vec3;

vec3 vec3New(const float x, const float y, const float z);
vec3 vec3NewS(const float s);
void vec3Set(vec3 *const restrict v, const float x, const float y, const float z);
void vec3SetS(vec3 *const restrict v, const float s);
void vec3Zero(vec3 *const restrict v);

vec3 vec3VAddV(const vec3 *const restrict v1, const vec3 *const restrict v2);
vec3 vec3VAddN(const vec3 *const restrict v, const float x, const float y, const float z);
vec3 vec3VAddS(const vec3 *const restrict v, const float s);
void vec3AddVToV(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3AddVToVR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);
void vec3AddNToV(vec3 *const restrict v, const float x, const float y, const float z);
void vec3AddSToV(vec3 *const restrict v, const float s);

vec3 vec3VSubV(const vec3 *const restrict v1, const vec3 *const restrict v2);
vec3 vec3VSubN(const vec3 *const restrict v, const float x, const float y, const float z);
vec3 vec3VSubS(const vec3 *const restrict v, const float s);
vec3 vec3NSubV(const float x, const float y, const float z, const vec3 *const restrict v);
vec3 vec3SSubV(const float s, const vec3 *const restrict v);
void vec3SubVFromV1(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3SubVFromV2(const vec3 *const restrict v1, vec3 *const restrict v2);
void vec3SubVFromVR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);
void vec3SubNFromV(vec3 *const restrict v, const float x, const float y, const float z);
void vec3SubSFromV(vec3 *const restrict v, const float s);
void vec3SubVFromN(const float x, const float y, const float z, vec3 *const restrict v);
void vec3SubVFromS(const float s, vec3 *const restrict v);

vec3 vec3VMultV(const vec3 *const restrict v1, const vec3 *const restrict v2);
vec3 vec3VMultN(const vec3 *const restrict v, const float x, const float y, const float z);
vec3 vec3VMultS(const vec3 *const restrict v, const float s);
void vec3MultVByV(vec3 *v1, const vec3 *v2);
void vec3MultVByVR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);
void vec3MultVByN(vec3 *const restrict v, const float x, const float y, const float z);
void vec3MultVByS(vec3 *const restrict v, const float s);
void vec3MultVBySR(const vec3 *const restrict v, const float s, vec3 *const restrict r);

vec3 vec3VDivV(const vec3 *const restrict v1, const vec3 *const restrict v2);
vec3 vec3VDivN(const vec3 *const restrict v, const float x, const float y, const float z);
vec3 vec3VDivS(const vec3 *const restrict v, const float s);
vec3 vec3NDivV(const float x, const float y, const float z, const vec3 *const restrict v);
vec3 vec3SDivV(const float s, const vec3 *const restrict v);
void vec3DivVByV1(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3DivVByV2(const vec3 *const restrict v1, vec3 *const restrict v2);
void vec3DivVByVR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);
void vec3DivVByN(vec3 *const restrict v, const float x, const float y, const float z);
void vec3DivVByS(vec3 *const restrict v, const float s);
void vec3DivNByV(const float x, const float y, const float z, vec3 *const restrict v);
void vec3DivSByV(const float s, vec3 *const restrict v);

void vec3Min(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);
void vec3Max(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);

void vec3Negate(vec3 *const restrict v);
void vec3NegateR(const vec3 *const restrict v, vec3 *const restrict r);

float vec3Magnitude(const vec3 *const restrict v);
float vec3MagnitudeSquared(const vec3 *const restrict v);
float vec3InverseMagnitude(const vec3 *const restrict v);

vec3 vec3GetUnit(const vec3 *const restrict v);
vec3 vec3GetUnitFast(const vec3 *const restrict v);
vec3 vec3GetUnitFastAccurate(const vec3 *const restrict v);
void vec3Normalize(vec3 *const restrict v);
void vec3NormalizeFast(vec3 *const restrict v);
void vec3NormalizeFastAccurate(vec3 *const restrict v);

float vec3Dot(const vec3 *const restrict v1, const vec3 *const restrict v2);
vec3 vec3GetCross(const vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3Cross1(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3Cross2(const vec3 *const restrict v1, vec3 *const restrict v2);
void vec3CrossR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);

void vec3Lerp1(vec3 *const restrict v1, const vec3 *const restrict v2, const float t);
void vec3Lerp2(const vec3 *const restrict v1, vec3 *const restrict v2, const float t);
void vec3LerpR(const vec3 *const restrict v1, const vec3 *const restrict v2, const float t, vec3 *const restrict r);

void vec3CombineLinear(const vec3 *const restrict v1, const vec3 *const restrict v2, const vec3 *const restrict v3,
                       const float a, const float b, const float c, vec3 *const restrict r);

#endif
