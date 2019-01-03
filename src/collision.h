#ifndef COLLISION_H
#define COLLISION_H

#include "collider.h"
#include "vec3.h"
#include "return.h"

#ifndef COLLISION_MANIFOLD_MAX_CONTACT_POINTS
	#define COLLISION_MANIFOLD_MAX_CONTACT_POINTS 4
	#define COLLISION_MANIFOLD_REDUCTION_ENABLED
#else
	#undef COLLISION_MANIFOLD_REDUCTION_ENABLED
#endif

// The size of the largest separation type.
// In this case, it is the mesh-mesh separation at 9 bytes.
#define COLLISION_SEPARATION_MAX_SIZE 9

typedef uint_least8_t cContactIndex_t;

typedef struct {
	vec3 pointA;  // Contact point on collider A's surface.
	vec3 pointB;  // Contact point on collider B's surface.
	float depth;
} cContact;

typedef struct {
	vec3 normal;
	vec3 tangents[2];
	cContact contacts[COLLISION_MANIFOLD_MAX_CONTACT_POINTS];
	cContactIndex_t contactNum;
} cContactManifold;

// Stores a type of separation.
typedef struct {
	byte_t data[COLLISION_SEPARATION_MAX_SIZE];
} cSeparationContainer;

return_t cCollision(const collider *const restrict c1, const vec3 *const restrict c1c, const collider *const restrict c2, const vec3 *const restrict c2c, cSeparationContainer *const restrict sc, cContactManifold *const restrict cm);
return_t cSeparation(const collider *const restrict c1, const vec3 *const restrict c1c, const collider *const restrict c2, const vec3 *const restrict c2c, const cSeparationContainer *const restrict sc);

void cGenerateContactTangents(const vec3 *const restrict normal, vec3 *const restrict tangentA, vec3 *const restrict tangentB);

#endif
