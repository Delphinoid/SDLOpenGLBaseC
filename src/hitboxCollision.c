#include "hitboxCollision.h"
#include <math.h>

signed char hbMeshCollisionSAT(const hbMesh *c1, const hbMesh *c2, const vec3 *centroid, hbCollisionInfo *info, hbCollisionContactManifold *cm);
//signed char hbMeshCollisionGJK(const hbMesh *c1, const vec3 *c1c, const hbMesh *c2, const vec3 *c2c, hbCollisionContactManifold *cm);
//signed char hbMeshCollisionMPR(const hbMesh *c1, const vec3 *c1c, const hbMesh *c2, const vec3 *c2c, hbCollisionContactManifold *cm);

signed char hbCollisionMesh(const byte_t *c1h, const vec3 *c1c, const byte_t *c2h, const vec3 *c2c, hbCollisionInfo *info, hbCollisionContactManifold *cm){
	hbCollisionContactManifoldInit(cm);  /** Temporary? **/
	return hbMeshCollisionSAT((const hbMesh *)c1h, (const hbMesh *)c2h, c1c, info, cm);
}


typedef signed char (*hbCollisionPrototype)(const byte_t*, const vec3*, const byte_t*, const vec3*, hbCollisionInfo*, hbCollisionContactManifold*);

/** The lines below should eventually be removed. **/
#define hbCollisionMeshCapsule NULL
#define hbCollisionMeshSphere  NULL
#define hbCollisionMeshAABB    NULL

#define hbCollisionCapsuleMesh   NULL
#define hbCollisionCapsule       NULL
#define hbCollisionCapsuleSphere NULL
#define hbCollisionCapsuleAABB   NULL

#define hbCollisionSphereMesh    NULL
#define hbCollisionSphereCapsule NULL
#define hbCollisionSphere        NULL
#define hbCollisionSphereAABB    NULL

#define hbCollisionAABBMesh    NULL
#define hbCollisionAABBCapsule NULL
#define hbCollisionAABBSphere  NULL
#define hbCollisionAABB        NULL

static const hbCollisionPrototype hbCollisionJumpTable[4][4] = {
	{hbCollisionMesh,        hbCollisionMeshCapsule,   hbCollisionMeshSphere,    hbCollisionMeshAABB},
	{hbCollisionCapsuleMesh, hbCollisionCapsule,       hbCollisionCapsuleSphere, hbCollisionCapsuleAABB},
	{hbCollisionSphereMesh,  hbCollisionSphereCapsule, hbCollisionSphere,        hbCollisionSphereAABB},
	{hbCollisionAABBMesh,    hbCollisionAABBCapsule,   hbCollisionAABBSphere,    hbCollisionAABB}
};

signed char hbCollision(const hitbox *c1, const vec3 *c1c, const hitbox *c2, const vec3 *c2c, hbCollisionInfo *info, hbCollisionContactManifold *cm){
	return hbCollisionJumpTable[c1->type][c2->type](c1->hull, c1c, c2->hull, c2c, info, cm);
}

void hbCollisionContactManifoldInit(hbCollisionContactManifold *cm){
	cm->contactNum = 0;
}

void hbCollisionGenerateContactTangents(const vec3 *normal, vec3 *tangentA, vec3 *tangentB){
	// Generate the contact tangents, perpendicular to each other and the contact normal.
	// Used for frictional calculations.
	if(fabsf(normal->x) >= 0.57735f){
		vec3Set(tangentA, normal->y, -normal->x, 0.f);
	}else{
		vec3Set(tangentA, 0.f, normal->z, -normal->y);
	}
	vec3NormalizeFast(tangentA);
	vec3Cross(normal, tangentA, tangentB);
}
