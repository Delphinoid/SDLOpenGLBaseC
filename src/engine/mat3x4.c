#include "mat3x4.h"
#include "mat3.h"
#include <stddef.h>
#include <math.h>
#include <string.h>

// Remember that matrices are stored in column-major order!!
mat3x4 g_mat3x4Identity = {
	.m = {{1.f, 0.f, 0.f},
	      {0.f, 1.f, 0.f},
	      {0.f, 0.f, 1.f},
	      {0.f, 0.f, 0.f}}
};
mat3x4 g_mat3x4Zero = {
	.m = {{0.f, 0.f, 0.f},
	      {0.f, 0.f, 0.f},
	      {0.f, 0.f, 0.f},
	      {0.f, 0.f, 0.f}}
};

__HINT_INLINE__ void mat3x4IdentityP(mat3x4 *const __RESTRICT__ m){
	*m = g_mat3x4Identity;
}
__HINT_INLINE__ void mat3x4ZeroP(mat3x4 *const __RESTRICT__ m){
	memset(m->m, 0, sizeof(mat3x4));
}

__HINT_INLINE__ mat3x4 mat3x4DiagonalV(const vec3 v){
	const mat3x4 r = {.m = {{v.x, 0.f, 0.f},
	                        {0.f, v.y, 0.f},
	                        {0.f, 0.f, v.z},
	                        {0.f, 0.f, 0.f}}};
	return r;
}
__HINT_INLINE__ void mat3x4DiagonalVP(mat3x4 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ v){
	memset(m->m, 0, sizeof(mat3x4));
	m->m[0][0] = v->x;
	m->m[1][1] = v->y;
	m->m[2][2] = v->z;
}
__HINT_INLINE__ mat3x4 mat3x4DiagonalS(const float s){
	const mat3x4 r = {.m = {{s, 0.f, 0.f},
	                        {0.f, s, 0.f},
	                        {0.f, 0.f, s},
	                        {0.f, 0.f, 0.f}}};
	return r;
}
__HINT_INLINE__ void mat3x4DiagonalSP(mat3x4 *const __RESTRICT__ m, const float s){
	memset(m->m, 0, sizeof(mat3x4));
	m->m[0][0] = s;
	m->m[1][1] = s;
	m->m[2][2] = s;
}
__HINT_INLINE__ mat3x4 mat3x4DiagonalN(const float x, const float y, const float z){
	const mat3x4 r = {.m = {{x, 0.f, 0.f},
	                        {0.f, y, 0.f},
	                        {0.f, 0.f, z},
	                        {0.f, 0.f, 0.f}}};
	return r;
}
__HINT_INLINE__ void mat3x4DiagonalNP(mat3x4 *const __RESTRICT__ m, const float x, const float y, const float z){
	memset(m->m, 0, sizeof(mat3x4));
	m->m[0][0] = x;
	m->m[1][1] = y;
	m->m[2][2] = z;
}

__HINT_INLINE__ mat3x4 mat3x4MMultM(const mat3x4 m1, const mat3x4 m2){

	/**size_t i, j;
	for(i = 0; i < 4; ++i){
		for(j = 0; j < 4; ++j){
			r.m[i][j] = (m2->m[i][0] * m1->m[0][j]) +
			            (m2->m[i][1] * m1->m[1][j]) +
			            (m2->m[i][2] * m1->m[2][j]) +
			            (m2->m[i][3] * m1->m[3][j]);
		}
	}**/

	const mat3x4 r = {.m = {{m1.m[0][0]*m2.m[0][0] + m1.m[1][0]*m2.m[0][1] + m1.m[2][0]*m2.m[0][2],
	                         m1.m[0][1]*m2.m[0][0] + m1.m[1][1]*m2.m[0][1] + m1.m[2][1]*m2.m[0][2],
	                         m1.m[0][2]*m2.m[0][0] + m1.m[1][2]*m2.m[0][1] + m1.m[2][2]*m2.m[0][2]},
	                        {m1.m[0][0]*m2.m[1][0] + m1.m[1][0]*m2.m[1][1] + m1.m[2][0]*m2.m[1][2],
	                         m1.m[0][1]*m2.m[1][0] + m1.m[1][1]*m2.m[1][1] + m1.m[2][1]*m2.m[1][2],
	                         m1.m[0][2]*m2.m[1][0] + m1.m[1][2]*m2.m[1][1] + m1.m[2][2]*m2.m[1][2]},
	                        {m1.m[0][0]*m2.m[2][0] + m1.m[1][0]*m2.m[2][1] + m1.m[2][0]*m2.m[2][2],
	                         m1.m[0][1]*m2.m[2][0] + m1.m[1][1]*m2.m[2][1] + m1.m[2][1]*m2.m[2][2],
	                         m1.m[0][2]*m2.m[2][0] + m1.m[1][2]*m2.m[2][1] + m1.m[2][2]*m2.m[2][2]},
	                        {m1.m[0][0]*m2.m[3][0] + m1.m[1][0]*m2.m[3][1] + m1.m[2][0]*m2.m[3][2] + m1.m[3][0],
	                         m1.m[0][1]*m2.m[3][0] + m1.m[1][1]*m2.m[3][1] + m1.m[2][1]*m2.m[3][2] + m1.m[3][1],
	                         m1.m[0][2]*m2.m[3][0] + m1.m[1][2]*m2.m[3][1] + m1.m[2][2]*m2.m[3][2] + m1.m[3][2]}}};
	return r;

}
__HINT_INLINE__ void mat3x4MMultMP1(mat3x4 *const __RESTRICT__ m1, const mat3x4 *const __RESTRICT__ m2){

	const mat3x4 r = {.m = {{m1->m[0][0]*m2->m[0][0] + m1->m[1][0]*m2->m[0][1] + m1->m[2][0]*m2->m[0][2],
	                         m1->m[0][1]*m2->m[0][0] + m1->m[1][1]*m2->m[0][1] + m1->m[2][1]*m2->m[0][2],
	                         m1->m[0][2]*m2->m[0][0] + m1->m[1][2]*m2->m[0][1] + m1->m[2][2]*m2->m[0][2]},
	                        {m1->m[0][0]*m2->m[1][0] + m1->m[1][0]*m2->m[1][1] + m1->m[2][0]*m2->m[1][2],
	                         m1->m[0][1]*m2->m[1][0] + m1->m[1][1]*m2->m[1][1] + m1->m[2][1]*m2->m[1][2],
	                         m1->m[0][2]*m2->m[1][0] + m1->m[1][2]*m2->m[1][1] + m1->m[2][2]*m2->m[1][2]},
	                        {m1->m[0][0]*m2->m[2][0] + m1->m[1][0]*m2->m[2][1] + m1->m[2][0]*m2->m[2][2],
	                         m1->m[0][1]*m2->m[2][0] + m1->m[1][1]*m2->m[2][1] + m1->m[2][1]*m2->m[2][2],
	                         m1->m[0][2]*m2->m[2][0] + m1->m[1][2]*m2->m[2][1] + m1->m[2][2]*m2->m[2][2]},
	                        {m1->m[0][0]*m2->m[3][0] + m1->m[1][0]*m2->m[3][1] + m1->m[2][0]*m2->m[3][2] + m1->m[3][0],
	                         m1->m[0][1]*m2->m[3][0] + m1->m[1][1]*m2->m[3][1] + m1->m[2][1]*m2->m[3][2] + m1->m[3][1],
	                         m1->m[0][2]*m2->m[3][0] + m1->m[1][2]*m2->m[3][1] + m1->m[2][2]*m2->m[3][2] + m1->m[3][2]}}};
	*m1 = r;

}
__HINT_INLINE__ void mat3x4MMultMP2(const mat3x4 *const __RESTRICT__ m1, mat3x4 *const __RESTRICT__ m2){

	const mat3x4 r = {.m = {{m1->m[0][0]*m2->m[0][0] + m1->m[1][0]*m2->m[0][1] + m1->m[2][0]*m2->m[0][2],
	                         m1->m[0][1]*m2->m[0][0] + m1->m[1][1]*m2->m[0][1] + m1->m[2][1]*m2->m[0][2],
	                         m1->m[0][2]*m2->m[0][0] + m1->m[1][2]*m2->m[0][1] + m1->m[2][2]*m2->m[0][2]},
	                        {m1->m[0][0]*m2->m[1][0] + m1->m[1][0]*m2->m[1][1] + m1->m[2][0]*m2->m[1][2],
	                         m1->m[0][1]*m2->m[1][0] + m1->m[1][1]*m2->m[1][1] + m1->m[2][1]*m2->m[1][2],
	                         m1->m[0][2]*m2->m[1][0] + m1->m[1][2]*m2->m[1][1] + m1->m[2][2]*m2->m[1][2]},
	                        {m1->m[0][0]*m2->m[2][0] + m1->m[1][0]*m2->m[2][1] + m1->m[2][0]*m2->m[2][2],
	                         m1->m[0][1]*m2->m[2][0] + m1->m[1][1]*m2->m[2][1] + m1->m[2][1]*m2->m[2][2],
	                         m1->m[0][2]*m2->m[2][0] + m1->m[1][2]*m2->m[2][1] + m1->m[2][2]*m2->m[2][2]},
	                        {m1->m[0][0]*m2->m[3][0] + m1->m[1][0]*m2->m[3][1] + m1->m[2][0]*m2->m[3][2] + m1->m[3][0],
	                         m1->m[0][1]*m2->m[3][0] + m1->m[1][1]*m2->m[3][1] + m1->m[2][1]*m2->m[3][2] + m1->m[3][1],
	                         m1->m[0][2]*m2->m[3][0] + m1->m[1][2]*m2->m[3][1] + m1->m[2][2]*m2->m[3][2] + m1->m[3][2]}}};
	*m2 = r;

}
__HINT_INLINE__ void mat3x4MMultMPR(const mat3x4 *const __RESTRICT__ m1, const mat3x4 *const __RESTRICT__ m2, mat3x4 *const __RESTRICT__ r){

	r->m[0][0] = m1->m[0][0]*m2->m[0][0] + m1->m[1][0]*m2->m[0][1] + m1->m[2][0]*m2->m[0][2];
	r->m[0][1] = m1->m[0][1]*m2->m[0][0] + m1->m[1][1]*m2->m[0][1] + m1->m[2][1]*m2->m[0][2];
	r->m[0][2] = m1->m[0][2]*m2->m[0][0] + m1->m[1][2]*m2->m[0][1] + m1->m[2][2]*m2->m[0][2];

	r->m[1][0] = m1->m[0][0]*m2->m[1][0] + m1->m[1][0]*m2->m[1][1] + m1->m[2][0]*m2->m[1][2];
	r->m[1][1] = m1->m[0][1]*m2->m[1][0] + m1->m[1][1]*m2->m[1][1] + m1->m[2][1]*m2->m[1][2];
	r->m[1][2] = m1->m[0][2]*m2->m[1][0] + m1->m[1][2]*m2->m[1][1] + m1->m[2][2]*m2->m[1][2];

	r->m[2][0] = m1->m[0][0]*m2->m[2][0] + m1->m[1][0]*m2->m[2][1] + m1->m[2][0]*m2->m[2][2];
	r->m[2][1] = m1->m[0][1]*m2->m[2][0] + m1->m[1][1]*m2->m[2][1] + m1->m[2][1]*m2->m[2][2];
	r->m[2][2] = m1->m[0][2]*m2->m[2][0] + m1->m[1][2]*m2->m[2][1] + m1->m[2][2]*m2->m[2][2];

	r->m[3][0] = m1->m[0][0]*m2->m[3][0] + m1->m[1][0]*m2->m[3][1] + m1->m[2][0]*m2->m[3][2] + m1->m[3][0];
	r->m[3][1] = m1->m[0][1]*m2->m[3][0] + m1->m[1][1]*m2->m[3][1] + m1->m[2][1]*m2->m[3][2] + m1->m[3][1];
	r->m[3][2] = m1->m[0][2]*m2->m[3][0] + m1->m[1][2]*m2->m[3][1] + m1->m[2][2]*m2->m[3][2] + m1->m[3][2];

}

__HINT_INLINE__ mat3x4 mat3x4MMultM3(const mat3x4 m1, const mat3 m2){

	/**size_t i, j;
	for(i = 0; i < 4; ++i){
		for(j = 0; j < 4; ++j){
			r.m[i][j] = (m2->m[i][0] * m1->m[0][j]) +
			            (m2->m[i][1] * m1->m[1][j]) +
			            (m2->m[i][2] * m1->m[2][j]) +
			            (m2->m[i][3] * m1->m[3][j]);
		}
	}**/

	const mat3x4 r = {.m = {{m1.m[0][0]*m2.m[0][0] + m1.m[1][0]*m2.m[0][1] + m1.m[2][0]*m2.m[0][2],
	                         m1.m[0][1]*m2.m[0][0] + m1.m[1][1]*m2.m[0][1] + m1.m[2][1]*m2.m[0][2],
	                         m1.m[0][2]*m2.m[0][0] + m1.m[1][2]*m2.m[0][1] + m1.m[2][2]*m2.m[0][2]},
	                        {m1.m[0][0]*m2.m[1][0] + m1.m[1][0]*m2.m[1][1] + m1.m[2][0]*m2.m[1][2],
	                         m1.m[0][1]*m2.m[1][0] + m1.m[1][1]*m2.m[1][1] + m1.m[2][1]*m2.m[1][2],
	                         m1.m[0][2]*m2.m[1][0] + m1.m[1][2]*m2.m[1][1] + m1.m[2][2]*m2.m[1][2]},
	                        {m1.m[0][0]*m2.m[2][0] + m1.m[1][0]*m2.m[2][1] + m1.m[2][0]*m2.m[2][2],
	                         m1.m[0][1]*m2.m[2][0] + m1.m[1][1]*m2.m[2][1] + m1.m[2][1]*m2.m[2][2],
	                         m1.m[0][2]*m2.m[2][0] + m1.m[1][2]*m2.m[2][1] + m1.m[2][2]*m2.m[2][2]},
	                        {m1.m[3][0],
	                         m1.m[3][1],
	                         m1.m[3][2]}}};
	return r;

}
__HINT_INLINE__ void mat3x4MMultM3P(mat3x4 *const __RESTRICT__ m1, const mat3 *const __RESTRICT__ m2){

	const mat3x4 r = {.m = {{m1->m[0][0]*m2->m[0][0] + m1->m[1][0]*m2->m[0][1] + m1->m[2][0]*m2->m[0][2],
	                         m1->m[0][1]*m2->m[0][0] + m1->m[1][1]*m2->m[0][1] + m1->m[2][1]*m2->m[0][2],
	                         m1->m[0][2]*m2->m[0][0] + m1->m[1][2]*m2->m[0][1] + m1->m[2][2]*m2->m[0][2]},
	                        {m1->m[0][0]*m2->m[1][0] + m1->m[1][0]*m2->m[1][1] + m1->m[2][0]*m2->m[1][2],
	                         m1->m[0][1]*m2->m[1][0] + m1->m[1][1]*m2->m[1][1] + m1->m[2][1]*m2->m[1][2],
	                         m1->m[0][2]*m2->m[1][0] + m1->m[1][2]*m2->m[1][1] + m1->m[2][2]*m2->m[1][2]},
	                        {m1->m[0][0]*m2->m[2][0] + m1->m[1][0]*m2->m[2][1] + m1->m[2][0]*m2->m[2][2],
	                         m1->m[0][1]*m2->m[2][0] + m1->m[1][1]*m2->m[2][1] + m1->m[2][1]*m2->m[2][2],
	                         m1->m[0][2]*m2->m[2][0] + m1->m[1][2]*m2->m[2][1] + m1->m[2][2]*m2->m[2][2]},
	                        {m1->m[3][0],
	                         m1->m[3][1],
	                         m1->m[3][2]}}};
	*m1 = r;

}
__HINT_INLINE__ void mat3x4MMultM3PR(const mat3x4 *const __RESTRICT__ m1, const mat3 *const __RESTRICT__ m2, mat3x4 *const __RESTRICT__ r){

	r->m[0][0] = m1->m[0][0]*m2->m[0][0] + m1->m[1][0]*m2->m[0][1] + m1->m[2][0]*m2->m[0][2];
	r->m[0][1] = m1->m[0][1]*m2->m[0][0] + m1->m[1][1]*m2->m[0][1] + m1->m[2][1]*m2->m[0][2];
	r->m[0][2] = m1->m[0][2]*m2->m[0][0] + m1->m[1][2]*m2->m[0][1] + m1->m[2][2]*m2->m[0][2];

	r->m[1][0] = m1->m[0][0]*m2->m[1][0] + m1->m[1][0]*m2->m[1][1] + m1->m[2][0]*m2->m[1][2];
	r->m[1][1] = m1->m[0][1]*m2->m[1][0] + m1->m[1][1]*m2->m[1][1] + m1->m[2][1]*m2->m[1][2];
	r->m[1][2] = m1->m[0][2]*m2->m[1][0] + m1->m[1][2]*m2->m[1][1] + m1->m[2][2]*m2->m[1][2];

	r->m[2][0] = m1->m[0][0]*m2->m[2][0] + m1->m[1][0]*m2->m[2][1] + m1->m[2][0]*m2->m[2][2];
	r->m[2][1] = m1->m[0][1]*m2->m[2][0] + m1->m[1][1]*m2->m[2][1] + m1->m[2][1]*m2->m[2][2];
	r->m[2][2] = m1->m[0][2]*m2->m[2][0] + m1->m[1][2]*m2->m[2][1] + m1->m[2][2]*m2->m[2][2];

	r->m[3][0] = m1->m[3][0];
	r->m[3][1] = m1->m[3][1];
	r->m[3][2] = m1->m[3][2];

}

__HINT_INLINE__ vec4 mat3x4VMultM(const vec4 v, const mat3x4 m){
	const vec4 r = {.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2],
	                .y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2],
	                .z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2],
	                .w = v.x * m.m[3][0] + v.y * m.m[3][1] + v.z * m.m[3][2] + v.w};
	return r;
}
__HINT_INLINE__ void mat3x4VMultMP(vec4 *const __RESTRICT__ v, const mat3x4 *const __RESTRICT__ m){
	const vec4 r = {.x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2],
	                .y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2],
	                .z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2],
	                .w = v->x * m->m[3][0] + v->y * m->m[3][1] + v->z * m->m[3][2] + v->w};
	*v = r;
}
__HINT_INLINE__ void mat3x4VMultMPR(const vec4 *const __RESTRICT__ v, const mat3x4 *const __RESTRICT__ m, vec4 *const __RESTRICT__ r){
	r->x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2];
	r->y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2];
	r->z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2];
	r->w = v->x * m->m[3][0] + v->y * m->m[3][1] + v->z * m->m[3][2] + v->w;
}
__HINT_INLINE__ vec4 mat3x4MMultV(const mat3x4 m, const vec4 v){
	const vec4 r = {.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0],
	                .y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1],
	                .z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2],
	                .w = v.w};
	return r;
}
__HINT_INLINE__ void mat3x4MMultVP(const mat3x4 *const __RESTRICT__ m, vec4 *const __RESTRICT__ v){
	const vec4 r = {.x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0] + v->w * m->m[3][0],
	                .y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1] + v->w * m->m[3][1],
	                .z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2] + v->w * m->m[3][2],
	                .w = v->w};
	*v = r;
}
__HINT_INLINE__ void mat3x4MMultVPR(const mat3x4 *const __RESTRICT__ m, const vec4 *const __RESTRICT__ v, vec4 *const __RESTRICT__ r){
	r->x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0] + v->w * m->m[3][0];
	r->y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1] + v->w * m->m[3][1];
	r->z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2] + v->w * m->m[3][2];
	r->w = v->w;
}

__HINT_INLINE__ vec3 mat3x4V3MultM(const vec3 v, const mat3x4 m){
	const vec3 r = {.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2],
	                .y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2],
	                .z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2]};
	return r;
}
__HINT_INLINE__ void mat3x4V3MultMP(vec3 *const __RESTRICT__ v, const mat3x4 *const __RESTRICT__ m){
	const vec3 r = {.x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2],
	                .y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2],
	                .z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2]};
	*v = r;
}
__HINT_INLINE__ void mat3x4V3MultMPR(const vec3 *const __RESTRICT__ v, const mat3x4 *const __RESTRICT__ m, vec3 *const __RESTRICT__ r){
	r->x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2];
	r->y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2];
	r->z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2];
}
__HINT_INLINE__ vec3 mat3x4MMultV3(const mat3x4 m, const vec3 v){
	const vec3 r = {.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0],
	                .y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1],
	                .z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]};
	return r;
}
__HINT_INLINE__ void mat3x4MMultV3P(const mat3x4 *const __RESTRICT__ m, vec3 *const __RESTRICT__ v){
	const vec3 r = {.x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0] + m->m[3][0],
	                .y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1] + m->m[3][1],
	                .z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2] + m->m[3][2]};
	*v = r;
}
__HINT_INLINE__ void mat3x4MMultV3PR(const mat3x4 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){
	r->x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0] + m->m[3][0];
	r->y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1] + m->m[3][1];
	r->z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2] + m->m[3][2];
}

__HINT_INLINE__ vec4 mat3x4NMultM(const float x, const float y, const float z, const float w, const mat3x4 m){
	const vec4 r = {.x = x * m.m[0][0] + y * m.m[0][1] + z * m.m[0][2],
	                .y = x * m.m[1][0] + y * m.m[1][1] + z * m.m[1][2],
	                .z = x * m.m[2][0] + y * m.m[2][1] + z * m.m[2][2],
	                .w = x * m.m[3][0] + y * m.m[3][1] + z * m.m[3][2] + w};
	return r;
}
__HINT_INLINE__ void mat3x4NMultMPR(const float x, const float y, const float z, const float w, const mat3x4 *const __RESTRICT__ m, vec4 *const __RESTRICT__ r){
	r->x = x * m->m[0][0] + y * m->m[0][1] + z * m->m[0][2];
	r->y = x * m->m[1][0] + y * m->m[1][1] + z * m->m[1][2];
	r->z = x * m->m[2][0] + y * m->m[2][1] + z * m->m[2][2];
	r->w = x * m->m[3][0] + y * m->m[3][1] + z * m->m[3][2] + w;
}
__HINT_INLINE__ vec4 mat3x4MMultN(const mat3x4 m, const float x, const float y, const float z, const float w){
	const vec4 r = {.x = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + w * m.m[3][0],
	                .y = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + w * m.m[3][1],
	                .z = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + w * m.m[3][2],
	                .w = w};
	return r;
}
__HINT_INLINE__ void mat3x4MMultNPR(const mat3x4 *const __RESTRICT__ m, const float x, const float y, const float z, const float w, vec4 *r){
	r->x = x * m->m[0][0] + y * m->m[1][0] + z * m->m[2][0] + w * m->m[3][0];
	r->y = x * m->m[0][1] + y * m->m[1][1] + z * m->m[2][1] + w * m->m[3][1];
	r->z = x * m->m[0][2] + y * m->m[1][2] + z * m->m[2][2] + w * m->m[3][2];
	r->w = w;
}

__HINT_INLINE__ vec3 mat3x4N3MultM(const float x, const float y, const float z, const mat3x4 m){
	const vec3 r = {.x = x * m.m[0][0] + y * m.m[0][1] + z * m.m[0][2],
	                .y = x * m.m[1][0] + y * m.m[1][1] + z * m.m[1][2],
	                .z = x * m.m[2][0] + y * m.m[2][1] + z * m.m[2][2]};
	return r;
}
__HINT_INLINE__ void mat3x4N3MultMPR(const float x, const float y, const float z, const mat3x4 *const __RESTRICT__ m, vec3 *const __RESTRICT__ r){
	r->x = x * m->m[0][0] + y * m->m[0][1] + z * m->m[0][2];
	r->y = x * m->m[1][0] + y * m->m[1][1] + z * m->m[1][2];
	r->z = x * m->m[2][0] + y * m->m[2][1] + z * m->m[2][2];
}
__HINT_INLINE__ vec3 mat3x4MMultN3(const mat3x4 m, const float x, const float y, const float z){
	const vec3 r = {.x = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + m.m[3][0],
	                .y = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + m.m[3][1],
	                .z = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + m.m[3][2]};
	return r;
}
__HINT_INLINE__ void mat3x4MMultN3PR(const mat3x4 *const __RESTRICT__ m, const float x, const float y, const float z, vec3 *r){
	r->x = x * m->m[0][0] + y * m->m[1][0] + z * m->m[2][0] + m->m[3][0];
	r->y = x * m->m[0][1] + y * m->m[1][1] + z * m->m[2][1] + m->m[3][1];
	r->z = x * m->m[0][2] + y * m->m[1][2] + z * m->m[2][2] + m->m[3][2];
}

/**__HINT_INLINE__ vec3 mat3x4GetTransformedV(const mat3x4 m, const vec3 v){
	const vec3 r = {.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + m.m[0][3],
	                .y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + m.m[1][3],
	                .z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + m.m[2][3]};
	return r;
}
__HINT_INLINE__ void mat3x4TransformV(const mat3x4 *const __RESTRICT__ m, vec3 *const __RESTRICT__ v){
	const vec3 r = {.x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2] + m->m[0][3],
	                .y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2] + m->m[1][3],
	                .z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2] + m->m[2][3]};
	*v = r;
}**/

__HINT_INLINE__ mat3x4 mat3x4MAddM(const mat3x4 m1, const mat3x4 m2){
	const mat3x4 r = {.m = {{m1.m[0][0] + m2.m[0][0],
	                         m1.m[0][1] + m2.m[0][1],
	                         m1.m[0][2] + m2.m[0][2]},
	                        {m1.m[1][0] + m2.m[1][0],
	                         m1.m[1][1] + m2.m[1][1],
	                         m1.m[1][2] + m2.m[1][2]},
	                        {m1.m[2][0] + m2.m[2][0],
	                         m1.m[2][1] + m2.m[2][1],
	                         m1.m[2][2] + m2.m[2][2]},
	                        {m1.m[3][0] + m2.m[3][0],
	                         m1.m[3][1] + m2.m[3][1],
	                         m1.m[3][2] + m2.m[3][2]}}};
	return r;
}
__HINT_INLINE__ void mat3x4MAddMP(mat3x4 *const __RESTRICT__ m1, const mat3x4 *const __RESTRICT__ m2){
	m1->m[0][0] += m2->m[0][0]; m1->m[0][1] += m2->m[0][1]; m1->m[0][2] += m2->m[0][2];
	m1->m[1][0] += m2->m[1][0]; m1->m[1][1] += m2->m[1][1]; m1->m[1][2] += m2->m[1][2];
	m1->m[2][0] += m2->m[2][0]; m1->m[2][1] += m2->m[2][1]; m1->m[2][2] += m2->m[2][2];
	m1->m[3][0] += m2->m[3][0]; m1->m[3][1] += m2->m[3][1]; m1->m[3][2] += m2->m[3][2];
}
__HINT_INLINE__ void mat3x4MAddMPR(const mat3x4 *const __RESTRICT__ m1, const mat3x4 *const __RESTRICT__ m2, mat3x4 *const __RESTRICT__ r){
	r->m[0][0] = m1->m[0][0] + m2->m[0][0]; r->m[0][1] = m1->m[0][1] + m2->m[0][1]; r->m[0][2] = m1->m[0][2] + m2->m[0][2];
	r->m[1][0] = m1->m[1][0] + m2->m[1][0]; r->m[1][1] = m1->m[1][1] + m2->m[1][1]; r->m[1][2] = m1->m[1][2] + m2->m[1][2];
	r->m[2][0] = m1->m[2][0] + m2->m[2][0]; r->m[2][1] = m1->m[2][1] + m2->m[2][1]; r->m[2][2] = m1->m[2][2] + m2->m[2][2];
	r->m[3][0] = m1->m[3][0] + m2->m[3][0]; r->m[3][1] = m1->m[3][1] + m2->m[3][1]; r->m[3][2] = m1->m[3][2] + m2->m[3][2];
}

__HINT_INLINE__ float mat3x4Determinant(const mat3x4 m){
	return m.m[0][0] * (m.m[1][1] * (m.m[2][2] - m.m[3][2]) -
	                    m.m[1][2] * (m.m[2][1] - m.m[3][1])) -
	       m.m[0][1] * (m.m[1][0] * (m.m[2][2] - m.m[3][2]) -
	                    m.m[1][2] * (m.m[2][0] - m.m[3][0])) +
	       m.m[0][2] * (m.m[1][0] * (m.m[2][1] - m.m[3][1]) -
	                    m.m[1][1] * (m.m[2][0] - m.m[3][0]));
}
__HINT_INLINE__ float mat3x4DeterminantP(const mat3x4 *const __RESTRICT__ m){
	return m->m[0][0] * (m->m[1][1] * (m->m[2][2] - m->m[3][2]) -
	                     m->m[1][2] * (m->m[2][1] - m->m[3][1])) -
	       m->m[0][1] * (m->m[1][0] * (m->m[2][2] - m->m[3][2]) -
	                     m->m[1][2] * (m->m[2][0] - m->m[3][0])) +
	       m->m[0][2] * (m->m[1][0] * (m->m[2][1] - m->m[3][1]) -
	                     m->m[1][1] * (m->m[2][0] - m->m[3][0]));
}

__HINT_INLINE__ mat3x4 mat3x4Invert(const mat3x4 m){
	// Find the inverse using Gauss-Jordan elimination.
	///
	return m;
}
__HINT_INLINE__ return_t mat3x4InvertR(const mat3x4 m, mat3x4 *const __RESTRICT__ r){
	// Find the inverse using Gauss-Jordan elimination.
	///
	return 0;
}
__HINT_INLINE__ return_t mat3x4InvertP(mat3x4 *const __RESTRICT__ m){
	// Find the inverse using Gauss-Jordan elimination.
	///
	return 0;
}
__HINT_INLINE__ return_t mat3x4InvertPR(const mat3x4 *const __RESTRICT__ m, mat3x4 *const __RESTRICT__ r){
	// Find the inverse using Gauss-Jordan elimination.
	///
	return 0;
}

__HINT_INLINE__ mat3x4 mat3x4RotateToFace(const vec3 eye, const vec3 target, const vec3 up){
	const vec3 zAxis = vec3NormalizeFast(vec3VSubV(target, eye));
	const vec3 xAxis = vec3NormalizeFast(vec3Cross(up, zAxis));
	const vec3 yAxis = vec3NormalizeFast(vec3Cross(zAxis, xAxis));
	const mat3x4 r = {.m = {{xAxis.x, xAxis.y, xAxis.z},
	                        {yAxis.x, yAxis.y, yAxis.z},
                            {zAxis.x, zAxis.y, zAxis.z},
	                        {0.f,     0.f,     0.f}}};
	return r;
}
__HINT_INLINE__ void mat3x4RotateToFaceP(mat3x4 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ eye, const vec3 *const __RESTRICT__ target, const vec3 *const __RESTRICT__ up){
	vec3 xAxis, yAxis, zAxis;
	vec3VSubVPR(target, eye, &zAxis);
	vec3NormalizeFastP(&zAxis);
	vec3CrossPR(up, &zAxis, &xAxis);
	vec3NormalizeFastP(&xAxis);
	vec3CrossPR(&zAxis, &xAxis, &yAxis);
	vec3NormalizeFastP(&yAxis);
	m->m[0][0] = xAxis.x; m->m[0][1] = xAxis.y; m->m[0][2] = xAxis.z;
	m->m[1][0] = yAxis.x; m->m[1][1] = yAxis.y; m->m[1][2] = yAxis.z;
	m->m[2][0] = zAxis.x; m->m[2][1] = zAxis.y; m->m[2][2] = zAxis.z;
	m->m[3][0] = 0.f;     m->m[3][1] = 0.f;     m->m[3][2] = 0.f;
}
__HINT_INLINE__ mat3x4 mat3x4LookAt(const vec3 eye, const vec3 target, const vec3 up){
	// The xAxis, yAxis and zAxis vectors appear as rows
	// as we really need an inverse matrix. Because it is
	// orthogonal, we can just take the transpose.
	// The inverse of the translation component is just
	// the additive inverse of the translation.
	const vec3 zAxis = vec3NormalizeFast(vec3VSubV(eye, target));
	const vec3 xAxis = vec3NormalizeFast(vec3Cross(up, zAxis));
	const vec3 yAxis = vec3NormalizeFast(vec3Cross(zAxis, xAxis));
	const mat3x4 r = {.m = {{xAxis.x,              yAxis.x,              zAxis.x},
	                        {xAxis.y,              yAxis.y,              zAxis.y},
	                        {xAxis.z,              yAxis.z,              zAxis.z},
	                        {-vec3Dot(xAxis, eye), -vec3Dot(yAxis, eye), -vec3Dot(zAxis, eye)}}};
	return r;
}
__HINT_INLINE__ void mat3x4LookAtP(mat3x4 *const __RESTRICT__ m, const vec3 *const __RESTRICT__ eye, const vec3 *const __RESTRICT__ target, const vec3 *const __RESTRICT__ up){
	// The xAxis, yAxis and zAxis vectors appear as rows
	// as we really need an inverse matrix. Because it is
	// orthogonal, we can just take the transpose.
	// The inverse of the translation component is just
	// the additive inverse of the translation.
	vec3 xAxis, yAxis, zAxis;
	vec3VSubVPR(eye, target, &zAxis);
	vec3NormalizeFastP(&zAxis);
	vec3CrossPR(up, &zAxis, &xAxis);
	vec3NormalizeFastP(&xAxis);
	vec3CrossPR(&zAxis, &xAxis, &yAxis);
	vec3NormalizeFastP(&yAxis);
	m->m[0][0] = xAxis.x;                m->m[0][1] = yAxis.x;                m->m[0][2] = zAxis.x;
	m->m[1][0] = xAxis.y;                m->m[1][1] = yAxis.y;                m->m[1][2] = zAxis.y;
	m->m[2][0] = xAxis.z;                m->m[2][1] = yAxis.z;                m->m[2][2] = zAxis.z;
	m->m[3][0] = -vec3DotP(&xAxis, eye); m->m[3][1] = -vec3DotP(&yAxis, eye); m->m[3][2] = -vec3DotP(&zAxis, eye);
}

__HINT_INLINE__ mat3x4 mat3x4TranslationMatrix(const float x, const float y, const float z){
	const mat3x4 r = {.m = {{1.f, 0.f, 0.f},
	                        {0.f, 1.f, 0.f},
	                        {0.f, 0.f, 1.f},
	                        {  x,   y,   z}}};
	return r;
}
__HINT_INLINE__ void mat3x4TranslationMatrixP(mat3x4 *const __RESTRICT__ m, const float x, const float y, const float z){
	m->m[0][0] = 1.f; m->m[0][1] = 0.f; m->m[0][2] = 0.f;
	m->m[1][0] = 0.f; m->m[1][1] = 1.f; m->m[1][2] = 0.f;
	m->m[2][0] = 0.f; m->m[2][1] = 0.f; m->m[2][2] = 1.f;
	m->m[3][0] = x;   m->m[3][1] = y;   m->m[3][2] = z;
}
__HINT_INLINE__ mat3x4 mat3x4Translate(const float x, const float y, const float z, const mat3x4 m){
	const mat3x4 r = {.m = {{m.m[0][0],   m.m[0][1],   m.m[0][2]},
	                        {m.m[1][0],   m.m[1][1],   m.m[1][2]},
	                        {m.m[2][0],   m.m[2][1],   m.m[2][2]},
	                        {m.m[3][0]+x, m.m[3][1]+y, m.m[3][2]+z}}};
	return r;
}
__HINT_INLINE__ void mat3x4TranslateP(const float x, const float y, const float z, mat3x4 *const __RESTRICT__ m){
	m->m[3][0] += x;
	m->m[3][1] += y;
	m->m[3][2] += z;
}
__HINT_INLINE__ void mat3x4TranslatePR(const float x, const float y, const float z, const mat3x4 *const __RESTRICT__ m, mat3x4 *const __RESTRICT__ r){
	*r = *m;
	r->m[3][0] += x;
	r->m[3][1] += y;
	r->m[3][2] += z;
}
__HINT_INLINE__ mat3x4 mat3x4TranslatePre(const mat3x4 m, const float x, const float y, const float z){
	const mat3x4 r = {.m = {{m.m[0][0], m.m[0][1], m.m[0][2]},
	                        {m.m[1][0], m.m[1][1], m.m[1][2]},
	                        {m.m[2][0], m.m[2][1], m.m[2][2]},
	                        {m.m[0][0] * x + m.m[1][0] * y + m.m[2][0] * z + m.m[3][0],
	                         m.m[0][1] * x + m.m[1][1] * y + m.m[2][1] * z + m.m[3][1],
	                         m.m[0][2] * x + m.m[1][2] * y + m.m[2][2] * z + m.m[3][2]}}};
	return r;
}

__HINT_INLINE__ mat3x4 mat3x4RotationMatrix(const quat q){
	return mat3x4Quaternion(q);
}
__HINT_INLINE__ void mat3x4RotationMatrixPR(const quat *const __RESTRICT__ q, mat3x4 *const __RESTRICT__ r){
	mat3x4QuaternionPR(q, r);
}
__HINT_INLINE__ mat3x4 mat3x4Rotate(const quat q, const mat3x4 m){
	return mat3x4MMultM(mat3x4Quaternion(q), m);
}
__HINT_INLINE__ void mat3x4RotateP(const quat *const __RESTRICT__ q, mat3x4 *const __RESTRICT__ m){
	mat3x4 r;
	mat3x4QuaternionPR(q, &r);
	mat3x4MMultMP2(&r, m);
}
__HINT_INLINE__ void mat3x4RotatePR(const quat *const __RESTRICT__ q, const mat3x4 *const __RESTRICT__ m, mat3x4 *const __RESTRICT__ r){
	mat3x4QuaternionPR(q, r);
	mat3x4MMultMP1(r, m);
}

mat3x4 mat3x4ScaleMatrix(const float x, const float y, const float z){
	const mat3x4 r = {.m = {{  x, 0.f, 0.f},
	                        {0.f,   y, 0.f},
	                        {0.f, 0.f,   z},
	                        {0.f, 0.f, 0.f}}};
	return r;
}
__HINT_INLINE__ void mat3x4ScaleMatrixP(mat3x4 *const __RESTRICT__ m, const float x, const float y, const float z){
	m->m[0][0] = x;   m->m[0][1] = 0.f; m->m[0][2] = 0.f;
	m->m[1][0] = 0.f; m->m[1][1] = y;   m->m[1][2] = 0.f;
	m->m[2][0] = 0.f; m->m[2][1] = 0.f; m->m[2][2] = z;
	m->m[3][0] = 0.f; m->m[3][1] = 0.f; m->m[3][2] = 0.f;
}
__HINT_INLINE__ mat3x4 mat3x4Scale(const float x, const float y, const float z, const mat3x4 m){
	const mat3x4 r = {.m = {{m.m[0][0] * x, m.m[0][1] * x, m.m[0][2] * x},
	                        {m.m[1][0] * y, m.m[1][1] * y, m.m[1][2] * y},
	                        {m.m[2][0] * z, m.m[2][1] * z, m.m[2][2] * z},
	                        {m.m[3][0],     m.m[3][1],     m.m[3][2]}}};
	return r;
}
__HINT_INLINE__ void mat3x4ScaleP(const float x, const float y, const float z, mat3x4 *const __RESTRICT__ m){
	m->m[0][0] *= x; m->m[0][1] *= x; m->m[0][2] *= x;
	m->m[1][0] *= y; m->m[1][1] *= y; m->m[1][2] *= y;
	m->m[2][0] *= z; m->m[2][1] *= z; m->m[2][2] *= z;
}
__HINT_INLINE__ void mat3x4ScalePR(const float x, const float y, const float z, const mat3x4 *const __RESTRICT__ m, mat3x4 *r){
	r->m[0][0] = m->m[0][0] * x; r->m[0][1] = m->m[0][1] * x; r->m[0][2] = m->m[0][2] * x;
	r->m[1][0] = m->m[1][0] * y; r->m[1][1] = m->m[1][1] * y; r->m[1][2] = m->m[1][2] * y;
	r->m[2][0] = m->m[2][0] * z; r->m[2][1] = m->m[2][1] * z; r->m[2][2] = m->m[2][2] * z;
}
__HINT_INLINE__ mat3x4 mat3x4ScalePre(const mat3x4 m, const float x, const float y, const float z){
	///return mat3x4MMultM(mat3x4ScaleMatrix(x, y, z), m);
	const mat3x4 r = {.m = {{x * m.m[0][0], y * m.m[0][1], z * m.m[0][2]},
	                        {x * m.m[1][0], y * m.m[1][1], z * m.m[1][2]},
	                        {x * m.m[2][0], y * m.m[2][1], z * m.m[2][2]},
	                        {x * m.m[3][0], y * m.m[3][1], z * m.m[3][2]}}};
	return r;
}

__HINT_INLINE__ mat3x4 mat3x4ShearMatrix(const quat q, const vec3 s){

	// Given a quaternion q with matrix Q and a scale matrix S with
	// diagonal s, determine the corresponding shear matrix m = QSQ^T.

	mat3x4 r;

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
	r.m[2][2] = m.m[0][2]*s.x*m.m[0][2] +  m.m[1][2]*s.y*m.m[1][2] + m.m[2][2]*s.z*m.m[2][2];
	r.m[3][0] = 0.f;
	r.m[3][1] = 0.f;
	r.m[3][2] = 0.f;

	return r;

}
__HINT_INLINE__ void mat3x4ShearMatrixPR(const quat *const __RESTRICT__ q, const vec3 *const __RESTRICT__ s, mat3x4 *const __RESTRICT__ r){

	// Given a quaternion q with matrix Q and a scale matrix S with
	// diagonal s, determine the corresponding shear matrix m = QSQ^T.

	// Convert the quaternion to a rotation matrix.
	mat3 m;
	mat3QuaternionPR(q, &m);

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
	r->m[3][0] = 0.f;
	r->m[3][1] = 0.f;
	r->m[3][2] = 0.f;

}

__HINT_INLINE__ mat3x4 mat3x4Quaternion(const quat q){
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
	///if(isnan(yy)){exit(1);}
	/**printf("%f %f %f %f\n", 1.f-y2y-z2z, x2y+w2z,     x2z-w2y,     0.f);
	printf("%f %f %f %f\n", x2y-w2z,     1.f-x2x-z2z, y2z+w2x,     0.f);
	printf("%f %f %f %f\n", x2z+w2y,     y2z-w2x,     1.f-x2x-y2y, 0.f);
	printf("%f %f %f %f\n\n", 0.f,             0.f,             0.f,             1.f);**/
	const mat3x4 r = {.m = {{1.f-y2y-z2z, x2y+w2z,     x2z-w2y},
	                        {x2y-w2z,     1.f-x2x-z2z, y2z+w2x},
	                        {x2z+w2y,     y2z-w2x,     1.f-x2x-y2y},
	                        {0.f,         0.f,         0.f}}};
	return r;
}
__HINT_INLINE__ void mat3x4QuaternionPR(const quat *const __RESTRICT__ q, mat3x4 *const __RESTRICT__ r){
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
	r->m[0][0] = 1.f-y2y-z2z; r->m[0][1] = x2y+w2z;     r->m[0][2] = x2z-w2y;
	r->m[1][0] = x2y-w2z;     r->m[1][1] = 1.f-x2x-z2z; r->m[1][2] = y2z+w2x;
	r->m[2][0] = x2z+w2y;     r->m[2][1] = y2z-w2x;     r->m[2][2] = 1.f-x2x-y2y;
	r->m[3][0] = 0.f;         r->m[3][1] = 0.f;         r->m[3][2] = 0.f;
}
