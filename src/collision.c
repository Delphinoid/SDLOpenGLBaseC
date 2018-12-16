#include "colliderConvexMesh.h"
#include "inline.h"
#include <math.h>

return_t cCollisionMesh(const byte_t *const restrict c1h, const vec3 *const restrict c1c, const byte_t *const restrict c2h, const vec3 *const restrict c2c, cCollisionInfo *const restrict info, cCollisionContactManifold *const restrict cm){
	cCollisionContactManifoldInit(cm);  /** Temporary? **/
	return cMeshCollisionSAT((const cMesh *const restrict)c1h, (const cMesh *const restrict)c2h, c1c, info, cm);
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
	const byte_t *const restrict,
	const vec3 *const restrict,
	const byte_t *const restrict,
	const vec3 *const restrict,
	cCollisionInfo *const restrict,
	cCollisionContactManifold *const restrict
) = {
	{cCollisionMesh,        cCollisionMeshCapsule,   cCollisionMeshSphere,    cCollisionMeshAABB,    cCollisionMeshPoint},
	{cCollisionCapsuleMesh, cCollisionCapsule,       cCollisionCapsuleSphere, cCollisionCapsuleAABB, cCollisionCapsulePoint},
	{cCollisionSphereMesh,  cCollisionSphereCapsule, cCollisionSphere,        cCollisionSphereAABB,  cCollisionSpherePoint},
	{cCollisionAABBMesh,    cCollisionAABBCapsule,   cCollisionAABBSphere,    cCollisionAABB,        cCollisionAABBPoint},
	{cCollisionPointMesh,   cCollisionPointCapsule,  cCollisionPointSphere,   cCollisionPointAABB,   cCollisionPoint}
};
__FORCE_INLINE__ return_t cCollision(const collider *const restrict c1, const vec3 *const restrict c1c, const collider *const restrict c2, const vec3 *const restrict c2c, cCollisionInfo *const restrict info, cCollisionContactManifold *const restrict cm){
	return cCollisionJumpTable[c1->type][c2->type](c1->hull, c1c, c2->hull, c2c, info, cm);
}

void cCollisionContactManifoldInit(cCollisionContactManifold *const restrict cm){
	cm->contactNum = 0;
}

void cCollisionGenerateContactTangents(const vec3 *const restrict normal, vec3 *const restrict tangentA, vec3 *const restrict tangentB){
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
