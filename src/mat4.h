#ifndef MAT4_H
#define MAT4_H

#include "vec4.h"
#include "quat.h"

typedef struct {
	float m[4][4];
} mat4;

void mat4Identity(mat4 *m);
mat4 mat4GetIdentity();

mat4 mat4MMultM(mat4 *m1, mat4 *m2);
void mat4MMultByM(mat4 *m1, mat4 *m2, mat4 *r);
void mat4MultMByM1(mat4 *m1, mat4 *m2);
void mat4MultMByM2(mat4 *m1, mat4 *m2);
vec4 mat4VMultM(vec4 v, mat4 *m);
void mat4MultVByM(vec4 *v, mat4 *m);
vec4 mat4MMultV(mat4 *m, vec4 v);
void mat4MultMByV(mat4 *m, vec4 *v);

mat4 mat4GetTranspose(mat4 *m);
void mat4Transpose(mat4 *m);

unsigned char mat4Frustum(mat4 *m, float left, float right, float bottom, float top, float zNear, float zFar);
unsigned char mat4Ortho(mat4 *m, float left, float right, float bottom, float top, float zNear, float zFar);
unsigned char mat4Perspective(mat4 *m, float fovy, float aspectRatio, float zNear, float zFar);
void mat4LookAt(mat4 *m, vec3 eye, vec3 target, vec3 up);

void mat4Translate(mat4 *m, float x, float y, float z);
mat4 mat4TranslationMatrix(float x, float y, float z);
void mat4Rotate(mat4 *m, quat q);
mat4 mat4RotationMatrix(quat q);
void mat4Scale(mat4 *m, float x, float y, float z);
mat4 mat4ScaleMatrix(float x, float y, float z);

void mat4Quat(mat4 *m, quat q);

#endif
