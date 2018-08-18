#ifndef PHYSICSSOLVER_H
#define PHYSICSSOLVER_H

#include "physicsIsland.h"

typedef struct {
	physRBInstance *body;
	signed char active;
} physBodyReference;

typedef struct {

	// Array of pointers to object physics bodies.
	physicsBodyIndex_t bodyNum;
	physicsBodyIndex_t bodyCapacity;
	physBodyReference *bodies;

	/** Constraints? **/

	// Islands that the objects have been sorted into.
	//size_t islandNum;
	//size_t islandCapacity;
	//physIsland *islands;

} physicsSolver;

void physSolverInit(physicsSolver *solver);
void physSolverReset(physicsSolver *solver);
signed char physSolverAddBody(physicsSolver *solver, physRBInstance *body);
void physSolverUpdate(physicsSolver *solver);
void physSolverDelete(physicsSolver *solver);

#endif
