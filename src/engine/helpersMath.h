#ifndef HELPERSMATH_H
#define HELPERSMATH_H

#include "vec2.h"
#include "vec3.h"
#include "return.h"
#include <math.h>

#define MATH_NORMALIZE_EPSILON 0.000001f
#define floatIsZero(x) (fabsf(x) < MATH_NORMALIZE_EPSILON)
#define floatIsUnit(x) (fabsf((x) - 1.f) < MATH_NORMALIZE_EPSILON)

// Modern processors have reciprocal square root intrinsics,
// which will be faster than a software implementation.
#ifdef MATH_USE_FAST_INVERSE_SQRT
	#define rsqrt(x)         fastInvSqrt(x)
	#define rsqrtAccurate(x) fastInvSqrtAccurate(x)
#else
	#define rsqrt(x)         (1.f/sqrtf(x))
	#define rsqrtAccurate(x) (1.f/sqrtf(x))
#endif

// Note that these are prone to double evaluation,
// so the equivalent functions should be used to prevent this.
#define floatMinFast(x, y) (((x) < (y)) ? (x) : (y))
#define floatMaxFast(x, y) (((x) > (y)) ? (x) : (y))
#define uintMinFast(x, y) (((x) < (y)) ? (x) : (y))
#define uintMaxFast(x, y) (((x) > (y)) ? (x) : (y))

#define floatIsZero(x) (fabsf(x) < MATH_NORMALIZE_EPSILON)
#define floatIsUnit(x) (fabsf((x) - 1.f) < MATH_NORMALIZE_EPSILON)

// According to Mark Harris in his 2015 blog "GPU Pro Tip: Lerp Faster in C++",
// we can achieve better performance and accuracy by using two fmas here.
//
// Note that these are prone to double evaluation,
// so the equivalent functions should be used to prevent this.
#ifdef FP_FAST_FMAF
	#define floatLerpFast(x, y, t) fmaf(t, y, fmaf(-t, x, x))
	#define floatLerpDiffFast(x, y, t) fmaf(t, y, x)
#else
	#define floatLerpFast(x, y, t) ((x) + (t)*((y) - (x)))
	#define floatLerpDiffFast(x, y, t) ((x) + (t)*(y))
#endif

// This should only be used on ancient hardware!
#ifdef MATH_USE_FAST_INV_SQRT
	#define invSqrt(x)     fastInvSqrtAccurate(x)
	#define invSqrtFast(x) fastInvSqrt(x)
#else
	#define invSqrt(x)     (1.f/sqrtf(x))
	#define invSqrtFast(x) (1.f/sqrtf(x))
#endif


float copySign(const float x, const float y);

float fastInvSqrt(float x);
float fastInvSqrtAccurate(float x);

vec3 pointLineProjection(const vec3 a, const vec3 b, const vec3 p);
void pointLineProjectionPR(const vec3 *const __RESTRICT__ a, const vec3 *const __RESTRICT__ b, const vec3 *const __RESTRICT__ p, vec3 *const __RESTRICT__ r);

vec3 faceNormal(const vec3 a, const vec3 b, const vec3 c);
void faceNormalPR(const vec3 *const __RESTRICT__ a, const vec3 *const __RESTRICT__ b, const vec3 *const __RESTRICT__ c, vec3 *const __RESTRICT__ r);

vec3 barycentric(const vec3 a, const vec3 b, const vec3 c, const vec3 p);
void barycentricP(const vec3 *const __RESTRICT__ a, const vec3 *const __RESTRICT__ b, const vec3 *const __RESTRICT__ c, vec3 *const __RESTRICT__ p);
void barycentricPR(const vec3 *const __RESTRICT__ a, const vec3 *const __RESTRICT__ b, const vec3 *const __RESTRICT__ c, const vec3 *const __RESTRICT__ p, vec3 *const __RESTRICT__ r);

float pointPlaneDistance(const vec3 normal, const vec3 vertex, const vec3 point);
float pointPlaneDistanceP(const vec3 *const __RESTRICT__ normal, const vec3 *const __RESTRICT__ vertex, const vec3 *const __RESTRICT__ point);
vec3 pointPlaneProject(const vec3 normal, const vec3 vertex, const vec3 point);
void pointPlaneProjectP(const vec3 *const __RESTRICT__ normal, const vec3 *const __RESTRICT__ vertex, vec3 *const __RESTRICT__ point);
void pointPlaneProjectPR(const vec3 *const __RESTRICT__ normal, const vec3 *const __RESTRICT__ vertex, const vec3 *const __RESTRICT__ point, vec3 *const __RESTRICT__ r);

vec3 linePlaneIntersection(const vec3 normal, const vec3 vertex, const vec3 line);
void linePlaneIntersectionP(const vec3 *const __RESTRICT__ normal, const vec3 *const __RESTRICT__ vertex, const vec3 *const __RESTRICT__ line, vec3 *const __RESTRICT__ point);
vec3 segmentPlaneIntersection(const vec3 normal, const vec3 vertex, const vec3 start, const vec3 end);
return_t segmentPlaneIntersectionR(const vec3 normal, const vec3 vertex, const vec3 start, const vec3 end, vec3 *const __RESTRICT__ point);
return_t segmentPlaneIntersectionP(const vec3 *const __RESTRICT__ normal, const vec3 *const __RESTRICT__ vertex, const vec3 *const __RESTRICT__ start, const vec3 *const __RESTRICT__ end, vec3 *const __RESTRICT__ point);

void segmentClosestPoints(const vec3 s1, const vec3 e1, const vec3 s2, const vec3 e2, vec3 *const __RESTRICT__ p1, vec3 *const __RESTRICT__ p2);
void segmentClosestPointsPR(const vec3 *const __RESTRICT__ s1, const vec3 *const __RESTRICT__ e1, const vec3 *const __RESTRICT__ s2, const vec3 *const __RESTRICT__ e2, vec3 *const __RESTRICT__ p1, vec3 *const __RESTRICT__ p2);
vec3 segmentClosestPointReference(const vec3 s1, const vec3 e1, const vec3 s2, const vec3 e2);
void segmentClosestPointReferencePR(const vec3 *const __RESTRICT__ s1, const vec3 *const __RESTRICT__ e1, const vec3 *const __RESTRICT__ s2, const vec3 *const __RESTRICT__ e2, vec3 *const __RESTRICT__ p1);
vec3 segmentClosestPointIncident(const vec3 s1, const vec3 e1, const vec3 s2, const vec3 e2);
void segmentClosestPointIncidentPR(const vec3 *const __RESTRICT__ s1, const vec3 *const __RESTRICT__ e1, const vec3 *const __RESTRICT__ s2, const vec3 *const __RESTRICT__ e2, vec3 *const __RESTRICT__ p2);

float floatLerp(const float f1, const float f2, const float t);
float floatMA(const float f1, const float f2, const float t);

float floatMin(const float x, const float y);
float floatMax(const float x, const float y);
float floatClamp(const float x, const float min, const float max);

float clampEllipseDistanceFast(const float Ex, const float Ey, const float Ea, const float Eb);
float clampEllipseDistanceNormalFast(const float Ex, const float Ey, const float Ea, const float Eb, vec2 *const restrict normal);

#endif
