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

vec2 vec2VAddV(const vec2 *const restrict v1, const vec2 *const restrict v2);
vec2 vec2VAddN(const vec2 *const restrict v, const float x, const float y);
vec2 vec2VAddS(const vec2 *const restrict v, const float s);
void vec2AddVToV(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2AddVToVR(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r);
void vec2AddNToV(vec2 *const restrict v, const float x, const float y);
void vec2AddSToV(vec2 *const restrict v, const float s);

vec2 vec2VSubV(const vec2 *const restrict v1, const vec2 *const restrict v2);
vec2 vec2VSubN(const vec2 *const restrict v, const float x, const float y);
vec2 vec2VSubS(const vec2 *const restrict v, const float s);
vec2 vec2NSubV(const float x, const float y, const vec2 *const restrict v);
vec2 vec2SSubV(const float s, const vec2 *const restrict v);
void vec2SubVFromV1(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2SubVFromV2(const vec2 *const restrict v1, vec2 *const restrict v2);
void vec2SubVFromVR(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r);
void vec2SubNFromV(vec2 *const restrict v, const float x, const float y);
void vec2SubSFromV(vec2 *const restrict v, const float s);
void vec2SubVFromN(const float x, const float y, vec2 *const restrict v);
void vec2SubVFromS(const float s, vec2 *const restrict v);

vec2 vec2VMultV(const vec2 *const restrict v1, const vec2 *const restrict v2);
vec2 vec2VMultN(const vec2 *const restrict v, const float x, const float y);
vec2 vec2VMultS(const vec2 *const restrict v, const float s);
void vec2MultVByV(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2MultVByVR(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r);
void vec2MultVByN(vec2 *const restrict v, const float x, const float y);
void vec2MultVByS(vec2 *const restrict v, const float s);

vec2 vec2VDivV(const vec2 *const restrict v1, const vec2 *const restrict v2);
vec2 vec2VDivN(const vec2 *const restrict v, const float x, const float y);
vec2 vec2VDivS(const vec2 *const restrict v, const float s);
vec2 vec2NDivV(const float x, const float y, const vec2 *const restrict v);
vec2 vec2SDivV(const float s, const vec2 *const restrict v);
void vec2DivVByV1(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2DivVByV2(const vec2 *const restrict v1, vec2 *const restrict v2);
void vec2DivVByVR(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r);
void vec2DivVByN(vec2 *const restrict v, const float x, const float y);
void vec2DivVByS(vec2 *const restrict v, const float s);
void vec2DivNByV(const float x, const float y, vec2 *const restrict v);
void vec2DivSByV(const float s, vec2 *const restrict v);

void vec2Min(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r);
void vec2Max(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict r);

void vec2Negate(vec2 *const restrict v);
void vec2NegateR(const vec2 *const restrict v, vec2 *const restrict r);

float vec2Magnitude(const vec2 *const restrict v);
float vec2MagnitudeSquared(const vec2 *const restrict v);
float vec2InverseMagnitude(const vec2 *const restrict v);

vec2 vec2GetUnit(const vec2 *const restrict v);
vec2 vec2GetUnitFast(const vec2 *const restrict v);
vec2 vec2GetUnitFastAccurate(const vec2 *const restrict v);
void vec2Normalize(vec2 *const restrict v);
void vec2NormalizeFast(vec2 *const restrict v);
void vec2NormalizeFastAccurate(vec2 *const restrict v);

float vec2Dot(const vec2 *const restrict v1, const vec2 *const restrict v2);

void vec2Lerp1(vec2 *const restrict v1, const vec2 *const restrict v2, const float t);
void vec2Lerp2(const vec2 *const restrict v1, vec2 *const restrict v2, const float t);
void vec2LerpR(const vec2 *const restrict v1, const vec2 *const restrict v2, const float t, vec2 *const restrict r);

#endif
