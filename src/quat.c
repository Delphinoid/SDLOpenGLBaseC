#include "quat.h"
#include <math.h>

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
	quat r = {.w   = cosf(angle/2.f),
	          .v.x = axisX * t,
	          .v.y = axisY * t,
	          .v.z = axisZ * t};
	return r;
}
quat quatNewEuler(float x, float y, float z){
	float hx = x*0.5f;
	float hy = y*0.5f;
	float hz = z*0.5f;
	float cb = cosf(hx);
	float ch = cosf(hy);
	float ca = cosf(hz);
	float sb = sinf(hx);
	float sh = sinf(hy);
	float sa = sinf(hz);
	quat r = {.w   = cb*ch*ca+sb*sh*sa,
	          .v.x = sb*ch*ca-cb*sh*sa,
	          .v.y = cb*sh*ca+sb*ch*sa,
	          .v.z = cb*ch*sa-sb*sh*ca};
	return r;
}
void quatSet(quat *q, float w, float x, float y, float z){
	q->w = w; q->v.x = x; q->v.y = y; q->v.z = z;
}
void quatSetS(quat *q, float s){
	q->w = s; q->v.x = s; q->v.y = s; q->v.z = s;
}
void quatSetAxisAngle(quat *q, float angle, float axisX, float axisY, float axisZ){
	float t = sinf(angle/2.f);
	q->w = cosf(angle/2.f);
	q->v.x = axisX * t;
	q->v.y = axisY * t;
	q->v.z = axisZ * t;
}
void quatSetEuler(quat *q, float x, float y, float z){
	float hx = x*0.5f;
	float hy = y*0.5f;
	float hz = z*0.5f;
	float cb = cosf(hx);
	float ch = cosf(hy);
	float ca = cosf(hz);
	float sb = sinf(hx);
	float sh = sinf(hy);
	float sa = sinf(hz);
	q->w   = cb*ch*ca+sb*sh*sa;
	q->v.x = sb*ch*ca-cb*sh*sa;
	q->v.y = cb*sh*ca+sb*ch*sa;
	q->v.z = cb*ch*sa-sb*sh*ca;
}

quat quatQAddQ(quat q1, quat q2){
	quat r = {.w   = q1.w   + q2.w,
	          .v.x = q1.v.x + q2.v.x,
	          .v.y = q1.v.y + q2.v.y,
	          .v.z = q1.v.z + q2.v.z};
	return r;
}
quat quatQAddW(quat q, float w){
	quat r = {.w   = q.w + w,
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
	quat r = {.w   = q1.w   - q2.w,
	          .v.x = q1.v.x - q2.v.x,
	          .v.y = q1.v.y - q2.v.y,
	          .v.z = q1.v.z - q2.v.z};
	return r;
}
quat quatQSubW(quat q, float w){
	quat r = {.w   = q.w - w,
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
	quat r = {.w   = q.w   * s,
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
	vec3MultVByS(&q->v, s);
}

quat quatQDivQ(quat q1, quat q2){
	quat r = {.w   = q1.w   / q2.w,
	          .v.x = q1.v.x / q2.v.x,
	          .v.y = q1.v.y / q2.v.y,
	          .v.z = q1.v.z / q2.v.z};
	return r;
}
quat quatQDivS(quat q, float s){
	quat r = {.w   = q.w   / s,
	          .v.x = q.v.x / s,
	          .v.y = q.v.y / s,
	          .v.z = q.v.z / s};
	return r;
}
void quatDivQByQ1(quat *q1, quat q2){
	if(q2.w != 0.f && q2.v.x != 0.f && q2.v.y != 0.f && q2.v.z != 0.f){
		q1->w   /= q2.w;
		q1->v.x /= q2.v.x;
		q1->v.y /= q2.v.y;
		q1->v.z /= q2.v.z;
	}
}
void quatDivQByQ2(quat q1, quat *q2){
	if(q1.w != 0.f && q1.v.x != 0.f && q1.v.y != 0.f && q1.v.z != 0.f){
		q2->w   = q1.w   / q2->w;
		q2->v.x = q1.v.x / q2->v.x;
		q2->v.y = q1.v.y / q2->v.y;
		q2->v.z = q1.v.z / q2->v.z;
	}
}
void quatDivQByS(quat *q, float s){
	if(s != 0.f){
		q->w   /= s;
		q->v.x /= s;
		q->v.y /= s;
		q->v.z /= s;
	}
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
	if(magnitude != 0.f){
		quat r = {.w = q.w / magnitude,
		          .v.x = q.v.x / magnitude,
		          .v.y = q.v.y / magnitude,
		          .v.z = q.v.z / magnitude};
		return r;
	}
	return q;
}
void quatNormalize(quat *q){
	float magnitude = quatGetMagnitude(*q);
	if(magnitude != 0.f){
		q->w /= magnitude; q->v.x /= magnitude; q->v.y /= magnitude; q->v.z /= magnitude;
	}
}

quat quatIdentity(){
	quat r = {.w = 1.f, .v.x = 0.f, .v.y = 0.f, .v.z = 0.f};
	return r;
}
void quatSetIdentity(quat *q){
	q->w = 1.f; q->v.x = 0.f; q->v.y = 0.f; q->v.z = 0.f;
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

float quatDot(quat q1, quat q2){
	return q1.w   * q2.w +
	       q1.v.x * q2.v.x +
	       q1.v.y * q2.v.y +
	       q1.v.z * q2.v.z;
}

quat quatLerp(quat q1, quat q2, float t){
	quatMultQByS(&q1, 1.f-t);
	quatMultQByS(&q2, t);
	return quatGetUnit(quatQAddQ(q1, q2));
}
quat quatSlerp(quat q1, quat q2, float t){

	float cosTheta = quatDot(q1, q2);

	// If q1 and q2 are > 90 degrees apart, invert one
	// so it doesn't go the long way around
	if(cosTheta < 0.f){
		cosTheta = -cosTheta;
		quatNegate(&q2);
	}

	if(cosTheta < 0.9995){
		float angle = acosf(cosTheta);
		quatMultQByS(&q1, sinf(angle*(1.f-t)));
		quatMultQByS(&q2, sinf(angle*t));
		return quatQDivS(quatQAddQ(q1, q2), sinf(angle));
	}

	return quatLerp(q1, q2, t);

}
