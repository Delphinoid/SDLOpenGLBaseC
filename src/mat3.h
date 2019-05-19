#ifndef MAT3_H
#define MAT3_H

#include "quat.h"
#include "return.h"

/** Use an alias? **/
typedef struct {
	float m[3][3];
} mat3;

mat3 mat3Identity();
mat3 mat3Zero();
void mat3IdentityP(mat3 *const restrict m);
void mat3ZeroP(mat3 *const restrict m);

mat3 mat3MMultM(const mat3 m1, const mat3 m2);
void mat3MMultMP1(mat3 *const restrict m1, const mat3 *const restrict m2);
void mat3MMultMP2(const mat3 *const restrict m1, mat3 *const restrict m2);
void mat3MMultMPR(const mat3 *const restrict m1, const mat3 *const restrict m2, mat3 *const restrict r);
vec3 mat3VMultMKet(const vec3 v, const mat3 m);
void mat3VMultMKetP(vec3 *const restrict v, const mat3 *const restrict m);
void mat3VMultMKetPR(const vec3 *const restrict v, const mat3 *const restrict m, vec3 *const restrict r);
vec3 mat3MMultVBra(const mat3 m, const vec3 v);
void mat3MMultVBraP(const mat3 *const restrict m, vec3 *const restrict v);
void mat3MMultVBraPR(const mat3 *const restrict m, const vec3 *const restrict v, vec3 *const restrict r);

mat3 mat3MAddM(const mat3 m1, const mat3 m2);
void mat3MAddMP(mat3 *const restrict m1, const mat3 *const restrict m2);
void mat3MAddMPR(const mat3 *const restrict m1, const mat3 *const restrict m2, mat3 *const restrict r);

float mat3Determinant(const mat3 m);
float mat3DeterminantP(const mat3 *const restrict m);

mat3 mat3Transpose(const mat3 m);
void mat3TransposeP(mat3 *const restrict m);
void mat3TransposePR(const mat3 *const restrict m, mat3 *const restrict r);

mat3 mat3Invert(const mat3 m);
return_t mat3InvertR(const mat3 m, mat3 *const restrict r);
return_t mat3InvertP(mat3 *const restrict m);
return_t mat3InvertPR(const mat3 *const restrict m, mat3 *const restrict r);

mat3 mat3Quaternion(const quat q);
void mat3QuaternionP(mat3 *const restrict m, const quat *const restrict q);

#endif
