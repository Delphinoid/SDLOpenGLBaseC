#ifndef MAT2_H
#define MAT2_H

#include "vec2.h"
#include "return.h"

/** Use an alias? **/
// All matrices are stored in column-major
// order, despite this being non-standard
// mathematically and C preferring row-major.
// This is both to keep it consistent with
// OpenGL and to make accessing column
// vectors faster.
typedef struct {
	float m[2][2];
} mat2;

mat2 mat2Identity();
mat2 mat2Zero();
void mat2IdentityP(mat2 *const restrict m);
void mat2ZeroP(mat2 *const restrict m);

mat2 mat2DiagonalV(const vec2 v);
void mat2DiagonalVP(mat2 *const restrict m, const vec2 *const restrict v);
mat2 mat2DiagonalS(const float s);
void mat2DiagonalSP(mat2 *const restrict m, const float s);
mat2 mat2DiagonalN(const float x, const float y);
void mat2DiagonalNP(mat2 *const restrict m, const float x, const float y);

mat2 mat2SkewSymmetric(const float s);
void mat2SkewSymmetricP(mat2 *const restrict m, const float s);

mat2 mat2MMultM(const mat2 m1, const mat2 m2);
void mat2MMultMP1(mat2 *const restrict m1, const mat2 *const restrict m2);
void mat2MMultMP2(const mat2 *const restrict m1, mat2 *const restrict m2);
void mat2MMultMPR(const mat2 *const restrict m1, const mat2 *const restrict m2, mat2 *const restrict r);
vec2 mat2VMultMBra(const vec2 v, const mat2 m);
void mat2VMultMBraP(vec2 *const restrict v, const mat2 *const restrict m);
void mat2VMultMBraPR(const vec2 *const restrict v, const mat2 *const restrict m, vec2 *const restrict r);
vec2 mat2MMultVKet(const mat2 m, const vec2 v);
void mat2MMultVKetP(const mat2 *const restrict m, vec2 *const restrict v);
void mat2MMultVKetPR(const mat2 *const restrict m, const vec2 *const restrict v, vec2 *const restrict r);

mat2 mat2MAddM(const mat2 m1, const mat2 m2);
void mat2MAddMP(mat2 *const restrict m1, const mat2 *const restrict m2);
void mat2MAddMPR(const mat2 *const restrict m1, const mat2 *const restrict m2, mat2 *const restrict r);

float mat2Determinant(const mat2 m);
float mat2DeterminantP(const mat2 *const restrict m);

mat2 mat2Transpose(const mat2 m);
void mat2TransposeP(mat2 *const restrict m);
void mat2TransposePR(const mat2 *const restrict m, mat2 *const restrict r);

mat2 mat2Invert(const mat2 m);
return_t mat2InvertR(const mat2 m, mat2 *const restrict r);
return_t mat2InvertP(mat2 *const restrict m);
return_t mat2InvertPR(const mat2 *const restrict m, mat2 *const restrict r);

vec2 mat2Solve(const mat2 A, const vec2 b);
return_t mat2SolveR(const mat2 A, const vec2 b, vec2 *const restrict r);
return_t mat2SolvePR(const mat2 *const restrict A, const vec2 *const restrict b, vec2 *const restrict r);

#endif
