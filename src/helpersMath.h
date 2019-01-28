#ifndef HELPERSMATH_H
#define HELPERSMATH_H

#include "vec3.h"
#include "return.h"

float fastInvSqrt(float x);
float fastInvSqrtAccurate(float x);

vec3 getPointLineProjection(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict p);
void pointLineProject(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict p, vec3 *const restrict r);

vec3 getFaceNormal(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c);
void faceNormal(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, vec3 *const restrict r);

vec3 getBarycentric(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, const vec3 *const restrict p);
void barycentric(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, vec3 *const restrict p);
void barycentricR(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, const vec3 *const restrict p, vec3 *const restrict r);

float pointPlaneDistance(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict point);
void pointPlaneProject(const vec3 *const restrict normal, const vec3 *const restrict vertex, vec3 *const restrict point);
void pointPlaneProjectR(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict point, vec3 *const restrict r);

void linePlaneIntersection(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict line, vec3 *const restrict point);
return_t segmentPlaneIntersection(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict start, const vec3 *const restrict end, vec3 *const restrict point);

void segmentClosestPoints(const vec3 *const restrict s1, const vec3 *const restrict e1, const vec3 *const restrict s2, const vec3 *const restrict e2, vec3 *const restrict p1, vec3 *const restrict p2);
void segmentClosestPointReference(const vec3 *const restrict s1, const vec3 *const restrict e1, const vec3 *const restrict s2, const vec3 *const restrict e2, vec3 *const restrict p1);
void segmentClosestPointIncident(const vec3 *const restrict s1, const vec3 *const restrict e1, const vec3 *const restrict s2, const vec3 *const restrict e2, vec3 *const restrict p2);

float floatLerp(const float f1, const float f2, const float t);

#endif
