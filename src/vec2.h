#ifndef VEC2_H
#define VEC2_H

typedef struct {
	float x, y;
} vec2;

vec2 vec2New(const float x, const float y);
vec2 vec2NewS(const float s);
void vec2Set(vec2 *const restrict v, const float x, const float y);
void vec2SetS(vec2 *const restrict v, const float s);
void vec2Zero(vec2 *const restrict v);

vec2 vec2VAddV(const vec2 v1, const vec2 v2);
vec2 vec2VAddN(const vec2 v, const float x, const float y);
vec2 vec2VAddS(const vec2 v, const float s);
void vec2VAddVP(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2VAddVPR(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r);
void vec2VAddNP(vec2 *const restrict v, const float x, const float y);
void vec2VAddSP(vec2 *const restrict v, const float s);

vec2 vec2VSubV(const vec2 v1, const vec2 v2);
vec2 vec2VSubN(const vec2 v, const float x, const float y);
vec2 vec2VSubS(const vec2 v, const float s);
vec2 vec2NSubV(const float x, const float y, const vec2 v);
vec2 vec2SSubV(const float s, const vec2 v);
void vec2VSubVP1(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2VSubVP2(const vec2 *const restrict v1, vec2 *const restrict v2);
void vec2VSubVPR(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r);
void vec2VSubNP(vec2 *const restrict v, const float x, const float y);
void vec2VSubSP(vec2 *const restrict v, const float s);
void vec2NSubVP(const float x, const float y, vec2 *const restrict v);
void vec2SSubVP(const float s, vec2 *const restrict v);

vec2 vec2VMultV(const vec2 v1, const vec2 v2);
vec2 vec2VMultN(const vec2 v, const float x, const float y);
vec2 vec2VMultS(const vec2 v, const float s);
void vec2VMultVP(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2VMultVPR(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r);
void vec2VMultNP(vec2 *const restrict v, const float x, const float y);
void vec2VMultSP(vec2 *const restrict v, const float s);

vec2 vec2VDivV(const vec2 v1, const vec2 v2);
vec2 vec2VDivN(const vec2 v, const float x, const float y);
vec2 vec2VDivS(const vec2 v, const float s);
vec2 vec2NDivV(const float x, const float y, const vec2 v);
vec2 vec2SDivV(const float s, const vec2 v);
void vec2VDivVP1(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2VDivVP2(const vec2 *const restrict v1, vec2 *const restrict v2);
void vec2VDivVPR(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r);
void vec2VDivNP(vec2 *const restrict v, const float x, const float y);
void vec2VDivSP(vec2 *const restrict v, const float s);
void vec2NDivVP(const float x, const float y, vec2 *const restrict v);
void vec2SDivVP(const float s, vec2 *const restrict v);

vec2 vec2Min(const vec2 v1, const vec2 v2);
void vec2MinP(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r);
vec2 vec2Max(const vec2 v1, const vec2 v2);
void vec2MaxP(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r);

vec2 vec2Negate(const vec2 v);
void vec2NegateP(vec2 *const restrict v);
void vec2NegatePR(const vec2 *const restrict v, vec2 *const restrict r);

float vec2Magnitude(const vec2 v);
float vec2MagnitudeP(const vec2 *const restrict v);
float vec2MagnitudeSquared(const vec2 v);
float vec2MagnitudeSquaredP(const vec2 *const restrict v);
float vec2MagnitudeInverse(const vec2 v);
float vec2MagnitudeInverseP(const vec2 *const restrict v);

vec2 vec2Normalize(const vec2 v);
vec2 vec2NormalizeFast(const vec2 v);
vec2 vec2NormalizeFastAccurate(const vec2 v);
void vec2NormalizeP(vec2 *const restrict v);
void vec2NormalizeFastP(vec2 *const restrict v);
void vec2NormalizeFastAccurateP(vec2 *const restrict v);

float vec2Dot(const vec2 v1, const vec2 v2);
float vec2DotP(const vec2 *const restrict v1, const vec2 *const restrict v2);

void vec2LerpP1(vec2 *const restrict v1, const vec2 *const restrict v2, const float t);
void vec2LerpP2(const vec2 *const restrict v1, vec2 *const restrict v2, const float t);
void vec2LerpPR(const vec2 *const restrict v1, const vec2 *const restrict v2, const float t, vec2 *const restrict r);

#endif
