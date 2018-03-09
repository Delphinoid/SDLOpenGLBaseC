#include "vertex.h"

void vertInit(vertex *v){
	vec3SetS(&v->position, 0.f);
	v->u  = 0.f; v->v  = 0.f;
	vec3SetS(&v->normal, 0.f);
	v->bIDs[0]     = 0;   v->bIDs[1]     = -1;  v->bIDs[2]     = -1;  v->bIDs[3]     = -1;
	v->bWeights[0] = 1.f; v->bWeights[1] = 0.f; v->bWeights[2] = 0.f; v->bWeights[3] = 0.f;
}

void vertTransform(vertex *v, const vec3 *position, const quat *orientation, const vec3 *scale){
	vec3MultVByV(&v->position, scale);
	quatRotateVec3(orientation, &v->position);
	vec3AddVToV(&v->position, position);
}
