#ifndef HELPERSMATH_H
#define HELPERSMATH_H

#include "vec3.h"
#include "return.h"

float fastInvSqrt(float x);
float fastInvSqrtAccurate(float x);

vec3 getPointLineProjection(const vec3 *a, const vec3 *b, const vec3 *p);
void pointLineProject(const vec3 *a, const vec3 *b, const vec3 *p, vec3 *r);

vec3 getFaceNormal(const vec3 *a, const vec3 *b, const vec3 *c);
void faceNormal(const vec3 *a, const vec3 *b, const vec3 *c, vec3 *r);

vec3 getBarycentric(const vec3 *a, const vec3 *b, const vec3 *c, const vec3 *p);
void barycentric(const vec3 *a, const vec3 *b, const vec3 *c, const vec3 *p, vec3 *r);

float pointPlaneDistance(const vec3 *normal, const vec3 *vertex, const vec3 *point);
void pointPlaneProject(const vec3 *normal, const vec3 *vertex, vec3 *point);

void linePlaneIntersection(const vec3 *normal, const vec3 *vertex, const vec3 *line, vec3 *point);
return_t segmentPlaneIntersection(const vec3 *normal, const vec3 *vertex, const vec3 *start, const vec3 *end, vec3 *point);

void segmentClosestPoints(const vec3 *s1, const vec3 *e1, const vec3 *s2, const vec3 *e2, vec3 *p1, vec3 *p2);

float floatLerp(const float f1, const float f2, const float t);

#endif
