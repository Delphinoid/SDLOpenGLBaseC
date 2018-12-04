#include "collision.h"
#include "inline.h"
#include <math.h>

return_t cMeshCollisionSAT(const cMesh *c1, const cMesh *c2, const vec3 *centroid, cCollisionInfo *info, cCollisionContactManifold *cm);
//return_t cMeshCollisionGJK(const cMesh *c1, const vec3 *c1c, const cMesh *c2, const vec3 *c2c, cCollisionContactManifold *cm);
//return_t cMeshCollisionMPR(const cMesh *c1, const vec3 *c1c, const cMesh *c2, const vec3 *c2c, cCollisionContactManifold *cm);

return_t cCollisionMesh(const byte_t *c1h, const vec3 *c1c, const byte_t *c2h, const vec3 *c2c, cCollisionInfo *info, cCollisionContactManifold *cm){
	cCollisionContactManifoldInit(cm);  /** Temporary? **/
	return cMeshCollisionSAT((const cMesh *)c1h, (const cMesh *)c2h, c1c, info, cm);
}

/** The lines below should eventually be removed. **/
#define cCollisionMeshCapsule   NULL
#define cCollisionMeshSphere    NULL
#define cCollisionMeshAABB      NULL
#define cCollisionMeshPoint     NULL

#define cCollisionCapsuleMesh   NULL
#define cCollisionCapsule       NULL
#define cCollisionCapsuleSphere NULL
#define cCollisionCapsuleAABB   NULL
#define cCollisionCapsulePoint  NULL

#define cCollisionSphereMesh    NULL
#define cCollisionSphereCapsule NULL
#define cCollisionSphere        NULL
#define cCollisionSphereAABB    NULL
#define cCollisionSpherePoint   NULL

#define cCollisionAABBMesh      NULL
#define cCollisionAABBCapsule   NULL
#define cCollisionAABBSphere    NULL
#define cCollisionAABB          NULL
#define cCollisionAABBPoint     NULL

#define cCollisionPointMesh     NULL
#define cCollisionPointCapsule  NULL
#define cCollisionPointSphere   NULL
#define cCollisionPointAABB     NULL
#define cCollisionPoint         NULL

// Jump table for collision functions.
static return_t (* const cCollisionJumpTable[COLLIDER_TYPE_NUM][COLLIDER_TYPE_NUM])(
	const byte_t *,
	const vec3 *,
	const byte_t *,
	const vec3 *,
	cCollisionInfo *,
	cCollisionContactManifold *
) = {
	{cCollisionMesh,        cCollisionMeshCapsule,   cCollisionMeshSphere,    cCollisionMeshAABB,    cCollisionMeshPoint},
	{cCollisionCapsuleMesh, cCollisionCapsule,       cCollisionCapsuleSphere, cCollisionCapsuleAABB, cCollisionCapsulePoint},
	{cCollisionSphereMesh,  cCollisionSphereCapsule, cCollisionSphere,        cCollisionSphereAABB,  cCollisionSpherePoint},
	{cCollisionAABBMesh,    cCollisionAABBCapsule,   cCollisionAABBSphere,    cCollisionAABB,        cCollisionAABBPoint},
	{cCollisionPointMesh,   cCollisionPointCapsule,  cCollisionPointSphere,   cCollisionPointAABB,   cCollisionPoint}
};
__FORCE_INLINE__ return_t cCollision(const collider *c1, const vec3 *c1c, const collider *c2, const vec3 *c2c, cCollisionInfo *info, cCollisionContactManifold *cm){
	return cCollisionJumpTable[c1->type][c2->type](c1->hull, c1c, c2->hull, c2c, info, cm);
}

void cCollisionContactManifoldInit(cCollisionContactManifold *cm){
	cm->contactNum = 0;
}

void cCollisionGenerateContactTangents(const vec3 *normal, vec3 *tangentA, vec3 *tangentB){
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
