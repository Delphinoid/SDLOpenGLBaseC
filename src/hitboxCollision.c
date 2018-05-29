#include "hitboxCollision.h"

signed char hbMeshCollisionGJK(const hbMesh *c1, const vec3 *c1c, const hbMesh *c2, const vec3 *c2c, hbMeshSupportVertex *simplex);
signed char hbMeshCollisionMPR(const hbMesh *c1, const vec3 *c1c, const hbMesh *c2, const vec3 *c2c, hbMeshSupportVertex *simplex);
void hbMeshCollisionEPA(const hbMesh *c1, const hbMesh *c2, hbMeshSupportVertex *simplex, hbCollisionData *cd);

signed char hbCollisionMesh(const char *c1h, const vec3 *c1c, const char *c2h, const vec3 *c2c, hbCollisionData *cd){
	const hbMesh *c1 = (const hbMesh *)c1h;
	const hbMesh *c2 = (const hbMesh *)c2h;
	hbMeshSupportVertex simplex[4];
	if(hbMeshCollisionGJK(c1, c1c, c2, c2c, simplex)){
		if(cd != NULL){
			hbMeshCollisionEPA(c1, c2, simplex, cd);
		}
		return 1;
	}
	return 0;
}


typedef signed char (*hbCollisionPrototype)(const char*, const vec3*, const char*, const vec3*, hbCollisionData*);
/** The lines below should eventually be removed. **/
hbCollisionPrototype                         hbCollisionMeshCapsule,   hbCollisionMeshSphere,    hbCollisionMeshAABB;
hbCollisionPrototype hbCollisionCapsuleMesh, hbCollisionCapsule,       hbCollisionCapsuleSphere, hbCollisionCapsuleAABB;
hbCollisionPrototype hbCollisionSphereMesh,  hbCollisionSphereCapsule, hbCollisionSphere,        hbCollisionSphereAABB;
hbCollisionPrototype hbCollisionAABBMesh,    hbCollisionAABBCapsule,   hbCollisionAABBSphere,    hbCollisionAABB;
signed char hbCollision(const hitbox *c1, const vec3 *c1c, const hitbox *c2, const vec3 *c2c, hbCollisionData *cd){

	hbCollisionPrototype hbCollisionJumpTable[4][4] = {
		{hbCollisionMesh,        hbCollisionMeshCapsule,   hbCollisionMeshSphere,    hbCollisionMeshAABB},
		{hbCollisionCapsuleMesh, hbCollisionCapsule,       hbCollisionCapsuleSphere, hbCollisionCapsuleAABB},
		{hbCollisionSphereMesh,  hbCollisionSphereCapsule, hbCollisionSphere,        hbCollisionSphereAABB},
		{hbCollisionAABBMesh,    hbCollisionAABBCapsule,   hbCollisionAABBSphere,    hbCollisionAABB}
	};

	return hbCollisionJumpTable[c1->type][c2->type](c1->hull, c1c, c2->hull, c2c, cd);

}
