#ifndef PHYSICSCOLLIDER_H
#define PHYSICSCOLLIDER_H

#include "physicsShared.h"
#include "collider.h"
#include "mat3.h"

/*
** The pair linked lists are ordered from
** smallest colliderB address to largest.
**
** The collider with the larger address
** is the "owner" of a particular pair.
**
** The last linked list node points to the
** last pair that the collider is involved
** in but doesn't own. This list may be
** traversed with "prevB" and "nextB". It
** is not sorted.
*/

typedef struct aabbNode aabbNode;
typedef struct physContactPair physContactPair;
typedef struct physSeparationPair physSeparationPair;
typedef struct physIsland physIsland;

typedef uint_least16_t physCollisionMask_t;

typedef struct physCollider {

	collider c;  // Convex collider in global space.
	             // Set as a base collider if the physCollider has no base.
	cAABB aabb;  // Tightly-fitting AABB, as opposed to the node's looser bounding box.

	// Physical properties.
	float density;      // Collider density.
	float friction;     // The coefficient of friction.
	float restitution;  // The coefficient of restitution, or the ratio of energy kept after a collision.

	// Broadphase data.
	aabbNode *node;  // Pointer to the collider's node in the AABB tree.
	physContactPair    *contactCache;     // A QLink of contact pairs.
	physSeparationPair *separationCache;  // A QLink of separation pairs.

	void *body;      // Owner rigid body or rigid body base.
	collider *base;  // Convex collider in local space.

	// Collision layer bitmask.
	physCollisionMask_t layers;

} physCollider;

// Forward declarations for inlining.
extern void (* const physColliderGenerateMassJumpTable[COLLIDER_TYPE_NUM])(
	void *const restrict local,
	float *const restrict mass,
	float *const restrict inverseMass,
	vec3 *const restrict centroid,
	const float **const restrict vertexMassArray
);
extern void (* const physColliderGenerateMomentJumpTable[COLLIDER_TYPE_NUM])(
	void *const restrict local,
	mat3 *const restrict inertiaTensor,
	vec3 *const restrict centroid,
	const float **const restrict vertexMassArray
);
extern return_t (* const physColliderTransformJumpTable[COLLIDER_TYPE_NUM])(
	physCollider *const restrict c,
	physIsland *const restrict island
);

void physColliderInit(physCollider *const restrict c, const colliderType_t type, void *const restrict body);
void physColliderInstantiate(physCollider *const restrict instance, physCollider *const restrict local, void *const restrict body);
return_t physColliderPermitCollision(const physCollider *const restrict c1, const physCollider *const restrict c2);

void physColliderGenerateMass(collider *const restrict local, float *const restrict mass, float *const restrict inverseMass, vec3 *const restrict centroid, const float **const vertexMassArray);
void physColliderGenerateMoment(collider *const restrict local, mat3 *const restrict inertiaTensor, vec3 *const restrict centroid, const float **const vertexMassArray);
return_t physColliderTransform(physCollider *const restrict c, physIsland *const restrict island);

physContactPair *physColliderFindContact(const physCollider *const c1, const physCollider *const c2, physContactPair **const previous, physContactPair **const next);
physSeparationPair *physColliderFindSeparation(const physCollider *const c1, const physCollider *const c2, physSeparationPair **const previous, physSeparationPair **const next);

#ifndef PHYSICS_GAUSS_SEIDEL_SOLVER
void physColliderUpdateContacts(physCollider *const c, const float dt);
#else
void physColliderUpdateContacts(physCollider *const c);
#endif
void physColliderUpdateSeparations(physCollider *const c);

void physColliderDelete(physCollider *const restrict c);

#endif
