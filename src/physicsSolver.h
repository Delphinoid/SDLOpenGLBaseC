#ifndef PHYSICSSOLVER_H
#define PHYSICSSOLVER_H

#include "physicsIsland.h"
#include "memoryPool.h"

#ifndef PHYSICS_SOLVER_DEFAULT_BODY_NUM
	#define PHYSICS_SOLVER_DEFAULT_BODY_NUM 1024
#endif

typedef struct {
	physRBInstance *body;
	flags_t active;
} physBodyReference;

typedef struct {

	// Array of pointers to object physics bodies.
	memoryPool bodies;  // Contains physRBInstance pointers.

	/** Constraints? **/

	// Islands that the objects have been sorted into.
	//size_t islandNum;
	//size_t islandCapacity;
	//physIsland *islands;

} physicsSolver;

return_t physSolverInit(physicsSolver *solver, size_t bodyNum);
void physSolverReset(physicsSolver *solver);
return_t physSolverAddBody(physicsSolver *solver, physRBInstance *body);
void physSolverUpdate(physicsSolver *solver);
void physSolverDelete(physicsSolver *solver);

#endif
