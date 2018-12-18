#ifndef COLLISION_H
#define COLLISION_H

#include "collider.h"
#include "vec3.h"
#include "return.h"

#define COLLISION_MANIFOLD_MAX_CONTACT_POINTS 4
#define COLLISION_MANIFOLD_CONTACT_TANGENTS 2

#define COLLISION_CACHE_SEPARATION_TYPE_NULL   0
#define COLLISION_CACHE_SEPARATION_TYPE_FACE_1 1  // cSeparationCache contains a face from the first
                                                  // collider and a vertex from the second collider.
#define COLLISION_CACHE_SEPARATION_TYPE_FACE_2 2  // cSeparationCache contains a face from the second
                                                  // collider and a vertex from the first collider.
#define COLLISION_CACHE_SEPARATION_TYPE_EDGE   3  // cSeparationCache contains two edges.

typedef uint_least8_t separationType_t;
typedef uint_least8_t contactIndex_t;

typedef struct {
	separationType_t type;
	size_t featureA;
	size_t featureB;
} cSeparationCache;

typedef struct {
	vec3 pointA;  // Contact point on collider A's surface.
	vec3 pointB;  // Contact point on collider B's surface.
	float depth;
} cContact;

typedef struct {
	vec3 normal;
	contactIndex_t contactNum;
	cContact contacts[COLLISION_MANIFOLD_MAX_CONTACT_POINTS];
	vec3 tangents[COLLISION_MANIFOLD_CONTACT_TANGENTS];
} cContactManifold;

return_t cCollision(const collider *const restrict c1, const vec3 *const restrict c1c, const collider *const restrict c2, const vec3 *const restrict c2c, cSeparationCache *const restrict info, cContactManifold *const restrict cm);
void cSeparationCacheInit(cSeparationCache *const restrict sc);
void cContactManifoldInit(cContactManifold *const restrict cm);
void cGenerateContactTangents(const vec3 *const restrict normal, vec3 *const restrict tangentA, vec3 *const restrict tangentB);

#endif
