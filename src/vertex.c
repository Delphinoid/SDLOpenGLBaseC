#include "vertex.h"

void vertInit(vertex *const restrict v){
	vec3Zero(&v->position);
	v->u  = 0.f; v->v  = 0.f;
	vec3Zero(&v->normal);
	v->bIDs[0]     = 0;   v->bIDs[1]     = -1;  v->bIDs[2]     = -1;  v->bIDs[3]     = -1;
	v->bWeights[0] = 1.f; v->bWeights[1] = 0.f; v->bWeights[2] = 0.f; v->bWeights[3] = 0.f;
}

void vertTransform(vertex *const restrict v, const vec3 *const restrict position, const quat *const restrict orientation, const vec3 *const restrict scale){
	vec3MultVByV(&v->position, scale);
	quatRotateVec3Fast(orientation, &v->position);
	vec3AddVToV(&v->position, position);
}
