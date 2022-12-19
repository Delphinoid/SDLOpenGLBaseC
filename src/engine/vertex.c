#include "vertex.h"

void vInit(vertex *const __RESTRICT__ v){
	v->position = g_vec3Zero;
	v->u  = 0.f; v->v  = 0.f;
	v->normal = g_vec3Zero;
	v->bIDs[0]     = 0;   v->bIDs[1]     = -1;  v->bIDs[2]     = -1;  v->bIDs[3]     = -1;
	v->bWeights[0] = 1.f; v->bWeights[1] = 0.f; v->bWeights[2] = 0.f; v->bWeights[3] = 0.f;
}