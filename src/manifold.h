#ifndef MANIFOLD_H
#define MANIFOLD_H

#include <stdint.h>
#include "vec3.h"

#ifndef COLLISION_MANIFOLD_MAX_CONTACT_POINTS
	#define COLLISION_MANIFOLD_MAX_CONTACT_POINTS 4
	#define COLLISION_MANIFOLD_REDUCTION_ENABLED
#else
	#undef COLLISION_MANIFOLD_REDUCTION_ENABLED
#endif

typedef uint_least8_t cContactPointIndex_t;

typedef struct {
	vec3 pointA;  // Contact point on collider A's surface.
	vec3 pointB;  // Contact point on collider B's surface.
	float depth;
	float impulseDenominator;  // Denominator in the impulse magnitude equation. Stored here rather than in physContact for better packing.
} cContactPoint;

typedef struct {
	vec3 normal;
	vec3 tangents[2];  // Contact tangents used for simulating friction. Stored here rather than in physContact for efficiency.
	cContactPoint contacts[COLLISION_MANIFOLD_MAX_CONTACT_POINTS];
	cContactPointIndex_t contactNum;
} cContact;

void cContactGenerateTangents(const vec3 *const restrict normal, vec3 *const restrict tangentA, vec3 *const restrict tangentB);

#endif
