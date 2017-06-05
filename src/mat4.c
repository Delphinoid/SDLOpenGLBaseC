#include "mat4.h"
#include "vec4.h"
#include "quat.h"
#include "math.h"

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

mat4 mat4MMultM(mat4 *m1, mat4 *m2){
	mat4 r;
	unsigned int d, f;
	for(d = 0; d < 4; d++){
		for(f = 0; f < 4; f++){
			r.m[d][f] = (m2->m[d][0] * m1->m[0][f]) +
			            (m2->m[d][1] * m1->m[1][f]) +
			            (m2->m[d][2] * m1->m[2][f]) +
			            (m2->m[d][3] * m1->m[3][f]);
		}
	}
	return r;
}
void mat4MultMByM1(mat4 *m1, mat4 *m2){
	mat4 r;
	unsigned int d, f;
	for(d = 0; d < 4; d++){
		for(f = 0; f < 4; f++){
			r.m[d][f] = (m2->m[d][0] * m1->m[0][f]) +
			            (m2->m[d][1] * m1->m[1][f]) +
			            (m2->m[d][2] * m1->m[2][f]) +
			            (m2->m[d][3] * m1->m[3][f]);
		}
	}
	*m1 = r;
}
void mat4MultMByM2(mat4 *m1, mat4 *m2){
	mat4 r;
	unsigned int d, f;
	for(d = 0; d < 4; d++){
		for(f = 0; f < 4; f++){
			r.m[d][f] = (m2->m[d][0] * m1->m[0][f]) +
			            (m2->m[d][1] * m1->m[1][f]) +
			            (m2->m[d][2] * m1->m[2][f]) +
			            (m2->m[d][3] * m1->m[3][f]);
		}
	}
	*m2 = r;
}
vec4 mat4VMultM(vec4 v, mat4 *m){
	vec4 r;
	r.x = v.x * m->m[0][0] + v.y * m->m[0][1] + v.z * m->m[0][2] + v.w * m->m[0][3];
	r.y = v.x * m->m[1][0] + v.y * m->m[1][1] + v.z * m->m[1][2] + v.w * m->m[1][3];
	r.z = v.x * m->m[2][0] + v.y * m->m[2][1] + v.z * m->m[2][2] + v.w * m->m[2][3];
	r.w = v.x * m->m[3][0] + v.y * m->m[3][1] + v.z * m->m[3][2] + v.w * m->m[3][3];
	return r;
}
void mat4MultVByM(vec4 *v, mat4 *m){
	vec4 r;
	r.x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2] + v->w * m->m[0][3];
	r.y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2] + v->w * m->m[1][3];
	r.z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2] + v->w * m->m[2][3];
	r.w = v->x * m->m[3][0] + v->y * m->m[3][1] + v->z * m->m[3][2] + v->w * m->m[3][3];
	v->x = r.x; v->y = r.y; v->z = r.z; v->w = r.w;
}
vec4 mat4MMultV(mat4 *m, vec4 v){
	vec4 r;
	r.x = v.x * m->m[0][0] + v.y * m->m[1][0] + v.z * m->m[2][0] + v.w * m->m[3][0];
	r.y = v.x * m->m[0][1] + v.y * m->m[1][1] + v.z * m->m[2][1] + v.w * m->m[3][1];
	r.z = v.x * m->m[0][2] + v.y * m->m[1][2] + v.z * m->m[2][2] + v.w * m->m[3][2];
	r.w = v.x * m->m[0][3] + v.y * m->m[1][3] + v.z * m->m[2][3] + v.w * m->m[3][3];
	return r;
}
void mat4MultMByV(mat4 *m, vec4 *v){
	vec4 r;
	r.x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0] + v->w * m->m[3][0];
	r.y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1] + v->w * m->m[3][1];
	r.z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2] + v->w * m->m[3][2];
	r.w = v->x * m->m[0][3] + v->y * m->m[1][3] + v->z * m->m[2][3] + v->w * m->m[3][3];
	v->x = r.x; v->y = r.y; v->z = r.z; v->w = r.w;
}

mat4 mat4GetTranspose(mat4 *m){
	mat4 r = {.m = {{m->m[0][0], m->m[1][0], m->m[2][0], m->m[3][0]},
	                {m->m[0][1], m->m[1][1], m->m[2][1], m->m[3][1]},
	                {m->m[0][2], m->m[1][2], m->m[2][2], m->m[3][2]},
	                {m->m[0][3], m->m[1][3], m->m[2][3], m->m[3][3]}}};
	return r;
}
void mat4Transpose(mat4 *m){
	*m = mat4GetTranspose(m);
}
unsigned char mat4Frustum(mat4 *m, float left, float right, float bottom, float top, float zNear, float zFar){
	if(left == right || bottom == top || zNear == zFar){
		return 0;
	}
	m->m[0][0] = 2.f*zNear/(right-left);    m->m[0][1] = 0.f;                       m->m[0][2] = 0.f;                         m->m[0][3] = 0.f;
	m->m[1][0] = 0.f;                       m->m[1][1] = 2.f*zNear/(top-bottom);    m->m[1][2] = 0.f;                         m->m[1][3] = 0.f;
	m->m[2][0] = (right+left)/(right-left); m->m[2][1] = (top+bottom)/(top-bottom); m->m[2][2] = -(zFar+zNear)/(zFar-zNear);  m->m[2][3] = -1.f;
	m->m[3][0] = 0.f;                       m->m[3][1] = 0.f;                       m->m[3][2] = 2.f*zFar*zNear/(zFar-zNear); m->m[3][3] = 0.f;
	return 1;
}
unsigned char mat4Ortho(mat4 *m, float left, float right, float bottom, float top, float zNear, float zFar){
	if(left == right || bottom == top || zNear == zFar){
		return 0;
	}
	m->m[0][0] = 2.f/(right-left);             m->m[0][1] = 0.f;                          m->m[0][2] = 0.f;                          m->m[0][3] = 0.f;
	m->m[1][0] = 0.f;                          m->m[1][1] = 2.f/(top-bottom);             m->m[1][2] = 0.f;                          m->m[1][3] = 0.f;
	m->m[2][0] = 0.f;                          m->m[2][1] = 0.f;                          m->m[2][2] = -2.f/(zFar-zNear);            m->m[2][3] = 0.f;
	m->m[3][0] = -((right+left)/(right-left)); m->m[3][1] = -((top+bottom)/(top-bottom)); m->m[3][2] = -((zFar+zNear)/(zFar-zNear)); m->m[3][3] = 1.f;
	return 1;
}
unsigned char mat4Perspective(mat4 *m, float fovy, float aspectRatio, float zNear, float zFar){
	if(fovy == 0 || aspectRatio == 0 || zNear == zFar){
		return 0;
	}
	float scale = tanf(fovy * 0.5f);
	// Currently right-handed for OpenGL. For left-handed, use the additive inverses of the values in the third column
	m->m[0][0] = 1.f/(scale*aspectRatio); m->m[0][1] = 0.f;       m->m[0][2] = 0.f;                          m->m[0][3] = 0.f;
	m->m[1][0] = 0.f;                     m->m[1][1] = 1.f/scale; m->m[1][2] = 0.f;                          m->m[1][3] = 0.f;
	m->m[2][0] = 0.f;                     m->m[2][1] = 0.f;       m->m[2][2] = -(zFar+zNear)/(zFar-zNear);   m->m[2][3] = -1.f;
	m->m[3][0] = 0.f;                     m->m[3][1] = 0.f;       m->m[3][2] = -2.f*zFar*zNear/(zFar-zNear); m->m[3][3] = 0.f;
	return 1;
}
void mat4LookAt(mat4 *m, vec3 eye, vec3 target, vec3 up){
	vec3 zAxis = vec3VSubV(target, eye);
	vec3Normalize(&zAxis);
	vec3 xAxis = vec3Cross(zAxis, up);
	vec3Normalize(&xAxis);
	vec3 yAxis = vec3Cross(xAxis, zAxis);
	vec3Normalize(&yAxis);
	// Currently right-handed for OpenGL. For left-handed, use the additive inverses of the values in the third column
	m->m[0][0] = xAxis.x;              m->m[0][1] = yAxis.x;              m->m[0][2] = -zAxis.x;            m->m[0][3] = 0.f;
	m->m[1][0] = xAxis.y;              m->m[1][1] = yAxis.y;              m->m[1][2] = -zAxis.y;            m->m[1][3] = 0.f;
	m->m[2][0] = xAxis.z;              m->m[2][1] = yAxis.z;              m->m[2][2] = -zAxis.z;            m->m[2][3] = 0.f;
	m->m[3][0] = -vec3Dot(xAxis, eye); m->m[3][1] = -vec3Dot(yAxis, eye); m->m[3][2] = vec3Dot(zAxis, eye); m->m[3][3] = 1.f;
}

void mat4Translate(mat4 *m, float x, float y, float z){
	m->m[3][0] = m->m[0][0] * x + m->m[1][0] * y + m->m[2][0] * z + m->m[3][0];
	m->m[3][1] = m->m[0][1] * x + m->m[1][1] * y + m->m[2][1] * z + m->m[3][1];
	m->m[3][2] = m->m[0][2] * x + m->m[1][2] * y + m->m[2][2] * z + m->m[3][2];
	m->m[3][3] = m->m[0][3] * x + m->m[1][3] * y + m->m[2][3] * z + m->m[3][3];
}
mat4 mat4TranslationMatrix(float x, float y, float z){
	mat4 r = {.m = {{1.f, 0.f, 0.f, 0.f},
	                {0.f, 1.f, 0.f, 0.f},
	                {0.f, 0.f, 1.f, 0.f},
	                { x,   y,   z,  1.f}}};
	return r;
}
void mat4Rotate(mat4 *m, quat q){
	mat4 r; quatMat4(q, &r);
	mat4MultMByM1(m, &r);
}
mat4 mat4RotationMatrix(quat q){
	mat4 r; quatMat4(q, &r);
	return r;
}
void mat4Scale(mat4 *m, float x, float y, float z){
	m->m[0][0] *= x; m->m[0][1] *= x; m->m[0][2] *= x; m->m[0][3] *= x;
	m->m[1][0] *= y; m->m[1][1] *= y; m->m[1][2] *= y; m->m[1][3] *= y;
	m->m[2][0] *= z; m->m[2][1] *= z; m->m[2][2] *= z; m->m[2][3] *= z;
}
mat4 mat4ScaleMatrix(float x, float y, float z){
	mat4 r = {.m = {{ x,  0.f, 0.f, 0.f},
	                {0.f,  y,  0.f, 0.f},
	                {0.f, 0.f,  z,  0.f},
	                {0.f, 0.f, 0.f, 1.f}}};
	return r;
}
