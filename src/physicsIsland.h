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

void physIslandInit(physIsland *const __RESTRICT__ island);
return_t physIslandUpdateCollider(physIsland *const __RESTRICT__ island, physCollider *const __RESTRICT__ c);
void physIslandRemoveCollider(physIsland *const __RESTRICT__ island, physCollider *const __RESTRICT__ c);
#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
return_t physIslandQuery(const physIsland *const __RESTRICT__ island, const float frequency);
#else
return_t physIslandQuery(const physIsland *const __RESTRICT__ island);
#endif
void physIslandDelete(physIsland *const __RESTRICT__ island);

#endif
