#ifndef MAT2_H
#define MAT2_H

#include "vec2.h"
#include "return.h"

/** Use an alias? **/
typedef struct {
	float m[2][2];
} mat2;

mat2 mat2Identity();
mat2 mat2Zero();
void mat2IdentityP(mat2 *const restrict m);
void mat2ZeroP(mat2 *const restrict m);

mat2 mat2MMultM(const mat2 m1, const mat2 m2);
void mat2MMultMP1(mat2 *const restrict m1, const mat2 *const restrict m2);
void mat2MMultMP2(const mat2 *const restrict m1, mat2 *const restrict m2);
void mat2MMultMPR(const mat2 *const restrict m1, const mat2 *const restrict m2, mat2 *const restrict r);
vec2 mat2VMultMKet(const vec2 v, const mat2 m);
void mat2VMultMKetP(vec2 *const restrict v, const mat2 *const restrict m);
void mat2VMultMKetPR(const vec2 *const restrict v, const mat2 *const restrict m, vec2 *const restrict r);
vec2 mat2MMultVBra(const mat2 m, const vec2 v);
void mat2MMultVBraP(const mat2 *const restrict m, vec2 *const restrict v);
void mat2MMultVBraPR(const mat2 *const restrict m, const vec2 *const restrict v, vec2 *const restrict r);

mat2 mat2MAddM(const mat2 m1, const mat2 m2);
void mat2MAddMP(mat2 *const restrict m1, const mat2 *const restrict m2);
void mat2MAddMPR(const mat2 *const restrict m1, const mat2 *const restrict m2, mat2 *const restrict r);

float mat2Determinant(const mat2 m);
float mat2DeterminantP(const mat2 *const restrict m);

mat2 mat2Transpose(const mat2 m);
void mat2TransposeP(mat2 *const restrict m);
void mat2TransposePR(const mat2 *const restrict m, mat2 *const restrict r);

mat2 mat2Invert(const mat2 m);
return_t mat2InvertR(const mat2 m, mat2 *const restrict r);
return_t mat2InvertP(mat2 *const restrict m);
return_t mat2InvertPR(const mat2 *const restrict m, mat2 *const restrict r);

#endif
