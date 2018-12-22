#include "physicsConstraint.h"

void physConstraintInit(physConstraint *const restrict constraint){
	constraint->id = (physConstraintIndex_t)-1;
	constraint->ownerID = (physConstraintIndex_t)-1;
	vec3SetS(&constraint->constraintOffsetMin, 0.f);
	vec3SetS(&constraint->constraintOffsetMax, 0.f);
	vec3SetS(&constraint->constraintRotationMin, 0.f);
	vec3SetS(&constraint->constraintRotationMax, 0.f);
	constraint->flags = PHYSICS_CONSTRAINT_TYPE_1;
}
