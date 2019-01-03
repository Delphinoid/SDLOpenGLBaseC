#include "physicsConstraint.h"

void physConstraintInit(physConstraint *const restrict constraint){
	//constraint->bodyA = NULL;
	//constraint->bodyB = NULL;
	constraint->flags = PHYSICS_CONSTRAINT_TYPE_UNKNOWN;
}
