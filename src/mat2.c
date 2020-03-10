#include "mat2.h"
#include <string.h>

__HINT_INLINE__ mat2 mat2Identity(){
	const mat2 r = {.m = {{1.f, 0.f},
	                      {0.f, 1.f}}};
	return r;
}
__HINT_INLINE__ void mat2IdentityP(mat2 *const __RESTRICT__ m){
	m->m[0][0] = 1.f; m->m[0][1] = 0.f;
	m->m[1][0] = 0.f; m->m[1][1] = 1.f;
}
__HINT_INLINE__ mat2 mat2Zero(){
	const mat2 r = {.m = {{0.f, 0.f},
	                      {0.f, 0.f}}};
	return r;
}
__HINT_INLINE__ void mat2ZeroP(mat2 *const __RESTRICT__ m){
	memset(m->m, 0, sizeof(mat2));
}

__HINT_INLINE__ mat2 mat2DiagonalV(const vec2 v){
	const mat2 r = {.m = {{v.x, 0.f},
	                      {0.f, v.y}}};
	return r;
}
__HINT_INLINE__ void mat2DiagonalVP(mat2 *const __RESTRICT__ m, const vec2 *const __RESTRICT__ v){
	memset(m->m, 0, sizeof(mat2));
	m->m[0][0] = v->x;
	m->m[1][1] = v->y;
}
__HINT_INLINE__ mat2 mat2DiagonalS(const float s){
	const mat2 r = {.m = {{s, 0.f},
	                      {0.f, s}}};
	return r;
}
__HINT_INLINE__ void mat2DiagonalSP(mat2 *const __RESTRICT__ m, const float s){
	memset(m->m, 0, sizeof(mat2));
	m->m[0][0] = s;
	m->m[1][1] = s;
}
__HINT_INLINE__ mat2 mat2DiagonalN(const float x, const float y){
	const mat2 r = {.m = {{x, 0.f},
	                      {0.f, y}}};
	return r;
}
__HINT_INLINE__ void mat2DiagonalNP(mat2 *const __RESTRICT__ m, const float x, const float y){
	memset(m->m, 0, sizeof(mat2));
	m->m[0][0] = x;
	m->m[1][1] = y;
}

__HINT_INLINE__ mat2 mat2SkewSymmetric(const float s){
	const mat2 r = {.m = {{0.f, -s},
	                      {s, 0.f}}};
	return r;
}
__HINT_INLINE__ void mat2SkewSymmetricP(mat2 *const __RESTRICT__ m, const float s){
	memset(m->m, 0, sizeof(mat2));
	m->m[0][1] = -s;
	m->m[1][0] = s;
}

__HINT_INLINE__ mat2 mat2MMultM(const mat2 m1, const mat2 m2){

	const mat2 r = {.m = {{m1.m[0][0]*m2.m[0][0] + m1.m[1][0]*m2.m[0][1],
	                       m1.m[0][1]*m2.m[0][0] + m1.m[1][1]*m2.m[0][1]},
	                      {m1.m[0][0]*m2.m[1][0] + m1.m[1][0]*m2.m[1][1],
	                       m1.m[0][1]*m2.m[1][0] + m1.m[1][1]*m2.m[1][1]}}};
	return r;

}
__HINT_INLINE__ void mat2MMultMP1(mat2 *const __RESTRICT__ m1, const mat2 *const __RESTRICT__ m2){

	const mat2 r = {.m = {{m1->m[0][0]*m2->m[0][0] + m1->m[1][0]*m2->m[0][1],
	                       m1->m[0][1]*m2->m[0][0] + m1->m[1][1]*m2->m[0][1]},
	                      {m1->m[0][0]*m2->m[1][0] + m1->m[1][0]*m2->m[1][1],
	                       m1->m[0][1]*m2->m[1][0] + m1->m[1][1]*m2->m[1][1]}}};
	*m1 = r;

}
__HINT_INLINE__ void mat2MMultMP2(const mat2 *const __RESTRICT__ m1, mat2 *const __RESTRICT__ m2){

	const mat2 r = {.m = {{m1->m[0][0]*m2->m[0][0] + m1->m[1][0]*m2->m[0][1],
	                       m1->m[0][1]*m2->m[0][0] + m1->m[1][1]*m2->m[0][1]},
	                      {m1->m[0][0]*m2->m[1][0] + m1->m[1][0]*m2->m[1][1],
	                       m1->m[0][1]*m2->m[1][0] + m1->m[1][1]*m2->m[1][1]}}};
	*m2 = r;

}
__HINT_INLINE__ void mat2MMultMPR(const mat2 *const __RESTRICT__ m1, const mat2 *const __RESTRICT__ m2, mat2 *const __RESTRICT__ r){

	r->m[0][0] = m1->m[0][0]*m2->m[0][0] + m1->m[1][0]*m2->m[0][1];
	r->m[0][1] = m1->m[0][1]*m2->m[0][0] + m1->m[1][1]*m2->m[0][1];
	r->m[1][0] = m1->m[0][0]*m2->m[1][0] + m1->m[1][0]*m2->m[1][1];
	r->m[1][1] = m1->m[0][1]*m2->m[1][0] + m1->m[1][1]*m2->m[1][1];

}
__HINT_INLINE__ vec2 mat2VMultMBra(const vec2 v, const mat2 m){
	const vec2 r = {.x = v.x * m.m[0][0] + v.y * m.m[0][1],
	                .y = v.x * m.m[1][0] + v.y * m.m[1][1]};
	return r;
}
__HINT_INLINE__ void mat2VMultMBraP(vec2 *const __RESTRICT__ v, const mat2 *const __RESTRICT__ m){
	const vec2 r = {.x = v->x * m->m[0][0] + v->y * m->m[0][1],
	                .y = v->x * m->m[1][0] + v->y * m->m[1][1]};
	*v = r;
}
__HINT_INLINE__ void mat2VMultMBraPR(const vec2 *const __RESTRICT__ v, const mat2 *const __RESTRICT__ m, vec2 *const __RESTRICT__ r){
	r->x = v->x * m->m[0][0] + v->y * m->m[0][1];
	r->y = v->x * m->m[1][0] + v->y * m->m[1][1];
}
__HINT_INLINE__ vec2 mat2MMultVKet(const mat2 m, const vec2 v){
	const vec2 r = {.x = v.x * m.m[0][0] + v.y * m.m[1][0],
	                .y = v.x * m.m[0][1] + v.y * m.m[1][1]};
	return r;
}
__HINT_INLINE__ void mat2MMultVKetP(const mat2 *const __RESTRICT__ m, vec2 *const __RESTRICT__ v){
	const vec2 r = {.x = v->x * m->m[0][0] + v->y * m->m[1][0],
	                .y = v->x * m->m[0][1] + v->y * m->m[1][1]};
	*v = r;
}
__HINT_INLINE__ void mat2MMultVKetPR(const mat2 *const __RESTRICT__ m, const vec2 *const __RESTRICT__ v, vec2 *const __RESTRICT__ r){
	r->x = v->x * m->m[0][0] + v->y * m->m[1][0];
	r->y = v->x * m->m[0][1] + v->y * m->m[1][1];
}

__HINT_INLINE__ mat2 mat2MAddM(const mat2 m1, const mat2 m2){
	const mat2 r = {.m = {{m1.m[0][0] + m2.m[0][0],
	                       m1.m[0][1] + m2.m[0][1]},
	                      {m1.m[1][0] + m2.m[1][0],
	                       m1.m[1][1] + m2.m[1][1]}}};
	return r;
}
__HINT_INLINE__ void mat2MAddMP(mat2 *const __RESTRICT__ m1, const mat2 *const __RESTRICT__ m2){
	m1->m[0][0] += m2->m[0][0]; m1->m[0][1] += m2->m[0][1];
	m1->m[1][0] += m2->m[1][0]; m1->m[1][1] += m2->m[1][1];
}
__HINT_INLINE__ void mat2MAddMPR(const mat2 *const __RESTRICT__ m1, const mat2 *const __RESTRICT__ m2, mat2 *const __RESTRICT__ r){
	r->m[0][0] = m1->m[0][0] + m2->m[0][0]; r->m[0][1] = m1->m[0][1] + m2->m[0][1];
	r->m[1][0] = m1->m[1][0] + m2->m[1][0]; r->m[1][1] = m1->m[1][1] + m2->m[1][1];
}

__HINT_INLINE__ float mat2Determinant(const mat2 m){
	return m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1];
}
__HINT_INLINE__ float mat2DeterminantP(const mat2 *const __RESTRICT__ m){
	return m->m[0][0] * m->m[1][1] - m->m[1][0] * m->m[0][1];
}

__HINT_INLINE__ mat2 mat2Transpose(const mat2 m){
	const mat2 r = {.m = {{m.m[0][0], m.m[1][0]},
	                      {m.m[0][1], m.m[1][1]}}};
	return r;
}
__HINT_INLINE__ void mat2TransposeP(mat2 *const __RESTRICT__ m){
	const float swap = m->m[1][0];
	m->m[1][0] = m->m[0][1];
	m->m[0][1] = swap;
}

__HINT_INLINE__ void mat2TransposePR(const mat2 *const __RESTRICT__ m, mat2 *const __RESTRICT__ r){
	r->m[0][0] = m->m[0][0]; r->m[0][1] = m->m[1][0];
	r->m[1][0] = m->m[0][1]; r->m[1][1] = m->m[1][1];
}

__HINT_INLINE__ mat2 mat2Invert(const mat2 m){
	float invDet = mat2Determinant(m);
	if(invDet != 0.f){
		mat2 r;
		invDet = 1.f / invDet;
		r.m[0][0] = m.m[1][1] * invDet;
		r.m[0][1] = m.m[0][1] * -invDet;
		r.m[1][0] = m.m[1][0] * -invDet;
		r.m[1][1] = m.m[0][0] * invDet;
		return r;
	}
	return mat2Zero();
}
__HINT_INLINE__ return_t mat2InvertR(const mat2 m, mat2 *const __RESTRICT__ r){
	float invDet = mat2Determinant(m);
	if(invDet != 0.f){
		mat2 t;
		invDet = 1.f / invDet;
		t.m[0][0] = m.m[1][1] * invDet;
		t.m[0][1] = m.m[0][1] * -invDet;
		t.m[1][0] = m.m[1][0] * -invDet;
		t.m[1][1] = m.m[0][0] * invDet;
		*r = t;
		return 1;
	}
	return 0;
}
__HINT_INLINE__ return_t mat2InvertP(mat2 *const __RESTRICT__ m){
	float invDet = mat2DeterminantP(m);
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
__HINT_INLINE__ return_t mat2InvertPR(const mat2 *const __RESTRICT__ m, mat2 *const __RESTRICT__ r){
	float invDet = mat2DeterminantP(m);
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

__HINT_INLINE__ vec2 mat2Solve(const mat2 A, const vec2 b){

	// Solves Ax = b using Cramer's rule.
	// Cramer's rule states that
	//     x_1 = det(A_1) / det(A)
	//     x_2 = det(A_2) / det(A)
	// for matrices A_1, A_2 that are the
	// matrix A with the first and second
	// columns replaced with the solution
	// vector b.
	// If the determinant of A is zero,
	// Cramer's rule does not apply.

	float invDetA = mat2Determinant(A);

	if(invDetA != 0.f){

		vec2 r; mat2 A_b;
		invDetA = 1.f / invDetA;

		memcpy(A_b.m[0], &b, sizeof(vec2));
		memcpy(A_b.m[1], A.m[1], sizeof(vec2));
		r.x = mat2Determinant(A_b) * invDetA;

		memcpy(A_b.m[0], A.m[0], sizeof(vec2));
		memcpy(A_b.m[1], &b, sizeof(vec2));
		r.y = mat2Determinant(A_b) * invDetA;

		return r;

	}

	return vec2Zero();

}

__HINT_INLINE__ return_t mat2SolveR(const mat2 A, const vec2 b, vec2 *const __RESTRICT__ r){

	// Solves Ax = b using Cramer's rule.
	// Cramer's rule states that
	//     x_1 = det(A_1) / det(A)
	//     x_2 = det(A_2) / det(A)
	// for matrices A_1, A_2 that are the
	// matrix A with the first and second
	// columns replaced with the solution
	// vector b.
	// If the determinant of A is zero,
	// Cramer's rule does not apply.

	float invDetA = mat2Determinant(A);

	if(invDetA != 0.f){

		mat2 A_b;
		invDetA = 1.f / invDetA;

		memcpy(A_b.m[0], &b, sizeof(vec2));
		memcpy(A_b.m[1], A.m[1], sizeof(vec2));
		r->x = mat2Determinant(A_b) * invDetA;

		memcpy(A_b.m[0], A.m[0], sizeof(vec2));
		memcpy(A_b.m[1], &b, sizeof(vec2));
		r->y = mat2Determinant(A_b) * invDetA;

		return 1;

	}

	return 0;

}

__HINT_INLINE__ return_t mat2SolvePR(const mat2 *const __RESTRICT__ A, const vec2 *const __RESTRICT__ b, vec2 *const __RESTRICT__ r){

	// Solves Ax = b using Cramer's rule.
	// Cramer's rule states that
	//     x_1 = det(A_1) / det(A)
	//     x_2 = det(A_2) / det(A)
	// for matrices A_1, A_2 that are the
	// matrix A with the first and second
	// columns replaced with the solution
	// vector b.
	// If the determinant of A is zero,
	// Cramer's rule does not apply.

	float invDetA = mat2DeterminantP(A);

	if(invDetA != 0.f){

		mat2 A_b;
		invDetA = 1.f / invDetA;

		memcpy(A_b.m[0], b, sizeof(vec2));
		memcpy(A_b.m[1], A->m[1], sizeof(vec2));
		r->x = mat2DeterminantP(&A_b) * invDetA;

		memcpy(A_b.m[0], A->m[0], sizeof(vec2));
		memcpy(A_b.m[1], b, sizeof(vec2));
		r->y = mat2DeterminantP(&A_b) * invDetA;

		return 1;

	}

	return 0;

}
