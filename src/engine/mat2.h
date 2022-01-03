#ifndef MAT2_H
#define MAT2_H

#include "vec2.h"
#include "return.h"

// WARNING: Both quaternions and matrices often violate
// strict-aliasing rules. That is, it is common for the
// engine to cast adjacent floats in quaternion and matrix
// structures to vectors, for instance. While this is not
// an issue on most sensible compilers and platforms, it
// is still technically undefined behaviour.

/** Use an alias? **/
// All matrices are stored in column-major
// order, despite this being non-standard
// mathematically. This is both to keep
// it consistent with OpenGL and to make
// accessing column vectors faster.
typedef struct {
	float m[2][2];
} mat2;

extern mat2 g_mat2Identity;
extern mat2 g_mat2Zero;

void mat2IdentityP(mat2 *const __RESTRICT__ m);
void mat2ZeroP(mat2 *const __RESTRICT__ m);

mat2 mat2DiagonalV(const vec2 v);
void mat2DiagonalVP(mat2 *const __RESTRICT__ m, const vec2 *const __RESTRICT__ v);
mat2 mat2DiagonalS(const float s);
void mat2DiagonalSP(mat2 *const __RESTRICT__ m, const float s);
mat2 mat2DiagonalN(const float x, const float y);
void mat2DiagonalNP(mat2 *const __RESTRICT__ m, const float x, const float y);

mat2 mat2SkewSymmetric(const float s);
void mat2SkewSymmetricP(mat2 *const __RESTRICT__ m, const float s);

mat2 mat2MMultM(const mat2 m1, const mat2 m2);
void mat2MMultMP1(mat2 *const __RESTRICT__ m1, const mat2 *const __RESTRICT__ m2);
void mat2MMultMP2(const mat2 *const __RESTRICT__ m1, mat2 *const __RESTRICT__ m2);
void mat2MMultMPR(const mat2 *const __RESTRICT__ m1, const mat2 *const __RESTRICT__ m2, mat2 *const __RESTRICT__ r);
vec2 mat2VMultM(const vec2 v, const mat2 m);
void mat2VMultMP(vec2 *const __RESTRICT__ v, const mat2 *const __RESTRICT__ m);
void mat2VMultMPR(const vec2 *const __RESTRICT__ v, const mat2 *const __RESTRICT__ m, vec2 *const __RESTRICT__ r);
vec2 mat2MMultV(const mat2 m, const vec2 v);
void mat2MMultVP(const mat2 *const __RESTRICT__ m, vec2 *const __RESTRICT__ v);
void mat2MMultVPR(const mat2 *const __RESTRICT__ m, const vec2 *const __RESTRICT__ v, vec2 *const __RESTRICT__ r);

mat2 mat2MAddM(const mat2 m1, const mat2 m2);
void mat2MAddMP(mat2 *const __RESTRICT__ m1, const mat2 *const __RESTRICT__ m2);
void mat2MAddMPR(const mat2 *const __RESTRICT__ m1, const mat2 *const __RESTRICT__ m2, mat2 *const __RESTRICT__ r);

float mat2Determinant(const mat2 m);
float mat2DeterminantP(const mat2 *const __RESTRICT__ m);

mat2 mat2Transpose(const mat2 m);
void mat2TransposeP(mat2 *const __RESTRICT__ m);
void mat2TransposePR(const mat2 *const __RESTRICT__ m, mat2 *const __RESTRICT__ r);

mat2 mat2Invert(const mat2 m);
return_t mat2InvertR(const mat2 m, mat2 *const __RESTRICT__ r);
return_t mat2InvertP(mat2 *const __RESTRICT__ m);
return_t mat2InvertPR(const mat2 *const __RESTRICT__ m, mat2 *const __RESTRICT__ r);

vec2 mat2Solve(const mat2 A, const vec2 b);
return_t mat2SolveR(const mat2 A, const vec2 b, vec2 *const __RESTRICT__ r);
return_t mat2SolvePR(const mat2 *const __RESTRICT__ A, const vec2 *const __RESTRICT__ b, vec2 *const __RESTRICT__ r);

#endif
