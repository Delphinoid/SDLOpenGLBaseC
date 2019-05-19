#ifndef VEC4_H
#define VEC4_H

typedef struct {
	float x, y, z, w;
} vec4;

vec4 vec4New(const float x, const float y, const float z, const float w);
vec4 vec4NewS(const float s);
vec4 vec4Zero();
void vec4Set(vec4 *const restrict v, const float x, const float y, const float z, const float w);
void vec4SetS(vec4 *const restrict v, const float s);
void vec4ZeroP(vec4 *const restrict v);

vec4 vec4VAddV(const vec4 *const restrict v1, const vec4 *const restrict v2);
vec4 vec4VAddN(const vec4 *const restrict v, const float x, const float y, const float z, const float w);
vec4 vec4VAddS(const vec4 *const restrict v, const float s);
void vec4AddVToV(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4AddVToVR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r);
void vec4AddNToV(vec4 *const restrict v, const float x, const float y, const float z, const float w);
void vec4AddSToV(vec4 *const restrict v, const float s);

vec4 vec4VSubV(const vec4 *const restrict v1, const vec4 *const restrict v2);
vec4 vec4VSubN(const vec4 *const restrict v, const float x, const float y, const float z, const float w);
vec4 vec4VSubS(const vec4 *const restrict v, const float s);
vec4 vec4NSubV(const float x, const float y, const float z, const float w, const vec4 *const restrict v);
vec4 vec4SSubV(const float s, const vec4 *const restrict v);
void vec4SubVFromV1(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4SubVFromV2(const vec4 *const restrict v1, vec4 *const restrict v2);
void vec4SubVFromVR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r);
void vec4SubNFromV(vec4 *const restrict v, const float x, const float y, const float z, const float w);
void vec4SubSFromV(vec4 *const restrict v, const float s);
void vec4SubVFromN(const float x, const float y, const float z, const float w, vec4 *const restrict v);
void vec4SubVFromS(const float s, vec4 *const restrict v);

vec4 vec4VMultV(const vec4 *const restrict v1, const vec4 *const restrict v2);
vec4 vec4VMultN(const vec4 *const restrict v, const float x, const float y, const float z, const float w);
vec4 vec4VMultS(const vec4 *const restrict v, const float s);
void vec4MultVByV(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4MultVByVR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r);
void vec4MultVByN(vec4 *const restrict v, const float x, const float y, const float z, const float w);
void vec4MultVByS(vec4 *const restrict v, const float s);

vec4 vec4VDivV(const vec4 *const restrict v1, const vec4 *const restrict v2);
vec4 vec4VDivN(const vec4 *const restrict v, const float x, const float y, const float z, const float w);
vec4 vec4VDivS(const vec4 *const restrict v, const float s);
vec4 vec4NDivV(const float x, const float y, const float z, const float w, const vec4 *const restrict v);
vec4 vec4SDivV(const float s, const vec4 *const restrict v);
void vec4DivVByV1(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4DivVByV2(const vec4 *const restrict v1, vec4 *const restrict v2);
void vec4DivVByVR(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r);
void vec4DivVByN(vec4 *const restrict v, const float x, const float y, const float z, const float w);
void vec4DivVByS(vec4 *const restrict v, const float s);
void vec4DivNByV(const float x, const float y, const float z, const float w, vec4 *const restrict v);
void vec4DivSByV(const float s, vec4 *const restrict v);

void vec4Min(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r);
void vec4Max(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict r);

void vec4Negate(vec4 *const restrict v);
void vec4NegateR(const vec4 *const restrict v, vec4 *const restrict r);

float vec4Magnitude(const vec4 *const restrict v);
float vec4MagnitudeSquared(const vec4 *const restrict v);
float vec4MagnitudeInverse(const vec4 *const restrict v);

vec4 vec4GetUnit(const vec4 *const restrict v);
vec4 vec4GetUnitFast(const vec4 *const restrict v);
vec4 vec4GetUnitFastAccurate(const vec4 *const restrict v);
void vec4Normalize(vec4 *const restrict v);
void vec4NormalizeFast(vec4 *const restrict v);
void vec4NormalizeFastAccurate(vec4 *const restrict v);

float vec4Dot(const vec4 *const restrict v1, const vec4 *const restrict v2);

void vec4Lerp1(vec4 *const restrict v1, const vec4 *const restrict v2, const float t);
void vec4Lerp2(const vec4 *const restrict v1, vec4 *const restrict v2, const float t);
void vec4LerpR(const vec4 *const restrict v1, const vec4 *const restrict v2, const float t, vec4 *const restrict r);

#endif
