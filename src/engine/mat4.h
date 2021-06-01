#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"
#include "vec4.h"
#include "quat.h"
#include "return.h"

/** Use an alias? **/
// All matrices are stored in column-major
// order, despite this being non-standard
// mathematically. This is both to keep
// it consistent with OpenGL and to make
// accessing column vectors faster.
typedef struct {
	float m[4][4];
} mat4;

mat4 mat4Identity();
mat4 mat4Zero();
void mat4IdentityP(mat4 *const __RESTRICT__ m);
void mat4ZeroP(mat4 *const __RESTRICT__ m);

mat4 mat4DiagonalV(const vec4 v);
void mat4DiagonalVP(mat4 *const __RESTRICT__ m, const vec4 *const __RESTRICT__ v);
mat4 mat4DiagonalS(const float s);
void mat4DiagonalSP(mat4 *const __RESTRICT__ m, const float s);
mat4 mat4DiagonalN(const float x, const float y, const float z, const float w);
void mat4DiagonalNP(mat4 *const __RESTRICT__ m, const float x, const float y, const float z, const float w);

mat4 mat4MMultM(const mat4 m1, const mat4 m2);
void mat4MMultMP1(mat4 *const __RESTRICT__ m1, const mat4 *const __RESTRICT__ m2);
void mat4MMultMP2(const mat4 *const __RESTRICT__ m1, mat4 *const __RESTRICT__ m2);
void mat4MMultMPR(const mat4 *const __RESTRICT__ m1, const mat4 *const __RESTRICT__ m2, mat4 *const __RESTRICT__ r);

///void mat4TransformVP(const mat4 *const __RESTRICT__ m, vec3 *const __RESTRICT__ v);

vec4 mat4VMultM(const vec4 v, const mat4 m);
void mat4VMultMP(vec4 *const __RESTRICT__ v, const mat4 *const __RESTRICT__ m);
void mat4VMultMPR(const vec4 *const __RESTRICT__ v, const mat4 *const __RESTRICT__ m, vec4 *const __RESTRICT__ r);
vec4 mat4MMultV(const mat4 m, const vec4 v);
void mat4MMultVP(const mat4 *const __RESTRICT__ m, vec4 *const __RESTRICT__ v);
void mat4MMultVPR(const mat4 *const __RESTRICT__ m, const vec4 *const __RESTRICT__ v, vec4 *const __RESTRICT__ r);

vec3 mat4V3MultMBra(const vec3 v, const mat4 m);
void mat4V3MultMBraP(vec3 *const __RESTRICT__ v, const mat4 *const __RESTRICT__ m);
void mat4V3MultMBraPR(const vec3 *const __RESTRICT__ v, const mat4 *const __RESTRICT__ m, vec3 *const __RESTRICT__ r);
vec3 mat4MMultV3Ket(const mat4 m, const vec3 v);
void mat4MMultV3KetP(const mat4 *const __RESTRICT__ m, vec3 *const __RESTRICT__ v);
void mat4MMultV3KetPR(const mat4 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r);

vec4 mat4NMultMBra(const float x, const float y, const float z, const float w, const mat4 m);
void mat4NMultMBraPR(const float x, const float y, const float z, const float w, const mat4 *const __RESTRICT__ m, vec4 *const __RESTRICT__ r);
vec4 mat4MMultNKet(const mat4 m, const float x, const float y, const float z, const float w);
void mat4MMultNKetPR(const mat4 *const __RESTRICT__ m, const float x, const float y, const float z, const float w, vec4 *const __RESTRICT__ r);

vec3 mat4N3MultMBra(const float x, const float y, const float z, const mat4 m);
void mat4N3MultMBraPR(const float x, const float y, const float z, const mat4 *const __RESTRICT__ m, vec3 *const __RESTRICT__ r);
vec3 mat4MMultN3Ket(const mat4 m, const float x, const float y, const float z);
void mat4MMultN3KetPR(const mat4 *const __RESTRICT__ m, const float x, const float y, const float z, vec3 *const __RESTRICT__ r);

mat4 mat4MAddM(const mat4 m1, const mat4 m2);
void mat4MAddMP(mat4 *const __RESTRICT__ m1, const mat4 *const __RESTRICT__ m2);
void mat4MAddMPR(const mat4 *const __RESTRICT__ m1, const mat4 *const __RESTRICT__ m2, mat4 *const __RESTRICT__ r);

float mat4Determinant(const mat4 m);
float mat4DeterminantP(const mat4 *const __RESTRICT__ m);

mat4 mat4Transpose(const mat4 m);
void mat4TransposeP(mat4 *const __RESTRICT__ m);
void mat4TransposePR(const mat4 *const __RESTRICT__ m, mat4 *const __RESTRICT__ r);

mat4 mat4Invert(const mat4 m);
return_t mat4InvertR(const mat4 m, mat4 *const __RESTRICT__ r);
return_t mat4InvertP(mat4 *const __RESTRICT__ m);
return_t mat4InvertPR(const mat4 *const __RESTRICT__ m, mat4 *const __RESTRICT__ r);

mat4 mat4Frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
void mat4FrustumP(mat4 *const __RESTRICT__ m, const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
mat4 mat4Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
void mat4OrthoP(mat4 *const __RESTRICT__ m, const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
mat4 mat4Perspective(const float fovy, const float aspectRatio, const float zNear, const float zFar);
void mat4PerspectiveP(mat4 *const __RESTRICT__ m, const float fovy, const float aspectRatio, const float zNear, const float zFar);
mat4 mat4RotateToFace(const vec3 eye, const vec3 target, const vec3 up);
void mat4RotateToFaceP(mat4 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ eye, const vec3 *const __RESTRICT__ target, const vec3 *const __RESTRICT__ up);
mat4 mat4LookAt(const vec3 eye, const vec3 target, const vec3 up);
void mat4LookAtP(mat4 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ eye, const vec3 *const __RESTRICT__ target, const vec3 *const __RESTRICT__ up);

mat4 mat4TranslationMatrix(const float x, const float y, const float z);
void mat4TranslationMatrixP(mat4 *const __RESTRICT__ r, const float x, const float y, const float z);
mat4 mat4Translate(const mat4 m, const float x, const float y, const float z);
void mat4TranslateP(mat4 *const __RESTRICT__ m, const float x, const float y, const float z);
void mat4TranslatePR(const mat4 *const __RESTRICT__ m, const float x, const float y, const float z, mat4 *const __RESTRICT__ r);
mat4 mat4TranslatePre(const mat4 m, const float x, const float y, const float z);

mat4 mat4RotationMatrix(const quat q);
void mat4RotationMatrixP(mat4 *const __RESTRICT__ m, const quat *const __RESTRICT__ q);
mat4 mat4Rotate(const mat4 m, const quat q);
void mat4RotateP(mat4 *const __RESTRICT__ m, const quat *const __RESTRICT__ q);
void mat4RotatePR(const mat4 *const __RESTRICT__ m, const quat *const __RESTRICT__ q, mat4 *const __RESTRICT__ r);

mat4 mat4ScaleMatrix(const float x, const float y, const float z);
void mat4ScaleMatrixP(mat4 *const __RESTRICT__ m, const float x, const float y, const float z);
mat4 mat4Scale(const mat4 m, const float x, const float y, const float z);
void mat4ScaleP(mat4 *const __RESTRICT__ m, const float x, const float y, const float z);
void mat4ScalePR(const mat4 *const __RESTRICT__ m, const float x, const float y, const float z, mat4 *const __RESTRICT__ r);
mat4 mat4ScalePre(const mat4 m, const float x, const float y, const float z);

mat4 mat4Quaternion(const quat q);
void mat4QuaternionP(mat4 *const __RESTRICT__ m, const quat *const __RESTRICT__ q);

#endif
