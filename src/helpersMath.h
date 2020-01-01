#ifndef HELPERSMATH_H
#define HELPERSMATH_H

#include "vec3.h"
#include "return.h"
#include <math.h>

// Modern processors have reciprocal square root intrinsics,
// which will be faster than a software implementation.
#ifdef MATH_USE_FAST_INVERSE_SQRT
	#define rsqrt(x)         fastInvSqrt(x)
	#define rsqrtAccurate(x) fastInvSqrtAccurate(x)
#else
	#define rsqrt(x)         (1.f/sqrtf(x))
	#define rsqrtAccurate(x) (1.f/sqrtf(x))
#endif

float fastInvSqrt(float x);
float fastInvSqrtAccurate(float x);

vec3 pointLineProjection(const vec3 a, const vec3 b, const vec3 p);
void pointLineProjectionP(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict p, vec3 *const restrict r);

vec3 faceNormal(const vec3 a, const vec3 b, const vec3 c);
void faceNormalP(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, vec3 *const restrict r);

vec3 barycentric(const vec3 a, const vec3 b, const vec3 c, const vec3 p);
void barycentricP(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, vec3 *const restrict p);
void barycentricPR(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, const vec3 *const restrict p, vec3 *const restrict r);

float pointPlaneDistance(const vec3 normal, const vec3 vertex, const vec3 point);
float pointPlaneDistanceP(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict point);
vec3 pointPlaneProject(const vec3 normal, const vec3 vertex, const vec3 point);
void pointPlaneProjectP(const vec3 *const restrict normal, const vec3 *const restrict vertex, vec3 *const restrict point);
void pointPlaneProjectPR(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict point, vec3 *const restrict r);

vec3 linePlaneIntersection(const vec3 normal, const vec3 vertex, const vec3 line);
void linePlaneIntersectionP(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict line, vec3 *const restrict point);
vec3 segmentPlaneIntersection(const vec3 normal, const vec3 vertex, const vec3 start, const vec3 end);
return_t segmentPlaneIntersectionR(const vec3 normal, const vec3 vertex, const vec3 start, const vec3 end, vec3 *const restrict point);
return_t segmentPlaneIntersectionP(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict start, const vec3 *const restrict end, vec3 *const restrict point);

void segmentClosestPoints(const vec3 s1, const vec3 e1, const vec3 s2, const vec3 e2, vec3 *const restrict p1, vec3 *const restrict p2);
void segmentClosestPointsP(const vec3 *const restrict s1, const vec3 *const restrict e1, const vec3 *const restrict s2, const vec3 *const restrict e2, vec3 *const restrict p1, vec3 *const restrict p2);
vec3 segmentClosestPointReference(const vec3 s1, const vec3 e1, const vec3 s2, const vec3 e2);
void segmentClosestPointReferenceP(const vec3 *const restrict s1, const vec3 *const restrict e1, const vec3 *const restrict s2, const vec3 *const restrict e2, vec3 *const restrict p1);
vec3 segmentClosestPointIncident(const vec3 s1, const vec3 e1, const vec3 s2, const vec3 e2);
void segmentClosestPointIncidentP(const vec3 *const restrict s1, const vec3 *const restrict e1, const vec3 *const restrict s2, const vec3 *const restrict e2, vec3 *const restrict p2);

#ifdef FP_FAST_FMAF
#define floatLerp(f1, f2, t) fmaf(t, f2-f1, f1)
#define floatMA(f1, f2, t) fmaf(t, f2, f1)
#else
float floatLerp(const float f1, const float f2, const float t);
float floatMA(const float f1, const float f2, const float t);
#endif

#endif
