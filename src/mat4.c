#include "mat4.h"
#include <stddef.h>
#include <math.h>

void mat4Identity(mat4 *m){
	m->m[0][0] = 1.f; m->m[0][1] = 0.f; m->m[0][2] = 0.f; m->m[0][3] = 0.f;
	m->m[1][0] = 0.f; m->m[1][1] = 1.f; m->m[1][2] = 0.f; m->m[1][3] = 0.f;
	m->m[2][0] = 0.f; m->m[2][1] = 0.f; m->m[2][2] = 1.f; m->m[2][3] = 0.f;
	m->m[3][0] = 0.f; m->m[3][1] = 0.f; m->m[3][2] = 0.f; m->m[3][3] = 1.f;
}
mat4 mat4GetIdentity(){
	mat4 r = {.m = {{1.f, 0.f, 0.f, 0.f},
	                {0.f, 1.f, 0.f, 0.f},
	                {0.f, 0.f, 1.f, 0.f},
	                {0.f, 0.f, 0.f, 1.f}}};
	return r;
}

mat4 mat4MMultM(const mat4 *m1, const mat4 *m2){

	mat4 r;

	/*size_t i, j;
	for(i = 0; i < 4; ++i){
		for(j = 0; j < 4; ++j){
			r.m[i][j] = (m2->m[i][0] * m1->m[0][j]) +
			            (m2->m[i][1] * m1->m[1][j]) +
			            (m2->m[i][2] * m1->m[2][j]) +
			            (m2->m[i][3] * m1->m[3][j]);
		}
	}*/

	r.m[0][0] = m1->m[0][0]*m2->m[0][0] + m1->m[0][1]*m2->m[1][0] + m1->m[0][2]*m2->m[2][0] + m1->m[0][3]*m2->m[3][0];
	r.m[0][1] = m1->m[0][0]*m2->m[0][1] + m1->m[0][1]*m2->m[1][1] + m1->m[0][2]*m2->m[2][1] + m1->m[0][3]*m2->m[3][1];
	r.m[0][2] = m1->m[0][0]*m2->m[0][2] + m1->m[0][1]*m2->m[1][2] + m1->m[0][2]*m2->m[2][2] + m1->m[0][3]*m2->m[3][2];
	r.m[0][3] = m1->m[0][0]*m2->m[0][3] + m1->m[0][1]*m2->m[1][3] + m1->m[0][2]*m2->m[2][3] + m1->m[0][3]*m2->m[3][3];

	r.m[1][0] = m1->m[1][0]*m2->m[0][0] + m1->m[1][1]*m2->m[1][0] + m1->m[1][2]*m2->m[2][0] + m1->m[1][3]*m2->m[3][0];
	r.m[1][1] = m1->m[1][0]*m2->m[0][1] + m1->m[1][1]*m2->m[1][1] + m1->m[1][2]*m2->m[2][1] + m1->m[1][3]*m2->m[3][1];
	r.m[1][2] = m1->m[1][0]*m2->m[0][2] + m1->m[1][1]*m2->m[1][2] + m1->m[1][2]*m2->m[2][2] + m1->m[1][3]*m2->m[3][2];
	r.m[1][3] = m1->m[1][0]*m2->m[0][3] + m1->m[1][1]*m2->m[1][3] + m1->m[1][2]*m2->m[2][3] + m1->m[1][3]*m2->m[3][3];

	r.m[2][0] = m1->m[2][0]*m2->m[0][0] + m1->m[2][1]*m2->m[1][0] + m1->m[2][2]*m2->m[2][0] + m1->m[2][3]*m2->m[3][0];
	r.m[2][1] = m1->m[2][0]*m2->m[0][1] + m1->m[2][1]*m2->m[1][1] + m1->m[2][2]*m2->m[2][1] + m1->m[2][3]*m2->m[3][1];
	r.m[2][2] = m1->m[2][0]*m2->m[0][2] + m1->m[2][1]*m2->m[1][2] + m1->m[2][2]*m2->m[2][2] + m1->m[2][3]*m2->m[3][2];
	r.m[2][3] = m1->m[2][0]*m2->m[0][3] + m1->m[2][1]*m2->m[1][3] + m1->m[2][2]*m2->m[2][3] + m1->m[2][3]*m2->m[3][3];

	r.m[3][0] = m1->m[3][0]*m2->m[0][0] + m1->m[3][1]*m2->m[1][0] + m1->m[3][2]*m2->m[2][0] + m1->m[3][3]*m2->m[3][0];
	r.m[3][1] = m1->m[3][0]*m2->m[0][1] + m1->m[3][1]*m2->m[1][1] + m1->m[3][2]*m2->m[2][1] + m1->m[3][3]*m2->m[3][1];
	r.m[3][2] = m1->m[3][0]*m2->m[0][2] + m1->m[3][1]*m2->m[1][2] + m1->m[3][2]*m2->m[2][2] + m1->m[3][3]*m2->m[3][2];
	r.m[3][3] = m1->m[3][0]*m2->m[0][3] + m1->m[3][1]*m2->m[1][3] + m1->m[3][2]*m2->m[2][3] + m1->m[3][3]*m2->m[3][3];

	return r;

}
void mat4MultMByM1(mat4 *m1, const mat4 *m2){

	mat4 r;

	r.m[0][0] = m1->m[0][0]*m2->m[0][0] + m1->m[0][1]*m2->m[1][0] + m1->m[0][2]*m2->m[2][0] + m1->m[0][3]*m2->m[3][0];
	r.m[0][1] = m1->m[0][0]*m2->m[0][1] + m1->m[0][1]*m2->m[1][1] + m1->m[0][2]*m2->m[2][1] + m1->m[0][3]*m2->m[3][1];
	r.m[0][2] = m1->m[0][0]*m2->m[0][2] + m1->m[0][1]*m2->m[1][2] + m1->m[0][2]*m2->m[2][2] + m1->m[0][3]*m2->m[3][2];
	r.m[0][3] = m1->m[0][0]*m2->m[0][3] + m1->m[0][1]*m2->m[1][3] + m1->m[0][2]*m2->m[2][3] + m1->m[0][3]*m2->m[3][3];

	r.m[1][0] = m1->m[1][0]*m2->m[0][0] + m1->m[1][1]*m2->m[1][0] + m1->m[1][2]*m2->m[2][0] + m1->m[1][3]*m2->m[3][0];
	r.m[1][1] = m1->m[1][0]*m2->m[0][1] + m1->m[1][1]*m2->m[1][1] + m1->m[1][2]*m2->m[2][1] + m1->m[1][3]*m2->m[3][1];
	r.m[1][2] = m1->m[1][0]*m2->m[0][2] + m1->m[1][1]*m2->m[1][2] + m1->m[1][2]*m2->m[2][2] + m1->m[1][3]*m2->m[3][2];
	r.m[1][3] = m1->m[1][0]*m2->m[0][3] + m1->m[1][1]*m2->m[1][3] + m1->m[1][2]*m2->m[2][3] + m1->m[1][3]*m2->m[3][3];

	r.m[2][0] = m1->m[2][0]*m2->m[0][0] + m1->m[2][1]*m2->m[1][0] + m1->m[2][2]*m2->m[2][0] + m1->m[2][3]*m2->m[3][0];
	r.m[2][1] = m1->m[2][0]*m2->m[0][1] + m1->m[2][1]*m2->m[1][1] + m1->m[2][2]*m2->m[2][1] + m1->m[2][3]*m2->m[3][1];
	r.m[2][2] = m1->m[2][0]*m2->m[0][2] + m1->m[2][1]*m2->m[1][2] + m1->m[2][2]*m2->m[2][2] + m1->m[2][3]*m2->m[3][2];
	r.m[2][3] = m1->m[2][0]*m2->m[0][3] + m1->m[2][1]*m2->m[1][3] + m1->m[2][2]*m2->m[2][3] + m1->m[2][3]*m2->m[3][3];

	r.m[3][0] = m1->m[3][0]*m2->m[0][0] + m1->m[3][1]*m2->m[1][0] + m1->m[3][2]*m2->m[2][0] + m1->m[3][3]*m2->m[3][0];
	r.m[3][1] = m1->m[3][0]*m2->m[0][1] + m1->m[3][1]*m2->m[1][1] + m1->m[3][2]*m2->m[2][1] + m1->m[3][3]*m2->m[3][1];
	r.m[3][2] = m1->m[3][0]*m2->m[0][2] + m1->m[3][1]*m2->m[1][2] + m1->m[3][2]*m2->m[2][2] + m1->m[3][3]*m2->m[3][2];
	r.m[3][3] = m1->m[3][0]*m2->m[0][3] + m1->m[3][1]*m2->m[1][3] + m1->m[3][2]*m2->m[2][3] + m1->m[3][3]*m2->m[3][3];

	*m1 = r;

}
void mat4MultMByM2(const mat4 *m1, mat4 *m2){

	mat4 r;

	r.m[0][0] = m1->m[0][0]*m2->m[0][0] + m1->m[0][1]*m2->m[1][0] + m1->m[0][2]*m2->m[2][0] + m1->m[0][3]*m2->m[3][0];
	r.m[0][1] = m1->m[0][0]*m2->m[0][1] + m1->m[0][1]*m2->m[1][1] + m1->m[0][2]*m2->m[2][1] + m1->m[0][3]*m2->m[3][1];
	r.m[0][2] = m1->m[0][0]*m2->m[0][2] + m1->m[0][1]*m2->m[1][2] + m1->m[0][2]*m2->m[2][2] + m1->m[0][3]*m2->m[3][2];
	r.m[0][3] = m1->m[0][0]*m2->m[0][3] + m1->m[0][1]*m2->m[1][3] + m1->m[0][2]*m2->m[2][3] + m1->m[0][3]*m2->m[3][3];

	r.m[1][0] = m1->m[1][0]*m2->m[0][0] + m1->m[1][1]*m2->m[1][0] + m1->m[1][2]*m2->m[2][0] + m1->m[1][3]*m2->m[3][0];
	r.m[1][1] = m1->m[1][0]*m2->m[0][1] + m1->m[1][1]*m2->m[1][1] + m1->m[1][2]*m2->m[2][1] + m1->m[1][3]*m2->m[3][1];
	r.m[1][2] = m1->m[1][0]*m2->m[0][2] + m1->m[1][1]*m2->m[1][2] + m1->m[1][2]*m2->m[2][2] + m1->m[1][3]*m2->m[3][2];
	r.m[1][3] = m1->m[1][0]*m2->m[0][3] + m1->m[1][1]*m2->m[1][3] + m1->m[1][2]*m2->m[2][3] + m1->m[1][3]*m2->m[3][3];

	r.m[2][0] = m1->m[2][0]*m2->m[0][0] + m1->m[2][1]*m2->m[1][0] + m1->m[2][2]*m2->m[2][0] + m1->m[2][3]*m2->m[3][0];
	r.m[2][1] = m1->m[2][0]*m2->m[0][1] + m1->m[2][1]*m2->m[1][1] + m1->m[2][2]*m2->m[2][1] + m1->m[2][3]*m2->m[3][1];
	r.m[2][2] = m1->m[2][0]*m2->m[0][2] + m1->m[2][1]*m2->m[1][2] + m1->m[2][2]*m2->m[2][2] + m1->m[2][3]*m2->m[3][2];
	r.m[2][3] = m1->m[2][0]*m2->m[0][3] + m1->m[2][1]*m2->m[1][3] + m1->m[2][2]*m2->m[2][3] + m1->m[2][3]*m2->m[3][3];

	r.m[3][0] = m1->m[3][0]*m2->m[0][0] + m1->m[3][1]*m2->m[1][0] + m1->m[3][2]*m2->m[2][0] + m1->m[3][3]*m2->m[3][0];
	r.m[3][1] = m1->m[3][0]*m2->m[0][1] + m1->m[3][1]*m2->m[1][1] + m1->m[3][2]*m2->m[2][1] + m1->m[3][3]*m2->m[3][1];
	r.m[3][2] = m1->m[3][0]*m2->m[0][2] + m1->m[3][1]*m2->m[1][2] + m1->m[3][2]*m2->m[2][2] + m1->m[3][3]*m2->m[3][2];
	r.m[3][3] = m1->m[3][0]*m2->m[0][3] + m1->m[3][1]*m2->m[1][3] + m1->m[3][2]*m2->m[2][3] + m1->m[3][3]*m2->m[3][3];

	*m2 = r;

}
void mat4MultMByMR(const mat4 *m1, const mat4 *m2, mat4 *r){

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
vec4 mat4VMultM(vec4 v, const mat4 *m){
	vec4 r;
	r.x = v.x * m->m[0][0] + v.y * m->m[0][1] + v.z * m->m[0][2] + v.w * m->m[0][3];
	r.y = v.x * m->m[1][0] + v.y * m->m[1][1] + v.z * m->m[1][2] + v.w * m->m[1][3];
	r.z = v.x * m->m[2][0] + v.y * m->m[2][1] + v.z * m->m[2][2] + v.w * m->m[2][3];
	r.w = v.x * m->m[3][0] + v.y * m->m[3][1] + v.z * m->m[3][2] + v.w * m->m[3][3];
	return r;
}
void mat4MultVByM(vec4 *v, const mat4 *m){
	vec4 r;
	r.x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2] + v->w * m->m[0][3];
	r.y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2] + v->w * m->m[1][3];
	r.z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2] + v->w * m->m[2][3];
	r.w = v->x * m->m[3][0] + v->y * m->m[3][1] + v->z * m->m[3][2] + v->w * m->m[3][3];
	v->x = r.x; v->y = r.y; v->z = r.z; v->w = r.w;
}
vec4 mat4MMultV(const mat4 *m, vec4 v){
	vec4 r;
	r.x = v.x * m->m[0][0] + v.y * m->m[1][0] + v.z * m->m[2][0] + v.w * m->m[3][0];
	r.y = v.x * m->m[0][1] + v.y * m->m[1][1] + v.z * m->m[2][1] + v.w * m->m[3][1];
	r.z = v.x * m->m[0][2] + v.y * m->m[1][2] + v.z * m->m[2][2] + v.w * m->m[3][2];
	r.w = v.x * m->m[0][3] + v.y * m->m[1][3] + v.z * m->m[2][3] + v.w * m->m[3][3];
	return r;
}
void mat4MultMByV(const mat4 *m, vec4 *v){
	vec4 r;
	r.x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0] + v->w * m->m[3][0];
	r.y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1] + v->w * m->m[3][1];
	r.z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2] + v->w * m->m[3][2];
	r.w = v->x * m->m[0][3] + v->y * m->m[1][3] + v->z * m->m[2][3] + v->w * m->m[3][3];
	v->x = r.x; v->y = r.y; v->z = r.z; v->w = r.w;
}

float mat4Determinant(const mat4 *m){
	return m->m[0][0] * (m->m[1][1] * (m->m[2][2]*m->m[3][3] - m->m[2][3]*m->m[3][2]) -
	                     m->m[1][2] * (m->m[2][1]*m->m[3][3] - m->m[2][3]*m->m[3][1]) +
	                     m->m[1][3] * (m->m[2][1]*m->m[3][2] - m->m[2][2]*m->m[3][1])) -
	       m->m[0][1] * (m->m[1][0] * (m->m[2][2]*m->m[3][3] - m->m[2][3]*m->m[3][2]) -
	                     m->m[1][2] * (m->m[2][0]*m->m[3][3] - m->m[2][3]*m->m[3][0]) +
	                     m->m[1][3] * (m->m[2][0]*m->m[3][2] - m->m[2][2]*m->m[3][0])) +
	       m->m[0][2] * (m->m[1][0] * (m->m[2][1]*m->m[3][3] - m->m[2][3]*m->m[3][1]) -
	                     m->m[1][1] * (m->m[2][0]*m->m[3][3] - m->m[2][3]*m->m[3][0]) +
	                     m->m[1][3] * (m->m[2][0]*m->m[3][1] - m->m[2][1]*m->m[3][0])) -
	       m->m[0][3] * (m->m[1][0] * (m->m[2][1]*m->m[3][2] - m->m[2][2]*m->m[3][1]) -
	                     m->m[1][1] * (m->m[2][0]*m->m[3][2] - m->m[2][2]*m->m[3][0]) +
	                     m->m[1][2] * (m->m[2][0]*m->m[3][1] - m->m[2][1]*m->m[3][0]));
}

mat4 mat4GetTranspose(const mat4 *m){
	mat4 r = {.m = {{m->m[0][0], m->m[1][0], m->m[2][0], m->m[3][0]},
	                {m->m[0][1], m->m[1][1], m->m[2][1], m->m[3][1]},
	                {m->m[0][2], m->m[1][2], m->m[2][2], m->m[3][2]},
	                {m->m[0][3], m->m[1][3], m->m[2][3], m->m[3][3]}}};
	return r;
}
void mat4Transpose(mat4 *m){
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
void mat4TransposeR(const mat4 *m, mat4 *r){
	r->m[0][0] = m->m[0][0]; r->m[0][1] = m->m[1][0]; r->m[0][2] = m->m[2][0]; r->m[0][3] = m->m[3][0];
	r->m[1][0] = m->m[0][1]; r->m[1][1] = m->m[1][1]; r->m[1][2] = m->m[2][1]; r->m[1][3] = m->m[3][1];
	r->m[2][0] = m->m[0][2]; r->m[2][1] = m->m[1][2]; r->m[2][2] = m->m[2][2]; r->m[2][3] = m->m[3][2];
	r->m[3][0] = m->m[0][3]; r->m[3][1] = m->m[1][3]; r->m[3][2] = m->m[2][3]; r->m[3][3] = m->m[3][3];
}

signed char mat4Invert(mat4 *m){
	/* Find the inverse using Gauss-Jordan elimination. */
	return 0;
}
signed char mat4InvertR(const mat4 *m, mat4 *r){
	/* Find the inverse using Gauss-Jordan elimination. */
	return 0;
}

signed char mat4Frustum(mat4 *m, const float left, const float right, const float bottom, const float top, const float zNear, const float zFar){
	if(left == right || bottom == top || zNear == zFar){
		return 0;
	}
	m->m[0][0] = 2.f*zNear/(right-left);    m->m[0][1] = 0.f;                       m->m[0][2] = 0.f;                         m->m[0][3] = 0.f;
	m->m[1][0] = 0.f;                       m->m[1][1] = 2.f*zNear/(top-bottom);    m->m[1][2] = 0.f;                         m->m[1][3] = 0.f;
	m->m[2][0] = (right+left)/(right-left); m->m[2][1] = (top+bottom)/(top-bottom); m->m[2][2] = -(zFar+zNear)/(zFar-zNear);  m->m[2][3] = -1.f;
	m->m[3][0] = 0.f;                       m->m[3][1] = 0.f;                       m->m[3][2] = 2.f*zFar*zNear/(zFar-zNear); m->m[3][3] = 0.f;
	return 1;
}
signed char mat4Ortho(mat4 *m, const float left, const float right, const float bottom, const float top, const float zNear, const float zFar){
	if(left == right || bottom == top || zNear == zFar){
		return 0;
	}
	m->m[0][0] = 2.f/(right-left);             m->m[0][1] = 0.f;                          m->m[0][2] = 0.f;                          m->m[0][3] = 0.f;
	m->m[1][0] = 0.f;                          m->m[1][1] = 2.f/(top-bottom);             m->m[1][2] = 0.f;                          m->m[1][3] = 0.f;
	m->m[2][0] = 0.f;                          m->m[2][1] = 0.f;                          m->m[2][2] = -2.f/(zFar-zNear);            m->m[2][3] = 0.f;
	m->m[3][0] = -((right+left)/(right-left)); m->m[3][1] = -((top+bottom)/(top-bottom)); m->m[3][2] = -((zFar+zNear)/(zFar-zNear)); m->m[3][3] = 1.f;
	return 1;
}
signed char mat4Perspective(mat4 *m, const float fovy, const float aspectRatio, const float zNear, const float zFar){
	if(fovy == 0.f || aspectRatio == 0.f || zNear == zFar){
		return 0;
	}
	const float scale = tanf(fovy * 0.5f);
	// Currently right-handed for OpenGL. For left-handed, use the additive inverses of the values in the third column
	m->m[0][0] = 1.f/(scale*aspectRatio); m->m[0][1] = 0.f;       m->m[0][2] = 0.f;                          m->m[0][3] = 0.f;
	m->m[1][0] = 0.f;                     m->m[1][1] = 1.f/scale; m->m[1][2] = 0.f;                          m->m[1][3] = 0.f;
	m->m[2][0] = 0.f;                     m->m[2][1] = 0.f;       m->m[2][2] = -(zFar+zNear)/(zFar-zNear);   m->m[2][3] = -1.f;
	m->m[3][0] = 0.f;                     m->m[3][1] = 0.f;       m->m[3][2] = -2.f*zFar*zNear/(zFar-zNear); m->m[3][3] = 0.f;
	return 1;
}
void mat4RotateToFace(mat4 *m, const vec3 *eye, const vec3 *target, const vec3 *up){
	vec3 zAxis = vec3VSubV(target, eye);
	vec3NormalizeFast(&zAxis);
	vec3 xAxis;
	vec3Cross(&zAxis, up, &xAxis);
	vec3NormalizeFast(&xAxis);
	vec3 yAxis;
	vec3Cross(&xAxis, &zAxis, &yAxis);
	vec3NormalizeFast(&yAxis);
	// Currently right-handed for OpenGL. For left-handed, use the additive inverses of the values in the third row
	m->m[0][0] =  xAxis.x; m->m[0][1] =  xAxis.y; m->m[0][2] =  xAxis.z; m->m[0][3] = 0.f;
	m->m[1][0] =  yAxis.x; m->m[1][1] =  yAxis.y; m->m[1][2] =  yAxis.z; m->m[1][3] = 0.f;
	m->m[2][0] = -zAxis.x; m->m[2][1] = -zAxis.y; m->m[2][2] = -zAxis.z; m->m[2][3] = 0.f;
	m->m[3][0] = 0.f;      m->m[3][1] = 0.f;      m->m[3][2] = 0.f;      m->m[3][3] = 1.f;
}
void mat4LookAt(mat4 *m, const vec3 *eye, const vec3 *target, const vec3 *up){
	vec3 zAxis = vec3VSubV(target, eye);
	vec3NormalizeFast(&zAxis);
	vec3 xAxis;
	vec3Cross(&zAxis, up, &xAxis);
	vec3NormalizeFast(&xAxis);
	vec3 yAxis;
	vec3Cross(&xAxis, &zAxis, &yAxis);
	vec3NormalizeFast(&yAxis);
	// Currently right-handed for OpenGL. For left-handed, use the additive inverses of the values in the third column
	m->m[0][0] = xAxis.x;               m->m[0][1] = yAxis.x;               m->m[0][2] = -zAxis.x;             m->m[0][3] = 0.f;
	m->m[1][0] = xAxis.y;               m->m[1][1] = yAxis.y;               m->m[1][2] = -zAxis.y;             m->m[1][3] = 0.f;
	m->m[2][0] = xAxis.z;               m->m[2][1] = yAxis.z;               m->m[2][2] = -zAxis.z;             m->m[2][3] = 0.f;
	m->m[3][0] = -vec3Dot(&xAxis, eye); m->m[3][1] = -vec3Dot(&yAxis, eye); m->m[3][2] = vec3Dot(&zAxis, eye); m->m[3][3] = 1.f;
}

void mat4Translate(mat4 *m, const float x, const float y, const float z){
	m->m[3][0] = m->m[0][0] * x + m->m[1][0] * y + m->m[2][0] * z + m->m[3][0];
	m->m[3][1] = m->m[0][1] * x + m->m[1][1] * y + m->m[2][1] * z + m->m[3][1];
	m->m[3][2] = m->m[0][2] * x + m->m[1][2] * y + m->m[2][2] * z + m->m[3][2];
	m->m[3][3] = m->m[0][3] * x + m->m[1][3] * y + m->m[2][3] * z + m->m[3][3];
}
void mat4SetTranslationMatrix(mat4 *m, const float x, const float y, const float z){
	m->m[0][0] = 1.f; m->m[0][1] = 0.f; m->m[0][2] = 0.f; m->m[0][3] = 0.f;
	m->m[1][0] = 0.f; m->m[1][1] = 1.f; m->m[1][2] = 0.f; m->m[1][3] = 0.f;
	m->m[2][0] = 0.f; m->m[2][1] = 0.f; m->m[2][2] = 1.f; m->m[2][3] = 0.f;
	m->m[3][0] = x;   m->m[3][1] = y;   m->m[3][2] = z;   m->m[3][3] = 1.f;
}
mat4 mat4TranslationMatrix(const float x, const float y, const float z){
	mat4 r = {.m = {{1.f, 0.f, 0.f, 0.f},
	                {0.f, 1.f, 0.f, 0.f},
	                {0.f, 0.f, 1.f, 0.f},
	                {  x,   y,   z, 1.f}}};
	return r;
}
void mat4Rotate(mat4 *m, const quat *q){
	mat4 r; mat4Quat(&r, q);
	mat4MultMByM2(&r, m);
}
void mat4SetRotationMatrix(mat4 *m, const quat *q){
	mat4Quat(m, q);
}
mat4 mat4RotationMatrix(const quat *q){
	mat4 r; mat4Quat(&r, q);
	return r;
}
void mat4Scale(mat4 *m, const float x, const float y, const float z){
	m->m[0][0] *= x; m->m[0][1] *= x; m->m[0][2] *= x; m->m[0][3] *= x;
	m->m[1][0] *= y; m->m[1][1] *= y; m->m[1][2] *= y; m->m[1][3] *= y;
	m->m[2][0] *= z; m->m[2][1] *= z; m->m[2][2] *= z; m->m[2][3] *= z;
}
void mat4SetScaleMatrix(mat4 *m, const float x, const float y, const float z){
	m->m[0][0] = x;   m->m[0][1] = 0.f; m->m[0][2] = 0.f; m->m[0][3] = 0.f;
	m->m[1][0] = 0.f; m->m[1][1] = y;   m->m[1][2] = 0.f; m->m[1][3] = 0.f;
	m->m[2][0] = 0.f; m->m[2][1] = 0.f; m->m[2][2] = z;   m->m[2][3] = 0.f;
	m->m[3][0] = 0.f; m->m[3][1] = 0.f; m->m[3][2] = 0.f; m->m[3][3] = 1.f;
}
mat4 mat4ScaleMatrix(const float x, const float y, const float z){
	mat4 r = {.m = {{  x, 0.f, 0.f, 0.f},
	                {0.f,   y, 0.f, 0.f},
	                {0.f, 0.f,   z, 0.f},
	                {0.f, 0.f, 0.f, 1.f}}};
	return r;
}

void mat4Quat(mat4 *m, const quat *q){
	const float xx = q->v.x*q->v.x;
	const float yy = q->v.y*q->v.y;
	const float zz = q->v.z*q->v.z;
	const float xy = q->v.x*q->v.y;
	const float xz = q->v.x*q->v.z;
	const float xw = q->v.x*q->w;
	const float yz = q->v.y*q->v.z;
	const float yw = q->v.y*q->w;
	const float zw = q->v.z*q->w;
	m->m[0][0] = 1.f-2.f*(yy+zz); m->m[0][1] = 2.f*(xy-zw);     m->m[0][2] = 2.f*(xz+yw);     m->m[0][3] = 0.f;
	m->m[1][0] = 2.f*(xy+zw);     m->m[1][1] = 1.f-2.f*(xx+zz); m->m[1][2] = 2.f*(yz-xw);     m->m[1][3] = 0.f;
	m->m[2][0] = 2.f*(xz-yw);     m->m[2][1] = 2.f*(yz+xw);     m->m[2][2] = 1.f-2.f*(xx+yy); m->m[2][3] = 0.f;
	m->m[3][0] = 0.f;             m->m[3][1] = 0.f;             m->m[3][2] = 0.f;             m->m[3][3] = 1.f;
}
