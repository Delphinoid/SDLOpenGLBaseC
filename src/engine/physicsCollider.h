#ifndef PHYSICSCOLLIDER_H
#define PHYSICSCOLLIDER_H

#include "physicsShared.h"
#include "collider.h"
#include "mat3.h"

// The collider with the larger address
// is the "owner" of a particular contact
// or separation pair. Each collider will
// store the pairs that it owns first in
// these linked lists, sorted by smallest
// colliderB address to largest.
//
// The last linked list node points to the
// last pair that the collider is involved
// in but doesn't own. This list may be
// traversed with "prevB" and "nextB". It
// is not sorted.

typedef struct aabbNode aabbNode;
typedef struct physContactPair physContactPair;
typedef struct physSeparationPair physSeparationPair;

typedef struct physCollider {

	collider c;  // Convex collider in global space.
	             // Set as a base collider if the physCollider has no base.
	cAABB aabb;  // Tightly-fitting AABB, as opposed to the node's looser bounding box.

	// Physical properties.
	float density;      // Collider density.
	float friction;     // The coefficient of friction.
	float restitution;  // The coefficient of restitution, or the ratio of energy kept after a collision.

	// Broadphase data.
	// Contacts and separations are stored in QLinks, as both colliders
	// involved in a collision store them in their own linked lists.
	aabbNode *node;  // Pointer to the collider's node in the AABB tree.
	physContactPair    *contactCache;     // A QLink of contact pairs.
	physSeparationPair *separationCache;  // A QLink of separation pairs.

	void *body;      // Owner rigid body or rigid body base.
	collider *base;  // Convex collider in local space.

	// Collision layer and mask.
	// Each bit of the layer refers to a collision layer that the collider is
	// on, while the mask contains the set of layers it permits collisions with.
	colliderMask_t layer;
	colliderMask_t mask;

} physCollider;

// Forward declarations for inlining.
extern void (* const physColliderGenerateMassJumpTable[COLLIDER_TYPE_NUM])(
	void *const __RESTRICT__ local,
	float *const __RESTRICT__ mass,
	float *const __RESTRICT__ inverseMass,
	vec3 *const __RESTRICT__ centroid,
	const float **const __RESTRICT__ vertexMassArray
);
extern void (* const physColliderGenerateMomentJumpTable[COLLIDER_TYPE_NUM])(
	void *const __RESTRICT__ local,
	mat3 *const __RESTRICT__ inertiaTensor,
	vec3 *const __RESTRICT__ centroid,
	const float **const __RESTRICT__ vertexMassArray
);
extern void (* const physColliderTransformJumpTable[COLLIDER_TYPE_NUM])(
	physCollider *const __RESTRICT__ c
);

void physColliderInit(physCollider *const __RESTRICT__ c, const colliderType_t type, void *const __RESTRICT__ body);
void physColliderInstantiate(physCollider *const __RESTRICT__ instance, physCollider *const __RESTRICT__ local, void *const __RESTRICT__ body);
return_t physColliderPermitCollision(const physCollider *const __RESTRICT__ c1, const physCollider *const __RESTRICT__ c2);

void physColliderGenerateMass(collider *const __RESTRICT__ local, float *const __RESTRICT__ mass, float *const __RESTRICT__ inverseMass, vec3 *const __RESTRICT__ centroid, const float **const vertexMassArray);
void physColliderGenerateMoment(collider *const __RESTRICT__ local, mat3 *const __RESTRICT__ inertiaTensor, vec3 *const __RESTRICT__ centroid, const float **const vertexMassArray);
void physColliderTransform(physCollider *const __RESTRICT__ c);

physContactPair *physColliderFindContact(const physCollider *const c1, const physCollider *const c2, physContactPair **const previous, physContactPair **const next);
physSeparationPair *physColliderFindSeparation(const physCollider *const c1, const physCollider *const c2, physSeparationPair **const previous, physSeparationPair **const next);

#ifdef PHYSICS_CONTACT_STABILIZER_BAUMGARTE
void physColliderUpdateContacts(physCollider *const c, const float frequency);
#else
void physColliderUpdateContacts(physCollider *const c);
#endif
void physColliderUpdateSeparations(physCollider *const c);

void physColliderDelete(physCollider *const __RESTRICT__ c);

#endif
