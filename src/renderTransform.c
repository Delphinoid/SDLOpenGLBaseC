#include "renderTransform.h"

void rtInit(renderTransform *rt){
	vec3SetS(&rt->target, 0.f);
	vec3SetS(&rt->scale, 1.f);
	rt->alpha = 1.f;
}
