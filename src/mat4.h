#ifndef MAT4_H
#define MAT4_H

#include "vec4.h"
#include "quat.h"
#include "return.h"

/** Use an alias? **/
typedef struct {
	float m[4][4];
} mat4;

mat4 mat4GetIdentity();
void mat4Identity(mat4 *const restrict m);
void mat4Zero(mat4 *const restrict m);

mat4 mat4MMultM(const mat4 *const restrict m1, const mat4 *const restrict m2);
void mat4MultMByM1(mat4 *const restrict m1, const mat4 *const restrict m2);
void mat4MultMByM2(const mat4 *const restrict m1, mat4 *const restrict m2);
void mat4MultMByMR(const mat4 *const restrict m1, const mat4 *const restrict m2, mat4 *const restrict r);
void mat4TransformV(const mat4 *const restrict m, vec3 *const restrict v);

float mat4Determinant(const mat4 *const restrict m);

mat4 mat4GetTranspose(const mat4 *const restrict m);
void mat4Transpose(mat4 *const restrict m);
void mat4TransposeR(const mat4 *const restrict m, mat4 *const restrict r);

return_t mat4Invert(mat4 *const restrict m);
return_t mat4InvertR(const mat4 *const restrict m, mat4 *const restrict r);

return_t mat4Frustum(mat4 *const restrict m, const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
return_t mat4Ortho(mat4 *const restrict m, const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
return_t mat4Perspective(mat4 *const restrict m, const float fovy, const float aspectRatio, const float zNear, const float zFar);
void mat4RotateToFace(mat4 *const restrict m, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict up);
void mat4LookAt(mat4 *const restrict m, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict up);

void mat4Translate(mat4 *const restrict m, const float x, const float y, const float z);
void mat4TranslateR(const mat4 *const restrict m, const float x, const float y, const float z, mat4 *const restrict r);
void mat4SetTranslationMatrix(mat4 *const restrict r, const float x, const float y, const float z);
mat4 mat4TranslationMatrix(const float x, const float y, const float z);
void mat4Scale(mat4 *const restrict m, const float x, const float y, const float z);
void mat4ScaleR(const mat4 *const restrict m, const float x, const float y, const float z, mat4 *const restrict r);
void mat4SetScaleMatrix(mat4 *const restrict m, const float x, const float y, const float z);
mat4 mat4ScaleMatrix(const float x, const float y, const float z);

#ifdef VEC4_H
vec4 mat4VMultMKet(const vec4 *const restrict v, const mat4 *const restrict m);
void mat4MultVByMKet(vec4 *const restrict v, const mat4 *const restrict m);
void mat4MultVByMKetR(const vec4 *const restrict v, const mat4 *const restrict m, vec4 *const restrict r);
void mat4MultNByM(const float x, const float y, const float z, const float w, const mat4 *const restrict m, vec4 *const restrict r);
vec4 mat4MMultVBra(const mat4 *const restrict m, const vec4 *const restrict v);
void mat4MultMByVBra(const mat4 *const restrict m, vec4 *const restrict v);
void mat4MultMByVBraR(const mat4 *const restrict m, const vec4 *const restrict v, vec4 *const restrict r);
void mat4MultMByN(const mat4 *const restrict m, const float x, const float y, const float z, const float w, vec4 *const restrict r);
#endif

#ifdef QUAT_H
void mat4Rotate(mat4 *const restrict m, const quat *const restrict q);
void mat4RotateR(const mat4 *const restrict m, const quat *const restrict q, mat4 *const restrict r);
void mat4SetRotationMatrix(mat4 *const restrict m, const quat *const restrict q);
mat4 mat4RotationMatrix(const quat *const restrict q);
void mat4Quat(mat4 *const restrict m, const quat *const restrict q);
#endif

#endif
