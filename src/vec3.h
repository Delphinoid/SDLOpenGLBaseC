#ifndef VEC3_H
#define VEC3_H

typedef struct {
	float x, y, z;
} vec3;

vec3 vec3New(const float x, const float y, const float z);
vec3 vec3NewS(const float s);
vec3 vec3Zero();
void vec3Set(vec3 *const restrict v, const float x, const float y, const float z);
void vec3SetS(vec3 *const restrict v, const float s);
void vec3ZeroP(vec3 *const restrict v);

vec3 vec3VAddV(const vec3 v1, const vec3 v2);
vec3 vec3VAddN(const vec3 v, const float x, const float y, const float z);
vec3 vec3VAddS(const vec3 v, const float s);
void vec3VAddVP(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3VAddVPR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);
void vec3VAddNP(vec3 *const restrict v, const float x, const float y, const float z);
void vec3VAddNPR(vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict r);
void vec3VAddSP(vec3 *const restrict v, const float s);
void vec3VAddSPR(vec3 *const restrict v, const float s, vec3 *const restrict r);

vec3 vec3VSubV(const vec3 v1, const vec3 v2);
vec3 vec3VSubN(const vec3 v, const float x, const float y, const float z);
vec3 vec3VSubS(const vec3 v, const float s);
vec3 vec3NSubV(const float x, const float y, const float z, const vec3 v);
vec3 vec3SSubV(const float s, const vec3 v);
void vec3VSubVP1(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3VSubVP2(const vec3 *const restrict v1, vec3 *const restrict v2);
void vec3VSubVPR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);
void vec3VSubNP(vec3 *const restrict v, const float x, const float y, const float z);
void vec3VSubSP(vec3 *const restrict v, const float s);
void vec3NSubVP(const float x, const float y, const float z, vec3 *const restrict v);
void vec3SSubVP(const float s, vec3 *const restrict v);

vec3 vec3VMultV(const vec3 v1, const vec3 v2);
vec3 vec3VMultN(const vec3 v, const float x, const float y, const float z);
vec3 vec3VMultS(const vec3 v, const float s);
void vec3VMultVP(vec3 *v1, const vec3 *v2);
void vec3VMultVPR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);
void vec3VMultNP(vec3 *const restrict v, const float x, const float y, const float z);
void vec3VMultSP(vec3 *const restrict v, const float s);
void vec3VMultSPR(const vec3 *const restrict v, const float s, vec3 *const restrict r);

vec3 vec3VDivV(const vec3 v1, const vec3 v2);
vec3 vec3VDivN(const vec3 v, const float x, const float y, const float z);
vec3 vec3VDivS(const vec3 v, const float s);
vec3 vec3NDivV(const float x, const float y, const float z, const vec3 v);
vec3 vec3SDivV(const float s, const vec3 v);
void vec3VDivVP1(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3VDivVP2(const vec3 *const restrict v1, vec3 *const restrict v2);
void vec3VDivVPR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);
void vec3VDivNP(vec3 *const restrict v, const float x, const float y, const float z);
void vec3VDivSP(vec3 *const restrict v, const float s);
void vec3NDivVP(const float x, const float y, const float z, vec3 *const restrict v);
void vec3SDivVP(const float s, vec3 *const restrict v);

vec3 vec3Min(const vec3 v1, const vec3 v2);
void vec3MinP(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);
vec3 vec3Max(const vec3 v1, const vec3 v2);
void vec3MaxP(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);

vec3 vec3Negate(const vec3 v);
void vec3NegateP(vec3 *const restrict v);
void vec3NegatePR(const vec3 *const restrict v, vec3 *const restrict r);

float vec3Magnitude(const vec3 v);
float vec3MagnitudeP(const vec3 *const restrict v);
float vec3MagnitudeSquared(const vec3 v);
float vec3MagnitudeSquaredP(const vec3 *const restrict v);
float vec3MagnitudeInverse(const vec3 v);
float vec3MagnitudeInverseP(const vec3 *const restrict v);
float vec3MagnitudeInverseFast(const vec3 v);
float vec3MagnitudeInverseFastP(const vec3 *const restrict v);
float vec3MagnitudeInverseFastAccurate(const vec3 v);
float vec3MagnitudeInverseFastAccurateP(const vec3 *const restrict v);

vec3 vec3Normalize(const vec3 v);
vec3 vec3NormalizeFast(const vec3 v);
vec3 vec3NormalizeFastAccurate(const vec3 v);
void vec3NormalizeP(vec3 *const restrict v);
void vec3NormalizePR(const vec3 *const restrict v, vec3 *const restrict r);
void vec3NormalizeFastP(vec3 *const restrict v);
void vec3NormalizeFastPR(const vec3 *const restrict v, vec3 *const restrict r);
void vec3NormalizeFastAccurateP(vec3 *const restrict v);
void vec3NormalizeFastAccuratePR(const vec3 *const restrict v, vec3 *const restrict r);

float vec3Dot(const vec3 v1, const vec3 v2);
float vec3DotP(const vec3 *const restrict v1, const vec3 *const restrict v2);
vec3 vec3Cross(const vec3 v1, const vec3 v2);
void vec3CrossP1(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3CrossP2(const vec3 *const restrict v1, vec3 *const restrict v2);
void vec3CrossPR(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict r);

vec3 vec3Perpendicular(const vec3 v);
void vec3PerpendicularP(vec3 *const restrict v);
void vec3PerpendicularPR(const vec3 *const restrict v, vec3 *const restrict r);

void vec3OrthonormalBasis(const vec3 v1, vec3 *const restrict v2, vec3 *const restrict v3);
void vec3OrthonormalBasisP(const vec3 *const restrict v1, vec3 *const restrict v2, vec3 *const restrict v3);

vec3 vec3Lerp(const vec3 v1, const vec3 v2, const float t);
void vec3LerpP1(vec3 *const restrict v1, const vec3 *const restrict v2, const float t);
void vec3LerpP2(const vec3 *const restrict v1, vec3 *const restrict v2, const float t);
void vec3LerpPR(const vec3 *const restrict v1, const vec3 *const restrict v2, const float t, vec3 *const restrict r);

vec3 vec3LinearCombination(const vec3 v1, const vec3 v2, const vec3 v3, const float a, const float b, const float c);
void vec3LinearCombinationP(const vec3 *const restrict v1, const vec3 *const restrict v2, const vec3 *const restrict v3,
                            const float a, const float b, const float c, vec3 *const restrict r);

#endif
