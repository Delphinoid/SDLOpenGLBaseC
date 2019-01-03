#include "helpersMath.h"
#include "inline.h"

__HINT_INLINE__ float fastInvSqrt(float x){
	// Black magic perfected by some very clever people.
	const float halfx = x*0.5f;
	union {
		float f;
		uint32_t l;
	} i;
	i.f = x;
	i.l = 0x5F3504F3 - (i.l>>1);  // Floating-point approximation of sqrtf(powf(2.f, 127.f)).
	i.f *= 1.5f - halfx*i.f*i.f;  // Initial Newton-Raphson iteration (repeat this line for more accurate results).
	return i.f;
}

__HINT_INLINE__ float fastInvSqrtAccurate(float x){

	// Black magic perfected by some very clever people.
	const float halfx = x*0.5f;
	union {
		float f;
		uint32_t l;
	} i;
	i.f = x;
	i.l = 0x5F3504F3 - (i.l>>1);  // Floating-point approximation of sqrtf(powf(2.f, 127.f)).
	i.f *= 1.5f - halfx*i.f*i.f;  // Initial Newton-Raphson iteration (repeat this line for more accurate results).

	// Second and third iterations.
	// The engine primarily uses this function for contact normal generation.
	// Highly accurate unit vectors are required for contact normals in order
	// to prevent as much energy loss as possible during contact resolution.
	// Three iterations seems to give the best ratio of accuracy to performance.
	i.f *= 1.5f - halfx*i.f*i.f;
	i.f *= 1.5f - halfx*i.f*i.f;

	return i.f;

}

__HINT_INLINE__ vec3 getPointLineProjection(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict p){
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
__HINT_INLINE__ void pointLineProject(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict p, vec3 *const restrict r){
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

__HINT_INLINE__ vec3 getFaceNormal(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c){
	/*
	** r = (b - a) x (c - a)
	*/
	vec3 r;
	const vec3 f1 = {.x = b->x - a->x,
	                 .y = b->y - a->y,
	                 .z = b->z - a->z};
	const vec3 f2 = {.x = c->x - a->x,
	                 .y = c->y - a->y,
	                 .z = c->z - a->z};
	vec3CrossR(&f1, &f2, &r);
	return r;
}
__HINT_INLINE__ void faceNormal(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, vec3 *const restrict r){
	/*
	** r = (b - a) x (c - a)
	*/
	const vec3 f1 = {.x = b->x - a->x,
	                 .y = b->y - a->y,
	                 .z = b->z - a->z};
	const vec3 f2 = {.x = c->x - a->x,
	                 .y = c->y - a->y,
	                 .z = c->z - a->z};
	vec3CrossR(&f1, &f2, r);
}

__HINT_INLINE__ vec3 getBarycentric(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, const vec3 *const restrict p){
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
__HINT_INLINE__ void barycentric(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, vec3 *const restrict p){
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
	p->y = (d22 * d31 - d12 * d32) * denomInv;
	p->z = (d11 * d32 - d12 * d31) * denomInv;
	p->x = 1.f - p->y - p->z;
}
__HINT_INLINE__ void barycentricR(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, const vec3 *const restrict p, vec3 *const restrict r){
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

__HINT_INLINE__ float pointPlaneDistance(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict point){
	/*
	** Calculates the distance
	** between a point and a plane.
	*/
	const vec3 offset = {.x = point->x - vertex->x,
	                     .y = point->y - vertex->y,
	                     .z = point->z - vertex->z};
	return vec3Dot(normal, &offset);
}
__HINT_INLINE__ void pointPlaneProject(const vec3 *const restrict normal, const vec3 *const restrict vertex, vec3 *const restrict point){
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
__HINT_INLINE__ void pointPlaneProjectR(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict point, vec3 *const restrict r){
	/*
	** Projects a point onto a plane.
	*/
	const vec3 psv = {.x = point->x - vertex->x,
	                  .y = point->y - vertex->y,
	                  .z = point->z - vertex->z};
	const float dot = vec3Dot(&psv, normal);
	r->x = point->x - dot * normal->x;
	r->y = point->y - dot * normal->y;
	r->z = point->z - dot * normal->z;
}

__HINT_INLINE__ void linePlaneIntersection(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict line, vec3 *const restrict point){
	/*
	** Finds the intersection between
	** a line and a plane.
	*/
	const float startDistance = vec3Dot(normal, vertex);
	const float t = startDistance / (startDistance + pointPlaneDistance(normal, vertex, line));
	// Lerp
	point->x = line->x * t;
	point->y = line->y * t;
	point->z = line->z * t;
}
__HINT_INLINE__ return_t segmentPlaneIntersection(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict start, const vec3 *const restrict end, vec3 *const restrict point){
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
	vec3LerpR(start, end, startDistance / (startDistance - endDistance), point);
	return 1;
}

__HINT_INLINE__ void segmentClosestPoints(const vec3 *const restrict s1, const vec3 *const restrict e1, const vec3 *const restrict s2, const vec3 *const restrict e2, vec3 *const restrict p1, vec3 *const restrict p2){
	/*
	** Finds the closest points that
	** lie on the two line segments.
	*/
	const vec3 v1 = {.x = s1->x - s2->x,
	                 .y = s1->y - s2->y,
	                 .z = s1->z - s2->z};
	const vec3 v2 = {.x = e1->x - s1->x,
	                 .y = e1->y - s1->y,
	                 .z = e1->z - s1->z};
	const vec3 v3 = {.x = e2->x - s2->x,
	                 .y = e2->y - s2->y,
	                 .z = e2->z - s2->z};
	const float d12 = vec3Dot(&v1, &v2);
	const float d13 = vec3Dot(&v1, &v3);
	const float d22 = vec3Dot(&v2, &v2);
	const float d32 = vec3Dot(&v3, &v2);
	const float d33 = vec3Dot(&v3, &v3);
	const float denom = d22 * d33 - d32 * d32;
	// If the denominator is 0, use 0.5 as the position
	// along the first line segment. This puts the closest
	// point in the very center.
	const float m1 = denom == 0.f ? 0.5f : (d13 * d32 - d12 * d33) / denom;
	const float m2 = (d13 + m1 * d32) / d33;
	// Calculate the point on the first line segment.
	p1->x = s1->x + m1 * (e1->x - s1->x);
	p1->y = s1->y + m1 * (e1->y - s1->y);
	p1->z = s1->z + m1 * (e1->z - s1->z);
	// Calculate the point on the second line segment.
	p2->x = s2->x + m2 * (e2->x - s2->x);
	p2->y = s2->y + m2 * (e2->y - s2->y);
	p2->z = s2->z + m2 * (e2->z - s2->z);
}

__HINT_INLINE__ float floatLerp(const float f1, const float f2, const float t){
	if(t == 0.f){
		return f1;
	}else if(t == 1.f){
		return f2;
	}else{
		return f1 + (f2 - f1) * t;
	}
}
