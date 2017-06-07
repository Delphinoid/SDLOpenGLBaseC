#include "spaceTransform.h"

void stInit(spaceTransform *st){
	vec3SetS(&st->position, 0.f);
	quatSet(&st->orientation, 1.f, 0.f, 0.f, 0.f);
	vec3SetS(&st->changeRot, 0.f);
	vec3SetS(&st->relPivot, 0.f);
}
