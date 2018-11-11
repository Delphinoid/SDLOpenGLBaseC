#ifndef MAT4_H
#define MAT4_H

#include "vec4.h"
#include "quat.h"
#include "return.h"

typedef struct {
	float m[4][4];
} mat4;

void mat4Identity(mat4 *m);
mat4 mat4GetIdentity();

mat4 mat4MMultM(const mat4 *m1, const mat4 *m2);
void mat4MultMByM1(mat4 *m1, const mat4 *m2);
void mat4MultMByM2(const mat4 *m1, mat4 *m2);
void mat4MultMByMR(const mat4 *m1, const mat4 *m2, mat4 *r);
vec4 mat4VMultMColumn(const vec4 v, const mat4 *m);
void mat4MultVByMColumn(vec4 *v, const mat4 *m);
void mat4MultNByM(const float x, const float y, const float z, const float w, const mat4 *m, vec4 *r);
vec4 mat4MMultVRow(const mat4 *m, const vec4 v);
void mat4MultMByVRow(const mat4 *m, vec4 *v);
void mat4MultMByN(const mat4 *m, const float x, const float y, const float z, const float w, vec4 *r);
void mat4TransformV(const mat4 *m, vec3 *v);

float mat4Determinant(const mat4 *m);

mat4 mat4GetTranspose(const mat4 *m);
void mat4Transpose(mat4 *m);
void mat4TransposeR(const mat4 *m, mat4 *r);

return_t mat4Invert(mat4 *m);
return_t mat4InvertR(const mat4 *m, mat4 *r);

return_t mat4Frustum(mat4 *m, const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
return_t mat4Ortho(mat4 *m, const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
return_t mat4Perspective(mat4 *m, const float fovy, const float aspectRatio, const float zNear, const float zFar);
void mat4RotateToFace(mat4 *m, const vec3 *eye, const vec3 *target, const vec3 *up);
void mat4LookAt(mat4 *m, const vec3 *eye, const vec3 *target, const vec3 *up);

void mat4Translate(mat4 *m, const float x, const float y, const float z);
void mat4TranslateR(const mat4 *m, const float x, const float y, const float z, mat4 *r);
void mat4SetTranslationMatrix(mat4 *r, const float x, const float y, const float z);
mat4 mat4TranslationMatrix(const float x, const float y, const float z);
void mat4Rotate(mat4 *m, const quat *q);
void mat4RotateR(const mat4 *m, const quat *q, mat4 *r);
void mat4SetRotationMatrix(mat4 *m, const quat *q);
mat4 mat4RotationMatrix(const quat *q);
void mat4Scale(mat4 *m, const float x, const float y, const float z);
void mat4ScaleR(const mat4 *m, const float x, const float y, const float z, mat4 *r);
void mat4SetScaleMatrix(mat4 *m, const float x, const float y, const float z);
mat4 mat4ScaleMatrix(const float x, const float y, const float z);

void mat4Quat(mat4 *m, const quat *q);

#endif
