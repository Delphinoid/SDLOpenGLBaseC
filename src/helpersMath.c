#include "helpersMath.h"

float fastInvSqrt(float x){
	/* Black magic perfected by some very clever people. */
	const float halfX = x*0.5f;
	const int *ir = (int *)&x;  // Dereferenced on next line to avoid warnings
	const int i = 0x5f3759df - (*ir>>1);
	const float *xr = (float *)&i;  // Dereferenced on next line to avoid warnings
	x = *xr;
	x *= 1.5f-halfX*x*x;  // Initial Newton-Raphson iteration (repeat this line for more accurate results).
	return x;
}

float fastInvSqrtAccurate(float x){

	/* Black magic perfected by some very clever people. */
	const float halfX = x*0.5f;
	const int *ir = (int *)&x;  // Dereferenced on next line to avoid warnings
	const int i = 0x5f3759df - (*ir>>1);
	const float *xr = (float *)&i;  // Dereferenced on next line to avoid warnings
	x = *xr;
	x *= 1.5f-halfX*x*x;  // Initial Newton-Raphson iteration (repeat this line for more accurate results).

	// Second and third iterations.
	// The engine primarily uses this function for contact normal generation.
	// Highly accurate unit vectors are required for contact normals in order
	// to prevent as much energy loss as possible during contact resolution.
	// Three iterations seems to give the best ratio of accuracy to performance.
	x *= 1.5f-halfX*x*x;
	x *= 1.5f-halfX*x*x;

	return x;

}

vec3 getPointLineProjection(const vec3 *a, const vec3 *b, const vec3 *p){
	/*
	** Project a point onto a line.
	*/
	const vec3 pa = {.x = p->x - a->x,
	                 .y = p->y - a->y,
	                 .z = p->z - a->z};
	const vec3 ba = {.x = b->x - a->x,
	                 .y = b->y - a->y,
	                 .z = b->z - a->z};
	const float d = vec3Dot(&pa, &ba) / vec3Dot(&ba, &ba);
	vec3 r = {.x = a->x + d * ba.x,
	          .y = a->y + d * ba.y,
	          .z = a->z + d * ba.z};
	return r;
}
void pointLineProject(const vec3 *a, const vec3 *b, const vec3 *p, vec3 *r){
	/*
	** Project a point onto a line.
	*/
	const vec3 pa = {.x = p->x - a->x,
	                 .y = p->y - a->y,
	                 .z = p->z - a->z};
	const vec3 ba = {.x = b->x - a->x,
	                 .y = b->y - a->y,
	                 .z = b->z - a->z};
	const float d = vec3Dot(&pa, &ba) / vec3Dot(&ba, &ba);
	r->x = a->x + d * ba.x;
	r->y = a->y + d * ba.y;
	r->z = a->z + d * ba.z;
}

vec3 getFaceNormal(const vec3 *a, const vec3 *b, const vec3 *c){
	/*
	** r = (b - a) X (c - a)
	*/
	vec3 r;
	const vec3 f1 = {.x = b->x - a->x,
	                 .y = b->y - a->y,
	                 .z = b->z - a->z};
	const vec3 f2 = {.x = c->x - a->x,
	                 .y = c->y - a->y,
	                 .z = c->z - a->z};
	vec3Cross(&f1, &f2, &r);
	return r;
}
void faceNormal(const vec3 *a, const vec3 *b, const vec3 *c, vec3 *r){
	/*
	** r = (b - a) X (c - a)
	*/
	const vec3 f1 = {.x = b->x - a->x,
	                 .y = b->y - a->y,
	                 .z = b->z - a->z};
	const vec3 f2 = {.x = c->x - a->x,
	                 .y = c->y - a->y,
	                 .z = c->z - a->z};
	vec3Cross(&f1, &f2, r);
}

vec3 getBarycentric(const vec3 *a, const vec3 *b, const vec3 *c, const vec3 *p){
	/*
	** Calculate the barycentric coordinates of
	** point p in triangle abc.
	*/
	const vec3 v1 = {.x = b->x - a->x,
	                 .y = b->y - a->y,
	                 .z = b->z - a->z};
	const vec3 v2 = {.x = c->x - a->x,
	                 .y = c->y - a->y,
	                 .z = c->z - a->z};
	const vec3 v3 = {.x = p->x - a->x,
	                 .y = p->y - a->y,
	                 .z = p->z - a->z};
	const float d11 = vec3Dot(&v1, &v1);
	const float d12 = vec3Dot(&v1, &v2);
	const float d22 = vec3Dot(&v2, &v2);
	const float d31 = vec3Dot(&v3, &v1);
	const float d32 = vec3Dot(&v3, &v2);
	const float denomInv = 1.f / (d11 * d22 - d12 * d12);
	vec3 r = {.y = (d22 * d31 - d12 * d32) * denomInv,
	          .z = (d11 * d32 - d12 * d31) * denomInv,
	          .x = 1.f - r.y - r.z};
	return r;
}
void barycentric(const vec3 *a, const vec3 *b, const vec3 *c, const vec3 *p, vec3 *r){
	/*
	** Calculate the barycentric coordinates of
	** point p in triangle abc.
	*/
	const vec3 v1 = {.x = b->x - a->x,
	                 .y = b->y - a->y,
	                 .z = b->z - a->z};
	const vec3 v2 = {.x = c->x - a->x,
	                 .y = c->y - a->y,
	                 .z = c->z - a->z};
	const vec3 v3 = {.x = p->x - a->x,
	                 .y = p->y - a->y,
	                 .z = p->z - a->z};
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

float pointPlaneDistance(const vec3 *normal, const vec3 *vertex, const vec3 *point){
	/*
	** Calculates the squared distance
	** between a point and a plane.
	*/
	const vec3 offset = {.x = point->x - vertex->x,
	                     .y = point->y - vertex->y,
	                     .z = point->z - vertex->z};
	return vec3Dot(normal, &offset);
}
void pointPlaneProject(const vec3 *normal, const vec3 *vertex, vec3 *point){
	/*
	** Projects a point onto a plane.
	*/
	const vec3 psv = {.x = point->x - vertex->x,
	                  .y = point->y - vertex->y,
	                  .z = point->z - vertex->z};
	const float dot = vec3Dot(&psv, normal);
	point->x -= dot * normal->x;
	point->y -= dot * normal->y;
	point->z -= dot * normal->z;
}

void linePlaneIntersection(const vec3 *normal, const vec3 *vertex, const vec3 *line, vec3 *point){
	/*
	** Finds the intersection between
	** a line and a plane.
	*/
	const float startDistance = vec3Dot(normal, vertex);
	const float t = startDistance / (startDistance + pointPlaneDistance(normal, vertex, line));
	/* Lerp */
	point->x = line->x * t;
	point->y = line->y * t;
	point->z = line->z * t;
}
return_t segmentPlaneIntersection(const vec3 *normal, const vec3 *vertex, const vec3 *start, const vec3 *end, vec3 *point){
	/*
	** Finds the intersection between
	** a line segment and a plane.
	*/
	const float startDistance = pointPlaneDistance(normal, vertex, start);
	const float endDistance = pointPlaneDistance(normal, vertex, end);
	if(startDistance <= 0.f){
		if(endDistance <= 0.f){
			return 0;
		}
	}else if(endDistance > 0.f){
		return 0;
	}
	vec3Lerp(start, end, startDistance / (startDistance - endDistance), point);
	return 1;
}

float floatLerp(const float f1, const float f2, const float t){
	if(t == 0.f){
		return f1;
	}else if(t == 1.f){
		return f2;
	}else{
		return f1 + (f2 - f1) * t;
	}
}
