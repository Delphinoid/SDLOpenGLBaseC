#include "engineMath.h"

float fastInvSqrt(float x){
	/* Black magic perfected by some very clever people. */
	const float halfX = x*0.5f;
	const int *ir = (int *)&x;  // Dereferenced on next line to avoid warnings
	const int i = 0x5f3759df - (*ir>>1);
	const float *xr = (float *)&i;  // Dereferenced on next line to avoid warnings
	x = *xr;
	x *= 1.5f-halfX*x*x;  // Initial Newton-Raphson iteration (repeat this line for more accurate results)
	//x *= 1.5f-halfX*x*x;
	return x;
}

vec3 getPointLineProjection(const vec3 *a, const vec3 *b, const vec3 *p){
	/*
	** Project a point onto a line.
	*/
	vec3 r;
	vec3 pa; vec3SubVFromVR(p, a, &pa);
	vec3 ba; vec3SubVFromVR(b, a, &ba);
	float d = vec3Dot(&pa, &ba) / vec3Dot(&ba, &ba);
	r.x = a->x + d * ba.x;
	r.y = a->y + d * ba.y;
	r.z = a->z + d * ba.z;
	return r;
}
void pointLineProject(const vec3 *a, const vec3 *b, const vec3 *p, vec3 *r){
	/*
	** Project a point onto a line.
	*/
	vec3 pa; vec3SubVFromVR(p, a, &pa);
	vec3 ba; vec3SubVFromVR(b, a, &ba);
	float d = vec3Dot(&pa, &ba) / vec3Dot(&ba, &ba);
	r->x = a->x + d * ba.x;
	r->y = a->y + d * ba.y;
	r->z = a->z + d * ba.z;
}

vec3 getFaceNormal(const vec3 *a, const vec3 *b, const vec3 *c){
	/*
	** r = (b - a) X (c - a)
	*/
	vec3 f1, f2, r;
	vec3SubVFromVR(b, a, &f1);
	vec3SubVFromVR(c, a, &f2);
	vec3Cross(&f1, &f2, &r);
	return r;
}
void faceNormal(const vec3 *a, const vec3 *b, const vec3 *c, vec3 *r){
	/*
	** r = (b - a) X (c - a)
	*/
	vec3 f1, f2;
	vec3SubVFromVR(b, a, &f1);
	vec3SubVFromVR(c, a, &f2);
	vec3Cross(&f1, &f2, r);
}

vec3 getBarycentric(const vec3 *a, const vec3 *b, const vec3 *c, const vec3 *p){
	/*
	** Calculate the barycentric coordinates of
	** point p in triangle abc.
	*/
	vec3 r, v1, v2, v3;
	vec3SubVFromVR(b, a, &v1);
	vec3SubVFromVR(c, a, &v2);
	vec3SubVFromVR(p, a, &v3);
	const float d11 = vec3Dot(&v1, &v1);
	const float d12 = vec3Dot(&v1, &v2);
	const float d22 = vec3Dot(&v2, &v2);
	const float d31 = vec3Dot(&v3, &v1);
	const float d32 = vec3Dot(&v3, &v2);
	const float denomInv = 1.f / (d11 * d22 - d12 * d12);
	r.y = (d22 * d31 - d12 * d32) * denomInv;
	r.z = (d11 * d32 - d12 * d31) * denomInv;
	r.x = 1.f - r.y - r.z;
	return r;
}
void barycentric(const vec3 *a, const vec3 *b, const vec3 *c, const vec3 *p, vec3 *r){
	/*
	** Calculate the barycentric coordinates of
	** point p in triangle abc.
	*/
	vec3 v1, v2, v3;
	vec3SubVFromVR(b, a, &v1);
	vec3SubVFromVR(c, a, &v2);
	vec3SubVFromVR(p, a, &v3);
	const float d11 = vec3Dot(&v1, &v1);
	const float d12 = vec3Dot(&v1, &v2);
	const float d22 = vec3Dot(&v2, &v2);
	const float d31 = vec3Dot(&v3, &v1);
	const float d32 = vec3Dot(&v3, &v2);
	const float denom = d11 * d22 - d12 * d12;
	const float denomInv = denom == 0.f ? 0.f : 1.f / denom;
	r->y = (d22 * d31 - d12 * d32) * denomInv;
	r->z = (d11 * d32 - d12 * d31) * denomInv;
	r->x = 1.f - r->y - r->z;
}
