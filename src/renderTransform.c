#include "renderTransform.h"

void rtInit(renderTransform *rt){
	vec3SetS(&rt->targetPosition, 0.f);
	quatSetIdentity(&rt->targetOrientation);
	vec3SetS(&rt->scale, 1.f);
	rt->alpha = 1.f;
}
