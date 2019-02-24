#ifndef MAT2_H
#define MAT2_H

#include "vec2.h"
#include "return.h"

/** Use an alias? **/
typedef struct {
	float m[2][2];
} mat2;

mat2 mat2GetIdentity();
void mat2Identity(mat2 *const restrict m);
void mat2Zero(mat2 *const restrict m);

mat2 mat2MMultM(const mat2 m1, const mat2 m2);
void mat2MultMByM1(mat2 *const restrict m1, const mat2 m2);
void mat2MultMByM2(const mat2 m1, mat2 *const restrict m2);
void mat2MultMByMR(const mat2 m1, const mat2 m2, mat2 *const restrict r);
vec2 mat2VMultMKet(const vec2 *const restrict v, const mat2 *const restrict m);
void mat2MultVByMKet(vec2 *const restrict v, const mat2 *const restrict m);
void mat2MultVByMKetR(const vec2 *const restrict v, const mat2 *const restrict m, vec2 *const restrict r);
vec2 mat2MMultVBra(const mat2 *const restrict m, const vec2 *const restrict v);
void mat2MultMByVBra(const mat2 *const restrict m, vec2 *const restrict v);
void mat2MultMByVBraR(const mat2 *const restrict m, const vec2 *const restrict v, vec2 *const restrict r);

mat2 mat2MAddM(const mat2 *const restrict m1, const mat2 *const restrict m2);
void mat2AddMToM(mat2 *const restrict m1, const mat2 *const restrict m2);
void mat2AddMToMR(const mat2 *const restrict m1, const mat2 *const restrict m2, mat2 *const restrict r);

float mat2Determinant(const mat2 *const restrict m);

mat2 mat2GetTranspose(const mat2 *const restrict m);
void mat2Transpose(mat2 *const restrict m);
void mat2TransposeR(const mat2 *const restrict m, mat2 *const restrict r);

return_t mat2Invert(mat2 *const restrict m);
return_t mat2InvertR(const mat2 *const restrict m, mat2 *const restrict r);

#endif