#include "physicsConstraint.h"

void physConstraintInit(physConstraint *constraint){
	constraint->flags = PHYS_CONSTRAINT_TYPE_1;
	constraint->constraintID = (size_t)-1;
	vec3SetS(&constraint->constraintOffsetMin, 0.f);
	vec3SetS(&constraint->constraintOffsetMax, 0.f);
	vec3SetS(&constraint->constraintRotationMin, 0.f);
	vec3SetS(&constraint->constraintRotationMax, 0.f);
}
