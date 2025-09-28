#ifndef MAT3X4_H
#define MAT3X4_H

#include "mat3.h"
#include "vec4.h"
#include "quat.h"
#include "return.h"

// This structure implements a matrix with
// three rows and four columns. The reason
// why we want something like this is because
// in practice, the final row of 4x4 matrices
// will often be (0 0 0 1). Thus using 3x4
// matrices saves a lot of memory and CPU time.
//
// One should therefore keep in mind that
// many of these functions do not really apply
// to 3x4 matrices so much as they are reduced
// versions of the functions for 4x4 matrices.
// For instance, mat3x4MMultM computes the
// product of two 4x4 matrices with the final
// row assumed to be (0 0 0 1).

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
	float m[4][3];
} mat3x4;

extern mat3x4 g_mat3x4Identity;
extern mat3x4 g_mat3x4Zero;

void mat3x4IdentityP(mat3x4 *const __RESTRICT__ m);
void mat3x4ZeroP(mat3x4 *const __RESTRICT__ m);

mat3x4 mat3x4DiagonalV(const vec3 v);
void mat3x4DiagonalVP(mat3x4 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ v);
mat3x4 mat3x4DiagonalS(const float s);
void mat3x4DiagonalSP(mat3x4 *const __RESTRICT__ m, const float s);
mat3x4 mat3x4DiagonalN(const float x, const float y, const float z);
void mat3x4DiagonalNP(mat3x4 *const __RESTRICT__ m, const float x, const float y, const float z);

mat3x4 mat3x4MMultM(const mat3x4 m1, const mat3x4 m2);
void mat3x4MMultMP1(mat3x4 *const __RESTRICT__ m1, const mat3x4 *const __RESTRICT__ m2);
void mat3x4MMultMP2(const mat3x4 *const __RESTRICT__ m1, mat3x4 *const __RESTRICT__ m2);
void mat3x4MMultMPR(const mat3x4 *const __RESTRICT__ m1, const mat3x4 *const __RESTRICT__ m2, mat3x4 *const __RESTRICT__ r);

mat3x4 mat3x4MMultM3(const mat3x4 m1, const mat3 m2);
void mat3x4MMultM3P(mat3x4 *const __RESTRICT__ m1, const mat3 *const __RESTRICT__ m2);
void mat3x4MMultM3PR(const mat3x4 *const __RESTRICT__ m1, const mat3 *const __RESTRICT__ m2, mat3x4 *const __RESTRICT__ r);

///void mat3x4TransformVP(const mat3x4 *const __RESTRICT__ m, vec3 *const __RESTRICT__ v);

vec4 mat3x4VMultM(const vec4 v, const mat3x4 m);
void mat3x4VMultMP(vec4 *const __RESTRICT__ v, const mat3x4 *const __RESTRICT__ m);
void mat3x4VMultMPR(const vec4 *const __RESTRICT__ v, const mat3x4 *const __RESTRICT__ m, vec4 *const __RESTRICT__ r);
vec4 mat3x4MMultV(const mat3x4 m, const vec4 v);
void mat3x4MMultVP(const mat3x4 *const __RESTRICT__ m, vec4 *const __RESTRICT__ v);
void mat3x4MMultVPR(const mat3x4 *const __RESTRICT__ m, const vec4 *const __RESTRICT__ v, vec4 *const __RESTRICT__ r);

vec3 mat3x4V3MultM(const vec3 v, const mat3x4 m);
void mat3x4V3MultMP(vec3 *const __RESTRICT__ v, const mat3x4 *const __RESTRICT__ m);
void mat3x4V3MultMPR(const vec3 *const __RESTRICT__ v, const mat3x4 *const __RESTRICT__ m, vec3 *const __RESTRICT__ r);
vec3 mat3x4MMultV3(const mat3x4 m, const vec3 v);
void mat3x4MMultV3P(const mat3x4 *const __RESTRICT__ m, vec3 *const __RESTRICT__ v);
void mat3x4MMultV3PR(const mat3x4 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r);

vec4 mat3x4NMultM(const float x, const float y, const float z, const float w, const mat3x4 m);
void mat3x4NMultMPR(const float x, const float y, const float z, const float w, const mat3x4 *const __RESTRICT__ m, vec4 *const __RESTRICT__ r);
vec4 mat3x4MMultN(const mat3x4 m, const float x, const float y, const float z, const float w);
void mat3x4MMultNPR(const mat3x4 *const __RESTRICT__ m, const float x, const float y, const float z, const float w, vec4 *const __RESTRICT__ r);

vec3 mat3x4N3MultM(const float x, const float y, const float z, const mat3x4 m);
void mat3x4N3MultMPR(const float x, const float y, const float z, const mat3x4 *const __RESTRICT__ m, vec3 *const __RESTRICT__ r);
vec3 mat3x4MMultN3(const mat3x4 m, const float x, const float y, const float z);
void mat3x4MMultN3PR(const mat3x4 *const __RESTRICT__ m, const float x, const float y, const float z, vec3 *const __RESTRICT__ r);

mat3x4 mat3x4MAddM(const mat3x4 m1, const mat3x4 m2);
void mat3x4MAddMP(mat3x4 *const __RESTRICT__ m1, const mat3x4 *const __RESTRICT__ m2);
void mat3x4MAddMPR(const mat3x4 *const __RESTRICT__ m1, const mat3x4 *const __RESTRICT__ m2, mat3x4 *const __RESTRICT__ r);

float mat3x4Determinant(const mat3x4 m);
float mat3x4DeterminantP(const mat3x4 *const __RESTRICT__ m);

mat3x4 mat3x4Transpose(const mat3x4 m);
void mat3x4TransposeP(mat3x4 *const __RESTRICT__ m);
void mat3x4TransposePR(const mat3x4 *const __RESTRICT__ m, mat3x4 *const __RESTRICT__ r);

mat3x4 mat3x4Invert(const mat3x4 m);
return_t mat3x4InvertR(const mat3x4 m, mat3x4 *const __RESTRICT__ r);
return_t mat3x4InvertP(mat3x4 *const __RESTRICT__ m);
return_t mat3x4InvertPR(const mat3x4 *const __RESTRICT__ m, mat3x4 *const __RESTRICT__ r);

mat3x4 mat3x4RotateToFace(const vec3 eye, const vec3 target, const vec3 up);
void mat3x4RotateToFaceP(mat3x4 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ eye, const vec3 *const __RESTRICT__ target, const vec3 *const __RESTRICT__ up);
mat3x4 mat3x4LookAt(const vec3 eye, const vec3 target, const vec3 up);
void mat3x4LookAtP(mat3x4 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ eye, const vec3 *const __RESTRICT__ target, const vec3 *const __RESTRICT__ up);

mat3x4 mat3x4TranslationMatrix(const float x, const float y, const float z);
void mat3x4TranslationMatrixP(mat3x4 *const __RESTRICT__ r, const float x, const float y, const float z);
mat3x4 mat3x4Translate(const float x, const float y, const float z, const mat3x4 m);
void mat3x4TranslateP(const float x, const float y, const float z, mat3x4 *const __RESTRICT__ m);
void mat3x4TranslatePR(const float x, const float y, const float z, const mat3x4 *const __RESTRICT__ m, mat3x4 *const __RESTRICT__ r);
mat3x4 mat3x4TranslatePre(const mat3x4 m, const float x, const float y, const float z);

mat3x4 mat3x4RotationMatrix(const quat q);
void mat3x4RotationMatrixPR(const quat *const __RESTRICT__ q, mat3x4 *const __RESTRICT__ r);
mat3x4 mat3x4Rotate(const quat q, const mat3x4 m);
void mat3x4RotateP(const quat *const __RESTRICT__ q, mat3x4 *const __RESTRICT__ m);
void mat3x4RotatePR(const quat *const __RESTRICT__ q, const mat3x4 *const __RESTRICT__ m, mat3x4 *const __RESTRICT__ r);

mat3x4 mat3x4ScaleMatrix(const float x, const float y, const float z);
void mat3x4ScaleMatrixP(mat3x4 *const __RESTRICT__ m, const float x, const float y, const float z);
mat3x4 mat3x4Scale(const float x, const float y, const float z, const mat3x4 m);
void mat3x4ScaleP(const float x, const float y, const float z, mat3x4 *const __RESTRICT__ m);
void mat3x4ScalePR(const float x, const float y, const float z, const mat3x4 *const __RESTRICT__ m, mat3x4 *const __RESTRICT__ r);
mat3x4 mat3x4ScalePre(const mat3x4 m, const float x, const float y, const float z);

mat3x4 mat3x4ShearMatrix(const quat q, const vec3 s);
void mat3x4ShearMatrixPR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ s, mat3x4 *const __RESTRICT__ r);

mat3x4 mat3x4Quaternion(const quat q);
void mat3x4QuaternionPR(const quat *const __RESTRICT__ q, mat3x4 *const __RESTRICT__ r);

#endif
