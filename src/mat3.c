#include "mat3.h"
#include "inline.h"
#include <string.h>

__HINT_INLINE__ mat3 mat3Identity(){
	const mat3 r = {.m = {{1.f, 0.f, 0.f},
	                      {0.f, 1.f, 0.f},
	                      {0.f, 0.f, 1.f}}};
	return r;
}
__HINT_INLINE__ void mat3IdentityP(mat3 *const restrict m){
	m->m[0][0] = 1.f; m->m[0][1] = 0.f; m->m[0][2] = 0.f;
	m->m[1][0] = 0.f; m->m[1][1] = 1.f; m->m[1][2] = 0.f;
	m->m[2][0] = 0.f; m->m[2][1] = 0.f; m->m[2][2] = 1.f;
}
__HINT_INLINE__ mat3 mat3Zero(){
	const mat3 r = {.m = {{0.f, 0.f, 0.f},
	                      {0.f, 0.f, 0.f},
	                      {0.f, 0.f, 0.f}}};
	return r;
}
__HINT_INLINE__ void mat3ZeroP(mat3 *const restrict m){
	memset(m->m, 0, sizeof(mat3));
}

__HINT_INLINE__ mat3 mat3MMultM(const mat3 m1, const mat3 m2){

	const mat3 r = {.m = {{m1.m[0][0]*m2.m[0][0] + m1.m[1][0]*m2.m[0][1] + m1.m[2][0]*m2.m[0][2],
	                       m1.m[0][1]*m2.m[0][0] + m1.m[1][1]*m2.m[0][1] + m1.m[2][1]*m2.m[0][2],
	                       m1.m[0][2]*m2.m[0][0] + m1.m[1][2]*m2.m[0][1] + m1.m[2][2]*m2.m[0][2]},
	                      {m1.m[0][0]*m2.m[1][0] + m1.m[1][0]*m2.m[1][1] + m1.m[2][0]*m2.m[1][2],
	                       m1.m[0][1]*m2.m[1][0] + m1.m[1][1]*m2.m[1][1] + m1.m[2][1]*m2.m[1][2],
	                       m1.m[0][2]*m2.m[1][0] + m1.m[1][2]*m2.m[1][1] + m1.m[2][2]*m2.m[1][2]},
	                      {m1.m[0][0]*m2.m[2][0] + m1.m[1][0]*m2.m[2][1] + m1.m[2][0]*m2.m[2][2],
	                       m1.m[0][1]*m2.m[2][0] + m1.m[1][1]*m2.m[2][1] + m1.m[2][1]*m2.m[2][2],
	                       m1.m[0][2]*m2.m[2][0] + m1.m[1][2]*m2.m[2][1] + m1.m[2][2]*m2.m[2][2]}}};
	return r;

}
__HINT_INLINE__ void mat3MMultMP1(mat3 *const restrict m1, const mat3 *const restrict m2){

	const mat3 r = {.m = {{m1->m[0][0]*m2->m[0][0] + m1->m[1][0]*m2->m[0][1] + m1->m[2][0]*m2->m[0][2],
	                       m1->m[0][1]*m2->m[0][0] + m1->m[1][1]*m2->m[0][1] + m1->m[2][1]*m2->m[0][2],
	                       m1->m[0][2]*m2->m[0][0] + m1->m[1][2]*m2->m[0][1] + m1->m[2][2]*m2->m[0][2]},
	                      {m1->m[0][0]*m2->m[1][0] + m1->m[1][0]*m2->m[1][1] + m1->m[2][0]*m2->m[1][2],
	                       m1->m[0][1]*m2->m[1][0] + m1->m[1][1]*m2->m[1][1] + m1->m[2][1]*m2->m[1][2],
	                       m1->m[0][2]*m2->m[1][0] + m1->m[1][2]*m2->m[1][1] + m1->m[2][2]*m2->m[1][2]},
	                      {m1->m[0][0]*m2->m[2][0] + m1->m[1][0]*m2->m[2][1] + m1->m[2][0]*m2->m[2][2],
	                       m1->m[0][1]*m2->m[2][0] + m1->m[1][1]*m2->m[2][1] + m1->m[2][1]*m2->m[2][2],
	                       m1->m[0][2]*m2->m[2][0] + m1->m[1][2]*m2->m[2][1] + m1->m[2][2]*m2->m[2][2]}}};
	*m1 = r;

}
__HINT_INLINE__ void mat3MMultMP2(const mat3 *const restrict m1, mat3 *const restrict m2){

	const mat3 r = {.m = {{m1->m[0][0]*m2->m[0][0] + m1->m[1][0]*m2->m[0][1] + m1->m[2][0]*m2->m[0][2],
	                       m1->m[0][1]*m2->m[0][0] + m1->m[1][1]*m2->m[0][1] + m1->m[2][1]*m2->m[0][2],
	                       m1->m[0][2]*m2->m[0][0] + m1->m[1][2]*m2->m[0][1] + m1->m[2][2]*m2->m[0][2]},
	                      {m1->m[0][0]*m2->m[1][0] + m1->m[1][0]*m2->m[1][1] + m1->m[2][0]*m2->m[1][2],
	                       m1->m[0][1]*m2->m[1][0] + m1->m[1][1]*m2->m[1][1] + m1->m[2][1]*m2->m[1][2],
	                       m1->m[0][2]*m2->m[1][0] + m1->m[1][2]*m2->m[1][1] + m1->m[2][2]*m2->m[1][2]},
	                      {m1->m[0][0]*m2->m[2][0] + m1->m[1][0]*m2->m[2][1] + m1->m[2][0]*m2->m[2][2],
	                       m1->m[0][1]*m2->m[2][0] + m1->m[1][1]*m2->m[2][1] + m1->m[2][1]*m2->m[2][2],
	                       m1->m[0][2]*m2->m[2][0] + m1->m[1][2]*m2->m[2][1] + m1->m[2][2]*m2->m[2][2]}}};
	*m2 = r;

}
__HINT_INLINE__ void mat3MMultMPR(const mat3 *const restrict m1, const mat3 *const restrict m2, mat3 *const restrict r){

	r->m[0][0] = m1->m[0][0]*m2->m[0][0] + m1->m[1][0]*m2->m[0][1] + m1->m[2][0]*m2->m[0][2];
	r->m[0][1] = m1->m[0][1]*m2->m[0][0] + m1->m[1][1]*m2->m[0][1] + m1->m[2][1]*m2->m[0][2];
	r->m[0][2] = m1->m[0][2]*m2->m[0][0] + m1->m[1][2]*m2->m[0][1] + m1->m[2][2]*m2->m[0][2];

	r->m[1][0] = m1->m[0][0]*m2->m[1][0] + m1->m[1][0]*m2->m[1][1] + m1->m[2][0]*m2->m[1][2];
	r->m[1][1] = m1->m[0][1]*m2->m[1][0] + m1->m[1][1]*m2->m[1][1] + m1->m[2][1]*m2->m[1][2];
	r->m[1][2] = m1->m[0][2]*m2->m[1][0] + m1->m[1][2]*m2->m[1][1] + m1->m[2][2]*m2->m[1][2];

	r->m[2][0] = m1->m[0][0]*m2->m[2][0] + m1->m[1][0]*m2->m[2][1] + m1->m[2][0]*m2->m[2][2];
	r->m[2][1] = m1->m[0][1]*m2->m[2][0] + m1->m[1][1]*m2->m[2][1] + m1->m[2][1]*m2->m[2][2];
	r->m[2][2] = m1->m[0][2]*m2->m[2][0] + m1->m[1][2]*m2->m[2][1] + m1->m[2][2]*m2->m[2][2];

}
__HINT_INLINE__ vec3 mat3VMultMBra(const vec3 v, const mat3 m){
	const vec3 r = {.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2],
	                .y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2],
	                .z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2]};
	return r;
}
__HINT_INLINE__ void mat3VMultMBraP(vec3 *const restrict v, const mat3 *const restrict m){
	const vec3 r = {.x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2],
	                .y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2],
	                .z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2]};
	*v = r;
}
__HINT_INLINE__ void mat3VMultMBraPR(const vec3 *const restrict v, const mat3 *const restrict m, vec3 *const restrict r){
	r->x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2];
	r->y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2];
	r->z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2];
}
__HINT_INLINE__ vec3 mat3MMultVKet(const mat3 m, const vec3 v){
	const vec3 r = {.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
	                .y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
	                .z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]};
	return r;
}
__HINT_INLINE__ void mat3MMultVKetP(const mat3 *const restrict m, vec3 *const restrict v){
	const vec3 r = {.x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0],
	                .y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1],
	                .z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2]};
	*v = r;
}
__HINT_INLINE__ void mat3MMultVKetPR(const mat3 *const restrict m, const vec3 *const restrict v, vec3 *const restrict r){
	r->x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0];
	r->y = v->x * m->m[0][2] + v->y * m->m[1][1] + v->z * m->m[2][1];
	r->z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2];
}

__HINT_INLINE__ mat3 mat3MAddM(const mat3 m1, const mat3 m2){
	mat3 r;
	r.m[0][0] = m1.m[0][0] + m2.m[0][0]; r.m[0][1] = m1.m[0][1] + m2.m[0][1]; r.m[0][2] = m1.m[0][2] + m2.m[0][2];
	r.m[1][0] = m1.m[1][0] + m2.m[1][0]; r.m[1][1] = m1.m[1][1] + m2.m[1][1]; r.m[1][2] = m1.m[1][2] + m2.m[1][2];
	r.m[2][0] = m1.m[2][0] + m2.m[2][0]; r.m[2][1] = m1.m[2][1] + m2.m[2][1]; r.m[2][2] = m1.m[2][2] + m2.m[2][2];
	return r;
}
__HINT_INLINE__ void mat3MAddMP(mat3 *const restrict m1, const mat3 *const restrict m2){
	m1->m[0][0] += m2->m[0][0]; m1->m[0][1] += m2->m[0][1]; m1->m[0][2] += m2->m[0][2];
	m1->m[1][0] += m2->m[1][0]; m1->m[1][1] += m2->m[1][1]; m1->m[1][2] += m2->m[1][2];
	m1->m[2][0] += m2->m[2][0]; m1->m[2][1] += m2->m[2][1]; m1->m[2][2] += m2->m[2][2];
}
__HINT_INLINE__ void mat3MAddMPR(const mat3 *const restrict m1, const mat3 *const restrict m2, mat3 *const restrict r){
	r->m[0][0] = m1->m[0][0] + m2->m[0][0]; r->m[0][1] = m1->m[0][1] + m2->m[0][1]; r->m[0][2] = m1->m[0][2] + m2->m[0][2];
	r->m[1][0] = m1->m[1][0] + m2->m[1][0]; r->m[1][1] = m1->m[1][1] + m2->m[1][1]; r->m[1][2] = m1->m[1][2] + m2->m[1][2];
	r->m[2][0] = m1->m[2][0] + m2->m[2][0]; r->m[2][1] = m1->m[2][1] + m2->m[2][1]; r->m[2][2] = m1->m[2][2] + m2->m[2][2];
}

__HINT_INLINE__ float mat3Determinant(const mat3 m){
	return m.m[0][0] * (m.m[1][1]*m.m[2][2] - m.m[1][2]*m.m[2][1]) -
	       m.m[0][1] * (m.m[1][0]*m.m[2][2] - m.m[1][2]*m.m[2][0]) +
	       m.m[0][2] * (m.m[1][0]*m.m[2][1] - m.m[1][1]*m.m[2][0]);
}
__HINT_INLINE__ float mat3DeterminantP(const mat3 *const restrict m){
	return m->m[0][0] * (m->m[1][1]*m->m[2][2] - m->m[1][2]*m->m[2][1]) -
	       m->m[0][1] * (m->m[1][0]*m->m[2][2] - m->m[1][2]*m->m[2][0]) +
	       m->m[0][2] * (m->m[1][0]*m->m[2][1] - m->m[1][1]*m->m[2][0]);
}

__HINT_INLINE__ mat3 mat3Transpose(const mat3 m){
	const mat3 r = {.m = {{m.m[0][0], m.m[1][0], m.m[2][0]},
	                      {m.m[0][1], m.m[1][1], m.m[2][1]},
	                      {m.m[0][2], m.m[1][2], m.m[2][2]}}};
	return r;
}
__HINT_INLINE__ void mat3TransposeP(mat3 *const restrict m){
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

__HINT_INLINE__ void mat3TransposePR(const mat3 *const restrict m, mat3 *const restrict r){
	r->m[0][0] = m->m[0][0]; r->m[0][1] = m->m[1][0]; r->m[0][2] = m->m[2][0];
	r->m[1][0] = m->m[0][1]; r->m[1][1] = m->m[1][1]; r->m[1][2] = m->m[2][1];
	r->m[2][0] = m->m[0][2]; r->m[2][1] = m->m[1][2]; r->m[2][2] = m->m[2][2];
}

mat3 mat3Invert(const mat3 m){

	const float f0 = m.m[1][1] * m.m[2][2] - m.m[2][1] * m.m[1][2];
	const float f1 = m.m[1][2] * m.m[2][0] - m.m[1][0] * m.m[2][2];
	const float f2 = m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0];
	float invDet = m.m[0][0] * f0 + m.m[0][1] * f1 + m.m[0][2] * f2;

	if(invDet != 0.f){

		mat3 r;
		invDet = 1.f / invDet;

		r.m[0][0] = f0 * invDet;
		r.m[0][1] = (m.m[0][2] * m.m[2][1] - m.m[0][1] * m.m[1][1]) * invDet;
		r.m[0][2] = (m.m[0][1] * m.m[1][2] - m.m[0][2] * m.m[2][2]) * invDet;
		r.m[1][0] = f1 * invDet;
		r.m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[0][2] * m.m[2][0]) * invDet;
		r.m[1][2] = (m.m[1][0] * m.m[0][2] - m.m[0][0] * m.m[1][2]) * invDet;
		r.m[2][0] = f2 * invDet;
		r.m[2][1] = (m.m[2][0] * m.m[0][1] - m.m[0][0] * m.m[2][1]) * invDet;
		r.m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1]) * invDet;

		return r;

	}

	return mat3Zero();

}
return_t mat3InvertR(const mat3 m, mat3 *const restrict r){

	const float f0 = m.m[1][1] * m.m[2][2] - m.m[2][1] * m.m[1][2];
	const float f1 = m.m[1][2] * m.m[2][0] - m.m[1][0] * m.m[2][2];
	const float f2 = m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0];
	float invDet = m.m[0][0] * f0 +
	               m.m[0][1] * f1 +
	               m.m[0][2] * f2;

	if(invDet != 0.f){

		mat3 t;
		invDet = 1.f / invDet;

		t.m[0][0] = f0 * invDet;
		t.m[0][1] = (m.m[0][2] * m.m[2][1] - m.m[0][1] * m.m[1][1]) * invDet;
		t.m[0][2] = (m.m[0][1] * m.m[1][2] - m.m[0][2] * m.m[2][2]) * invDet;
		t.m[1][0] = f1 * invDet;
		t.m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[0][2] * m.m[2][0]) * invDet;
		t.m[1][2] = (m.m[1][0] * m.m[0][2] - m.m[0][0] * m.m[1][2]) * invDet;
		t.m[2][0] = f2 * invDet;
		t.m[2][1] = (m.m[2][0] * m.m[0][1] - m.m[0][0] * m.m[2][1]) * invDet;
		t.m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1]) * invDet;

		*r = t;
		return 1;

	}

	return 0;

}
__HINT_INLINE__ return_t mat3InvertP(mat3 *const restrict m){

	const float f0 = m->m[1][1] * m->m[2][2] - m->m[2][1] * m->m[1][2];
	const float f1 = m->m[1][2] * m->m[2][0] - m->m[1][0] * m->m[2][2];
	const float f2 = m->m[1][0] * m->m[2][1] - m->m[1][1] * m->m[2][0];
	float invDet = m->m[0][0] * f0 +
	               m->m[0][1] * f1 +
	               m->m[0][2] * f2;

	if(invDet != 0.f){

		const mat3 temp = *m;
		invDet = 1.f / invDet;

		m->m[0][0] = f0 * invDet;
		m->m[0][1] = (temp.m[0][2] * temp.m[2][1] - temp.m[0][1] * temp.m[1][1]) * invDet;
		m->m[0][2] = (temp.m[0][1] * temp.m[1][2] - temp.m[0][2] * temp.m[2][2]) * invDet;
		m->m[1][0] = f1 * invDet;
		m->m[1][1] = (temp.m[0][0] * temp.m[2][2] - temp.m[0][2] * temp.m[2][0]) * invDet;
		m->m[1][2] = (temp.m[1][0] * temp.m[0][2] - temp.m[0][0] * temp.m[1][2]) * invDet;
		m->m[2][0] = f2 * invDet;
		m->m[2][1] = (temp.m[2][0] * temp.m[0][1] - temp.m[0][0] * temp.m[2][1]) * invDet;
		m->m[2][2] = (temp.m[0][0] * temp.m[1][1] - temp.m[1][0] * temp.m[0][1]) * invDet;

		return 1;

	}

	return 0;

}
__HINT_INLINE__ return_t mat3InvertPR(const mat3 *const restrict m, mat3 *const restrict r){

	const float f0 = m->m[1][1] * m->m[2][2] - m->m[2][1] * m->m[1][2];
	const float f1 = m->m[1][2] * m->m[2][0] - m->m[1][0] * m->m[2][2];
	const float f2 = m->m[1][0] * m->m[2][1] - m->m[1][1] * m->m[2][0];
	float invDet = m->m[0][0] * f0 +
	               m->m[0][1] * f1 +
	               m->m[0][2] * f2;

	if(invDet != 0.f){

		const mat3 temp = *m;
		invDet = 1.f / invDet;

		r->m[0][0] = f0 * invDet;
		r->m[0][1] = (temp.m[0][2] * temp.m[2][1] - temp.m[0][1] * temp.m[1][1]) * invDet;
		r->m[0][2] = (temp.m[0][1] * temp.m[1][2] - temp.m[0][2] * temp.m[2][2]) * invDet;
		r->m[1][0] = f1 * invDet;
		r->m[1][1] = (temp.m[0][0] * temp.m[2][2] - temp.m[0][2] * temp.m[2][0]) * invDet;
		r->m[1][2] = (temp.m[1][0] * temp.m[0][2] - temp.m[0][0] * temp.m[1][2]) * invDet;
		r->m[2][0] = f2 * invDet;
		r->m[2][1] = (temp.m[2][0] * temp.m[0][1] - temp.m[0][0] * temp.m[2][1]) * invDet;
		r->m[2][2] = (temp.m[0][0] * temp.m[1][1] - temp.m[1][0] * temp.m[0][1]) * invDet;

		return 1;

	}

	return 0;

}

__HINT_INLINE__ mat3 mat3Quaternion(const quat q){
	const float x2 = q.v.x+q.v.x;
	const float y2 = q.v.y+q.v.y;
	const float z2 = q.v.z+q.v.z;
	const float w2x = q.w*x2;
	const float w2y = q.w*y2;
	const float w2z = q.w*z2;
	const float x2x = q.v.x*x2;
	const float x2y = q.v.x*y2;
	const float x2z = q.v.x*z2;
	const float y2y = q.v.y*y2;
	const float y2z = q.v.y*z2;
	const float z2z = q.v.z*z2;
	const mat3 r = {.m = {{1.f-y2y-z2z, x2y+w2z,     x2z-w2y},
	                      {x2y-w2z,     1.f-x2x-z2z, y2z+w2x},
	                      {x2z+w2y,     y2z-w2x,     1.f-x2x-y2y}}};
	return r;
}
__HINT_INLINE__ void mat3QuaternionP(mat3 *const restrict m, const quat *const restrict q){
	const float x2 = q->v.x+q->v.x;
	const float y2 = q->v.y+q->v.y;
	const float z2 = q->v.z+q->v.z;
	const float w2x = q->w*x2;
	const float w2y = q->w*y2;
	const float w2z = q->w*z2;
	const float x2x = q->v.x*x2;
	const float x2y = q->v.x*y2;
	const float x2z = q->v.x*z2;
	const float y2y = q->v.y*y2;
	const float y2z = q->v.y*z2;
	const float z2z = q->v.z*z2;
	m->m[0][0] = 1.f-y2y-z2z; m->m[0][1] = x2y+w2z;     m->m[0][2] = x2z-w2y;
	m->m[1][0] = x2y-w2z;     m->m[1][1] = 1.f-x2x-z2z; m->m[1][2] = y2z+w2x;
	m->m[2][0] = x2z+w2y;     m->m[2][1] = y2z-w2x;     m->m[2][2] = 1.f-x2x-y2y;
}
