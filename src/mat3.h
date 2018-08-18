#ifndef MAT3_H
#define MAT3_H

#include "quat.h"

typedef struct {
	float m[3][3];
} mat3;

void mat3Identity(mat3 *m);
mat3 mat3GetIdentity();

mat3 mat3MMultM(const mat3 *m1, const mat3 *m2);
void mat3MultMByM1(mat3 *m1, const mat3 *m2);
void mat3MultMByM2(const mat3 *m1, mat3 *m2);
void mat3MultMByMR(const mat3 *m1, const mat3 *m2, mat3 *r);
vec3 mat3VMultMColumn(const vec3 v, const mat3 *m);
void mat3MultVByMColumn(vec3 *v, const mat3 *m);
vec3 mat3MMultVRow(const mat3 *m, const vec3 v);
void mat3MultMByVRow(const mat3 *m, vec3 *v);

float mat3Determinant(const mat3 *m);

mat3 mat3GetTranspose(const mat3 *m);
void mat3Transpose(mat3 *m);
void mat3TransposeR(const mat3 *m, mat3 *r);

signed char mat3Invert(mat3 *m);
signed char mat3InvertR(const mat3 *m, mat3 *r);

void mat3Quat(mat3 *m, const quat *q);

#endif
