#include "quat.h"
#include "mat4.h"
#include "math.h"

quat quatNew(float w, float x, float y, float z){
	quat r = {.w = w, .v.x = x, .v.y = y, .v.z = z};
	return r;
}
quat quatNewS(float s){
	quat r = {.w = s, .v.x = s, .v.y = s, .v.z = s};
	return r;
}
quat quatNewAxisAngle(float angle, float axisX, float axisY, float axisZ){
	float t = sinf(angle/2.f);
	quat r = {.w = cosf(angle/2.f),
	          .v.x = axisX * t,
	          .v.y = axisY * t,
	          .v.z = axisZ * t};
	return r;
}
quat quatNewEuler(vec3 v){
	float cb = cosf(v.x*0.5f);
	float ch = cosf(v.y*0.5f);
	float ca = cosf(v.z*0.5f);
	float sb = sinf(v.x*0.5f);
	float sh = sinf(v.y*0.5f);
	float sa = sinf(v.z*0.5f);
	quat r;
	r.w   = cb*ch*ca+sb*sh*sa;
	r.v.x = sb*ch*ca-cb*sh*sa;
	r.v.y = cb*sh*ca+sb*ch*sa;
	r.v.z = cb*ch*sa-sb*sh*ca;
	return r;
}
void quatSet(quat *q, float w, float x, float y, float z){
	q->w = w; q->v.x = x; q->v.y = y; q->v.z = z;
}
void quatSetS(quat *q, float s){
	q->w = s; q->v.x = s; q->v.y = s; q->v.z = s;
}

quat quatQAddQ(quat q1, quat q2){
	quat r = {.w = q1.w + q2.w,
	          .v.x = q1.v.x + q2.v.x,
	          .v.y = q1.v.y + q2.v.y,
	          .v.z = q1.v.z + q2.v.z};
	return r;
}
quat quatQAddW(quat q, float w){
	quat r = {.w = q.w + w,
	          .v.x = q.v.x,
	          .v.y = q.v.y,
	          .v.z = q.v.z};
	return r;
}
void quatAddQToQ(quat *q1, quat q2){
	q1->w += q2.w; q1->v.x += q2.v.x; q1->v.y += q2.v.y; q1->v.z += q2.v.z;
}
void quatAddWToQ(quat *q, float w){
	q->w += w;
}

quat quatQSubQ(quat q1, quat q2){
	quat r = {.w = q1.w - q2.w,
	          .v.x = q1.v.x - q2.v.x,
	          .v.y = q1.v.y - q2.v.y,
	          .v.z = q1.v.z - q2.v.z};
	return r;
}
quat quatQSubW(quat q, float w){
	quat r = {.w = q.w - w,
	          .v.x = q.v.x,
	          .v.y = q.v.y,
	          .v.z = q.v.z};
	return r;
}
void quatSubQFromQ1(quat *q1, quat q2){
	q1->w -= q2.w; q1->v.x -= q2.v.x; q1->v.y -= q2.v.y; q1->v.z -= q2.v.z;
}
void quatSubQFromQ2(quat q1, quat *q2){
	q2->w = q1.w - q2->w; q2->v.x = q1.v.x - q2->v.x; q2->v.y = q1.v.y - q2->v.y; q2->v.z = q1.v.z - q2->v.z;
}
void quatSubWFromQ(quat *q, float w){
	q->w -= w;
}

quat quatQMultQ(quat q1, quat q2){
	float prodW = q1.w * q2.w - vec3Dot(q1.v, q2.v);
	vec3 prodV = vec3VMultS(q2.v, q1.w);
	vec3AddVToV(&prodV, vec3VMultS(q1.v, q2.w));
	vec3AddVToV(&prodV, vec3Cross(q1.v, q2.v));
	return quatNew(prodW, prodV.x, prodV.y, prodV.z);
}
quat quatQMultS(quat q, float s){
	quat r = {.w = q.w * s,
	          .v.x = q.v.x * s,
	          .v.y = q.v.y * s,
	          .v.z = q.v.z * s};
	return r;
}
void quatMultQByQ1(quat *q1, quat q2){
	float prodW = q1->w * q2.w - vec3Dot(q1->v, q2.v);
	vec3 prodV = vec3VMultS(q2.v, q1->w);
	vec3AddVToV(&prodV, vec3VMultS(q1->v, q2.w));
	vec3AddVToV(&prodV, vec3Cross(q1->v, q2.v));
	q1->w = prodW; q1->v = prodV;
}
void quatMultQByQ2(quat q1, quat *q2){
	float prodW = q1.w * q2->w - vec3Dot(q1.v, q2->v);
	vec3 prodV = vec3VMultS(q2->v, q1.w);
	vec3AddVToV(&prodV, vec3VMultS(q1.v, q2->w));
	vec3AddVToV(&prodV, vec3Cross(q1.v, q2->v));
	q2->w = prodW; q2->v = prodV;
}
void quatMultQByS(quat *q, float s){
	q->w *= s;
	vec3MultVByN(&q->v, s, s, s);
}

float quatGetMagnitude(quat q){
	return sqrtf(q.w*q.w + q.v.x*q.v.x + q.v.y*q.v.y + q.v.z*q.v.z);
}

quat quatGetConjugate(quat q){
	return quatNew(q.w, -q.v.x, -q.v.y, -q.v.z);
}
void quatConjugate(quat *q){
	quatSet(q, q->w, -q->v.x, -q->v.y, -q->v.z);
}

quat quatGetNegative(quat q){
	return quatNew(-q.w, -q.v.x, -q.v.y, -q.v.z);
}
void quatNegate(quat *q){
	quatSet(q, -q->w, -q->v.x, -q->v.y, -q->v.z);
}

quat quatGetInverse(quat q){
	return quatQMultQ(q, quatGetConjugate(q));
}
void quatInvert(quat *q){
	quatMultQByQ1(q, quatGetConjugate(*q));
}

quat quatGetUnit(quat q){
	float magnitude = quatGetMagnitude(q);
	quat r = {.w = q.w / magnitude,
	          .v.x = q.v.x / magnitude,
	          .v.y = q.v.y / magnitude,
	          .v.z = q.v.z / magnitude};
	return r;
}
void quatNormalize(quat *q){
	float magnitude = quatGetMagnitude(*q);
	if(magnitude != 0){
		q->w /= magnitude; q->v.x /= magnitude; q->v.y /= magnitude; q->v.z /= magnitude;
	}
}

void quatMat4(quat q, mat4 *m){
	float sqx = q.v.x*q.v.x;
	float sqy = q.v.y*q.v.y;
	float sqz = q.v.z*q.v.z;
	float txy = q.v.x*q.v.y;
	float txz = q.v.x*q.v.z;
	float txw = q.v.x*q.w;
	float tyz = q.v.y*q.v.z;
	float tyw = q.v.y*q.w;
	float tzw = q.v.z*q.w;
	m->m[0][0] = 1.f-2.f*(sqy+sqz); m->m[0][1] = 2.f*(txy-tzw);     m->m[0][2] = 2.f*(txz+tyw);     m->m[0][3] = 0.f;
	m->m[1][0] = 2.f*(txy+tzw);     m->m[1][1] = 1.f-2.f*(sqx+sqz); m->m[1][2] = 2.f*(tyz-txw);     m->m[1][3] = 0.f;
	m->m[2][0] = 2.f*(txz-tyw);     m->m[2][1] = 2.f*(tyz+txw);     m->m[2][2] = 1.f-2.f*(sqx+sqy); m->m[2][3] = 0.f;
	m->m[3][0] = 0.f;               m->m[3][1] = 0.f;               m->m[3][2] = 0.f;               m->m[3][3] = 1.f;
}
void quatAxisAngle(quat q, float *angle, float *axisX, float *axisY, float *axisZ){
	float scale = sqrtf(1.f-q.w*q.w);  // Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1
	if(scale != 0.f){  // We don't want to risk a potential divide-by-zero error
		*angle = 2.f*acosf(q.w);
		*axisX = q.v.x/scale;
		*axisY = q.v.y/scale;
		*axisZ = q.v.z/scale;
	}
}
