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

#ifndef PHYSICS_ISLAND_VELOCITY_SOLVER_ITERATIONS
	#define PHYSICS_ISLAND_VELOCITY_SOLVER_ITERATIONS 4
#endif
#ifndef PHYSICS_ISLAND_CONFIGURATION_SOLVER_ITERATIONS
	#define PHYSICS_ISLAND_CONFIGURATION_SOLVER_ITERATIONS 4
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
	#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
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

#ifdef PHYSICS_CONTACT_STABILIZER_BAUMGARTE
return_t physIslandTick(physIsland *const __RESTRICT__ island, const float dt_s, const float frequency);
#else
return_t physIslandTick(physIsland *const __RESTRICT__ island, const float dt_s);
#endif

void physIslandDelete(physIsland *const __RESTRICT__ island);

#endif
