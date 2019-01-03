#include "colliderConvexMesh.h"
#include "inline.h"
#include <math.h>
#include <string.h>

return_t cCollisionMesh(const byte_t *const restrict c1h, const vec3 *const restrict c1c, const byte_t *const restrict c2h, const vec3 *const restrict c2c, cSeparationContainer *const restrict sc, cContactManifold *const restrict cm){
	return cMeshCollisionSAT((const cMesh *const restrict)c1h, (const cMesh *const restrict)c2h, c1c, (cMeshSeparation *)sc, cm);
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
	cSeparationContainer *const restrict,
	cContactManifold *const restrict
) = {
	{cCollisionMesh,        cCollisionMeshCapsule,   cCollisionMeshSphere,    cCollisionMeshAABB,    cCollisionMeshPoint},
	{cCollisionCapsuleMesh, cCollisionCapsule,       cCollisionCapsuleSphere, cCollisionCapsuleAABB, cCollisionCapsulePoint},
	{cCollisionSphereMesh,  cCollisionSphereCapsule, cCollisionSphere,        cCollisionSphereAABB,  cCollisionSpherePoint},
	{cCollisionAABBMesh,    cCollisionAABBCapsule,   cCollisionAABBSphere,    cCollisionAABB,        cCollisionAABBPoint},
	{cCollisionPointMesh,   cCollisionPointCapsule,  cCollisionPointSphere,   cCollisionPointAABB,   cCollisionPoint}
};
__FORCE_INLINE__ return_t cCollision(const collider *const restrict c1, const vec3 *const restrict c1c, const collider *const restrict c2, const vec3 *const restrict c2c, cSeparationContainer *const restrict sc, cContactManifold *const restrict cm){
	return cCollisionJumpTable[c1->type][c2->type](c1->data, c1c, c2->data, c2c, sc, cm);
}


return_t cSeparationMesh(const byte_t *const restrict c1h, const vec3 *const restrict c1c, const byte_t *const restrict c2h, const vec3 *const restrict c2c, const cSeparationContainer *const restrict sc){
	return cMeshSeparationSAT((const cMesh *const restrict)c1h, (const cMesh *const restrict)c2h, c1c, (cMeshSeparation *)sc);
}

/** The lines below should eventually be removed. **/
#define cSeparationMeshCapsule   NULL
#define cSeparationMeshSphere    NULL
#define cSeparationMeshAABB      NULL
#define cSeparationMeshPoint     NULL

#define cSeparationCapsuleMesh   NULL
#define cSeparationCapsule       NULL
#define cSeparationCapsuleSphere NULL
#define cSeparationCapsuleAABB   NULL
#define cSeparationCapsulePoint  NULL

#define cSeparationSphereMesh    NULL
#define cSeparationSphereCapsule NULL
#define cSeparationSphere        NULL
#define cSeparationSphereAABB    NULL
#define cSeparationSpherePoint   NULL

#define cSeparationAABBMesh      NULL
#define cSeparationAABBCapsule   NULL
#define cSeparationAABBSphere    NULL
#define cSeparationAABB          NULL
#define cSeparationAABBPoint     NULL

#define cSeparationPointMesh     NULL
#define cSeparationPointCapsule  NULL
#define cSeparationPointSphere   NULL
#define cSeparationPointAABB     NULL
#define cSeparationPoint         NULL

// Jump table for separation functions.
static return_t (* const cSeparationJumpTable[COLLIDER_TYPE_NUM][COLLIDER_TYPE_NUM])(
	const byte_t *const restrict,
	const vec3 *const restrict,
	const byte_t *const restrict,
	const vec3 *const restrict,
	const cSeparationContainer *const restrict
) = {
	{cSeparationMesh,        cSeparationMeshCapsule,   cSeparationMeshSphere,    cSeparationMeshAABB,    cSeparationMeshPoint},
	{cSeparationCapsuleMesh, cSeparationCapsule,       cSeparationCapsuleSphere, cSeparationCapsuleAABB, cSeparationCapsulePoint},
	{cSeparationSphereMesh,  cSeparationSphereCapsule, cSeparationSphere,        cSeparationSphereAABB,  cSeparationSpherePoint},
	{cSeparationAABBMesh,    cSeparationAABBCapsule,   cSeparationAABBSphere,    cSeparationAABB,        cSeparationAABBPoint},
	{cSeparationPointMesh,   cSeparationPointCapsule,  cSeparationPointSphere,   cSeparationPointAABB,   cSeparationPoint}
};
__FORCE_INLINE__ return_t cSeparation(const collider *const restrict c1, const vec3 *const restrict c1c, const collider *const restrict c2, const vec3 *const restrict c2c, const cSeparationContainer *const restrict sc){
	return cSeparationJumpTable[c1->type][c2->type](c1->data, c1c, c2->data, c2c, sc);
}

void cGenerateContactTangents(const vec3 *const restrict normal, vec3 *const restrict tangentA, vec3 *const restrict tangentB){
	// Generate the contact tangents, perpendicular to each other and the contact normal.
	// Used for frictional calculations.
	if(fabsf(normal->x) >= 0x3F13CD3A){  // Floating-point approximation of sqrtf(1.f / 3.f).
		vec3Set(tangentA, normal->y, -normal->x, 0.f);
	}else{
		vec3Set(tangentA, 0.f, normal->z, -normal->y);
	}
	vec3NormalizeFast(tangentA);
	vec3CrossR(normal, tangentA, tangentB);
}
