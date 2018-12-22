#ifndef PHYSICSSOLVER_H
#define PHYSICSSOLVER_H

#include "physicsIsland.h"
#include "memoryShared.h"
#include "flags.h"

#ifndef PHYSICS_SOLVER_DEFAULT_BODY_NUM
	#define PHYSICS_SOLVER_DEFAULT_BODY_NUM 1024
#endif

/**
*** NOTE: Eventually there will be no physics solver
*** class, all constraints will be solved in
*** modulePhysicsSolve() or some similar function.
**/

/**typedef struct {
	physRBInstance *body;
	flags_t active;
} physBodyReference;**/

typedef struct {

	// Array of pointers to object physics bodies.
	memoryRegion *bodies;  // Contains physRBInstance pointers.
	size_t bodyNum;

	/** Constraints? **/

	// Islands that the objects have been sorted into.
	//size_t islandNum;
	//size_t islandCapacity;
	//physIsland *islands;

} physicsSolver;

return_t physSolverInit(physicsSolver *const restrict solver, size_t bodyNum);
void physSolverReset(physicsSolver *const restrict solver);
return_t physSolverAllocate(physicsSolver *const restrict solver, physRBInstance *const body);
return_t physSolverUpdate(physicsSolver *const restrict solver);
void physSolverDelete(physicsSolver *const restrict solver);

#endif
