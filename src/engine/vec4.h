#ifndef VEC4_H
#define VEC4_H

#include "qualifiers.h"

typedef struct {
	float x;
	float y;
	float z;
	float w;
} vec4;

extern vec4 g_vec4Zero;

vec4 vec4New(const float x, const float y, const float z, const float w);
vec4 vec4NewS(const float s);
void vec4Set(vec4 *const __RESTRICT__ v, const float x, const float y, const float z, const float w);
void vec4SetS(vec4 *const __RESTRICT__ v, const float s);
void vec4ZeroP(vec4 *const __RESTRICT__ v);

vec4 vec4VAddV(const vec4 v1, const vec4 v2);
vec4 vec4VAddN(const vec4 v, const float x, const float y, const float z, const float w);
vec4 vec4VAddS(const vec4 v, const float s);
void vec4VAddVP(vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2);
void vec4VAddVPR(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, vec4 *const __RESTRICT__ r);
void vec4VAddNP(vec4 *const __RESTRICT__ v, const float x, const float y, const float z, const float w);
void vec4VAddSP(vec4 *const __RESTRICT__ v, const float s);

vec4 vec4VSubV(const vec4 v1, const vec4 v2);
vec4 vec4VSubN(const vec4 v, const float x, const float y, const float z, const float w);
vec4 vec4VSubS(const vec4 v, const float s);
vec4 vec4NSubV(const float x, const float y, const float z, const float w, const vec4 v);
vec4 vec4SSubV(const float s, const vec4 v);
void vec4VSubVP1(vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2);
void vec4VSubVP2(const vec4 *const __RESTRICT__ v1, vec4 *const __RESTRICT__ v2);
void vec4VSubVPR(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, vec4 *const __RESTRICT__ r);
void vec4VSubNP(vec4 *const __RESTRICT__ v, const float x, const float y, const float z, const float w);
void vec4VSubSP(vec4 *const __RESTRICT__ v, const float s);
void vec4NSubVP(const float x, const float y, const float z, const float w, vec4 *const __RESTRICT__ v);
void vec4SSubVP(const float s, vec4 *const __RESTRICT__ v);

vec4 vec4VMultV(const vec4 v1, const vec4 v2);
vec4 vec4VMultN(const vec4 v, const float x, const float y, const float z, const float w);
vec4 vec4VMultS(const vec4 v, const float s);
void vec4VMultVP(vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2);
void vec4VMultVPR(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, vec4 *const __RESTRICT__ r);
void vec4VMultNP(vec4 *const __RESTRICT__ v, const float x, const float y, const float z, const float w);
void vec4VMultSP(vec4 *const __RESTRICT__ v, const float s);

vec4 vec4VDivV(const vec4 v1, const vec4 v2);
vec4 vec4VDivN(const vec4 v, const float x, const float y, const float z, const float w);
vec4 vec4VDivS(const vec4 v, const float s);
vec4 vec4NDivV(const float x, const float y, const float z, const float w, const vec4 v);
vec4 vec4SDivV(const float s, const vec4 v);
void vec4VDivVP1(vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2);
void vec4VDivVP2(const vec4 *const __RESTRICT__ v1, vec4 *const __RESTRICT__ v2);
void vec4VDivVPR(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, vec4 *const __RESTRICT__ r);
void vec4VDivNP(vec4 *const __RESTRICT__ v, const float x, const float y, const float z, const float w);
void vec4VDivSP(vec4 *const __RESTRICT__ v, const float s);
void vec4NDivVP(const float x, const float y, const float z, const float w, vec4 *const __RESTRICT__ v);
void vec4SDivVP(const float s, vec4 *const __RESTRICT__ v);

vec4 vec4fmaf(const float x, const vec4 u, const vec4 v);
void vec4fmafP(const float x, const vec4 *const __RESTRICT__ u, vec4 *const __RESTRICT__ v);
void vec4fmafPR(const float x, const vec4 *const __RESTRICT__ u, const vec4 *const __RESTRICT__ v, vec4 *const __RESTRICT__ r);

vec4 vec4Min(const vec4 v1, const vec4 v2);
void vec4MinP(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, vec4 *const __RESTRICT__ r);
vec4 vec4Max(const vec4 v1, const vec4 v2);
void vec4MaxP(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, vec4 *const __RESTRICT__ r);

vec4 vec4Negate(const vec4 v);
void vec4NegateP(vec4 *const __RESTRICT__ v);
void vec4NegatePR(const vec4 *const __RESTRICT__ v, vec4 *const __RESTRICT__ r);

float vec4Magnitude(const vec4 v);
float vec4MagnitudeP(const vec4 *const __RESTRICT__ v);
float vec4MagnitudeSquared(const vec4 v);
float vec4MagnitudeSquaredP(const vec4 *const __RESTRICT__ v);
float vec4MagnitudeInverse(const vec4 v);
float vec4MagnitudeInverseP(const vec4 *const __RESTRICT__ v);
float vec4MagnitudeInverseFast(const vec4 v);
float vec4MagnitudeInverseFastP(const vec4 *const __RESTRICT__ v);
float vec4MagnitudeInverseFastAccurate(const vec4 v);
float vec4MagnitudeInverseFastAccurateP(const vec4 *const __RESTRICT__ v);

vec4 vec4Normalize(const vec4 v);
vec4 vec4NormalizeFast(const vec4 v);
vec4 vec4NormalizeFastAccurate(const vec4 v);
float vec4NormalizeP(vec4 *const __RESTRICT__ v);
void vec4NormalizeFastP(vec4 *const __RESTRICT__ v);
void vec4NormalizeFastAccurateP(vec4 *const __RESTRICT__ v);

float vec4Dot(const vec4 v1, const vec4 v2);
float vec4DotP(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2);

vec4 vec4Lerp(const vec4 v1, const vec4 v2, const float t);
void vec4LerpP1(vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, const float t);
void vec4LerpP2(const vec4 *const __RESTRICT__ v1, vec4 *const __RESTRICT__ v2, const float t);
void vec4LerpPR(const vec4 *const __RESTRICT__ v1, const vec4 *const __RESTRICT__ v2, const float t, vec4 *const __RESTRICT__ r);

#endif
