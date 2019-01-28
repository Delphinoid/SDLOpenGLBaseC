#include "mat3.h"
#include "inline.h"
#include <string.h>

__HINT_INLINE__ mat3 mat3GetIdentity(){
	const mat3 r = {.m = {{1.f, 0.f, 0.f},
	                      {0.f, 1.f, 0.f},
	                      {0.f, 0.f, 1.f}}};
	return r;
}
__HINT_INLINE__ void mat3Identity(mat3 *const restrict m){
	m->m[0][0] = 1.f; m->m[0][1] = 0.f; m->m[0][2] = 0.f;
	m->m[1][0] = 0.f; m->m[1][1] = 1.f; m->m[1][2] = 0.f;
	m->m[2][0] = 0.f; m->m[2][1] = 0.f; m->m[2][2] = 1.f;
}
__HINT_INLINE__ void mat3Zero(mat3 *const restrict m){
	memset(m->m, 0, sizeof(mat3));
}

__HINT_INLINE__ mat3 mat3MMultM(const mat3 *const restrict m1, const mat3 *const restrict m2){

	const mat3 r = {.m = {{m1->m[0][0]*m2->m[0][0] + m1->m[0][1]*m2->m[1][0] + m1->m[0][2]*m2->m[2][0],
	                       m1->m[0][0]*m2->m[0][1] + m1->m[0][1]*m2->m[1][1] + m1->m[0][2]*m2->m[2][1],
	                       m1->m[0][0]*m2->m[0][2] + m1->m[0][1]*m2->m[1][2] + m1->m[0][2]*m2->m[2][2]},
	                      {m1->m[1][0]*m2->m[0][0] + m1->m[1][1]*m2->m[1][0] + m1->m[1][2]*m2->m[2][0],
	                       m1->m[1][0]*m2->m[0][1] + m1->m[1][1]*m2->m[1][1] + m1->m[1][2]*m2->m[2][1],
	                       m1->m[1][0]*m2->m[0][2] + m1->m[1][1]*m2->m[1][2] + m1->m[1][2]*m2->m[2][2]},
	                      {m1->m[2][0]*m2->m[0][0] + m1->m[2][1]*m2->m[1][0] + m1->m[2][2]*m2->m[2][0],
	                       m1->m[2][0]*m2->m[0][1] + m1->m[2][1]*m2->m[1][1] + m1->m[2][2]*m2->m[2][1],
	                       m1->m[2][0]*m2->m[0][2] + m1->m[2][1]*m2->m[1][2] + m1->m[2][2]*m2->m[2][2]}}};
	return r;

}
__HINT_INLINE__ void mat3MultMByM1(mat3 *const restrict m1, const mat3 *const restrict m2){

	const mat3 r = {.m = {{m1->m[0][0]*m2->m[0][0] + m1->m[0][1]*m2->m[1][0] + m1->m[0][2]*m2->m[2][0],
	                       m1->m[0][0]*m2->m[0][1] + m1->m[0][1]*m2->m[1][1] + m1->m[0][2]*m2->m[2][1],
	                       m1->m[0][0]*m2->m[0][2] + m1->m[0][1]*m2->m[1][2] + m1->m[0][2]*m2->m[2][2]},
	                      {m1->m[1][0]*m2->m[0][0] + m1->m[1][1]*m2->m[1][0] + m1->m[1][2]*m2->m[2][0],
	                       m1->m[1][0]*m2->m[0][1] + m1->m[1][1]*m2->m[1][1] + m1->m[1][2]*m2->m[2][1],
	                       m1->m[1][0]*m2->m[0][2] + m1->m[1][1]*m2->m[1][2] + m1->m[1][2]*m2->m[2][2]},
	                      {m1->m[2][0]*m2->m[0][0] + m1->m[2][1]*m2->m[1][0] + m1->m[2][2]*m2->m[2][0],
	                       m1->m[2][0]*m2->m[0][1] + m1->m[2][1]*m2->m[1][1] + m1->m[2][2]*m2->m[2][1],
	                       m1->m[2][0]*m2->m[0][2] + m1->m[2][1]*m2->m[1][2] + m1->m[2][2]*m2->m[2][2]}}};
	*m1 = r;

}
__HINT_INLINE__ void mat3MultMByM2(const mat3 *const restrict m1, mat3 *const restrict m2){

	const mat3 r = {.m = {{m1->m[0][0]*m2->m[0][0] + m1->m[0][1]*m2->m[1][0] + m1->m[0][2]*m2->m[2][0],
	                       m1->m[0][0]*m2->m[0][1] + m1->m[0][1]*m2->m[1][1] + m1->m[0][2]*m2->m[2][1],
	                       m1->m[0][0]*m2->m[0][2] + m1->m[0][1]*m2->m[1][2] + m1->m[0][2]*m2->m[2][2]},
	                      {m1->m[1][0]*m2->m[0][0] + m1->m[1][1]*m2->m[1][0] + m1->m[1][2]*m2->m[2][0],
	                       m1->m[1][0]*m2->m[0][1] + m1->m[1][1]*m2->m[1][1] + m1->m[1][2]*m2->m[2][1],
	                       m1->m[1][0]*m2->m[0][2] + m1->m[1][1]*m2->m[1][2] + m1->m[1][2]*m2->m[2][2]},
	                      {m1->m[2][0]*m2->m[0][0] + m1->m[2][1]*m2->m[1][0] + m1->m[2][2]*m2->m[2][0],
	                       m1->m[2][0]*m2->m[0][1] + m1->m[2][1]*m2->m[1][1] + m1->m[2][2]*m2->m[2][1],
	                       m1->m[2][0]*m2->m[0][2] + m1->m[2][1]*m2->m[1][2] + m1->m[2][2]*m2->m[2][2]}}};
	*m2 = r;

}
__HINT_INLINE__ void mat3MultMByMR(const mat3 *const restrict m1, const mat3 *const restrict m2, mat3 *const restrict r){

	r->m[0][0] = m1->m[0][0]*m2->m[0][0] + m1->m[0][1]*m2->m[1][0] + m1->m[0][2]*m2->m[2][0];
	r->m[0][1] = m1->m[0][0]*m2->m[0][1] + m1->m[0][1]*m2->m[1][1] + m1->m[0][2]*m2->m[2][1];
	r->m[0][2] = m1->m[0][0]*m2->m[0][2] + m1->m[0][1]*m2->m[1][2] + m1->m[0][2]*m2->m[2][2];

	r->m[1][0] = m1->m[1][0]*m2->m[0][0] + m1->m[1][1]*m2->m[1][0] + m1->m[1][2]*m2->m[2][0];
	r->m[1][1] = m1->m[1][0]*m2->m[0][1] + m1->m[1][1]*m2->m[1][1] + m1->m[1][2]*m2->m[2][1];
	r->m[1][2] = m1->m[1][0]*m2->m[0][2] + m1->m[1][1]*m2->m[1][2] + m1->m[1][2]*m2->m[2][2];

	r->m[2][0] = m1->m[2][0]*m2->m[0][0] + m1->m[2][1]*m2->m[1][0] + m1->m[2][2]*m2->m[2][0];
	r->m[2][1] = m1->m[2][0]*m2->m[0][1] + m1->m[2][1]*m2->m[1][1] + m1->m[2][2]*m2->m[2][1];
	r->m[2][2] = m1->m[2][0]*m2->m[0][2] + m1->m[2][1]*m2->m[1][2] + m1->m[2][2]*m2->m[2][2];

}
__HINT_INLINE__ vec3 mat3VMultMKet(const vec3 *const restrict v, const mat3 *const restrict m){
	const vec3 r = {.x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0],
	                .y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1],
	                .z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2]};
	return r;
}
__HINT_INLINE__ void mat3MultVByMKet(vec3 *const restrict v, const mat3 *const restrict m){
	const vec3 r = {.x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0],
	                .y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1],
	                .z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2]};
	*v = r;
}
__HINT_INLINE__ void mat3MultVByMKetR(const vec3 *const restrict v, const mat3 *const restrict m, vec3 *const restrict r){
	r->x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0];
	r->y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1];
	r->z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2];
}
__HINT_INLINE__ vec3 mat3MMultVBra(const mat3 *const restrict m, const vec3 *const restrict v){
	const vec3 r = {.x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2],
	                .y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2],
	                .z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2]};
	return r;
}
__HINT_INLINE__ void mat3MultMByVBra(const mat3 *const restrict m, vec3 *const restrict v){
	const vec3 r = {.x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2],
	                .y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2],
	                .z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2]};
	*v = r;
}
__HINT_INLINE__ void mat3MultMByVBraR(const mat3 *const restrict m, const vec3 *const restrict v, vec3 *const restrict r){
	r->x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2];
	r->y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2];
	r->z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2];
}

__HINT_INLINE__ mat3 mat3MAddM(const mat3 *const restrict m1, const mat3 *const restrict m2){
	mat3 r;
	r.m[0][0] = m1->m[0][0] + m2->m[0][0]; r.m[0][1] = m1->m[0][1] + m2->m[0][1]; r.m[0][2] = m1->m[0][2] + m2->m[0][2];
	r.m[1][0] = m1->m[1][0] + m2->m[1][0]; r.m[1][1] = m1->m[1][1] + m2->m[1][1]; r.m[1][2] = m1->m[1][2] + m2->m[1][2];
	r.m[2][0] = m1->m[2][0] + m2->m[2][0]; r.m[2][1] = m1->m[2][1] + m2->m[2][1]; r.m[2][2] = m1->m[2][2] + m2->m[2][2];
	return r;
}
__HINT_INLINE__ void mat3AddMToM(mat3 *const restrict m1, const mat3 *const restrict m2){
	m1->m[0][0] += m2->m[0][0]; m1->m[0][1] += m2->m[0][1]; m1->m[0][2] += m2->m[0][2];
	m1->m[1][0] += m2->m[1][0]; m1->m[1][1] += m2->m[1][1]; m1->m[1][2] += m2->m[1][2];
	m1->m[2][0] += m2->m[2][0]; m1->m[2][1] += m2->m[2][1]; m1->m[2][2] += m2->m[2][2];
}
__HINT_INLINE__ void mat3AddMToMR(const mat3 *const restrict m1, const mat3 *const restrict m2, mat3 *const restrict r){
	r->m[0][0] = m1->m[0][0] + m2->m[0][0]; r->m[0][1] = m1->m[0][1] + m2->m[0][1]; r->m[0][2] = m1->m[0][2] + m2->m[0][2];
	r->m[1][0] = m1->m[1][0] + m2->m[1][0]; r->m[1][1] = m1->m[1][1] + m2->m[1][1]; r->m[1][2] = m1->m[1][2] + m2->m[1][2];
	r->m[2][0] = m1->m[2][0] + m2->m[2][0]; r->m[2][1] = m1->m[2][1] + m2->m[2][1]; r->m[2][2] = m1->m[2][2] + m2->m[2][2];
}

__HINT_INLINE__ float mat3Determinant(const mat3 *const restrict m){
	return m->m[0][0] * (m->m[1][1]*m->m[2][2] - m->m[2][1]*m->m[1][2]) -
	       m->m[1][0] * (m->m[0][1]*m->m[2][2] - m->m[2][1]*m->m[0][2]) +
	       m->m[2][0] * (m->m[0][1]*m->m[1][2] - m->m[1][1]*m->m[0][2]);
}

__HINT_INLINE__ mat3 mat3GetTranspose(const mat3 *const restrict m){
	const mat3 r = {.m = {{m->m[0][0], m->m[1][0], m->m[2][0]},
	                      {m->m[0][1], m->m[1][1], m->m[2][1]},
	                      {m->m[0][2], m->m[1][2], m->m[2][2]}}};
	return r;
}
__HINT_INLINE__ void mat3Transpose(mat3 *const restrict m){
	float swap = m->m[1][0];
	m->m[1][0] = m->m[0][1];
	m->m[0][1] = swap;
	swap = m->m[2][0];
	m->m[2][0] = m->m[0][2];
	m->m[0][2] = swap;
	swap = m->m[2][1];
	m->m[2][1] = m->m[1][2];
	m->m[1][2] = swap;
}

__HINT_INLINE__ void mat3TransposeR(const mat3 *const restrict m, mat3 *const restrict r){
	r->m[0][0] = m->m[0][0]; r->m[0][1] = m->m[1][0]; r->m[0][2] = m->m[2][0];
	r->m[1][0] = m->m[0][1]; r->m[1][1] = m->m[1][1]; r->m[1][2] = m->m[2][1];
	r->m[2][0] = m->m[0][2]; r->m[2][1] = m->m[1][2]; r->m[2][2] = m->m[2][2];
}

__HINT_INLINE__ return_t mat3Invert(mat3 *const restrict m){

	const float f0 = m->m[1][1] * m->m[2][2] - m->m[1][2] * m->m[2][1];
	const float f1 = m->m[2][1] * m->m[0][2] - m->m[0][1] * m->m[2][2];
	const float f2 = m->m[0][1] * m->m[1][2] - m->m[1][1] * m->m[0][2];
	float invDet = m->m[0][0] * f0 +
	               m->m[1][0] * f1 +
	               m->m[2][0] * f2;

	if(invDet != 0.f){

		const mat3 temp = *m;
		invDet = 1.f / invDet;

		m->m[0][0] = f0 * invDet;
		m->m[1][0] = (temp.m[2][0] * temp.m[1][2] - temp.m[1][0] * temp.m[1][1]) * invDet;
		m->m[2][0] = (temp.m[1][0] * temp.m[2][1] - temp.m[2][0] * temp.m[2][2]) * invDet;
		m->m[0][1] = f1 * invDet;
		m->m[1][1] = (temp.m[0][0] * temp.m[2][2] - temp.m[2][0] * temp.m[0][2]) * invDet;
		m->m[2][1] = (temp.m[0][1] * temp.m[2][0] - temp.m[0][0] * temp.m[2][1]) * invDet;
		m->m[0][2] = f2 * invDet;
		m->m[1][2] = (temp.m[0][2] * temp.m[1][0] - temp.m[0][0] * temp.m[1][2]) * invDet;
		m->m[2][2] = (temp.m[0][0] * temp.m[1][1] - temp.m[0][1] * temp.m[1][0]) * invDet;

		return 1;

	}

	return 0;

}
__HINT_INLINE__ return_t mat3InvertR(const mat3 *const restrict m, mat3 *const restrict r){

	const float f0 = m->m[1][1] * m->m[2][2] - m->m[1][2] * m->m[2][1];
	const float f1 = m->m[2][1] * m->m[0][2] - m->m[0][1] * m->m[2][2];
	const float f2 = m->m[0][1] * m->m[1][2] - m->m[1][1] * m->m[0][2];
	float invDet = m->m[0][0] * f0 +
	               m->m[1][0] * f1 +
	               m->m[2][0] * f2;

	if(invDet != 0.f){

		const mat3 temp = *m;
		invDet = 1.f / invDet;

		r->m[0][0] = f0 * invDet;
		r->m[1][0] = (temp.m[2][0] * temp.m[1][2] - temp.m[1][0] * temp.m[1][1]) * invDet;
		r->m[2][0] = (temp.m[1][0] * temp.m[2][1] - temp.m[2][0] * temp.m[2][2]) * invDet;
		r->m[0][1] = f1 * invDet;
		r->m[1][1] = (temp.m[0][0] * temp.m[2][2] - temp.m[2][0] * temp.m[0][2]) * invDet;
		r->m[2][1] = (temp.m[0][1] * temp.m[2][0] - temp.m[0][0] * temp.m[2][1]) * invDet;
		r->m[0][2] = f2 * invDet;
		r->m[1][2] = (temp.m[0][2] * temp.m[1][0] - temp.m[0][0] * temp.m[1][2]) * invDet;
		r->m[2][2] = (temp.m[0][0] * temp.m[1][1] - temp.m[0][1] * temp.m[1][0]) * invDet;

		return 1;

	}

	return 0;

}

__HINT_INLINE__ void mat3Quat(mat3 *const restrict m, const quat *const restrict q){
	const float xx = q->v.x*q->v.x;
	const float yy = q->v.y*q->v.y;
	const float zz = q->v.z*q->v.z;
	const float xy = q->v.x*q->v.y;
	const float xz = q->v.x*q->v.z;
	const float xw = q->v.x*q->w;
	const float yz = q->v.y*q->v.z;
	const float yw = q->v.y*q->w;
	const float zw = q->v.z*q->w;
	m->m[0][0] = 1.f-2.f*(yy+zz); m->m[0][1] = 2.f*(xy+zw);     m->m[0][2] = 2.f*(xz-yw);
	m->m[1][0] = 2.f*(xy-zw);     m->m[1][1] = 1.f-2.f*(xx+zz); m->m[1][2] = 2.f*(yz+xw);
	m->m[2][0] = 2.f*(xz+yw);     m->m[2][1] = 2.f*(yz-xw);     m->m[2][2] = 1.f-2.f*(xx+yy);
}
