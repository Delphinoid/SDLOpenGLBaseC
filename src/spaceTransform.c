#include "spaceTransform.h"

void stInit(spaceTransform *st){
	vec3SetS(&st->position, 0.f);
	quatSetIdentity(&st->orientation);
	vec3SetS(&st->changeRot, 0.f);
	vec3SetS(&st->relPivot, 0.f);
}
