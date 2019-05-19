#include "mat4.h"
#include "inline.h"
#include <stddef.h>
#include <math.h>
#include <string.h>

__HINT_INLINE__ mat4 mat4Identity(){
	const mat4 r = {.m = {{1.f, 0.f, 0.f, 0.f},
	                      {0.f, 1.f, 0.f, 0.f},
	                      {0.f, 0.f, 1.f, 0.f},
	                      {0.f, 0.f, 0.f, 1.f}}};
	return r;
}
__HINT_INLINE__ void mat4IdentityP(mat4 *const restrict m){
	m->m[0][0] = 1.f; m->m[0][1] = 0.f; m->m[0][2] = 0.f; m->m[0][3] = 0.f;
	m->m[1][0] = 0.f; m->m[1][1] = 1.f; m->m[1][2] = 0.f; m->m[1][3] = 0.f;
	m->m[2][0] = 0.f; m->m[2][1] = 0.f; m->m[2][2] = 1.f; m->m[2][3] = 0.f;
	m->m[3][0] = 0.f; m->m[3][1] = 0.f; m->m[3][2] = 0.f; m->m[3][3] = 1.f;
}
__HINT_INLINE__ mat4 mat4Zero(){
	const mat4 r = {.m = {{0.f, 0.f, 0.f, 0.f},
	                      {0.f, 0.f, 0.f, 0.f},
	                      {0.f, 0.f, 0.f, 0.f},
	                      {0.f, 0.f, 0.f, 0.f}}};
	return r;
}
__HINT_INLINE__ void mat4ZeroP(mat4 *const restrict m){
	memset(m->m, 0, sizeof(mat4));
}

__HINT_INLINE__ mat4 mat4MMultM(const mat4 m1, const mat4 m2){

	/*size_t i, j;
	for(i = 0; i < 4; ++i){
		for(j = 0; j < 4; ++j){
			r.m[i][j] = (m2->m[i][0] * m1->m[0][j]) +
			            (m2->m[i][1] * m1->m[1][j]) +
			            (m2->m[i][2] * m1->m[2][j]) +
			            (m2->m[i][3] * m1->m[3][j]);
		}
	}*/

	const mat4 r = {.m = {{m1.m[0][0]*m2.m[0][0] + m1.m[0][1]*m2.m[1][0] + m1.m[0][2]*m2.m[2][0] + m1.m[0][3]*m2.m[3][0],
	                       m1.m[0][0]*m2.m[0][1] + m1.m[0][1]*m2.m[1][1] + m1.m[0][2]*m2.m[2][1] + m1.m[0][3]*m2.m[3][1],
	                       m1.m[0][0]*m2.m[0][2] + m1.m[0][1]*m2.m[1][2] + m1.m[0][2]*m2.m[2][2] + m1.m[0][3]*m2.m[3][2],
	                       m1.m[0][0]*m2.m[0][3] + m1.m[0][1]*m2.m[1][3] + m1.m[0][2]*m2.m[2][3] + m1.m[0][3]*m2.m[3][3]},
	                      {m1.m[1][0]*m2.m[0][0] + m1.m[1][1]*m2.m[1][0] + m1.m[1][2]*m2.m[2][0] + m1.m[1][3]*m2.m[3][0],
	                       m1.m[1][0]*m2.m[0][1] + m1.m[1][1]*m2.m[1][1] + m1.m[1][2]*m2.m[2][1] + m1.m[1][3]*m2.m[3][1],
	                       m1.m[1][0]*m2.m[0][2] + m1.m[1][1]*m2.m[1][2] + m1.m[1][2]*m2.m[2][2] + m1.m[1][3]*m2.m[3][2],
	                       m1.m[1][0]*m2.m[0][3] + m1.m[1][1]*m2.m[1][3] + m1.m[1][2]*m2.m[2][3] + m1.m[1][3]*m2.m[3][3]},
	                      {m1.m[2][0]*m2.m[0][0] + m1.m[2][1]*m2.m[1][0] + m1.m[2][2]*m2.m[2][0] + m1.m[2][3]*m2.m[3][0],
	                       m1.m[2][0]*m2.m[0][1] + m1.m[2][1]*m2.m[1][1] + m1.m[2][2]*m2.m[2][1] + m1.m[2][3]*m2.m[3][1],
	                       m1.m[2][0]*m2.m[0][2] + m1.m[2][1]*m2.m[1][2] + m1.m[2][2]*m2.m[2][2] + m1.m[2][3]*m2.m[3][2],
	                       m1.m[2][0]*m2.m[0][3] + m1.m[2][1]*m2.m[1][3] + m1.m[2][2]*m2.m[2][3] + m1.m[2][3]*m2.m[3][3]},
	                      {m1.m[3][0]*m2.m[0][0] + m1.m[3][1]*m2.m[1][0] + m1.m[3][2]*m2.m[2][0] + m1.m[3][3]*m2.m[3][0],
	                       m1.m[3][0]*m2.m[0][1] + m1.m[3][1]*m2.m[1][1] + m1.m[3][2]*m2.m[2][1] + m1.m[3][3]*m2.m[3][1],
	                       m1.m[3][0]*m2.m[0][2] + m1.m[3][1]*m2.m[1][2] + m1.m[3][2]*m2.m[2][2] + m1.m[3][3]*m2.m[3][2],
	                       m1.m[3][0]*m2.m[0][3] + m1.m[3][1]*m2.m[1][3] + m1.m[3][2]*m2.m[2][3] + m1.m[3][3]*m2.m[3][3]}}};
	return r;

}
__HINT_INLINE__ void mat4MMultMP1(mat4 *const restrict m1, const mat4 *const restrict m2){

	const mat4 r = {.m = {{m1->m[0][0]*m2->m[0][0] + m1->m[0][1]*m2->m[1][0] + m1->m[0][2]*m2->m[2][0] + m1->m[0][3]*m2->m[3][0],
	                       m1->m[0][0]*m2->m[0][1] + m1->m[0][1]*m2->m[1][1] + m1->m[0][2]*m2->m[2][1] + m1->m[0][3]*m2->m[3][1],
	                       m1->m[0][0]*m2->m[0][2] + m1->m[0][1]*m2->m[1][2] + m1->m[0][2]*m2->m[2][2] + m1->m[0][3]*m2->m[3][2],
	                       m1->m[0][0]*m2->m[0][3] + m1->m[0][1]*m2->m[1][3] + m1->m[0][2]*m2->m[2][3] + m1->m[0][3]*m2->m[3][3]},
	                      {m1->m[1][0]*m2->m[0][0] + m1->m[1][1]*m2->m[1][0] + m1->m[1][2]*m2->m[2][0] + m1->m[1][3]*m2->m[3][0],
	                       m1->m[1][0]*m2->m[0][1] + m1->m[1][1]*m2->m[1][1] + m1->m[1][2]*m2->m[2][1] + m1->m[1][3]*m2->m[3][1],
	                       m1->m[1][0]*m2->m[0][2] + m1->m[1][1]*m2->m[1][2] + m1->m[1][2]*m2->m[2][2] + m1->m[1][3]*m2->m[3][2],
	                       m1->m[1][0]*m2->m[0][3] + m1->m[1][1]*m2->m[1][3] + m1->m[1][2]*m2->m[2][3] + m1->m[1][3]*m2->m[3][3]},
	                      {m1->m[2][0]*m2->m[0][0] + m1->m[2][1]*m2->m[1][0] + m1->m[2][2]*m2->m[2][0] + m1->m[2][3]*m2->m[3][0],
	                       m1->m[2][0]*m2->m[0][1] + m1->m[2][1]*m2->m[1][1] + m1->m[2][2]*m2->m[2][1] + m1->m[2][3]*m2->m[3][1],
	                       m1->m[2][0]*m2->m[0][2] + m1->m[2][1]*m2->m[1][2] + m1->m[2][2]*m2->m[2][2] + m1->m[2][3]*m2->m[3][2],
	                       m1->m[2][0]*m2->m[0][3] + m1->m[2][1]*m2->m[1][3] + m1->m[2][2]*m2->m[2][3] + m1->m[2][3]*m2->m[3][3]},
	                      {m1->m[3][0]*m2->m[0][0] + m1->m[3][1]*m2->m[1][0] + m1->m[3][2]*m2->m[2][0] + m1->m[3][3]*m2->m[3][0],
	                       m1->m[3][0]*m2->m[0][1] + m1->m[3][1]*m2->m[1][1] + m1->m[3][2]*m2->m[2][1] + m1->m[3][3]*m2->m[3][1],
	                       m1->m[3][0]*m2->m[0][2] + m1->m[3][1]*m2->m[1][2] + m1->m[3][2]*m2->m[2][2] + m1->m[3][3]*m2->m[3][2],
	                       m1->m[3][0]*m2->m[0][3] + m1->m[3][1]*m2->m[1][3] + m1->m[3][2]*m2->m[2][3] + m1->m[3][3]*m2->m[3][3]}}};
	*m1 = r;

}
__HINT_INLINE__ void mat4MMultMP2(const mat4 *const restrict m1, mat4 *const restrict m2){

	const mat4 r = {.m = {{m1->m[0][0]*m2->m[0][0] + m1->m[0][1]*m2->m[1][0] + m1->m[0][2]*m2->m[2][0] + m1->m[0][3]*m2->m[3][0],
	                       m1->m[0][0]*m2->m[0][1] + m1->m[0][1]*m2->m[1][1] + m1->m[0][2]*m2->m[2][1] + m1->m[0][3]*m2->m[3][1],
	                       m1->m[0][0]*m2->m[0][2] + m1->m[0][1]*m2->m[1][2] + m1->m[0][2]*m2->m[2][2] + m1->m[0][3]*m2->m[3][2],
	                       m1->m[0][0]*m2->m[0][3] + m1->m[0][1]*m2->m[1][3] + m1->m[0][2]*m2->m[2][3] + m1->m[0][3]*m2->m[3][3]},
	                      {m1->m[1][0]*m2->m[0][0] + m1->m[1][1]*m2->m[1][0] + m1->m[1][2]*m2->m[2][0] + m1->m[1][3]*m2->m[3][0],
	                       m1->m[1][0]*m2->m[0][1] + m1->m[1][1]*m2->m[1][1] + m1->m[1][2]*m2->m[2][1] + m1->m[1][3]*m2->m[3][1],
	                       m1->m[1][0]*m2->m[0][2] + m1->m[1][1]*m2->m[1][2] + m1->m[1][2]*m2->m[2][2] + m1->m[1][3]*m2->m[3][2],
	                       m1->m[1][0]*m2->m[0][3] + m1->m[1][1]*m2->m[1][3] + m1->m[1][2]*m2->m[2][3] + m1->m[1][3]*m2->m[3][3]},
	                      {m1->m[2][0]*m2->m[0][0] + m1->m[2][1]*m2->m[1][0] + m1->m[2][2]*m2->m[2][0] + m1->m[2][3]*m2->m[3][0],
	                       m1->m[2][0]*m2->m[0][1] + m1->m[2][1]*m2->m[1][1] + m1->m[2][2]*m2->m[2][1] + m1->m[2][3]*m2->m[3][1],
	                       m1->m[2][0]*m2->m[0][2] + m1->m[2][1]*m2->m[1][2] + m1->m[2][2]*m2->m[2][2] + m1->m[2][3]*m2->m[3][2],
	                       m1->m[2][0]*m2->m[0][3] + m1->m[2][1]*m2->m[1][3] + m1->m[2][2]*m2->m[2][3] + m1->m[2][3]*m2->m[3][3]},
	                      {m1->m[3][0]*m2->m[0][0] + m1->m[3][1]*m2->m[1][0] + m1->m[3][2]*m2->m[2][0] + m1->m[3][3]*m2->m[3][0],
	                       m1->m[3][0]*m2->m[0][1] + m1->m[3][1]*m2->m[1][1] + m1->m[3][2]*m2->m[2][1] + m1->m[3][3]*m2->m[3][1],
	                       m1->m[3][0]*m2->m[0][2] + m1->m[3][1]*m2->m[1][2] + m1->m[3][2]*m2->m[2][2] + m1->m[3][3]*m2->m[3][2],
	                       m1->m[3][0]*m2->m[0][3] + m1->m[3][1]*m2->m[1][3] + m1->m[3][2]*m2->m[2][3] + m1->m[3][3]*m2->m[3][3]}}};
	*m2 = r;

}
__HINT_INLINE__ void mat4MMultMPR(const mat4 *const restrict m1, const mat4 *const restrict m2, mat4 *const restrict r){

	r->m[0][0] = m1->m[0][0]*m2->m[0][0] + m1->m[0][1]*m2->m[1][0] + m1->m[0][2]*m2->m[2][0] + m1->m[0][3]*m2->m[3][0];
	r->m[0][1] = m1->m[0][0]*m2->m[0][1] + m1->m[0][1]*m2->m[1][1] + m1->m[0][2]*m2->m[2][1] + m1->m[0][3]*m2->m[3][1];
	r->m[0][2] = m1->m[0][0]*m2->m[0][2] + m1->m[0][1]*m2->m[1][2] + m1->m[0][2]*m2->m[2][2] + m1->m[0][3]*m2->m[3][2];
	r->m[0][3] = m1->m[0][0]*m2->m[0][3] + m1->m[0][1]*m2->m[1][3] + m1->m[0][2]*m2->m[2][3] + m1->m[0][3]*m2->m[3][3];

	r->m[1][0] = m1->m[1][0]*m2->m[0][0] + m1->m[1][1]*m2->m[1][0] + m1->m[1][2]*m2->m[2][0] + m1->m[1][3]*m2->m[3][0];
	r->m[1][1] = m1->m[1][0]*m2->m[0][1] + m1->m[1][1]*m2->m[1][1] + m1->m[1][2]*m2->m[2][1] + m1->m[1][3]*m2->m[3][1];
	r->m[1][2] = m1->m[1][0]*m2->m[0][2] + m1->m[1][1]*m2->m[1][2] + m1->m[1][2]*m2->m[2][2] + m1->m[1][3]*m2->m[3][2];
	r->m[1][3] = m1->m[1][0]*m2->m[0][3] + m1->m[1][1]*m2->m[1][3] + m1->m[1][2]*m2->m[2][3] + m1->m[1][3]*m2->m[3][3];

	r->m[2][0] = m1->m[2][0]*m2->m[0][0] + m1->m[2][1]*m2->m[1][0] + m1->m[2][2]*m2->m[2][0] + m1->m[2][3]*m2->m[3][0];
	r->m[2][1] = m1->m[2][0]*m2->m[0][1] + m1->m[2][1]*m2->m[1][1] + m1->m[2][2]*m2->m[2][1] + m1->m[2][3]*m2->m[3][1];
	r->m[2][2] = m1->m[2][0]*m2->m[0][2] + m1->m[2][1]*m2->m[1][2] + m1->m[2][2]*m2->m[2][2] + m1->m[2][3]*m2->m[3][2];
	r->m[2][3] = m1->m[2][0]*m2->m[0][3] + m1->m[2][1]*m2->m[1][3] + m1->m[2][2]*m2->m[2][3] + m1->m[2][3]*m2->m[3][3];

	r->m[3][0] = m1->m[3][0]*m2->m[0][0] + m1->m[3][1]*m2->m[1][0] + m1->m[3][2]*m2->m[2][0] + m1->m[3][3]*m2->m[3][0];
	r->m[3][1] = m1->m[3][0]*m2->m[0][1] + m1->m[3][1]*m2->m[1][1] + m1->m[3][2]*m2->m[2][1] + m1->m[3][3]*m2->m[3][1];
	r->m[3][2] = m1->m[3][0]*m2->m[0][2] + m1->m[3][1]*m2->m[1][2] + m1->m[3][2]*m2->m[2][2] + m1->m[3][3]*m2->m[3][2];
	r->m[3][3] = m1->m[3][0]*m2->m[0][3] + m1->m[3][1]*m2->m[1][3] + m1->m[3][2]*m2->m[2][3] + m1->m[3][3]*m2->m[3][3];

}
__HINT_INLINE__ vec4 mat4VMultMKet(const vec4 v, const mat4 m){
	const vec4 r = {.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0],
	                .y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1],
	                .z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2],
	                .w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3]};
	return r;
}
__HINT_INLINE__ void mat4VMultMKetP(vec4 *const restrict v, const mat4 *const restrict m){
	const vec4 r = {.x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0] + v->w * m->m[3][0],
	                .y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1] + v->w * m->m[3][1],
	                .z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2] + v->w * m->m[3][2],
	                .w = v->x * m->m[0][3] + v->y * m->m[1][3] + v->z * m->m[2][3] + v->w * m->m[3][3]};
	*v = r;
}
__HINT_INLINE__ void mat4VMultMKetPR(const vec4 *const restrict v, const mat4 *const restrict m, vec4 *const restrict r){
	r->x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0] + v->w * m->m[3][0];
	r->y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1] + v->w * m->m[3][1];
	r->z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2] + v->w * m->m[3][2];
	r->w = v->x * m->m[0][3] + v->y * m->m[1][3] + v->z * m->m[2][3] + v->w * m->m[3][3];
}
__HINT_INLINE__ vec4 mat4MMultVBra(const mat4 m, const vec4 v){
	const vec4 r = {.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + v.w * m.m[0][3],
	                .y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + v.w * m.m[1][3],
	                .z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + v.w * m.m[2][3],
	                .w = v.x * m.m[3][0] + v.y * m.m[3][1] + v.z * m.m[3][2] + v.w * m.m[3][3]};
	return r;
}
__HINT_INLINE__ void mat4MMultVBraP(const mat4 *const restrict m, vec4 *const restrict v){
	const vec4 r = {.x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2] + v->w * m->m[0][3],
	                .y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2] + v->w * m->m[1][3],
	                .z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2] + v->w * m->m[2][3],
	                .w = v->x * m->m[3][0] + v->y * m->m[3][1] + v->z * m->m[3][2] + v->w * m->m[3][3]};
	*v = r;
}
__HINT_INLINE__ void mat4MMultVBraPR(const mat4 *const restrict m, const vec4 *const restrict v, vec4 *const restrict r){
	r->x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2] + v->w * m->m[0][3];
	r->y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2] + v->w * m->m[1][3];
	r->z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2] + v->w * m->m[2][3];
	r->w = v->x * m->m[3][0] + v->y * m->m[3][1] + v->z * m->m[3][2] + v->w * m->m[3][3];
}

__HINT_INLINE__ vec4 mat4NMultM(const float x, const float y, const float z, const float w, const mat4 m){
	const vec4 r = {.x = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + w * m.m[3][0],
	                .y = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + w * m.m[3][1],
	                .z = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + w * m.m[3][2],
	                .w = x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + w * m.m[3][3]};
	return r;
}
__HINT_INLINE__ void mat4NMultMP(const float x, const float y, const float z, const float w, const mat4 *const restrict m, vec4 *const restrict r){
	r->x = x * m->m[0][0] + y * m->m[1][0] + z * m->m[2][0] + w * m->m[3][0];
	r->y = x * m->m[0][1] + y * m->m[1][1] + z * m->m[2][1] + w * m->m[3][1];
	r->z = x * m->m[0][2] + y * m->m[1][2] + z * m->m[2][2] + w * m->m[3][2];
	r->w = x * m->m[0][3] + y * m->m[1][3] + z * m->m[2][3] + w * m->m[3][3];
}
__HINT_INLINE__ vec4 mat4MMultN(const mat4 m, const float x, const float y, const float z, const float w){
	const vec4 r = {.x = x * m.m[0][0] + y * m.m[0][1] + z * m.m[0][2] + w * m.m[0][3],
	                .y = x * m.m[1][0] + y * m.m[1][1] + z * m.m[1][2] + w * m.m[1][3],
	                .z = x * m.m[2][0] + y * m.m[2][1] + z * m.m[2][2] + w * m.m[2][3],
	                .w = x * m.m[3][0] + y * m.m[3][1] + z * m.m[3][2] + w * m.m[3][3]};
	return r;
}
__HINT_INLINE__ void mat4MMultNP(const mat4 *const restrict m, const float x, const float y, const float z, const float w, vec4 *r){
	r->x = x * m->m[0][0] + y * m->m[0][1] + z * m->m[0][2] + w * m->m[0][3];
	r->y = x * m->m[1][0] + y * m->m[1][1] + z * m->m[1][2] + w * m->m[1][3];
	r->z = x * m->m[2][0] + y * m->m[2][1] + z * m->m[2][2] + w * m->m[2][3];
	r->w = x * m->m[3][0] + y * m->m[3][1] + z * m->m[3][2] + w * m->m[3][3];
}

/**__HINT_INLINE__ vec3 mat4GetTransformedV(const mat4 m, const vec3 v){
	const vec3 r = {.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + m.m[0][3],
	                .y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + m.m[1][3],
	                .z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + m.m[2][3]};
	return r;
}
__HINT_INLINE__ void mat4TransformV(const mat4 *const restrict m, vec3 *const restrict v){
	const vec3 r = {.x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2] + m->m[0][3],
	                .y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2] + m->m[1][3],
	                .z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2] + m->m[2][3]};
	*v = r;
}**/

__HINT_INLINE__ mat4 mat4MAddM(const mat4 m1, const mat4 m2){
	const mat4 r = {.m = {{m1.m[0][0] + m2.m[0][0],
	                       m1.m[0][1] + m2.m[0][1],
	                       m1.m[0][2] + m2.m[0][2],
	                       m1.m[0][3] + m2.m[0][3]},
	                      {m1.m[1][0] + m2.m[1][0],
	                       m1.m[1][1] + m2.m[1][1],
	                       m1.m[1][2] + m2.m[1][2],
	                       m1.m[1][3] + m2.m[1][3]},
	                      {m1.m[2][0] + m2.m[2][0],
	                       m1.m[2][1] + m2.m[2][1],
	                       m1.m[2][2] + m2.m[2][2],
	                       m1.m[2][3] + m2.m[2][3]},
	                      {m1.m[3][0] + m2.m[3][0],
	                       m1.m[3][1] + m2.m[3][1],
	                       m1.m[3][2] + m2.m[3][2],
	                       m1.m[3][3] + m2.m[3][3]}}};
	return r;
}
__HINT_INLINE__ void mat4MAddMP(mat4 *const restrict m1, const mat4 *const restrict m2){
	m1->m[0][0] += m2->m[0][0]; m1->m[0][1] += m2->m[0][1]; m1->m[0][2] += m2->m[0][2]; m1->m[0][3] += m2->m[0][3];
	m1->m[1][0] += m2->m[1][0]; m1->m[1][1] += m2->m[1][1]; m1->m[1][2] += m2->m[1][2]; m1->m[1][3] += m2->m[1][3];
	m1->m[2][0] += m2->m[2][0]; m1->m[2][1] += m2->m[2][1]; m1->m[2][2] += m2->m[2][2]; m1->m[2][3] += m2->m[2][3];
	m1->m[3][0] += m2->m[3][0]; m1->m[3][1] += m2->m[3][1]; m1->m[3][2] += m2->m[3][2]; m1->m[3][3] += m2->m[3][3];
}
__HINT_INLINE__ void mat4MAddMPR(const mat4 *const restrict m1, const mat4 *const restrict m2, mat4 *const restrict r){
	r->m[0][0] = m1->m[0][0] + m2->m[0][0]; r->m[0][1] = m1->m[0][1] + m2->m[0][1]; r->m[0][2] = m1->m[0][2] + m2->m[0][2]; r->m[0][3] = m1->m[0][3] + m2->m[0][3];
	r->m[1][0] = m1->m[1][0] + m2->m[1][0]; r->m[1][1] = m1->m[1][1] + m2->m[1][1]; r->m[1][2] = m1->m[1][2] + m2->m[1][2]; r->m[1][3] = m1->m[1][3] + m2->m[1][3];
	r->m[2][0] = m1->m[2][0] + m2->m[2][0]; r->m[2][1] = m1->m[2][1] + m2->m[2][1]; r->m[2][2] = m1->m[2][2] + m2->m[2][2]; r->m[2][3] = m1->m[2][3] + m2->m[2][3];
	r->m[3][0] = m1->m[3][0] + m2->m[3][0]; r->m[3][1] = m1->m[3][1] + m2->m[3][1]; r->m[3][2] = m1->m[3][2] + m2->m[3][2]; r->m[3][3] = m1->m[3][3] + m2->m[3][3];
}

__HINT_INLINE__ float mat4Determinant(const mat4 m){
	return m.m[0][0] * (m.m[1][1] * (m.m[2][2]*m.m[3][3] - m.m[3][3]*m.m[2][3]) -
	                    m.m[2][1] * (m.m[1][2]*m.m[3][3] - m.m[3][3]*m.m[1][3]) +
	                    m.m[3][1] * (m.m[1][2]*m.m[2][3] - m.m[2][3]*m.m[1][3])) -
	       m.m[1][0] * (m.m[0][1] * (m.m[2][2]*m.m[3][3] - m.m[3][3]*m.m[2][3]) -
	                    m.m[2][1] * (m.m[0][2]*m.m[3][3] - m.m[3][3]*m.m[0][3]) +
	                    m.m[3][1] * (m.m[0][2]*m.m[2][3] - m.m[2][3]*m.m[0][3])) +
	       m.m[2][0] * (m.m[0][1] * (m.m[1][2]*m.m[3][3] - m.m[3][3]*m.m[1][3]) -
	                    m.m[1][1] * (m.m[0][2]*m.m[3][3] - m.m[3][3]*m.m[0][3]) +
	                    m.m[3][1] * (m.m[0][2]*m.m[1][3] - m.m[1][3]*m.m[0][3])) -
	       m.m[3][0] * (m.m[0][1] * (m.m[1][2]*m.m[2][3] - m.m[2][3]*m.m[1][3]) -
	                    m.m[1][1] * (m.m[0][2]*m.m[2][3] - m.m[2][3]*m.m[0][3]) +
	                    m.m[2][1] * (m.m[0][2]*m.m[1][3] - m.m[1][3]*m.m[0][3]));
}
__HINT_INLINE__ float mat4DeterminantP(const mat4 *const restrict m){
	return m->m[0][0] * (m->m[1][1] * (m->m[2][2]*m->m[3][3] - m->m[3][3]*m->m[2][3]) -
	                     m->m[2][1] * (m->m[1][2]*m->m[3][3] - m->m[3][3]*m->m[1][3]) +
	                     m->m[3][1] * (m->m[1][2]*m->m[2][3] - m->m[2][3]*m->m[1][3])) -
	       m->m[1][0] * (m->m[0][1] * (m->m[2][2]*m->m[3][3] - m->m[3][3]*m->m[2][3]) -
	                     m->m[2][1] * (m->m[0][2]*m->m[3][3] - m->m[3][3]*m->m[0][3]) +
	                     m->m[3][1] * (m->m[0][2]*m->m[2][3] - m->m[2][3]*m->m[0][3])) +
	       m->m[2][0] * (m->m[0][1] * (m->m[1][2]*m->m[3][3] - m->m[3][3]*m->m[1][3]) -
	                     m->m[1][1] * (m->m[0][2]*m->m[3][3] - m->m[3][3]*m->m[0][3]) +
	                     m->m[3][1] * (m->m[0][2]*m->m[1][3] - m->m[1][3]*m->m[0][3])) -
	       m->m[3][0] * (m->m[0][1] * (m->m[1][2]*m->m[2][3] - m->m[2][3]*m->m[1][3]) -
	                     m->m[1][1] * (m->m[0][2]*m->m[2][3] - m->m[2][3]*m->m[0][3]) +
	                     m->m[2][1] * (m->m[0][2]*m->m[1][3] - m->m[1][3]*m->m[0][3]));
}

__HINT_INLINE__ mat4 mat4Transpose(const mat4 m){
	const mat4 r = {.m = {{m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0]},
	                      {m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1]},
	                      {m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2]},
	                      {m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]}}};
	return r;
}
__HINT_INLINE__ void mat4TransposeP(mat4 *const restrict m){
	float swap = m->m[1][0];
	m->m[1][0] = m->m[0][1];
	m->m[0][1] = swap;
	swap = m->m[2][0];
	m->m[2][0] = m->m[0][2];
	m->m[0][2] = swap;
	swap = m->m[3][0];
	m->m[3][0] = m->m[0][3];
	m->m[0][3] = swap;
	swap = m->m[2][1];
	m->m[2][1] = m->m[1][2];
	m->m[1][2] = swap;
	swap = m->m[3][1];
	m->m[3][1] = m->m[1][3];
	m->m[1][3] = swap;
	swap = m->m[3][2];
	m->m[3][2] = m->m[2][3];
	m->m[2][3] = swap;
}
__HINT_INLINE__ void mat4TransposePR(const mat4 *const restrict m, mat4 *const restrict r){
	r->m[0][0] = m->m[0][0]; r->m[0][1] = m->m[1][0]; r->m[0][2] = m->m[2][0]; r->m[0][3] = m->m[3][0];
	r->m[1][0] = m->m[0][1]; r->m[1][1] = m->m[1][1]; r->m[1][2] = m->m[2][1]; r->m[1][3] = m->m[3][1];
	r->m[2][0] = m->m[0][2]; r->m[2][1] = m->m[1][2]; r->m[2][2] = m->m[2][2]; r->m[2][3] = m->m[3][2];
	r->m[3][0] = m->m[0][3]; r->m[3][1] = m->m[1][3]; r->m[3][2] = m->m[2][3]; r->m[3][3] = m->m[3][3];
}

__HINT_INLINE__ mat4 mat4Invert(const mat4 m){
	/*
	** Find the inverse using Gauss-Jordan elimination.
	*/
	///
	return m;
}
__HINT_INLINE__ return_t mat4InvertR(const mat4 m, mat4 *const restrict r){
	/*
	** Find the inverse using Gauss-Jordan elimination.
	*/
	///
	return 0;
}
__HINT_INLINE__ return_t mat4InvertP(mat4 *const restrict m){
	/*
	** Find the inverse using Gauss-Jordan elimination.
	*/
	///
	return 0;
}
__HINT_INLINE__ return_t mat4InvertPR(const mat4 *const restrict m, mat4 *const restrict r){
	/*
	** Find the inverse using Gauss-Jordan elimination.
	*/
	///
	return 0;
}

__HINT_INLINE__ mat4 mat4Frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar){
	if(left != right && bottom != top && zNear != zFar){
		const mat4 r = {.m = {{2.f*zNear/(right-left),    0.f,                       0.f,                          0.f},
		                      {0.f,                       2.f*zNear/(top-bottom),    0.f,                          0.f},
		                      {(right+left)/(right-left), (top+bottom)/(top-bottom), -(zFar+zNear)/(zFar-zNear),  -1.f},
		                      {0.f,                       0.f,                       2.f*zFar*zNear/(zFar-zNear),  0.f}}};
		return r;
	}
	return mat4Identity();
}
__HINT_INLINE__ return_t mat4FrustumP(mat4 *const restrict m, const float left, const float right, const float bottom, const float top, const float zNear, const float zFar){
	if(left != right && bottom != top && zNear != zFar){
		m->m[0][0] = 2.f*zNear/(right-left);    m->m[0][1] = 0.f;                       m->m[0][2] = 0.f;                         m->m[0][3] = 0.f;
		m->m[1][0] = 0.f;                       m->m[1][1] = 2.f*zNear/(top-bottom);    m->m[1][2] = 0.f;                         m->m[1][3] = 0.f;
		m->m[2][0] = (right+left)/(right-left); m->m[2][1] = (top+bottom)/(top-bottom); m->m[2][2] = -(zFar+zNear)/(zFar-zNear);  m->m[2][3] = -1.f;
		m->m[3][0] = 0.f;                       m->m[3][1] = 0.f;                       m->m[3][2] = 2.f*zFar*zNear/(zFar-zNear); m->m[3][3] = 0.f;
		return 1;
	}
	return 0;
}
__HINT_INLINE__ mat4 mat4Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar){
	if(left != right && bottom != top && zNear != zFar){
		const mat4 r = {.m = {{2.f/(right-left),             0.f,                          0.f,                          0.f},
		                      {0.f,                          2.f/(top-bottom),             0.f,                          0.f},
		                      {0.f,                          0.f,                          -2.f/(zFar-zNear),            0.f},
		                      {-((right+left)/(right-left)), -((top+bottom)/(top-bottom)), -((zFar+zNear)/(zFar-zNear)), 1.f}}};
		return r;
	}
	return mat4Identity();
}
__HINT_INLINE__ return_t mat4OrthoP(mat4 *const restrict m, const float left, const float right, const float bottom, const float top, const float zNear, const float zFar){
	if(left != right && bottom != top && zNear != zFar){
		m->m[0][0] = 2.f/(right-left);             m->m[0][1] = 0.f;                          m->m[0][2] = 0.f;                          m->m[0][3] = 0.f;
		m->m[1][0] = 0.f;                          m->m[1][1] = 2.f/(top-bottom);             m->m[1][2] = 0.f;                          m->m[1][3] = 0.f;
		m->m[2][0] = 0.f;                          m->m[2][1] = 0.f;                          m->m[2][2] = -2.f/(zFar-zNear);            m->m[2][3] = 0.f;
		m->m[3][0] = -((right+left)/(right-left)); m->m[3][1] = -((top+bottom)/(top-bottom)); m->m[3][2] = -((zFar+zNear)/(zFar-zNear)); m->m[3][3] = 1.f;
		return 1;
	}
	return 0;
}
__HINT_INLINE__ mat4 mat4Perspective(const float fovy, const float aspectRatio, const float zNear, const float zFar){
	if(fovy != 0.f && aspectRatio != 0.f && zNear != zFar){
		const float scale = tanf(fovy * 0.5f);
		// Currently right-handed for OpenGL. For left-handed, use the additive inverses of the values in the third column.
		const mat4 r = {.m = {{1.f/(scale*aspectRatio), 0.f,       0.f,                          0.f},
		                      {0.f,                     1.f/scale, 0.f,                          0.f},
		                      {0.f,                     0.f,       -(zFar+zNear)/(zFar-zNear),   -1.f},
		                      {0.f,                     0.f,       -2.f*zFar*zNear/(zFar-zNear), 0.f}}};
		return r;
	}
	return mat4Identity();
}
__HINT_INLINE__ return_t mat4PerspectiveP(mat4 *const restrict m, const float fovy, const float aspectRatio, const float zNear, const float zFar){
	if(fovy != 0.f && aspectRatio != 0.f && zNear != zFar){
		const float scale = tanf(fovy * 0.5f);
		// Currently right-handed for OpenGL. For left-handed, use the additive inverses of the values in the third column.
		m->m[0][0] = 1.f/(scale*aspectRatio); m->m[0][1] = 0.f;       m->m[0][2] = 0.f;                          m->m[0][3] = 0.f;
		m->m[1][0] = 0.f;                     m->m[1][1] = 1.f/scale; m->m[1][2] = 0.f;                          m->m[1][3] = 0.f;
		m->m[2][0] = 0.f;                     m->m[2][1] = 0.f;       m->m[2][2] = -(zFar+zNear)/(zFar-zNear);   m->m[2][3] = -1.f;
		m->m[3][0] = 0.f;                     m->m[3][1] = 0.f;       m->m[3][2] = -2.f*zFar*zNear/(zFar-zNear); m->m[3][3] = 0.f;
		return 1;
	}
	return 0;
}
__HINT_INLINE__ mat4 mat4RotateToFace(const vec3 eye, const vec3 target, const vec3 up){
	const vec3 zAxis = vec3NormalizeFast(vec3VSubV(target, eye));
	const vec3 xAxis = vec3NormalizeFast(vec3Cross(zAxis, up));
	const vec3 yAxis = vec3NormalizeFast(vec3Cross(xAxis, zAxis));
	// Currently right-handed for OpenGL. For left-handed, use the additive inverses of the values in the third row.
	const mat4 r = {.m = {{xAxis.x,  xAxis.y,  xAxis.z,  0.f},
	                      {yAxis.x,  yAxis.y,  yAxis.z,  0.f},
	                      {-zAxis.x, -zAxis.y, -zAxis.z, 0.f},
	                      {0.f,      0.f,      0.f,      1.f}}};
	return r;
}
__HINT_INLINE__ void mat4RotateToFaceP(mat4 *const restrict m, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict up){
	vec3 xAxis, yAxis, zAxis;
	vec3VSubVPR(target, eye, &zAxis);
	vec3NormalizeFastP(&zAxis);
	vec3CrossPR(&zAxis, up, &xAxis);
	vec3NormalizeFastP(&xAxis);
	vec3CrossPR(&xAxis, &zAxis, &yAxis);
	vec3NormalizeFastP(&yAxis);
	// Currently right-handed for OpenGL. For left-handed, use the additive inverses of the values in the third row.
	m->m[0][0] =  xAxis.x; m->m[0][1] =  xAxis.y; m->m[0][2] =  xAxis.z; m->m[0][3] = 0.f;
	m->m[1][0] =  yAxis.x; m->m[1][1] =  yAxis.y; m->m[1][2] =  yAxis.z; m->m[1][3] = 0.f;
	m->m[2][0] = -zAxis.x; m->m[2][1] = -zAxis.y; m->m[2][2] = -zAxis.z; m->m[2][3] = 0.f;
	m->m[3][0] = 0.f;      m->m[3][1] = 0.f;      m->m[3][2] = 0.f;      m->m[3][3] = 1.f;
}
__HINT_INLINE__ mat4 mat4LookAt(const vec3 eye, const vec3 target, const vec3 up){
	const vec3 zAxis = vec3NormalizeFast(vec3VSubV(target, eye));
	const vec3 xAxis = vec3NormalizeFast(vec3Cross(zAxis, up));
	const vec3 yAxis = vec3NormalizeFast(vec3Cross(xAxis, zAxis));
	// Currently right-handed for OpenGL. For left-handed, use the additive inverses of the values in the third column.
	const mat4 r = {.m = {{xAxis.x,              yAxis.x,              -zAxis.x,            0.f},
	                      {xAxis.y,              yAxis.y,              -zAxis.y,            0.f},
	                      {xAxis.z,              yAxis.z,              -zAxis.z,            0.f},
	                      {-vec3Dot(xAxis, eye), -vec3Dot(yAxis, eye), vec3Dot(zAxis, eye), 1.f}}};
	return r;
}
__HINT_INLINE__ void mat4LookAtP(mat4 *const restrict m, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict up){
	vec3 xAxis, yAxis, zAxis;
	vec3VSubVPR(target, eye, &zAxis);
	vec3NormalizeFastP(&zAxis);
	vec3CrossPR(&zAxis, up, &xAxis);
	vec3NormalizeFastP(&xAxis);
	vec3CrossPR(&xAxis, &zAxis, &yAxis);
	vec3NormalizeFastP(&yAxis);
	// Currently right-handed for OpenGL. For left-handed, use the additive inverses of the values in the third column.
	m->m[0][0] = xAxis.x;                m->m[0][1] = yAxis.x;                m->m[0][2] = -zAxis.x;              m->m[0][3] = 0.f;
	m->m[1][0] = xAxis.y;                m->m[1][1] = yAxis.y;                m->m[1][2] = -zAxis.y;              m->m[1][3] = 0.f;
	m->m[2][0] = xAxis.z;                m->m[2][1] = yAxis.z;                m->m[2][2] = -zAxis.z;              m->m[2][3] = 0.f;
	m->m[3][0] = -vec3DotP(&xAxis, eye); m->m[3][1] = -vec3DotP(&yAxis, eye); m->m[3][2] = vec3DotP(&zAxis, eye); m->m[3][3] = 1.f;
}

__HINT_INLINE__ mat4 mat4TranslationMatrix(const float x, const float y, const float z){
	const mat4 r = {.m = {{1.f, 0.f, 0.f, 0.f},
	                      {0.f, 1.f, 0.f, 0.f},
	                      {0.f, 0.f, 1.f, 0.f},
	                      {  x,   y,   z, 1.f}}};
	return r;
}
__HINT_INLINE__ void mat4TranslationMatrixP(mat4 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] = 1.f; m->m[0][1] = 0.f; m->m[0][2] = 0.f; m->m[0][3] = 0.f;
	m->m[1][0] = 0.f; m->m[1][1] = 1.f; m->m[1][2] = 0.f; m->m[1][3] = 0.f;
	m->m[2][0] = 0.f; m->m[2][1] = 0.f; m->m[2][2] = 1.f; m->m[2][3] = 0.f;
	m->m[3][0] = x;   m->m[3][1] = y;   m->m[3][2] = z;   m->m[3][3] = 1.f;
}
__HINT_INLINE__ mat4 mat4Translate(const mat4 m, const float x, const float y, const float z){
	const mat4 r = {.m = {{m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3]},
	                      {m.m[1][0], m.m[1][1], m.m[1][2], m.m[1][3]},
	                      {m.m[2][0], m.m[2][1], m.m[2][2], m.m[2][3]},
	                      {m.m[0][0] * x + m.m[1][0] * y + m.m[2][0] * z + m.m[3][0],
	                       m.m[0][1] * x + m.m[1][1] * y + m.m[2][1] * z + m.m[3][1],
	                       m.m[0][2] * x + m.m[1][2] * y + m.m[2][2] * z + m.m[3][2],
	                       m.m[0][3] * x + m.m[1][3] * y + m.m[2][3] * z + m.m[3][3]}}};
	return r;
}
__HINT_INLINE__ void mat4TranslateP(mat4 *const restrict m, const float x, const float y, const float z){
	m->m[3][0] = m->m[0][0] * x + m->m[1][0] * y + m->m[2][0] * z + m->m[3][0];
	m->m[3][1] = m->m[0][1] * x + m->m[1][1] * y + m->m[2][1] * z + m->m[3][1];
	m->m[3][2] = m->m[0][2] * x + m->m[1][2] * y + m->m[2][2] * z + m->m[3][2];
	m->m[3][3] = m->m[0][3] * x + m->m[1][3] * y + m->m[2][3] * z + m->m[3][3];
}
__HINT_INLINE__ void mat4TranslatePR(const mat4 *const restrict m, const float x, const float y, const float z, mat4 *const restrict r){
	r->m[0][0] = m->m[0][0]; r->m[0][1] = m->m[0][1]; r->m[0][2] = m->m[0][2]; r->m[0][3] = m->m[0][3];
	r->m[1][0] = m->m[1][0]; r->m[1][1] = m->m[1][1]; r->m[1][2] = m->m[1][2]; r->m[1][3] = m->m[1][3];
	r->m[2][0] = m->m[2][0]; r->m[2][1] = m->m[2][1]; r->m[2][2] = m->m[2][2]; r->m[2][3] = m->m[2][3];
	r->m[3][0] = m->m[0][0] * x + m->m[1][0] * y + m->m[2][0] * z + m->m[3][0];
	r->m[3][1] = m->m[0][1] * x + m->m[1][1] * y + m->m[2][1] * z + m->m[3][1];
	r->m[3][2] = m->m[0][2] * x + m->m[1][2] * y + m->m[2][2] * z + m->m[3][2];
	r->m[3][3] = m->m[0][3] * x + m->m[1][3] * y + m->m[2][3] * z + m->m[3][3];
}

__HINT_INLINE__ mat4 mat4RotationMatrix(const quat q){
	return mat4Quaternion(q);
}
__HINT_INLINE__ void mat4RotationMatrixP(mat4 *const restrict m, const quat *const restrict q){
	mat4QuaternionP(m, q);
}
__HINT_INLINE__ mat4 mat4Rotate(const mat4 m, const quat q){
	return mat4MMultM(mat4Quaternion(q), m);
}
__HINT_INLINE__ void mat4RotateP(mat4 *const restrict m, const quat *const restrict q){
	mat4 r;
	mat4QuaternionP(&r, q);
	mat4MMultMP2(&r, m);
}
__HINT_INLINE__ void mat4RotatePR(const mat4 *const restrict m, const quat *const restrict q, mat4 *const restrict r){
	mat4QuaternionP(r, q);
	mat4MMultMP1(r, m);
}

mat4 mat4ScaleMatrix(const float x, const float y, const float z){
	const mat4 r = {.m = {{  x, 0.f, 0.f, 0.f},
	                      {0.f,   y, 0.f, 0.f},
	                      {0.f, 0.f,   z, 0.f},
	                      {0.f, 0.f, 0.f, 1.f}}};
	return r;
}
__HINT_INLINE__ void mat4ScaleMatrixP(mat4 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] = x;   m->m[0][1] = 0.f; m->m[0][2] = 0.f; m->m[0][3] = 0.f;
	m->m[1][0] = 0.f; m->m[1][1] = y;   m->m[1][2] = 0.f; m->m[1][3] = 0.f;
	m->m[2][0] = 0.f; m->m[2][1] = 0.f; m->m[2][2] = z;   m->m[2][3] = 0.f;
	m->m[3][0] = 0.f; m->m[3][1] = 0.f; m->m[3][2] = 0.f; m->m[3][3] = 1.f;
}
__HINT_INLINE__ mat4 mat4Scale(const mat4 m, const float x, const float y, const float z){
	const mat4 r = {.m = {{m.m[0][0] * x, m.m[0][1] * x, m.m[0][2] * x, m.m[0][3] * x},
	                      {m.m[1][0] * y, m.m[1][1] * y, m.m[1][2] * y, m.m[1][3] * y},
	                      {m.m[2][0] * z, m.m[2][1] * z, m.m[2][2] * z, m.m[2][3] * z},
	                      {m.m[3][0],     m.m[3][1],     m.m[3][2],     m.m[3][3]}}};
	return r;
}
__HINT_INLINE__ void mat4ScaleP(mat4 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] *= x; m->m[0][1] *= x; m->m[0][2] *= x; m->m[0][3] *= x;
	m->m[1][0] *= y; m->m[1][1] *= y; m->m[1][2] *= y; m->m[1][3] *= y;
	m->m[2][0] *= z; m->m[2][1] *= z; m->m[2][2] *= z; m->m[2][3] *= z;
}
__HINT_INLINE__ void mat4ScalePR(const mat4 *const restrict m, const float x, const float y, const float z, mat4 *r){
	r->m[0][0] = m->m[0][0] * x; r->m[0][1] = m->m[0][1] * x; r->m[0][2] = m->m[0][2] * x; r->m[0][3] = m->m[0][3] * x;
	r->m[1][0] = m->m[1][0] * y; r->m[1][1] = m->m[1][1] * y; r->m[1][2] = m->m[1][2] * y; r->m[1][3] = m->m[1][3] * y;
	r->m[2][0] = m->m[2][0] * z; r->m[2][1] = m->m[2][1] * z; r->m[2][2] = m->m[2][2] * z; r->m[2][3] = m->m[2][3] * z;
}

__HINT_INLINE__ mat4 mat4Quaternion(const quat q){
	const float xx = q.v.x*q.v.x;
	const float yy = q.v.y*q.v.y;
	const float zz = q.v.z*q.v.z;
	const float xy = q.v.x*q.v.y;
	const float xz = q.v.x*q.v.z;
	const float xw = q.v.x*q.w;
	const float yz = q.v.y*q.v.z;
	const float yw = q.v.y*q.w;
	const float zw = q.v.z*q.w;
	const mat4 r = {.m = {{1.f-2.f*(yy+zz), 2.f*(xy+zw),     2.f*(xz-yw),     0.f},
	                      {2.f*(xy-zw),     1.f-2.f*(xx+zz), 2.f*(yz+xw),     0.f},
	                      {2.f*(xz+yw),     2.f*(yz-xw),     1.f-2.f*(xx+yy), 0.f},
	                      {0.f,             0.f,             0.f,             1.f}}};
	return r;
}
__HINT_INLINE__ void mat4QuaternionP(mat4 *const restrict m, const quat *const restrict q){
	const float xx = q->v.x*q->v.x;
	const float yy = q->v.y*q->v.y;
	const float zz = q->v.z*q->v.z;
	const float xy = q->v.x*q->v.y;
	const float xz = q->v.x*q->v.z;
	const float xw = q->v.x*q->w;
	const float yz = q->v.y*q->v.z;
	const float yw = q->v.y*q->w;
	const float zw = q->v.z*q->w;
	m->m[0][0] = 1.f-2.f*(yy+zz); m->m[0][1] = 2.f*(xy+zw);     m->m[0][2] = 2.f*(xz-yw);     m->m[0][3] = 0.f;
	m->m[1][0] = 2.f*(xy-zw);     m->m[1][1] = 1.f-2.f*(xx+zz); m->m[1][2] = 2.f*(yz+xw);     m->m[1][3] = 0.f;
	m->m[2][0] = 2.f*(xz+yw);     m->m[2][1] = 2.f*(yz-xw);     m->m[2][2] = 1.f-2.f*(xx+yy); m->m[2][3] = 0.f;
	m->m[3][0] = 0.f;             m->m[3][1] = 0.f;             m->m[3][2] = 0.f;             m->m[3][3] = 1.f;
}
