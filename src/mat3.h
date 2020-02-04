#ifndef MAT3_H
#define MAT3_H

#include "quat.h"
#include "return.h"

/** Use an alias? **/
// All matrices are stored in column-major
// order, despite this being non-standard
// mathematically. This is both to keep
// it consistent with OpenGL and to make
// accessing column vectors faster.
typedef struct {
	float m[3][3];
} mat3;

mat3 mat3Identity();
mat3 mat3Zero();
void mat3IdentityP(mat3 *const __RESTRICT__ m);
void mat3ZeroP(mat3 *const __RESTRICT__ m);

mat3 mat3DiagonalV(const vec3 v);
void mat3DiagonalVP(mat3 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ v);
mat3 mat3DiagonalS(const float s);
void mat3DiagonalSP(mat3 *const __RESTRICT__ m, const float s);
mat3 mat3DiagonalN(const float x, const float y, const float z);
void mat3DiagonalNP(mat3 *const __RESTRICT__ m, const float x, const float y, const float z);

mat3 mat3SkewSymmetric(const vec3 v);
void mat3SkewSymmetricP(mat3 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ v);

mat3 mat3MMultM(const mat3 m1, const mat3 m2);
void mat3MMultMP1(mat3 *const __RESTRICT__ m1, const mat3 *const __RESTRICT__ m2);
void mat3MMultMP2(const mat3 *const __RESTRICT__ m1, mat3 *const __RESTRICT__ m2);
void mat3MMultMPR(const mat3 *const __RESTRICT__ m1, const mat3 *const __RESTRICT__ m2, mat3 *const __RESTRICT__ r);
vec3 mat3VMultMBra(const vec3 v, const mat3 m);
void mat3VMultMBraP(vec3 *const __RESTRICT__ v, const mat3 *const __RESTRICT__ m);
void mat3VMultMBraPR(const vec3 *const __RESTRICT__ v, const mat3 *const __RESTRICT__ m, vec3 *const __RESTRICT__ r);
vec3 mat3MMultVKet(const mat3 m, const vec3 v);
void mat3MMultVKetP(const mat3 *const __RESTRICT__ m, vec3 *const __RESTRICT__ v);
void mat3MMultVKetPR(const mat3 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r);

mat3 mat3MAddM(const mat3 m1, const mat3 m2);
void mat3MAddMP(mat3 *const __RESTRICT__ m1, const mat3 *const __RESTRICT__ m2);
void mat3MAddMPR(const mat3 *const __RESTRICT__ m1, const mat3 *const __RESTRICT__ m2, mat3 *const __RESTRICT__ r);

float mat3Determinant(const mat3 m);
float mat3DeterminantP(const mat3 *const __RESTRICT__ m);

mat3 mat3Transpose(const mat3 m);
void mat3TransposeP(mat3 *const __RESTRICT__ m);
void mat3TransposePR(const mat3 *const __RESTRICT__ m, mat3 *const __RESTRICT__ r);

mat3 mat3Invert(const mat3 m);
return_t mat3InvertR(const mat3 m, mat3 *const __RESTRICT__ r);
return_t mat3InvertP(mat3 *const __RESTRICT__ m);
return_t mat3InvertPR(const mat3 *const __RESTRICT__ m, mat3 *const __RESTRICT__ r);

vec3 mat3Solve(const mat3 A, const vec3 b);
return_t mat3SolveR(const mat3 A, const vec3 b, vec3 *const __RESTRICT__ r);
return_t mat3SolvePR(const mat3 *const __RESTRICT__ A, const vec3 *const __RESTRICT__ b, vec3 *const __RESTRICT__ r);

mat3 mat3Quaternion(const quat q);
void mat3QuaternionP(mat3 *const __RESTRICT__ m, const quat *const __RESTRICT__ q);

#endif
