#include "colliderConvexMesh.h"

void cInit(collider *const restrict c, const colliderType_t type){
	c->type = type;
}
void cDelete(collider *const restrict c){
	if(c->type == COLLIDER_TYPE_MESH){
		cMeshDelete((cMesh *)&c->data);
	}
}

void cArrayDelete(colliderArray *const restrict ca){
	if(ca->colliders != NULL){
		free(ca->colliders);
	}
}
