#include "mat2.h"
#include "inline.h"
#include <string.h>

__HINT_INLINE__ mat2 mat2GetIdentity(){
	const mat2 r = {.m = {{1.f, 0.f},
	                      {0.f, 1.f}}};
	return r;
}
__HINT_INLINE__ void mat2Identity(mat2 *const restrict m){
	m->m[0][0] = 1.f; m->m[0][1] = 0.f;
	m->m[1][0] = 0.f; m->m[1][1] = 1.f;
}
__HINT_INLINE__ void mat2Zero(mat2 *const restrict m){
	memset(m->m, 0, sizeof(mat2));
}

__HINT_INLINE__ mat2 mat2MMultM(const mat2 m1, const mat2 m2){

	const mat2 r = {.m = {{m1.m[0][0]*m2.m[0][0] + m1.m[0][1]*m2.m[1][0],
	                       m1.m[0][0]*m2.m[0][1] + m1.m[0][1]*m2.m[1][1]},
	                      {m1.m[1][0]*m2.m[0][0] + m1.m[1][1]*m2.m[1][0],
	                       m1.m[1][0]*m2.m[0][1] + m1.m[1][1]*m2.m[1][1]}}};
	return r;

}
__HINT_INLINE__ void mat2MultMByM1(mat2 *const restrict m1, const mat2 m2){

	const mat2 m1_c = *m1;
	const mat2 r = {.m = {{m1_c.m[0][0]*m2.m[0][0] + m1_c.m[0][1]*m2.m[1][0],
	                       m1_c.m[0][0]*m2.m[0][1] + m1_c.m[0][1]*m2.m[1][1]},
	                      {m1_c.m[1][0]*m2.m[0][0] + m1_c.m[1][1]*m2.m[1][0],
	                       m1_c.m[1][0]*m2.m[0][1] + m1_c.m[1][1]*m2.m[1][1]}}};
	*m1 = r;

}
__HINT_INLINE__ void mat2MultMByM2(const mat2 m1, mat2 *const restrict m2){

	const mat2 m2_c = *m2;
	const mat2 r = {.m = {{m1.m[0][0]*m2_c.m[0][0] + m1.m[0][1]*m2_c.m[1][0],
	                       m1.m[0][0]*m2_c.m[0][1] + m1.m[0][1]*m2_c.m[1][1]},
	                      {m1.m[1][0]*m2_c.m[0][0] + m1.m[1][1]*m2_c.m[1][0],
	                       m1.m[1][0]*m2_c.m[0][1] + m1.m[1][1]*m2_c.m[1][1]}}};
	*m2 = r;

}
__HINT_INLINE__ void mat2MultMByMR(const mat2 m1, const mat2 m2, mat2 *const restrict r){

	const mat2 r_c = {.m = {{m1.m[0][0]*m2.m[0][0] + m1.m[0][1]*m2.m[1][0],
	                         m1.m[0][0]*m2.m[0][1] + m1.m[0][1]*m2.m[1][1]},
	                        {m1.m[1][0]*m2.m[0][0] + m1.m[1][1]*m2.m[1][0],
	                         m1.m[1][0]*m2.m[0][1] + m1.m[1][1]*m2.m[1][1]}}};
	*r = r_c;

}
__HINT_INLINE__ vec2 mat2VMultMKet(const vec2 *const restrict v, const mat2 *const restrict m){
	const vec2 r = {.x = v->x * m->m[0][0] + v->y * m->m[1][0],
	                .y = v->x * m->m[0][1] + v->y * m->m[1][1]};
	return r;
}
__HINT_INLINE__ void mat2MultVByMKet(vec2 *const restrict v, const mat2 *const restrict m){
	const vec2 r = {.x = v->x * m->m[0][0] + v->y * m->m[1][0],
	                .y = v->x * m->m[0][1] + v->y * m->m[1][1]};
	*v = r;
}
__HINT_INLINE__ void mat2MultVByMKetR(const vec2 *const restrict v, const mat2 *const restrict m, vec2 *const restrict r){
	r->x = v->x * m->m[0][0] + v->y * m->m[1][0];
	r->y = v->x * m->m[0][1] + v->y * m->m[1][1];
}
__HINT_INLINE__ vec2 mat2MMultVBra(const mat2 *const restrict m, const vec2 *const restrict v){
	const vec2 r = {.x = v->x * m->m[0][0] + v->y * m->m[0][1],
	                .y = v->x * m->m[1][0] + v->y * m->m[1][1]};
	return r;
}
__HINT_INLINE__ void mat2MultMByVBra(const mat2 *const restrict m, vec2 *const restrict v){
	const vec2 r = {.x = v->x * m->m[0][0] + v->y * m->m[0][1],
	                .y = v->x * m->m[1][0] + v->y * m->m[1][1]};
	*v = r;
}
__HINT_INLINE__ void mat2MultMByVBraR(const mat2 *const restrict m, const vec2 *const restrict v, vec2 *const restrict r){
	r->x = v->x * m->m[0][0] + v->y * m->m[0][1];
	r->y = v->x * m->m[1][0] + v->y * m->m[1][1];
}

__HINT_INLINE__ mat2 mat2MAddM(const mat2 *const restrict m1, const mat2 *const restrict m2){
	mat2 r;
	r.m[0][0] = m1->m[0][0] + m2->m[0][0]; r.m[0][1] = m1->m[0][1] + m2->m[0][1];
	r.m[1][0] = m1->m[1][0] + m2->m[1][0]; r.m[1][1] = m1->m[1][1] + m2->m[1][1];
	return r;
}
__HINT_INLINE__ void mat2AddMToM(mat2 *const restrict m1, const mat2 *const restrict m2){
	m1->m[0][0] += m2->m[0][0]; m1->m[0][1] += m2->m[0][1];
	m1->m[1][0] += m2->m[1][0]; m1->m[1][1] += m2->m[1][1];
}
__HINT_INLINE__ void mat2AddMToMR(const mat2 *const restrict m1, const mat2 *const restrict m2, mat2 *const restrict r){
	r->m[0][0] = m1->m[0][0] + m2->m[0][0]; r->m[0][1] = m1->m[0][1] + m2->m[0][1];
	r->m[1][0] = m1->m[1][0] + m2->m[1][0]; r->m[1][1] = m1->m[1][1] + m2->m[1][1];
}

__HINT_INLINE__ float mat2Determinant(const mat2 *const restrict m){
	return m->m[0][0] * m->m[1][1] - m->m[0][1] * m->m[1][0];
}

__HINT_INLINE__ mat2 mat2GetTranspose(const mat2 *const restrict m){
	const mat2 r = {.m = {{m->m[0][0], m->m[1][0]},
	                      {m->m[0][1], m->m[1][1]}}};
	return r;
}
__HINT_INLINE__ void mat2Transpose(mat2 *const restrict m){
	const float swap = m->m[1][0];
	m->m[1][0] = m->m[0][1];
	m->m[0][1] = swap;
}

__HINT_INLINE__ void mat2TransposeR(const mat2 *const restrict m, mat2 *const restrict r){
	r->m[0][0] = m->m[0][0]; r->m[0][1] = m->m[1][0];
	r->m[1][0] = m->m[0][1]; r->m[1][1] = m->m[1][1];
}

__HINT_INLINE__ return_t mat2Invert(mat2 *const restrict m){
	float invDet = mat2Determinant(m);
	if(invDet != 0.f){
		const float temp = m->m[0][0];
		invDet = 1.f / invDet;
		m->m[0][0] = m->m[1][1] * invDet;
		m->m[0][1] *= -invDet;
		m->m[1][0] *= -invDet;
		m->m[1][1] = temp * invDet;
		return 1;
	}
	return 0;
}
__HINT_INLINE__ return_t mat2InvertR(const mat2 *const restrict m, mat2 *const restrict r){
	float invDet = mat2Determinant(m);
	if(invDet != 0.f){
		invDet = 1.f / invDet;
		r->m[0][0] = m->m[1][1] * invDet;
		r->m[0][1] = m->m[0][1] * -invDet;
		r->m[1][0] = m->m[1][0] * -invDet;
		r->m[1][1] = m->m[0][0] * invDet;
		return 1;
	}
	return 0;
}