#include "mat3.h"
#include <string.h>

__HINT_INLINE__ mat3 mat3Identity(){
	const mat3 r = {.m = {{1.f, 0.f, 0.f},
	                      {0.f, 1.f, 0.f},
	                      {0.f, 0.f, 1.f}}};
	return r;
}
__HINT_INLINE__ void mat3IdentityP(mat3 *const __RESTRICT__ m){
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
__HINT_INLINE__ void mat3ZeroP(mat3 *const __RESTRICT__ m){
	memset(m->m, 0, sizeof(mat3));
}

__HINT_INLINE__ mat3 mat3DiagonalV(const vec3 v){
	const mat3 r = {.m = {{v.x, 0.f, 0.f},
	                      {0.f, v.y, 0.f},
	                      {0.f, 0.f, v.z}}};
	return r;
}
__HINT_INLINE__ void mat3DiagonalVP(mat3 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ v){
	memset(m->m, 0, sizeof(mat3));
	m->m[0][0] = v->x;
	m->m[1][1] = v->y;
	m->m[2][2] = v->z;
}
__HINT_INLINE__ mat3 mat3DiagonalS(const float s){
	const mat3 r = {.m = {{s, 0.f, 0.f},
	                      {0.f, s, 0.f},
	                      {0.f, 0.f, s}}};
	return r;
}
__HINT_INLINE__ void mat3DiagonalSP(mat3 *const __RESTRICT__ m, const float s){
	memset(m->m, 0, sizeof(mat3));
	m->m[0][0] = s;
	m->m[1][1] = s;
	m->m[2][2] = s;
}
__HINT_INLINE__ mat3 mat3DiagonalN(const float x, const float y, const float z){
	const mat3 r = {.m = {{x, 0.f, 0.f},
	                      {0.f, y, 0.f},
	                      {0.f, 0.f, z}}};
	return r;
}
__HINT_INLINE__ void mat3DiagonalNP(mat3 *const __RESTRICT__ m, const float x, const float y, const float z){
	memset(m->m, 0, sizeof(mat3));
	m->m[0][0] = x;
	m->m[1][1] = y;
	m->m[2][2] = z;
}

__HINT_INLINE__ mat3 mat3SkewSymmetric(const vec3 v){
	const mat3 r = {.m = {{0.f, -v.z,  v.y},
	                      {v.z,  0.f, -v.x},
	                      {-v.y, v.x,  0.f}}};
	return r;
}
__HINT_INLINE__ void mat3SkewSymmetricP(mat3 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ v){
	memset(m->m, 0, sizeof(mat3));
	m->m[0][1] = -v->z;
	m->m[0][2] = v->y;
	m->m[1][0] = v->z;
	m->m[1][2] = -v->x;
	m->m[2][0] = -v->y;
	m->m[2][1] = v->x;
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
__HINT_INLINE__ void mat3MMultMP1(mat3 *const __RESTRICT__ m1, const mat3 *const __RESTRICT__ m2){

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
__HINT_INLINE__ void mat3MMultMP2(const mat3 *const __RESTRICT__ m1, mat3 *const __RESTRICT__ m2){

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
__HINT_INLINE__ void mat3MMultMPR(const mat3 *const __RESTRICT__ m1, const mat3 *const __RESTRICT__ m2, mat3 *const __RESTRICT__ r){

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
__HINT_INLINE__ vec3 mat3VMultM(const vec3 v, const mat3 m){
	const vec3 r = {.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2],
	                .y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2],
	                .z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2]};
	return r;
}
__HINT_INLINE__ void mat3VMultMP(vec3 *const __RESTRICT__ v, const mat3 *const __RESTRICT__ m){
	const vec3 r = {.x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2],
	                .y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2],
	                .z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2]};
	*v = r;
}
__HINT_INLINE__ void mat3VMultMPR(const vec3 *const __RESTRICT__ v, const mat3 *const __RESTRICT__ m, vec3 *const __RESTRICT__ r){
	r->x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2];
	r->y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2];
	r->z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2];
}
__HINT_INLINE__ vec3 mat3MMultV(const mat3 m, const vec3 v){
	const vec3 r = {.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
	                .y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
	                .z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]};
	return r;
}
__HINT_INLINE__ void mat3MMultVP(const mat3 *const __RESTRICT__ m, vec3 *const __RESTRICT__ v){
	const vec3 r = {.x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0],
	                .y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1],
	                .z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2]};
	*v = r;
}
__HINT_INLINE__ void mat3MMultVPR(const mat3 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){
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
__HINT_INLINE__ void mat3MAddMP(mat3 *const __RESTRICT__ m1, const mat3 *const __RESTRICT__ m2){
	m1->m[0][0] += m2->m[0][0]; m1->m[0][1] += m2->m[0][1]; m1->m[0][2] += m2->m[0][2];
	m1->m[1][0] += m2->m[1][0]; m1->m[1][1] += m2->m[1][1]; m1->m[1][2] += m2->m[1][2];
	m1->m[2][0] += m2->m[2][0]; m1->m[2][1] += m2->m[2][1]; m1->m[2][2] += m2->m[2][2];
}
__HINT_INLINE__ void mat3MAddMPR(const mat3 *const __RESTRICT__ m1, const mat3 *const __RESTRICT__ m2, mat3 *const __RESTRICT__ r){
	r->m[0][0] = m1->m[0][0] + m2->m[0][0]; r->m[0][1] = m1->m[0][1] + m2->m[0][1]; r->m[0][2] = m1->m[0][2] + m2->m[0][2];
	r->m[1][0] = m1->m[1][0] + m2->m[1][0]; r->m[1][1] = m1->m[1][1] + m2->m[1][1]; r->m[1][2] = m1->m[1][2] + m2->m[1][2];
	r->m[2][0] = m1->m[2][0] + m2->m[2][0]; r->m[2][1] = m1->m[2][1] + m2->m[2][1]; r->m[2][2] = m1->m[2][2] + m2->m[2][2];
}

__HINT_INLINE__ float mat3Determinant(const mat3 m){
	return m.m[0][0] * (m.m[1][1]*m.m[2][2] - m.m[1][2]*m.m[2][1]) -
	       m.m[0][1] * (m.m[1][0]*m.m[2][2] - m.m[1][2]*m.m[2][0]) +
	       m.m[0][2] * (m.m[1][0]*m.m[2][1] - m.m[1][1]*m.m[2][0]);
}
__HINT_INLINE__ float mat3DeterminantP(const mat3 *const __RESTRICT__ m){
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
__HINT_INLINE__ void mat3TransposeP(mat3 *const __RESTRICT__ m){
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

__HINT_INLINE__ void mat3TransposePR(const mat3 *const __RESTRICT__ m, mat3 *const __RESTRICT__ r){
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
return_t mat3InvertR(const mat3 m, mat3 *const __RESTRICT__ r){

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
__HINT_INLINE__ return_t mat3InvertP(mat3 *const __RESTRICT__ m){

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
__HINT_INLINE__ return_t mat3InvertPR(const mat3 *const __RESTRICT__ m, mat3 *const __RESTRICT__ r){

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

__HINT_INLINE__ vec3 mat3Solve(const mat3 A, const vec3 b){

	// Solves Ax = b using Cramer's rule.
	// Cramer's rule states that
	//     x_1 = det(A_1) / det(A)
	//     x_2 = det(A_2) / det(A)
	//     x_3 = det(A_3) / det(A)
	// for matrices A_1, A_2, A_3 that are
	// the matrix A with the first, second
	// and third columns replaced with the
	// solution vector b.
	// If the determinant of A is zero,
	// Cramer's rule does not apply.

	float invDetA = mat3Determinant(A);

	if(invDetA != 0.f){

		vec3 r; mat3 A_b;
		invDetA = 1.f / invDetA;

		memcpy(A_b.m[0], &b, sizeof(vec3));
		memcpy(A_b.m[1], A.m[1], sizeof(vec3)+sizeof(vec3));
		r.x = mat3Determinant(A_b) * invDetA;

		memcpy(A_b.m[0], A.m[0], sizeof(vec3));
		memcpy(A_b.m[1], &b, sizeof(vec3));
		r.y = mat3Determinant(A_b) * invDetA;

		memcpy(A_b.m[1], A.m[1], sizeof(vec3));
		memcpy(A_b.m[2], &b, sizeof(vec3));
		r.z = mat3Determinant(A_b) * invDetA;

		return r;

	}

	return vec3Zero();

}

__HINT_INLINE__ return_t mat3SolveR(const mat3 A, const vec3 b, vec3 *const __RESTRICT__ r){

	// Solves Ax = b using Cramer's rule.
	// Cramer's rule states that
	//     x_1 = det(A_1) / det(A)
	//     x_2 = det(A_2) / det(A)
	//     x_3 = det(A_3) / det(A)
	// for matrices A_1, A_2, A_3 that are
	// the matrix A with the first, second
	// and third columns replaced with the
	// solution vector b.
	// If the determinant of A is zero,
	// Cramer's rule does not apply.

	float invDetA = mat3Determinant(A);

	if(invDetA != 0.f){

		mat3 A_b;
		invDetA = 1.f / invDetA;

		memcpy(A_b.m[0], &b, sizeof(vec3));
		memcpy(A_b.m[1], A.m[1], sizeof(vec3)+sizeof(vec3));
		r->x = mat3Determinant(A_b) * invDetA;

		memcpy(A_b.m[0], A.m[0], sizeof(vec3));
		memcpy(A_b.m[1], &b, sizeof(vec3));
		r->y = mat3Determinant(A_b) * invDetA;

		memcpy(A_b.m[1], A.m[1], sizeof(vec3));
		memcpy(A_b.m[2], &b, sizeof(vec3));
		r->z = mat3Determinant(A_b) * invDetA;

		return 1;

	}

	return 0;

}

__HINT_INLINE__ return_t mat3SolvePR(const mat3 *const __RESTRICT__ A, const vec3 *const __RESTRICT__ b, vec3 *const __RESTRICT__ r){

	// Solves Ax = b using Cramer's rule.
	// Cramer's rule states that
	//     x_1 = det(A_1) / det(A)
	//     x_2 = det(A_2) / det(A)
	//     x_3 = det(A_3) / det(A)
	// for matrices A_1, A_2, A_3 that are
	// the matrix A with the first, second
	// and third columns replaced with the
	// solution vector b.
	// If the determinant of A is zero,
	// Cramer's rule does not apply.

	float invDetA = mat3DeterminantP(A);

	if(invDetA != 0.f){

		mat3 A_b;
		invDetA = 1.f / invDetA;

		memcpy(A_b.m[0], b, sizeof(vec3));
		memcpy(A_b.m[1], A->m[1], sizeof(vec3)+sizeof(vec3));
		r->x = mat3DeterminantP(&A_b) * invDetA;

		memcpy(A_b.m[0], A->m[0], sizeof(vec3));
		memcpy(A_b.m[1], b, sizeof(vec3));
		r->y = mat3DeterminantP(&A_b) * invDetA;

		memcpy(A_b.m[1], A->m[1], sizeof(vec3));
		memcpy(A_b.m[2], b, sizeof(vec3));
		r->z = mat3DeterminantP(&A_b) * invDetA;

		return 1;

	}

	return 0;

}

__HINT_INLINE__ mat3 mat3Quaternion(const quat q){
	const float x2 = 2.f*q.v.x;
	const float y2 = 2.f*q.v.y;
	const float z2 = 2.f*q.v.z;
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
__HINT_INLINE__ void mat3QuaternionP(mat3 *const __RESTRICT__ m, const quat *const __RESTRICT__ q){
	const float x2 = 2.f*q->v.x;
	const float y2 = 2.f*q->v.y;
	const float z2 = 2.f*q->v.z;
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