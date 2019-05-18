#ifndef PHYSICSISLAND_H
#define PHYSICSISLAND_H

#include "physicsShared.h"
#include "aabbTree.h"

#ifndef PHYSICS_ISLAND_COLLIDER_AABB_ADDEND
	#define PHYSICS_ISLAND_COLLIDER_AABB_ADDEND 0.2f
#endif
#ifndef PHYSICS_ISLAND_COLLIDER_AABB_VELOCITY_FACTOR
	#define PHYSICS_ISLAND_COLLIDER_AABB_VELOCITY_FACTOR 1.f
#endif

#ifndef PHYSICS_ISLAND_QUERY_STACK_SIZE
	#define PHYSICS_ISLAND_QUERY_STACK_SIZE AABB_TREE_QUERY_STACK_SIZE
#endif

typedef struct physCollider physCollider;

typedef struct physIsland {
	aabbTree tree;
} physIsland;

void physIslandInit(physIsland *const restrict island);
return_t physIslandUpdateCollider(physIsland *const restrict island, physCollider *const restrict c);
void physIslandRemoveCollider(physIsland *const restrict island, physCollider *const restrict c);
#if !defined PHYSICS_GAUSS_SEIDEL_SOLVER || defined PHYSICS_FORCE_VELOCITY_BAUMGARTE
return_t physIslandQuery(const physIsland *const restrict island, const float dt);
#else
return_t physIslandQuery(const physIsland *const restrict island);
#endif
void physIslandDelete(physIsland *const restrict island);

#endif
