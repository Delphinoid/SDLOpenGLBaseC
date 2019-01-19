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
typedef struct {
	#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
	cEdgeIndex_t edgeA;
	cEdgeIndex_t edgeB;
	#else
	cEdgeIndex_t inEdgeR;
	cEdgeIndex_t outEdgeR;
	cEdgeIndex_t inEdgeI;
	cEdgeIndex_t outEdgeI;
	#endif
} cContactKey;

typedef struct cContactPoint {

	// Contact points in both colliders' global spaces.
	vec3 pointA;
	vec3 pointB;

	// Contact normal.
	vec3 normal;

	// Penetration depth.
	float penetrationDepth;

	#ifdef COLLISION_ENABLE_CONTACT_PHYSICS

	// Contact tangents for simulating friction.
	vec3 tangentA;
	vec3 tangentB;

	// Impulse magnitude denominators.
	float normalImpulse;
	float tangentImpulseA;
	float tangentImpulseB;

	// Impulse magnitude accumulator.
	float normalImpulseAccumulator;

	// Bias for warm starting.
	float bias;

	#endif

	// Key for temporal coherence.
	cContactKey key;

} cContactPoint;

typedef struct cContact {

	// Contact array.
	cContactPoint contacts[COLLISION_MANIFOLD_MAX_CONTACT_POINTS];

	#ifdef COLLISION_ENABLE_CONTACT_PHYSICS

	// Impulse magnitude accumulators for friction.
	float tangentImpulseAccumulatorA;
	float tangentImpulseAccumulatorB;

	#endif

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
	cMeshSeparation mesh;
} cSeparation;
**/

// Forward declarations for inlining.
extern return_t (* const cCollisionJumpTable[COLLIDER_TYPE_NUM][COLLIDER_TYPE_NUM])(
	const void *const restrict,
	const void *const restrict,
	void *const restrict,
	cContact *const restrict
);
extern return_t (* const cSeparationJumpTable[COLLIDER_TYPE_NUM][COLLIDER_TYPE_NUM])(
	const void *const restrict,
	const void *const restrict,
	const void *const restrict
);

void cContactGenerateTangents(const vec3 *const restrict normal, vec3 *const restrict tangentA, vec3 *const restrict tangentB);

return_t cCheckCollision(const collider *const restrict c1, const collider *const restrict c2, cSeparation *const restrict sc, cContact *const restrict cm);
return_t cCheckSeparation(const collider *const restrict c1, const collider *const restrict c2, const cSeparation *const restrict sc);

#endif
