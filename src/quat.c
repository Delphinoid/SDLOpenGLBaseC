#include "quat.h"
#include <math.h>
#include <float.h>

quat quatNew(const float w, const float x, const float y, const float z){
	quat r = {.w = w, .v.x = x, .v.y = y, .v.z = z};
	return r;
}
quat quatNewS(const float s){
	quat r = {.w = s, .v.x = s, .v.y = s, .v.z = s};
	return r;
}
quat quatNewAxisAngle(const float angle, const float axisX, const float axisY, const float axisZ){
	float t = sinf(angle/2.f);
	quat r = {.w   = cosf(angle/2.f),
	          .v.x = axisX * t,
	          .v.y = axisY * t,
	          .v.z = axisZ * t};
	return r;
}
quat quatNewEuler(const float x, const float y, const float z){
	const float hx = x*0.5f;
	const float hy = y*0.5f;
	const float hz = z*0.5f;
	const float cb = cosf(hx);
	const float ch = cosf(hy);
	const float ca = cosf(hz);
	const float sb = sinf(hx);
	const float sh = sinf(hy);
	const float sa = sinf(hz);
	quat r = {.w   = cb*ch*ca+sb*sh*sa,
	          .v.x = sb*ch*ca-cb*sh*sa,
	          .v.y = cb*sh*ca+sb*ch*sa,
	          .v.z = cb*ch*sa-sb*sh*ca};
	return r;
}
void quatSet(quat *q, const float w, const float x, const float y, const float z){
	q->w = w; q->v.x = x; q->v.y = y; q->v.z = z;
}
void quatSetS(quat *q, const float s){
	q->w = s; q->v.x = s; q->v.y = s; q->v.z = s;
}
void quatSetAxisAngle(quat *q, const float angle, const float axisX, const float axisY, const float axisZ){
	const float t = sinf(angle/2.f);
	q->w = cosf(angle/2.f);
	q->v.x = axisX * t;
	q->v.y = axisY * t;
	q->v.z = axisZ * t;
}
void quatSetEuler(quat *q, const float x, const float y, const float z){
	const float hx = x*0.5f;
	const float hy = y*0.5f;
	const float hz = z*0.5f;
	const float cb = cosf(hx);
	const float ch = cosf(hy);
	const float ca = cosf(hz);
	const float sb = sinf(hx);
	const float sh = sinf(hy);
	const float sa = sinf(hz);
	q->w   = cb*ch*ca+sb*sh*sa;
	q->v.x = sb*ch*ca-cb*sh*sa;
	q->v.y = cb*sh*ca+sb*ch*sa;
	q->v.z = cb*ch*sa-sb*sh*ca;
}

quat quatQAddQ(const quat q1, const quat q2){
	quat r = {.w   = q1.w   + q2.w,
	          .v.x = q1.v.x + q2.v.x,
	          .v.y = q1.v.y + q2.v.y,
	          .v.z = q1.v.z + q2.v.z};
	return r;
}
quat quatQAddW(const quat q, const float w){
	quat r = {.w   = q.w + w,
	          .v.x = q.v.x,
	          .v.y = q.v.y,
	          .v.z = q.v.z};
	return r;
}
void quatAddQToQ(quat *q1, const quat q2){
	q1->w += q2.w; q1->v.x += q2.v.x; q1->v.y += q2.v.y; q1->v.z += q2.v.z;
}
void quatAddWToQ(quat *q, const float w){
	q->w += w;
}

quat quatQSubQ(const quat q1, const quat q2){
	quat r = {.w   = q1.w   - q2.w,
	          .v.x = q1.v.x - q2.v.x,
	          .v.y = q1.v.y - q2.v.y,
	          .v.z = q1.v.z - q2.v.z};
	return r;
}
quat quatQSubW(const quat q, const float w){
	quat r = {.w   = q.w - w,
	          .v.x = q.v.x,
	          .v.y = q.v.y,
	          .v.z = q.v.z};
	return r;
}
void quatSubQFromQ1(quat *q1, const quat q2){
	q1->w -= q2.w; q1->v.x -= q2.v.x; q1->v.y -= q2.v.y; q1->v.z -= q2.v.z;
}
void quatSubQFromQ2(const quat q1, quat *q2){
	q2->w = q1.w - q2->w; q2->v.x = q1.v.x - q2->v.x; q2->v.y = q1.v.y - q2->v.y; q2->v.z = q1.v.z - q2->v.z;
}
void quatSubWFromQ(quat *q, const float w){
	q->w -= w;
}

quat quatQMultQ(const quat q1, const quat q2){
	float prodW = q1.w * q2.w - vec3Dot(q1.v, q2.v);
	vec3 prodV = vec3VMultS(q2.v, q1.w);
	vec3AddVToV(&prodV, vec3VMultS(q1.v, q2.w));
	vec3AddVToV(&prodV, vec3Cross(q1.v, q2.v));
	return quatNew(prodW, prodV.x, prodV.y, prodV.z);
}
quat quatQMultS(const quat q, const float s){
	quat r = {.w   = q.w   * s,
	          .v.x = q.v.x * s,
	          .v.y = q.v.y * s,
	          .v.z = q.v.z * s};
	return r;
}
void quatMultQByQ1(quat *q1, const quat q2){
	float prodW = q1->w * q2.w - vec3Dot(q1->v, q2.v);
	vec3 prodV = vec3VMultS(q2.v, q1->w);
	vec3AddVToV(&prodV, vec3VMultS(q1->v, q2.w));
	vec3AddVToV(&prodV, vec3Cross(q1->v, q2.v));
	q1->w = prodW; q1->v = prodV;
}
void quatMultQByQ2(const quat q1, quat *q2){
	float prodW = q1.w * q2->w - vec3Dot(q1.v, q2->v);
	vec3 prodV = vec3VMultS(q2->v, q1.w);
	vec3AddVToV(&prodV, vec3VMultS(q1.v, q2->w));
	vec3AddVToV(&prodV, vec3Cross(q1.v, q2->v));
	q2->w = prodW; q2->v = prodV;
}
void quatMultQByS(quat *q, const float s){
	q->w *= s;
	vec3MultVByS(&q->v, s);
}

quat quatQDivQ(const quat q1, const quat q2){
	quat r = {.w   = q1.w   / q2.w,
	          .v.x = q1.v.x / q2.v.x,
	          .v.y = q1.v.y / q2.v.y,
	          .v.z = q1.v.z / q2.v.z};
	return r;
}
quat quatQDivS(const quat q, const float s){
	quat r = {.w   = q.w   / s,
	          .v.x = q.v.x / s,
	          .v.y = q.v.y / s,
	          .v.z = q.v.z / s};
	return r;
}
void quatDivQByQ1(quat *q1, const quat q2){
	if(q2.w != 0.f && q2.v.x != 0.f && q2.v.y != 0.f && q2.v.z != 0.f){
		q1->w   /= q2.w;
		q1->v.x /= q2.v.x;
		q1->v.y /= q2.v.y;
		q1->v.z /= q2.v.z;
	}
}
void quatDivQByQ2(const quat q1, quat *q2){
	if(q1.w != 0.f && q1.v.x != 0.f && q1.v.y != 0.f && q1.v.z != 0.f){
		q2->w   = q1.w   / q2->w;
		q2->v.x = q1.v.x / q2->v.x;
		q2->v.y = q1.v.y / q2->v.y;
		q2->v.z = q1.v.z / q2->v.z;
	}
}
void quatDivQByS(quat *q, const float s){
	if(s != 0.f){
		q->w   /= s;
		q->v.x /= s;
		q->v.y /= s;
		q->v.z /= s;
	}
}

float quatGetMagnitude(const quat q){
	return sqrtf(q.w*q.w + q.v.x*q.v.x + q.v.y*q.v.y + q.v.z*q.v.z);
}

quat quatGetConjugate(const quat q){
	return quatNew(q.w, -q.v.x, -q.v.y, -q.v.z);
}
void quatConjugate(quat *q){
	quatSet(q, q->w, -q->v.x, -q->v.y, -q->v.z);
}

quat quatGetNegative(const quat q){
	return quatNew(-q.w, -q.v.x, -q.v.y, -q.v.z);
}
void quatNegate(quat *q){
	quatSet(q, -q->w, -q->v.x, -q->v.y, -q->v.z);
}

quat quatGetInverse(const quat q){
	return quatQMultQ(q, quatGetConjugate(q));
}
void quatInvert(quat *q){
	quatMultQByQ1(q, quatGetConjugate(*q));
}

quat quatGetUnit(const quat q){
	float magnitude = quatGetMagnitude(q);
	if(magnitude != 0.f){
		quat r = {.w   = q.w   / magnitude,
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

void quatAxisAngle(const quat q, float *angle, float *axisX, float *axisY, float *axisZ){
	float scale = sqrtf(1.f-q.w*q.w);  // Optimization of x^2 + y^2 + z^2, as x^2 + y^2 + z^2 + w^2 = 1
	if(scale != 0.f){  // We don't want to risk a potential divide-by-zero error
		*angle = 2.f*acosf(q.w);
		*axisX = q.v.x/scale;
		*axisY = q.v.y/scale;
		*axisZ = q.v.z/scale;
	}
}

float quatDot(const quat q1, const quat q2){
	return q1.w   * q2.w +
	       q1.v.x * q2.v.x +
	       q1.v.y * q2.v.y +
	       q1.v.z * q2.v.z;
}

vec3 quatGetRotatedVec3(const quat q, const vec3 v){

	vec3 r;

	const float dot = vec3Dot(q.v, v);
	const vec3 cross = vec3Cross(q.v, v);

	float m = q.w*q.w-dot;
	r.x = m*v.x;
	r.y = m*v.y;
	r.z = m*v.z;

	m = 2.f*dot;
	r.x += m*q.v.x;
	r.y += m*q.v.y;
	r.z += m*q.v.z;

	m = 2.f*q.w;
	r.x += m*cross.x;
	r.y += m*cross.y;
	r.z += m*cross.z;

	return r;

}
void quatRotateVec3(const quat q, vec3 *v){

	const float dot = vec3Dot(q.v, *v);
	const vec3 cross = vec3Cross(q.v, *v);

	float m = q.w*q.w-dot;
	v->x *= m;
	v->y *= m;
	v->z *= m;

	m = 2.f*dot;
	v->x += m*q.v.x;
	v->y += m*q.v.y;
	v->z += m*q.v.z;

	m = 2.f*q.w;
	v->x += m*cross.x;
	v->y += m*cross.y;
	v->z += m*cross.z;

}

quat quatLookingAt(vec3 eye, vec3 target, vec3 up){

	quat r;

	const float dot = vec3Dot(eye, target);

	if(fabsf(dot + 1.f) < FLT_EPSILON){

		// Eye and target point in opposite directions,
		// 180 degree rotation around up vector
		r.w = M_PI;
		r.v = up;

	}else if(fabsf(dot - 1.f) < FLT_EPSILON){

		// Eye and target are pointing in the same direction
		quatSetIdentity(&r);

	}else{

		float angle = acosf(dot);
		vec3 axis = vec3Cross(eye, target);
		vec3Normalize(&axis);
		r.w = angle;
		r.v = axis;

	}

	return r;

}

void quatLookAt(quat *q, vec3 eye, vec3 target, vec3 up){

	const float dot = vec3Dot(eye, target);

	if(fabsf(dot + 1.f) < FLT_EPSILON){

		// Eye and target point in opposite directions,
		// 180 degree rotation around up vector
		q->w = M_PI;
		q->v = up;

	}else if(fabsf(dot - 1.f) < FLT_EPSILON){

		// Eye and target are pointing in the same direction
		quatSetIdentity(q);

	}else{

		const float angle = acosf(dot);
		vec3 axis = vec3Cross(eye, target);
		vec3Normalize(&axis);
		q->w = angle;
		q->v = axis;

	}

}

quat quatLerp(quat q1, quat q2, const float t){
	quatMultQByS(&q1, 1.f-t);
	quatMultQByS(&q2, t);
	return quatGetUnit(quatQAddQ(q1, q2));
}
quat quatSlerp(quat q1, quat q2, const float t){

	float cosTheta = quatDot(q1, q2);

	// If q1 and q2 are > 90 degrees apart, invert one
	// so it doesn't go the long way around
	if(cosTheta < 0.f){
		cosTheta = -cosTheta;
		quatNegate(&q2);
	}

	if(cosTheta < 1.f - FLT_EPSILON){
		const float angle = acosf(cosTheta);
		quatMultQByS(&q1, sinf(angle*(1.f-t)));
		quatMultQByS(&q2, sinf(angle*t));
		return quatQDivS(quatQAddQ(q1, q2), sinf(angle));
	}

	return quatLerp(q1, q2, t);

}
