#ifndef MAT4_H
#define MAT4_H

#include "vec4.h"
#include "quat.h"

typedef struct {
	float m[4][4];
} mat4;

void mat4Identity(mat4 *m);
mat4 mat4GetIdentity();

mat4 mat4MMultM(const mat4 *m1, const mat4 *m2);
void mat4MMultByM(const mat4 *m1, const mat4 *m2, mat4 *r);
void mat4MultMByM1(mat4 *m1, const mat4 *m2);
void mat4MultMByM2(const mat4 *m1, mat4 *m2);
vec4 mat4VMultM(const vec4 v, const mat4 *m);
void mat4MultVByM(vec4 *v, const mat4 *m);
vec4 mat4MMultV(const mat4 *m, const vec4 v);
void mat4MultMByV(const mat4 *m, vec4 *v);

mat4 mat4GetTranspose(const mat4 *m);
void mat4Transpose(mat4 *m);

unsigned char mat4Frustum(mat4 *m, const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
unsigned char mat4Ortho(mat4 *m, const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
unsigned char mat4Perspective(mat4 *m, const float fovy, const float aspectRatio, const float zNear, const float zFar);
void mat4LookAt(mat4 *m, const vec3 eye, const vec3 target, const vec3 up);

void mat4Translate(mat4 *m, const float x, const float y, const float z);
mat4 mat4TranslationMatrix(const float x, const float y, const float z);
void mat4Rotate(mat4 *m, quat q);
mat4 mat4RotationMatrix(quat q);
void mat4Scale(mat4 *m, const float x, const float y, const float z);
mat4 mat4ScaleMatrix(const float x, const float y, const float z);

void mat4Quat(mat4 *m, quat q);

#endif
