#include "collider.h"

void cInit(collider *c, const colliderType_t type){
	c->type = type;
}
void cDelete(collider *c){
	if(c->type == COLLIDER_TYPE_MESH){
		cMeshDelete((cMesh *)&c->hull);
	}
}

void cArrayDelete(colliderArray *ca){
	if(ca->colliders != NULL){
		free(ca->colliders);
	}
}
