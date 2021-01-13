#include "vertex.h"

void vertInit(vertex *const __RESTRICT__ v){
	vec3ZeroP(&v->position);
	v->u  = 0.f; v->v  = 0.f;
	vec3ZeroP(&v->normal);
	v->bIDs[0]     = 0;   v->bIDs[1]     = -1;  v->bIDs[2]     = -1;  v->bIDs[3]     = -1;
	v->bWeights[0] = 1.f; v->bWeights[1] = 0.f; v->bWeights[2] = 0.f; v->bWeights[3] = 0.f;
}

void vertTransform(vertex *const __RESTRICT__ v, const vec3 position, const quat orientation, const vec3 scale){
	v->position = vec3VAddV(quatRotateVec3FastApproximate(orientation, vec3VMultV(v->position, scale)), position);
}
