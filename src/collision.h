#ifndef COLLISION_H
#define COLLISION_H

#include "collider.h"
#include "vec3.h"
#include "return.h"

#define COLLISION_MANIFOLD_MAX_CONTACT_POINTS 4
#define COLLISION_MANIFOLD_CONTACT_TANGENTS 2

// The size of the largest separation type.
// In this case, it is the mesh-mesh separation at 9 bytes.
#define COLLISION_SEPARATION_MAX_SIZE 9

typedef uint_least8_t cContactIndex_t;

// Stores a type of separation.
typedef struct {
	byte_t data[COLLISION_SEPARATION_MAX_SIZE];
} cSeparationContainer;

typedef struct {
	vec3 pointA;  // Contact point on collider A's surface.
	vec3 pointB;  // Contact point on collider B's surface.
	float depth;
} cContact;

typedef struct {
	vec3 normal;
	cContactIndex_t contactNum;
	cContact contacts[COLLISION_MANIFOLD_MAX_CONTACT_POINTS];
	vec3 tangents[COLLISION_MANIFOLD_CONTACT_TANGENTS];
} cContactManifold;

return_t cCollision(const collider *const restrict c1, const vec3 *const restrict c1c, const collider *const restrict c2, const vec3 *const restrict c2c, cSeparationContainer *const restrict sc, cContactManifold *const restrict cm);
return_t cSeparation(const collider *const restrict c1, const vec3 *const restrict c1c, const collider *const restrict c2, const vec3 *const restrict c2c, const cSeparationContainer *const restrict sc);

void cSeparationContainerInit(cSeparationContainer *const restrict sc);
void cContactManifoldInit(cContactManifold *const restrict cm);

void cGenerateContactTangents(const vec3 *const restrict normal, vec3 *const restrict tangentA, vec3 *const restrict tangentB);

#endif
