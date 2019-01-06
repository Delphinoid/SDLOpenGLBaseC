#ifndef MAT3_H
#define MAT3_H

#include "quat.h"
#include "return.h"

/** Use an alias? **/
typedef struct {
	float m[3][3];
} mat3;

mat3 mat3GetIdentity();
void mat3Identity(mat3 *const restrict m);
void mat3Zero(mat3 *const restrict m);

mat3 mat3MMultM(const mat3 *const restrict m1, const mat3 *const restrict m2);
void mat3MultMByM1(mat3 *const restrict m1, const mat3 *const restrict m2);
void mat3MultMByM2(const mat3 *const restrict m1, mat3 *const restrict m2);
void mat3MultMByMR(const mat3 *const restrict m1, const mat3 *const restrict m2, mat3 *const restrict r);
vec3 mat3VMultMColumn(const vec3 v, const mat3 *const restrict m);
void mat3MultVByMColumn(vec3 *const restrict v, const mat3 *const restrict m);
void mat3MultVByMColumnR(const vec3 *const restrict v, const mat3 *const restrict m, vec3 *const restrict r);
vec3 mat3MMultVRow(const mat3 *const restrict m, const vec3 v);
void mat3MultMByVRow(const mat3 *const restrict m, vec3 *v);
void mat3MultMByVRowR(const mat3 *const restrict m, const vec3 *const restrict v, vec3 *const restrict r);

float mat3Determinant(const mat3 *const restrict m);

mat3 mat3GetTranspose(const mat3 *const restrict m);
void mat3Transpose(mat3 *const restrict m);
void mat3TransposeR(const mat3 *const restrict m, mat3 *const restrict r);

return_t mat3Invert(mat3 *const restrict m);
return_t mat3InvertR(const mat3 *const restrict m, mat3 *const restrict r);

void mat3Quat(mat3 *const restrict m, const quat *const restrict q);

#endif
