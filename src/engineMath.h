#ifndef ENGINEMATH_H
#define ENGINEMATH_H

#include "vec3.h"

#ifndef M_PI
	#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062
#endif

#ifndef INFINITY
	#define INFINITY  0x7F800000
#endif

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
signed char segmentPlaneIntersection(const vec3 *normal, const vec3 *vertex, const vec3 *start, const vec3 *end, vec3 *point);

#endif
