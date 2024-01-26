#include "helpersMath.h"

typedef union {
	float f;
	uint32_t l;
} bitFloat;

__HINT_INLINE__ float copySign(const float x, const float y){
	// Returns the float x with the sign of y.
	const bitFloat i = {.f = x}; const bitFloat j = {.f = y};
	const bitFloat k = {.l = (i.l & 0x7FFFFFFF) | (j.l & 0x80000000)};
	return k.f;
}

__HINT_INLINE__ float fastInvSqrt(float x){
	// Black magic perfected by some very clever people.
	bitFloat i = {.f = x};
	i.l = 0x5F1FFFF9 - (i.l >> 1);   // Jan Kadlec's brute-force optimization to the fast inverse square root.
	i.f *= 2.38924456f - x*i.f*i.f;  // Initial Newton-Raphson iteration (repeat this line for more accurate results).
	return 0.703952253f * i.f;
}

__HINT_INLINE__ float fastInvSqrtAccurate(float x){

	// Black magic perfected by some very clever people.
	bitFloat i = {.f = x};
	i.l = 0x5F1FFFF9 - (i.l >> 1);   // Jan Kadlec's brute-force optimization to the fast inverse square root.
	i.f *=
		(2.38924456f - x*i.f*i.f) *  // Initial Newton-Raphson iteration (repeat this line for more accurate results).
	// Second and third iterations.
	// The engine primarily uses this function for contact normal generation.
	// Highly accurate unit vectors are required for contact normals in order
	// to prevent as much energy loss as possible during contact resolution.
	// Three iterations seems to give the best ratio of accuracy to performance.
		(2.38924456f - x*i.f*i.f) * (2.38924456f - x*i.f*i.f);

	return 0.703952253f * i.f;

}

__HINT_INLINE__ vec3 pointLineProjection(const vec3 a, const vec3 b, const vec3 p){
	// Project a point onto a line.
	const vec3 pa = vec3VSubV(p, a);
	const vec3 ba = vec3VSubV(b, a);
	const float d = vec3Dot(pa, ba) / vec3Dot(ba, ba);
	// d*ba + a
	return vec3fmaf(d, ba, a);
}
__HINT_INLINE__ void pointLineProjectionPR(const vec3 *const __RESTRICT__ a, const vec3 *const __RESTRICT__ b, const vec3 *const __RESTRICT__ p, vec3 *const __RESTRICT__ r){
	// Project a point onto a line.
	vec3 pa, ba;
	float d;
	vec3VSubVPR(p, a, &pa);
	vec3VSubVPR(b, a, &ba);
	d = vec3DotP(&pa, &ba) / vec3DotP(&ba, &ba);
	vec3fmafPR(d, &ba, a, r);
}

__HINT_INLINE__ vec3 faceNormal(const vec3 a, const vec3 b, const vec3 c){
	// r = (b - a) x (c - a)
	return vec3Cross(vec3VSubV(b, a), vec3VSubV(c, a));
}
__HINT_INLINE__ void faceNormalPR(const vec3 *const __RESTRICT__ a, const vec3 *const __RESTRICT__ b, const vec3 *const __RESTRICT__ c, vec3 *const __RESTRICT__ r){
	// r = (b - a) x (c - a)
	vec3 f1, f2;
	vec3VSubVPR(b, a, &f1);
	vec3VSubVPR(c, a, &f2);
	vec3CrossPR(&f1, &f2, r);
}

__HINT_INLINE__ vec3 barycentric(const vec3 a, const vec3 b, const vec3 c, const vec3 p){
	// Calculate the barycentric coordinates of
	// point p in triangle abc.
	vec3 r;
	const vec3 v1 = vec3VSubV(b, a);
	const vec3 v2 = vec3VSubV(c, a);
	const vec3 v3 = vec3VSubV(p, a);
	const float d11 = vec3Dot(v1, v1);
	const float d12 = vec3Dot(v1, v2);
	const float d22 = vec3Dot(v2, v2);
	const float d31 = vec3Dot(v3, v1);
	const float d32 = vec3Dot(v3, v2);
	const float denom = d11 * d22 - d12 * d12;
	const float denomInv = denom == 0.f ? 0.f : 1.f / denom;
	r.z = (d11 * d32 - d12 * d31) * denomInv;
	r.y = (d22 * d31 - d12 * d32) * denomInv;
	r.x = 1.f - r.y - r.z;
	return r;
}
__HINT_INLINE__ void barycentricP(const vec3 *const __RESTRICT__ a, const vec3 *const __RESTRICT__ b, const vec3 *const __RESTRICT__ c, vec3 *const __RESTRICT__ p){
	// Calculate the barycentric coordinates of
	// point p in triangle abc.
	vec3 v1, v2, v3;
	float d11, d12, d22, d31, d32, denom, denomInv;
	vec3VSubVPR(b, a, &v1);
	vec3VSubVPR(c, a, &v2);
	vec3VSubVPR(p, a, &v3);
	d11 = vec3DotP(&v1, &v1);
	d12 = vec3DotP(&v1, &v2);
	d22 = vec3DotP(&v2, &v2);
	d31 = vec3DotP(&v3, &v1);
	d32 = vec3DotP(&v3, &v2);
	denom = d11 * d22 - d12 * d12;
	denomInv = denom == 0.f ? 0.f : 1.f / denom;
	p->z = (d11 * d32 - d12 * d31) * denomInv;
	p->y = (d22 * d31 - d12 * d32) * denomInv;
	p->x = 1.f - p->y - p->z;
}
__HINT_INLINE__ void barycentricPR(const vec3 *const __RESTRICT__ a, const vec3 *const __RESTRICT__ b, const vec3 *const __RESTRICT__ c, const vec3 *const __RESTRICT__ p, vec3 *const __RESTRICT__ r){
	// Calculate the barycentric coordinates of
	// point p in triangle abc.
	vec3 v1, v2, v3;
	float d11, d12, d22, d31, d32, denom, denomInv;
	vec3VSubVPR(b, a, &v1);
	vec3VSubVPR(c, a, &v2);
	vec3VSubVPR(p, a, &v3);
	d11 = vec3DotP(&v1, &v1);
	d12 = vec3DotP(&v1, &v2);
	d22 = vec3DotP(&v2, &v2);
	d31 = vec3DotP(&v3, &v1);
	d32 = vec3DotP(&v3, &v2);
	denom = d11 * d22 - d12 * d12;
	denomInv = denom == 0.f ? 0.f : 1.f / denom;
	r->z = (d11 * d32 - d12 * d31) * denomInv;
	r->y = (d22 * d31 - d12 * d32) * denomInv;
	r->x = 1.f - r->y - r->z;
}

__HINT_INLINE__ float pointPlaneDistance(const vec3 normal, const vec3 vertex, const vec3 point){
	// Calculates the distance
	// between a point and a plane.
	return vec3Dot(normal, vec3VSubV(point, vertex));
}
__HINT_INLINE__ float pointPlaneDistanceP(const vec3 *const __RESTRICT__ normal, const vec3 *const __RESTRICT__ vertex, const vec3 *const __RESTRICT__ point){
	// Calculates the distance
	// between a point and a plane.
	vec3 offset;
	vec3VSubVPR(point, vertex, &offset);
	return vec3DotP(normal, &offset);
}
__HINT_INLINE__ vec3 pointPlaneProject(const vec3 normal, const vec3 vertex, const vec3 point){
	// Projects a point onto a plane.
	//return vec3VSubV(point, vec3VMultS(normal, vec3Dot(vec3VSubV(point, vertex), normal)));
	return vec3fmaf(vec3Dot(vec3VSubV(vertex, point), normal), normal, point);
}
__HINT_INLINE__ void pointPlaneProjectP(const vec3 *const __RESTRICT__ normal, const vec3 *const __RESTRICT__ vertex, vec3 *const __RESTRICT__ point){
	// Projects a point onto a plane.
	vec3 psv;
	float dot;
	vec3VSubVPR(vertex, point, &psv);
	dot = vec3DotP(&psv, normal);
	vec3fmafP(dot, normal, point);
}
__HINT_INLINE__ void pointPlaneProjectPR(const vec3 *const __RESTRICT__ normal, const vec3 *const __RESTRICT__ vertex, const vec3 *const __RESTRICT__ point, vec3 *const __RESTRICT__ r){
	// Projects a point onto a plane.
	vec3 psv;
	float dot;
	vec3VSubVPR(vertex, point, &psv);
	dot = vec3DotP(&psv, normal);
	vec3fmafPR(dot, normal, point, r);
}

__HINT_INLINE__ vec3 linePlaneIntersection(const vec3 normal, const vec3 vertex, const vec3 line){
	// Finds the intersection between
	// a line and a plane.
	const float startDistance = vec3Dot(normal, vertex);
	const float t = startDistance / (startDistance + pointPlaneDistance(normal, vertex, line));
	// Lerp
	return vec3VMultS(line, t);
}
__HINT_INLINE__ void linePlaneIntersectionP(const vec3 *const __RESTRICT__ normal, const vec3 *const __RESTRICT__ vertex, const vec3 *const __RESTRICT__ line, vec3 *const __RESTRICT__ point){
	// Finds the intersection between
	// a line and a plane.
	const float startDistance = vec3DotP(normal, vertex);
	const float t = startDistance / (startDistance + pointPlaneDistanceP(normal, vertex, line));
	// Lerp
	vec3VMultSPR(line, t, point);
}
__HINT_INLINE__ vec3 segmentPlaneIntersection(const vec3 normal, const vec3 vertex, const vec3 start, const vec3 end){
	// Finds the intersection between
	// a line segment and a plane.
	const float startDistance = pointPlaneDistance(normal, vertex, start);
	return vec3Lerp(start, end, startDistance / (startDistance - pointPlaneDistance(normal, vertex, end)));
}
__HINT_INLINE__ return_t segmentPlaneIntersectionR(const vec3 normal, const vec3 vertex, const vec3 start, const vec3 end, vec3 *const __RESTRICT__ point){
	// Finds the intersection between
	// a line segment and a plane.
	const float startDistance = pointPlaneDistance(normal, vertex, start);
	const float endDistance = pointPlaneDistance(normal, vertex, end);
	if(startDistance <= 0.f){
		if(endDistance <= 0.f){
			return 0;
		}
	}else if(endDistance > 0.f){
		return 0;
	}
	*point = vec3Lerp(start, end, startDistance / (startDistance - endDistance));
	return 1;
}
__HINT_INLINE__ return_t segmentPlaneIntersectionP(const vec3 *const __RESTRICT__ normal, const vec3 *const __RESTRICT__ vertex, const vec3 *const __RESTRICT__ start, const vec3 *const __RESTRICT__ end, vec3 *const __RESTRICT__ point){
	// Finds the intersection between
	// a line segment and a plane.
	const float startDistance = pointPlaneDistanceP(normal, vertex, start);
	const float endDistance = pointPlaneDistanceP(normal, vertex, end);
	if(startDistance <= 0.f){
		if(endDistance <= 0.f){
			return 0;
		}
	}else if(endDistance > 0.f){
		return 0;
	}
	vec3LerpPR(start, end, startDistance / (startDistance - endDistance), point);
	return 1;
}

__HINT_INLINE__ void segmentClosestPoints(const vec3 s1, const vec3 e1, const vec3 s2, const vec3 e2, vec3 *const __RESTRICT__ p1, vec3 *const __RESTRICT__ p2){
	// Finds the closest points that
	// lie on the two line segments.
	const vec3 v1 = vec3VSubV(s1, s2);
	const vec3 v2 = vec3VSubV(e1, s1);
	const vec3 v3 = vec3VSubV(e2, s2);
	const float d12 = vec3Dot(v1, v2);
	const float d13 = vec3Dot(v1, v3);
	const float d22 = vec3Dot(v2, v2);
	const float d32 = vec3Dot(v3, v2);
	const float d33 = vec3Dot(v3, v3);
	const float denom = d22 * d33 - d32 * d32;
	// If the denominator is 0, use 0.5 as the position
	// along the first line segment. This puts the closest
	// point in the very center.
	const float m1 = denom == 0.f ? 0.5f : (d13 * d32 - d12 * d33) / denom;
	const float m2 = (d13 + m1 * d32) / d33;
	// Calculate the point on the first line segment (m1*v2 + s1).
	*p1 = vec3fmaf(m1, v2, s1);
	// Calculate the point on the second line segment (m2*v3 + s2).
	*p2 = vec3fmaf(m2, v3, s2);
}
__HINT_INLINE__ void segmentClosestPointsP(const vec3 *const __RESTRICT__ s1, const vec3 *const __RESTRICT__ e1, const vec3 *const __RESTRICT__ s2, const vec3 *const __RESTRICT__ e2, vec3 *const __RESTRICT__ p1, vec3 *const __RESTRICT__ p2){
	// Finds the closest points that
	// lie on the two line segments.
	vec3 v1, v2, v3;
	float d12, d13, d22, d32, d33, denom;
	float m1, m2;
	vec3VSubVPR(s1, s2, &v1);
	vec3VSubVPR(e1, s1, &v2);
	vec3VSubVPR(e2, s2, &v3);
	d12 = vec3DotP(&v1, &v2);
	d13 = vec3DotP(&v1, &v3);
	d22 = vec3DotP(&v2, &v2);
	d32 = vec3DotP(&v3, &v2);
	d33 = vec3DotP(&v3, &v3);
	denom = d22 * d33 - d32 * d32;
	// If the denominator is 0, use 0.5 as the position
	// along the first line segment. This puts the closest
	// point in the very center.
	m1 = denom == 0.f ? 0.5f : (d13 * d32 - d12 * d33) / denom;
	m2 = (d13 + m1 * d32) / d33;
	// Calculate the point on the first line segment (m1*v2 + s1).
	vec3fmafPR(m1, &v2, s1, p1);
	// Calculate the point on the second line segment (m2*v3 + s2).
	vec3fmafPR(m2, &v3, s2, p2);
}
__HINT_INLINE__ vec3 segmentClosestPointReference(const vec3 s1, const vec3 e1, const vec3 s2, const vec3 e2){
	// Finds the closest points that
	// lie on the two line segments.
	const vec3 v1 = vec3VSubV(s1, s2);
	const vec3 v2 = vec3VSubV(e1, s1);
	const vec3 v3 = vec3VSubV(e2, s2);
	const float d12 = vec3Dot(v1, v2);
	const float d13 = vec3Dot(v1, v3);
	const float d22 = vec3Dot(v2, v2);
	const float d32 = vec3Dot(v3, v2);
	const float d33 = vec3Dot(v3, v3);
	const float denom = d22 * d33 - d32 * d32;
	// If the denominator is 0, use 0.5 as the position
	// along the first line segment. This puts the closest
	// point in the very center.
	const float m1 = denom == 0.f ? 0.5f : (d13 * d32 - d12 * d33) / denom;
	// Calculate the point on the first line segment (m1*v2 + s1).
	return vec3fmaf(m1, v2, s1);
}
__HINT_INLINE__ void segmentClosestPointReferencePR(const vec3 *const __RESTRICT__ s1, const vec3 *const __RESTRICT__ e1, const vec3 *const __RESTRICT__ s2, const vec3 *const __RESTRICT__ e2, vec3 *const __RESTRICT__ p1){
	// Finds the closest point that
	// lies on the two line segments.
	vec3 v1, v2, v3;
	float d12, d13, d22, d32, d33, denom;
	float m1;
	vec3VSubVPR(s1, s2, &v1);
	vec3VSubVPR(e1, s1, &v2);
	vec3VSubVPR(e2, s2, &v3);
	d12 = vec3DotP(&v1, &v2);
	d13 = vec3DotP(&v1, &v3);
	d22 = vec3DotP(&v2, &v2);
	d32 = vec3DotP(&v3, &v2);
	d33 = vec3DotP(&v3, &v3);
	denom = d22 * d33 - d32 * d32;
	// If the denominator is 0, use 0.5 as the position
	// along the first line segment. This puts the closest
	// point in the very center.
	m1 = denom == 0.f ? 0.5f : (d13 * d32 - d12 * d33) / denom;
	// Calculate the point on the first line segment (m1*v2 + s1).
	vec3fmafPR(m1, &v2, s1, p1);
}
__HINT_INLINE__ vec3 segmentClosestPointIncident(const vec3 s1, const vec3 e1, const vec3 s2, const vec3 e2){
	// Finds the closest points that
	// lie on the two line segments.
	const vec3 v1 = vec3VSubV(s1, s2);
	const vec3 v2 = vec3VSubV(e1, s1);
	const vec3 v3 = vec3VSubV(e2, s2);
	const float d12 = vec3Dot(v1, v2);
	const float d13 = vec3Dot(v1, v3);
	const float d22 = vec3Dot(v2, v2);
	const float d32 = vec3Dot(v3, v2);
	const float d33 = vec3Dot(v3, v3);
	const float denom = d22 * d33 - d32 * d32;
	// If the denominator is 0, use 0.5 as the position
	// along the first line segment. This puts the closest
	// point in the very center.
	const float m1 = denom == 0.f ? 0.5f : (d13 * d32 - d12 * d33) / denom;
	const float m2 = (d13 + m1 * d32) / d33;
	// Calculate the point on the second line segment (m2*v3 + s2).
	return vec3fmaf(m2, v3, s2);
}
__HINT_INLINE__ void segmentClosestPointIncidentPR(const vec3 *const __RESTRICT__ s1, const vec3 *const __RESTRICT__ e1, const vec3 *const __RESTRICT__ s2, const vec3 *const __RESTRICT__ e2, vec3 *const __RESTRICT__ p2){
	// Finds the closest point that
	// lies on the two line segments.
	vec3 v1, v2, v3;
	float d12, d13, d22, d32, d33, denom;
	float m1, m2;
	vec3VSubVPR(s1, s2, &v1);
	vec3VSubVPR(e1, s1, &v2);
	vec3VSubVPR(e2, s2, &v3);
	d12 = vec3DotP(&v1, &v2);
	d13 = vec3DotP(&v1, &v3);
	d22 = vec3DotP(&v2, &v2);
	d32 = vec3DotP(&v3, &v2);
	d33 = vec3DotP(&v3, &v3);
	denom = d22 * d33 - d32 * d32;
	// If the denominator is 0, use 0.5 as the position
	// along the first line segment. This puts the closest
	// point in the very center.
	m1 = denom == 0.f ? 0.5f : (d13 * d32 - d12 * d33) / denom;
	m2 = (d13 + m1 * d32) / d33;
	// Calculate the point on the second line segment (m2*v3 + s2).
	vec3fmafPR(m2, &v3, s2, p2);
}

#ifndef FP_FAST_FMAF
__HINT_INLINE__ float floatLerp(const float f1, const float f2, const float t){
	return f1 + (f2 - f1) * t;
}
__HINT_INLINE__ float floatMA(const float f1, const float f2, const float t){
	return f1 + f2 * t;
}
#endif

__FORCE_INLINE__ float floatMin(const float x, const float y){
	return x < y ? x : y;
}

__FORCE_INLINE__ float floatMax(const float x, const float y){
	return x > y ? x : y;
}

__FORCE_INLINE__ float floatClamp(const float x, const float min, const float max){
	if(x <= min){
		return min;
	}
	return x > max ? max : x;
}

__HINT_INLINE__ float clampEllipseDistanceFast(const float Ex, const float Ey, const float Ea, const float Eb){

	// Clamp a quaternion's swing to an ellipse.
	//
	// Given a point P = (Ex, Ey) and an ellipse with radii Ea, Eb, return
	// the distance from P to the closest point on the ellipse using a fast,
	// approximate algorithm. That is, rather than finding the actual closest
	// point, we take the point of intersection along the line from the origin.
	//
	// This function returns the distance of this closest point from the ellipse's
	// centre. This distance is the maximum allowable angle about the swing axis.
	//
	// Note that we assume that Ea and Eb are non-negative. Furthermore, this
	// method generally gives poor results when Ea and Eb differ significantly.

	if(Ex != 0.f){
		// Consider the ellipse "x^2/a^2 + y^2/b^2 = 1",
		// and let (x0, y0) be a point satisfying
		//
		// x0^2/a^2 + y0^2/b^2 = L^2,
		// 1/L = ab/sqrt(b^2*x0^2 + a^2*y0^2).
		//
		// Then the closest point on the ellipse to (x0, y0)
		// is given by (1/L)(x0, y0). The distance is then
		// given simply by d = ||(x0, y0)||/L.
		const float Ebx = Eb*Ex;
		const float Eay = Ea*Ey;
		// Make sure we don't divide by 0.
		if(floatIsZero(Ebx) && floatIsZero(Eay)){
			return 0.f;
		}
		return (Ea*Eb) * sqrtf((Ex*Ex + Ey*Ey)/(Ebx*Ebx + Eay*Eay));
	}else{
		return Eb;
	}

}

__HINT_INLINE__ float clampEllipseDistanceNormalFast(const float Ex, const float Ey, const float Ea, const float Eb, vec2 *const restrict normal){

	// Return the distance from the origin to the closest point on the ellipse
	// with radii Ea, Eb to the point (Ex, Ey). This also computes the unit
	// normal at this point on the ellipse.

	if(Ex != 0.f){
		// Consider the ellipse "x^2/a^2 + y^2/b^2 = 1",
		// and let (x0, y0) be a point satisfying
		//
		// x0^2/a^2 + y0^2/b^2 = L^2,
		// 1/L = ab/sqrt(b^2*x0^2 + a^2*y0^2).
		//
		// Then the closest point on the ellipse to (x0, y0)
		// is given by (1/L)(x0, y0). The distance is then
		// given simply by d = ||(x0, y0)||/L.
		const float Ebx = Eb*Ex;
		const float Eay = Ea*Ey;
		// Make sure we don't divide by 0.
		if(floatIsZero(Ebx) && floatIsZero(Eay)){
			normal->x = 0.f;
			normal->y = 1.f;
			return 0.f;
		}
		// Using the formulae above, we can show that the
		// normal at the intersection point is given by
		//
		// (b^2*t_x, a^2*t_y).
		*normal = vec2NormalizeFastS(Eb*Ebx, Ea*Eay);
		return (Ea*Eb) * sqrtf((Ex*Ex + Ey*Ey)/(Ebx*Ebx + Eay*Eay));
	}else{
		normal->x = 0.f;
		normal->y = Ey < 0.f ? -1.f : 1.f;
		return Eb;
	}
}
