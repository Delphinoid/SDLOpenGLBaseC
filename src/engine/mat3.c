#include "mat3.h"
#include "quat.h"
#include "helpersMath.h"
#include <string.h>

#define MAT3_EPSILON 0.000001f

#define TWO_PI_ON_THREE 2.09439510239319549f

mat3 g_mat3Identity = {
	.m = {{1.f, 0.f, 0.f},
	      {0.f, 1.f, 0.f},
	      {0.f, 0.f, 1.f}}
};
mat3 g_mat3Zero = {
	.m = {{0.f, 0.f, 0.f},
	      {0.f, 0.f, 0.f},
	      {0.f, 0.f, 0.f}}
};

__HINT_INLINE__ void mat3IdentityP(mat3 *const __RESTRICT__ m){
	*m = g_mat3Identity;
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
	const float f1 = m.m[2][1] * m.m[0][2] - m.m[0][1] * m.m[2][2];
	const float f2 = m.m[0][1] * m.m[1][2] - m.m[1][1] * m.m[0][2];
	float invDet = m.m[0][0] * f0 + m.m[1][0] * f1 + m.m[2][0] * f2;

	if(invDet != 0.f){

		mat3 r;
		invDet = 1.f / invDet;

		r.m[0][0] = f0 * invDet;
		r.m[0][1] = f1 * invDet;
		r.m[0][2] = f2 * invDet;
		r.m[1][0] = (m.m[2][0] * m.m[1][2] - m.m[1][0] * m.m[2][2]) * invDet;
		r.m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[2][0] * m.m[0][2]) * invDet;
		r.m[1][2] = (m.m[0][2] * m.m[1][0] - m.m[0][0] * m.m[1][2]) * invDet;
		r.m[2][0] = (m.m[1][0] * m.m[2][1] - m.m[2][0] * m.m[1][1]) * invDet;
		r.m[2][1] = (m.m[0][1] * m.m[2][0] - m.m[0][0] * m.m[2][1]) * invDet;
		r.m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]) * invDet;

		return r;

	}

	return g_mat3Zero;

}
return_t mat3InvertR(const mat3 m, mat3 *const __RESTRICT__ r){

	const float f0 = m.m[1][1] * m.m[2][2] - m.m[2][1] * m.m[1][2];
	const float f1 = m.m[2][1] * m.m[0][2] - m.m[0][1] * m.m[2][2];
	const float f2 = m.m[0][1] * m.m[1][2] - m.m[1][1] * m.m[0][2];
	float invDet = m.m[0][0] * f0 + m.m[1][0] * f1 + m.m[2][0] * f2;

	if(invDet != 0.f){

		mat3 t;
		invDet = 1.f / invDet;

		t.m[0][0] = f0 * invDet;
		t.m[0][1] = f1 * invDet;
		t.m[0][2] = f2 * invDet;
		t.m[1][0] = (m.m[2][0] * m.m[1][2] - m.m[1][0] * m.m[2][2]) * invDet;
		t.m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[2][0] * m.m[0][2]) * invDet;
		t.m[1][2] = (m.m[0][2] * m.m[1][0] - m.m[0][0] * m.m[1][2]) * invDet;
		t.m[2][0] = (m.m[1][0] * m.m[2][1] - m.m[2][0] * m.m[1][1]) * invDet;
		t.m[2][1] = (m.m[0][1] * m.m[2][0] - m.m[0][0] * m.m[2][1]) * invDet;
		t.m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]) * invDet;

		*r = t;
		return 1;

	}

	return 0;

}
__HINT_INLINE__ return_t mat3InvertP(mat3 *const __RESTRICT__ m){

	const float f0 = m->m[1][1] * m->m[2][2] - m->m[2][1] * m->m[1][2];
	const float f1 = m->m[2][1] * m->m[0][2] - m->m[0][1] * m->m[2][2];
	const float f2 = m->m[0][1] * m->m[1][2] - m->m[1][1] * m->m[0][2];
	float invDet = m->m[0][0] * f0 + m->m[1][0] * f1 + m->m[2][0] * f2;

	if(invDet != 0.f){

		const mat3 temp = *m;
		invDet = 1.f / invDet;

		m->m[0][0] = f0 * invDet;
		m->m[0][1] = f1 * invDet;
		m->m[0][2] = f2 * invDet;
		m->m[1][0] = (temp.m[2][0] * temp.m[1][2] - temp.m[1][0] * temp.m[2][2]) * invDet;
		m->m[1][1] = (temp.m[0][0] * temp.m[2][2] - temp.m[2][0] * temp.m[0][2]) * invDet;
		m->m[1][2] = (temp.m[0][2] * temp.m[1][0] - temp.m[0][0] * temp.m[1][2]) * invDet;
		m->m[2][0] = (temp.m[1][0] * temp.m[2][1] - temp.m[2][0] * temp.m[1][1]) * invDet;
		m->m[2][1] = (temp.m[0][1] * temp.m[2][0] - temp.m[0][0] * temp.m[2][1]) * invDet;
		m->m[2][2] = (temp.m[0][0] * temp.m[1][1] - temp.m[0][1] * temp.m[1][0]) * invDet;

		return 1;

	}

	return 0;

}
__HINT_INLINE__ return_t mat3InvertPR(const mat3 *const __RESTRICT__ m, mat3 *const __RESTRICT__ r){

	const float f0 = m->m[1][1] * m->m[2][2] - m->m[2][1] * m->m[1][2];
	const float f1 = m->m[2][1] * m->m[0][2] - m->m[0][1] * m->m[2][2];
	const float f2 = m->m[0][1] * m->m[1][2] - m->m[1][1] * m->m[0][2];
	float invDet = m->m[0][0] * f0 + m->m[1][0] * f1 + m->m[2][0] * f2;

	if(invDet != 0.f){

		const mat3 temp = *m;
		invDet = 1.f / invDet;

		r->m[0][0] = f0 * invDet;
		r->m[0][1] = f1 * invDet;
		r->m[0][2] = f2 * invDet;
		r->m[1][0] = (temp.m[2][0] * temp.m[1][2] - temp.m[1][0] * temp.m[2][2]) * invDet;
		r->m[1][1] = (temp.m[0][0] * temp.m[2][2] - temp.m[2][0] * temp.m[0][2]) * invDet;
		r->m[1][2] = (temp.m[0][2] * temp.m[1][0] - temp.m[0][0] * temp.m[1][2]) * invDet;
		r->m[2][0] = (temp.m[1][0] * temp.m[2][1] - temp.m[2][0] * temp.m[1][1]) * invDet;
		r->m[2][1] = (temp.m[0][1] * temp.m[2][0] - temp.m[0][0] * temp.m[2][1]) * invDet;
		r->m[2][2] = (temp.m[0][0] * temp.m[1][1] - temp.m[0][1] * temp.m[1][0]) * invDet;

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
	const float x2 = 2.f*q.x;
	const float y2 = 2.f*q.y;
	const float z2 = 2.f*q.z;
	const float w2x = q.w*x2;
	const float w2y = q.w*y2;
	const float w2z = q.w*z2;
	const float x2x = q.x*x2;
	const float x2y = q.x*y2;
	const float x2z = q.x*z2;
	const float y2y = q.y*y2;
	const float y2z = q.y*z2;
	const float z2z = q.z*z2;
	const mat3 r = {.m = {{1.f-y2y-z2z, x2y+w2z,     x2z-w2y},
	                      {x2y-w2z,     1.f-x2x-z2z, y2z+w2x},
	                      {x2z+w2y,     y2z-w2x,     1.f-x2x-y2y}}};
	return r;
}
__HINT_INLINE__ void mat3QuaternionP(mat3 *const __RESTRICT__ m, const quat *const __RESTRICT__ q){
	const float x2 = 2.f*q->x;
	const float y2 = 2.f*q->y;
	const float z2 = 2.f*q->z;
	const float w2x = q->w*x2;
	const float w2y = q->w*y2;
	const float w2z = q->w*z2;
	const float x2x = q->x*x2;
	const float x2y = q->x*y2;
	const float x2z = q->x*z2;
	const float y2y = q->y*y2;
	const float y2z = q->y*z2;
	const float z2z = q->z*z2;
	m->m[0][0] = 1.f-y2y-z2z; m->m[0][1] = x2y+w2z;     m->m[0][2] = x2z-w2y;
	m->m[1][0] = x2y-w2z;     m->m[1][1] = 1.f-x2x-z2z; m->m[1][2] = y2z+w2x;
	m->m[2][0] = x2z+w2y;     m->m[2][1] = y2z-w2x;     m->m[2][2] = 1.f-x2x-y2y;
}

__HINT_INLINE__ quat quatMat3(const mat3 m){
	// Convert a 3x3 matrix to a quaternion.
	// For this to work, we assume that m is a special orthogonal matrix.
	// Implementation from Mike Day's "Converting a Rotation Matrix to a Quaternion".
	quat r;
	float t;
	if(m.m[2][2] < 0.f){
		if(m.m[0][0] > m.m[1][1]){
			t = m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.f;
			r = quatNew(m.m[1][2]-m.m[2][1], t, m.m[0][1]+m.m[1][0], m.m[2][0]+m.m[0][2]);
		}else{
			t = -m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.f;
			r = quatNew(m.m[2][0]-m.m[0][2], m.m[0][1]+m.m[1][0], t, m.m[1][2]+m.m[2][1]);
		}
	}else{
		if(m.m[0][0] < -m.m[1][1]){
			t = -m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.f;
			r = quatNew(m.m[0][1]-m.m[1][0], m.m[2][0]+m.m[0][2], m.m[1][2]+m.m[2][1], t);
		}else{
			t = m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.f;
			r = quatNew(t, m.m[1][2]-m.m[2][1], m.m[2][0]-m.m[0][2], m.m[0][1]-m.m[1][0]);
		}
	}
	return quatQMultS(r, 0.5f*rsqrtAccurate(t));
}
__HINT_INLINE__ void quatMat3PR(const mat3 *const __RESTRICT__ m, quat *const __RESTRICT__ r){
	// Convert a 3x3 matrix to a quaternion.
	// For this to work, we assume that m is a special orthogonal matrix.
	// Implementation from Mike Day's "Converting a Rotation Matrix to a Quaternion".
	float t;
	if(m->m[2][2] < 0.f){
		if(m->m[0][0] > m->m[1][1]){
			t = m->m[0][0] - m->m[1][1] - m->m[2][2] + 1.f;
			quatSet(r, m->m[1][2]-m->m[2][1], t, m->m[0][1]+m->m[1][0], m->m[2][0]+m->m[0][2]);
		}else{
			t = -m->m[0][0] + m->m[1][1] - m->m[2][2] + 1.f;
			quatSet(r, m->m[2][0]-m->m[0][2], m->m[0][1]+m->m[1][0], t, m->m[1][2]+m->m[2][1]);
		}
	}else{
		if(m->m[0][0] < -m->m[1][1]){
			t = -m->m[0][0] - m->m[1][1] + m->m[2][2] + 1.f;
			quatSet(r, m->m[0][1]-m->m[1][0], m->m[2][0]+m->m[0][2], m->m[1][2]+m->m[2][1], t);
		}else{
			t = m->m[0][0] + m->m[1][1] + m->m[2][2] + 1.f;
			quatSet(r, t, m->m[1][2]-m->m[2][1], m->m[2][0]-m->m[0][2], m->m[0][1]-m->m[1][0]);
		}
	}
	quatQMultSP(r, 0.5f*rsqrtAccurate(t));
}

__HINT_INLINE__ mat3 mat3ShearMatrix(const quat q, const vec3 s){

	// Given a quaternion q with matrix Q and a scale matrix S with
	// diagonal s, determine the corresponding shear matrix m = QSQ^T.

	mat3 r;

	// Convert the quaternion to a rotation matrix.
	const mat3 m = mat3Quaternion(q);

	float cx = s.x*m.m[0][0];
	float cy = s.y*m.m[1][0];
	float cz = s.z*m.m[2][0];

	r.m[0][0] = m.m[0][0]*cx + m.m[1][0]*cy + m.m[2][0]*cz;
	r.m[0][1] = m.m[0][1]*cx + m.m[1][1]*cy + m.m[2][1]*cz;
	r.m[0][2] = m.m[0][2]*cx + m.m[1][2]*cy + m.m[2][2]*cz;
	cx = s.x*m.m[0][1];
	cy = s.y*m.m[1][1];
	cz = s.z*m.m[2][1];
	r.m[1][0] = r.m[0][1];
	r.m[1][1] = m.m[0][1]*cx + m.m[1][1]*cy + m.m[2][1]*cz;
	r.m[1][2] = m.m[0][2]*cx + m.m[1][2]*cy + m.m[2][2]*cz;
	r.m[2][0] = r.m[0][2];
	r.m[2][1] = r.m[1][2];
	r.m[2][2] = m.m[0][2]*s.x*m.m[0][2] + m.m[1][2]*s.y*m.m[1][2] + m.m[2][2]*s.z*m.m[2][2];

	return r;

}
__HINT_INLINE__ void mat3ShearMatrixPR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ s, mat3 *const __RESTRICT__ r){

	// Given a quaternion q with matrix Q and a scale matrix S with
	// diagonal s, determine the corresponding shear matrix m = QSQ^T.

	// Convert the quaternion to a rotation matrix.
	mat3 m;
	mat3QuaternionP(&m, q);

	float cx = s->x*m.m[0][0];
	float cy = s->y*m.m[1][0];
	float cz = s->z*m.m[2][0];

	r->m[0][0] = m.m[0][0]*cx + m.m[1][0]*cy + m.m[2][0]*cz;
	r->m[0][1] = m.m[0][1]*cx + m.m[1][1]*cy + m.m[2][1]*cz;
	r->m[0][2] = m.m[0][2]*cx + m.m[1][2]*cy + m.m[2][2]*cz;
	cx = s->x*m.m[0][1];
	cy = s->y*m.m[1][1];
	cz = s->z*m.m[2][1];
	r->m[1][0] = r->m[0][1];
	r->m[1][1] = m.m[0][1]*cx + m.m[1][1]*cy + m.m[2][1]*cz;
	r->m[1][2] = m.m[0][2]*cx + m.m[1][2]*cy + m.m[2][2]*cz;
	r->m[2][0] = r->m[0][2];
	r->m[2][1] = r->m[1][2];
	r->m[2][2] = m.m[0][2]*s->x*m.m[0][2] + m.m[1][2]*s->y*m.m[1][2] + m.m[2][2]*s->z*m.m[2][2];

}

static __HINT_INLINE__ void ComputeEigenvector0(
	const float a00, const float a01, const float a02,
	const float a11, const float a12, const float a22,
	const float eval0, vec3 *const evec0
){
	// Compute a normalized eigenvector for eigenvalue 0.
	// We know that (A - eval0*I) has row rank 2, but we don't know which two rows
	// are linearly independent. To get the best results for the widest range of
	// inputs, we use the two rows whose cross product has the greatest magnitude.
	const vec3 r0 = {.x = a00-eval0, .y = a01,       .z = a02      };
	const vec3 r1 = {.x = a01,       .y = a11-eval0, .z = a12      };
	const vec3 r2 = {.x = a02,       .y = a12,       .z = a22-eval0};
	const vec3 r01 = vec3Cross(r0, r1);
	const vec3 r12 = vec3Cross(r1, r2);
	const vec3 r20 = vec3Cross(r2, r0);
	const float d01 = vec3Dot(r01, r01);
	const float d12 = vec3Dot(r12, r12);
	const float d20 = vec3Dot(r20, r20);
	if(d01 >= d12 && d01 >= d20){
		*evec0 = vec3VMultS(r01, rsqrtAccurate(d01));
	}else if(d12 >= d20){
		*evec0 = vec3VMultS(r12, rsqrtAccurate(d12));
	}else{
		*evec0 = vec3VMultS(r20, rsqrtAccurate(d20));
	}
}

static __HINT_INLINE__ void ComputeEigenvector1(
	const float a00, const float a01, const float a02,
	const float a11, const float a12, const float a22,
	const vec3 evec0, const float eval1, vec3 *const evec1
){

	// Compute a normalized eigenvector for eigenvalue 1 that
	// is orthogonal to the one computed for eigenvalue 0.

	vec3 u, v;
	// Because A is symmetric, it is orthogonally diagonalizable,
	// so we can choose two eigenvectors to be the orthogonal, even
	// if their eigenvalues are the same. Here, u and v define the
	// orthogonal complement of evec0, so evec1 is of the form
	//     evec1 = x_0 u + x_1 v,
	// where x_0^2 + x_1^2 = 1 since u, v, evec1 are unit length.
	vec3OrthonormalBasis(evec0, &u, &v);

	// Using the above notation, suppose we define
	//     x = (x_0, x_1)^T,
	//     J = [u, v],
	//     M = J^T*(A - eval1*I)*J.
	// Then evec1 = Jx and 0 = (A - eval1*I)*evec1 = Mx.
	// Here, M is a 2x2 matrix. If eval0 = eval1, then M
	// has rank 0, and we can choose any valid x for evec1.
	// Otherwise, it has rank 1, and x is easily solvable.
	{

		const vec3 Au = {
			.x = a00*u.x + a01*u.y + a02*u.z,
			.y = a01*u.x + a11*u.y + a12*u.z,
			.z = a02*u.x + a12*u.y + a22*u.z
		};
		const vec3 Av = {
			.x = a00*v.x + a01*v.y + a02*v.z,
			.y = a01*v.x + a11*v.y + a12*v.z,
			.z = a02*v.x + a12*v.y + a22*v.z
		};
		float m00 = vec3Dot(u, Au) - eval1;
		float m01 = vec3Dot(u, Av);
		float m11 = vec3Dot(v, Av) - eval1;

		// The eigenvector is orthogonal to the (non-zero) row of M.
		// Choose the row with the greatest magnitude for the best results.
		const float m00abs = fabsf(m00);
		const float m01abs = fabsf(m01);
		const float m11abs = fabsf(m11);
		if(m00abs >= m11abs){
			if(m00abs > MAT3_EPSILON || m01abs > MAT3_EPSILON){
				if(m00abs >= m01abs){
					m01 /= m00;
					m00 = 1.f/sqrtf(1.f + m01*m01);
					m01 *= m00;
				}else{
					m00 /= m01;
					m01 = 1.f/sqrtf(1.f + m00*m00);
					m00 *= m01;
				}
				evec1->x = m01*u.x - m00*v.x;
				evec1->y = m01*u.y - m00*v.y;
				evec1->z = m01*u.z - m00*v.z;
			}else{
				*evec1 = u;
			}
		}else{
			if(m11abs > MAT3_EPSILON || m01abs > MAT3_EPSILON){
				if(m11abs >= m01abs){
					m01 /= m11;
					m11 = 1.f/sqrtf(1.f + m01*m01);
					m01 *= m11;
				}else{
					m11 /= m01;
					m01 = 1.f/sqrtf(1.f + m11*m11);
					m11 *= m01;
				}
				evec1->x = m11*u.x - m01*v.x;
				evec1->y = m11*u.y - m01*v.y;
				evec1->z = m11*u.z - m01*v.z;
			}else{
				*evec1 = u;
			}
		}

	}

}

void mat3DiagonalizeSymmetric(
	float a00, float a01, float a02, float a11, float a12, float a22,
	vec3 *const __RESTRICT__ evals, quat *const __RESTRICT__ Q
){

	// Diagonalize a symmetric 3x3 matrix A and return its eigenvalues
	// and eigenvectors. Because our input matrix is symmetric, we need
	// only specify the unique values.
	//
	// Rather than using an approximate iterative algorithm, we compute
	// an exact analytic solution. Benchmarks seem to indicate that this
	// is significantly faster and more accurate for the 3x3 case.
	//
	// This is used for non-uniform scaling in our affine transformation
	// structure. Shears are stored as QSQ^T, where Q is a pure rotation
	// and S is a scale along the x, y and z axes. This function returns
	// Q as a quaternion and S as a vector.
	//
	// Special thanks to David Eberly for this implementation, which was
	// given in "A Robust Eigensolver for 3x3 Symmetric Matrices" (2014).

	// Factor out the maximum absolute value of the matrix entries to
	// prevent floating-point overflow when computing the eigenvalues.
	const float max0 = floatMax(fabsf(a00), fabsf(a01));
	const float max1 = floatMax(fabsf(a02), fabsf(a11));
	const float max2 = floatMax(fabsf(a12), fabsf(a22));
	const float maxAbsElement = floatMax(floatMax(max0, max1), max2);

	// If the maximum is 0, A is the zero matrix.
	if(maxAbsElement <= MAT3_EPSILON){
		evals->x = 0.f;
		evals->y = 0.f;
		evals->z = 0.f;
		quatIdentityP(Q);
	}else{

		const float invMaxAbsElement = 1.f/maxAbsElement;

		a00 *= invMaxAbsElement;
		a01 *= invMaxAbsElement;
		a02 *= invMaxAbsElement;
		a11 *= invMaxAbsElement;
		a12 *= invMaxAbsElement;
		a22 *= invMaxAbsElement;

		{
			const float norm = a01*a01 + a02*a02 + a12*a12;

			// Early exit if A is diagonal.
			if(norm <= MAT3_EPSILON){
				evals->x = a00;
				evals->y = a11;
				evals->z = a22;
				quatIdentityP(Q);
			}else{

				mat3 Qmat;

				// q = tr(A)/3
				const float q = (a00 + a11 + a22)/3.f;
				// B = (A - qI)/p
				const float b00 = a00 - q;
				const float b11 = a11 - q;
				const float b22 = a22 - q;
				// p = sqrt(tr((A - qI)^2)/6)
				const float p = sqrtf((b00*b00 + b11*b11 + b22*b22 + 2.f*norm)/6.f);

				const float c00 = b11*b22 - a12*a12;
				const float c01 = a01*b22 - a12*a02;
				const float c02 = a01*a12 - b11*a02;
				// Compute the determinant of B. When we calculated the diagonals
				// of B, we did not divide by p: that's why we divide by p^3 here.
				const float det = (b00*c00 - a01*c01 + a02*c02)/(p*p*p);

				// Recall that det(B)/2 = cos(3theta). Because det(B)/2 may
				// lie outside the domain of acos, that is [-1, 1], due to
				// rounding errors, we clamp it to this interval.
				const float halfDet = floatClamp(0.5f*det, -1.f, 1.f);

				// Compute the (increasing) ordered eigenvalues of B.
				const float angle = acosf(halfDet)/3.f;
				const float beta2 = 2.f*cosf(angle);
				const float beta0 = 2.f*cosf(angle + TWO_PI_ON_THREE);
				const float beta1 = -(beta0 + beta2);
				// Compute the (increasing) ordered eigenvalues of A.
				evals->x = q + p*beta0;
				evals->y = q + p*beta1;
				evals->z = q + p*beta2;

				// Compute the eigenvectors so that they form
				// an ordered, right-handed, orthonormal set.
				if(halfDet >= 0.f){
					ComputeEigenvector0(a00, a01, a02, a11, a12, a22, evals->z, (vec3 *)Qmat.m[2]);
					ComputeEigenvector1(a00, a01, a02, a11, a12, a22, *((vec3 *)Qmat.m[2]), evals->y, (vec3 *)Qmat.m[1]);
					*((vec3 *)Qmat.m[0]) = vec3Cross(*((vec3 *)Qmat.m[1]), *((vec3 *)Qmat.m[2]));
				}else{
					ComputeEigenvector0(a00, a01, a02, a11, a12, a22, evals->x, (vec3 *)Qmat.m[0]);
					ComputeEigenvector1(a00, a01, a02, a11, a12, a22, *((vec3 *)Qmat.m[0]), evals->y, (vec3 *)Qmat.m[1]);
					*((vec3 *)Qmat.m[2]) = vec3Cross(*((vec3 *)Qmat.m[0]), *((vec3 *)Qmat.m[1]));
				}

				// Convert the matrix for Q to a quaternion.
				*Q = quatMat3(Qmat);

			}

		}

		// Undo the preconditioning.
		evals->x *= maxAbsElement;
		evals->y *= maxAbsElement;
		evals->z *= maxAbsElement;

	}

}