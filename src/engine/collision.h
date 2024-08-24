#ifndef COLLISION_H
#define COLLISION_H

#include "collider.h"

#ifndef COLLISION_MANIFOLD_MAX_CONTACT_POINTS
	#define COLLISION_MANIFOLD_MAX_CONTACT_POINTS 4
#else
	#define COLLISION_MANIFOLD_REDUCTION_DISABLED
#endif

typedef uint_least8_t cContactPointIndex_t;
typedef uint_least8_t cSeparationFeature_t;

// Stores the indices of the edges involved
// in clipping a particular contact point.
// Allows for the identification of persistent contact points.
// Note that A corresponds to the reference collider,
// while B corresponds to the incident collider.
typedef struct {
	#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
	cEdgeIndex_t edgeA;
	cEdgeIndex_t edgeB;
	#else
	cEdgeIndex_t inEdgeA;
	cEdgeIndex_t outEdgeA;
	cEdgeIndex_t inEdgeB;
	cEdgeIndex_t outEdgeB;
	#endif
} cContactKey;

typedef struct cContactPoint {

	// Global contact points on the reference and incident colliders.
	vec3 pointA;
	vec3 pointB;

	// Contact normal.
	vec3 normal;

	// Penetration depth.
	float separation;

	// Key for temporal coherence.
	cContactKey key;

} cContactPoint;

typedef struct cContact {

	// Contact array.
	cContactPoint contacts[COLLISION_MANIFOLD_MAX_CONTACT_POINTS];

	// Number of contacts.
	cContactPointIndex_t contactNum;

} cContact;

typedef struct cSeparation {
	size_t featureA;
	size_t featureB;
	cSeparationFeature_t type;
} cSeparation;

// Stores a type of separation.
/**
typedef union cSeparation {
	cHullSeparation mesh;
} cSeparation;
**/

// Forward declarations for inlining.
extern return_t (* const cCollisionJumpTable[COLLIDER_TYPE_NUM][COLLIDER_TYPE_NUM])(
	const void *const __RESTRICT__,
	const void *const __RESTRICT__,
	void *const __RESTRICT__,
	cContact *const __RESTRICT__
);
extern return_t (* const cSeparationJumpTable[COLLIDER_TYPE_NUM][COLLIDER_TYPE_NUM])(
	const void *const __RESTRICT__,
	const void *const __RESTRICT__,
	const void *const __RESTRICT__
);

return_t cCheckCollision(const collider *const __RESTRICT__ c1, const collider *const __RESTRICT__ c2, cSeparation *const __RESTRICT__ sc, cContact *const __RESTRICT__ cm);
return_t cCheckSeparation(const collider *const __RESTRICT__ c1, const collider *const __RESTRICT__ c2, const cSeparation *const __RESTRICT__ sc);

#endif
