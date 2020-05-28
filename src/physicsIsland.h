#ifndef PHYSICSISLAND_H
#define PHYSICSISLAND_H

#include "physicsShared.h"
#include "aabbTree.h"

#ifndef PHYSICS_ISLAND_COLLIDER_AABB_PADDING
	#define PHYSICS_ISLAND_COLLIDER_AABB_PADDING 0.2f
#endif
#ifndef PHYSICS_ISLAND_COLLIDER_AABB_VELOCITY_FACTOR
	#define PHYSICS_ISLAND_COLLIDER_AABB_VELOCITY_FACTOR 1.f
#endif

#ifndef PHYSICS_ISLAND_QUERY_STACK_SIZE
	#define PHYSICS_ISLAND_QUERY_STACK_SIZE AABB_TREE_QUERY_STACK_SIZE
#endif

typedef struct physRigidBody physRigidBody;
typedef struct physCollider physCollider;
typedef struct physJoint physJoint;
typedef struct physContactPair physContactPair;
typedef struct physSeparationPair physSeparationPair;

typedef struct physIsland {

	// AABB tree for broadphase collision.
	aabbTree tree;

	// Arrays of rigid bodies and constraints.
	// All of these are stored in doubly-linked lists.
	physRigidBody *bodies;
	physJoint *joints;
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	memQLink *contacts;
	memQLink *separations;
	#else
	physContactPair *contacts;
	physSeparationPair *separations;
	#endif

} physIsland;

void physIslandInit(physIsland *const __RESTRICT__ island);

void physIslandInsertJoint(physIsland *const __RESTRICT__ island, physJoint *const joint);
void physIslandRemoveJoint(physIsland *const __RESTRICT__ island, physJoint *const joint);

void physIslandInsertRigidBody(physIsland *const __RESTRICT__ island, physRigidBody *const body);
void physIslandRemoveRigidBody(physIsland *const __RESTRICT__ island, physRigidBody *const body);

void physIslandInsertRigidBodies(physIsland *const __RESTRICT__ island, physRigidBody *const bodies, physicsBodyIndex_t bodyNum);
void physIslandRemoveRigidBodies(physIsland *const __RESTRICT__ island, physRigidBody *const bodies, physicsBodyIndex_t bodyNum);

#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
return_t physIslandTick(physIsland *const __RESTRICT__ island, const float dt, const float frequency);
#else
return_t physIslandTick(physIsland *const __RESTRICT__ island, const float dt);
#endif

void physIslandDelete(physIsland *const __RESTRICT__ island);

#endif
