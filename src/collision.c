#include "collision.h"
#include "inline.h"

return_t cCollisionMesh(const void *const restrict c1h, const void *const restrict c2h, void *const restrict sc, cContact *const restrict cm){
	return cMeshCollisionSAT((const cMesh *const restrict)c1h, (const cMesh *const restrict)c2h, (cMeshSeparation *const restrict)sc, cm);
}

/** The lines below should eventually be removed. **/
#define cCollisionMeshCapsule      NULL
#define cCollisionMeshSphere       NULL
#define cCollisionMeshAABB         NULL
#define cCollisionMeshPoint        NULL
#define cCollisionMeshComposite    NULL

#define cCollisionCapsuleMesh      NULL
#define cCollisionCapsule          NULL
#define cCollisionCapsuleSphere    NULL
#define cCollisionCapsuleAABB      NULL
#define cCollisionCapsulePoint     NULL
#define cCollisionCapsuleComposite NULL

#define cCollisionSphereMesh       NULL
#define cCollisionSphereCapsule    NULL
#define cCollisionSphere           NULL
#define cCollisionSphereAABB       NULL
#define cCollisionSpherePoint      NULL
#define cCollisionSphereComposite  NULL

#define cCollisionAABBMesh         NULL
#define cCollisionAABBCapsule      NULL
#define cCollisionAABBSphere       NULL
#define cCollisionAABB             NULL
#define cCollisionAABBPoint        NULL
#define cCollisionAABBComposite    NULL

#define cCollisionPointMesh        NULL
#define cCollisionPointCapsule     NULL
#define cCollisionPointSphere      NULL
#define cCollisionPointAABB        NULL
#define cCollisionPoint            NULL
#define cCollisionPointComposite   NULL

#define cCollisionCompositeMesh    NULL
#define cCollisionCompositeCapsule NULL
#define cCollisionCompositeSphere  NULL
#define cCollisionCompositeAABB    NULL
#define cCollisionCompositePoint   NULL
#define cCollisionComposite        NULL


return_t cSeparationMesh(const void *const restrict c1h, const void *const restrict c2h, const void *const restrict sc){
	return cMeshSeparationSAT((const cMesh *const restrict)c1h, (const cMesh *const restrict)c2h, (const cMeshSeparation *const restrict)sc);
}

/** The lines below should eventually be removed. **/
#define cSeparationMeshCapsule   NULL
#define cSeparationMeshSphere    NULL
#define cSeparationMeshAABB      NULL
#define cSeparationMeshPoint     NULL
#define cSeparationMeshComposite NULL

#define cSeparationCapsuleMesh      NULL
#define cSeparationCapsule          NULL
#define cSeparationCapsuleSphere    NULL
#define cSeparationCapsuleAABB      NULL
#define cSeparationCapsulePoint     NULL
#define cSeparationCapsuleComposite NULL

#define cSeparationSphereMesh       NULL
#define cSeparationSphereCapsule    NULL
#define cSeparationSphere           NULL
#define cSeparationSphereAABB       NULL
#define cSeparationSpherePoint      NULL
#define cSeparationSphereComposite  NULL

#define cSeparationAABBMesh         NULL
#define cSeparationAABBCapsule      NULL
#define cSeparationAABBSphere       NULL
#define cSeparationAABB             NULL
#define cSeparationAABBPoint        NULL
#define cSeparationAABBComposite    NULL

#define cSeparationPointMesh        NULL
#define cSeparationPointCapsule     NULL
#define cSeparationPointSphere      NULL
#define cSeparationPointAABB        NULL
#define cSeparationPoint            NULL
#define cSeparationPointComposite   NULL

#define cSeparationCompositeMesh    NULL
#define cSeparationCompositeCapsule NULL
#define cSeparationCompositeSphere  NULL
#define cSeparationCompositeAABB    NULL
#define cSeparationCompositePoint   NULL
#define cSeparationComposite        NULL


// Jump table for collision functions.
return_t (* const cCollisionJumpTable[COLLIDER_TYPE_NUM][COLLIDER_TYPE_NUM])(
	const void *const restrict,
	const void *const restrict,
	void *const restrict,
	cContact *const restrict
) = {
	{cCollisionMesh,          cCollisionMeshCapsule,      cCollisionMeshSphere,      cCollisionMeshAABB,      cCollisionMeshPoint,      cCollisionMeshComposite},
	{cCollisionCapsuleMesh,   cCollisionCapsule,          cCollisionCapsuleSphere,   cCollisionCapsuleAABB,   cCollisionCapsulePoint,   cCollisionCapsuleComposite},
	{cCollisionSphereMesh,    cCollisionSphereCapsule,    cCollisionSphere,          cCollisionSphereAABB,    cCollisionSpherePoint,    cCollisionSphereComposite},
	{cCollisionAABBMesh,      cCollisionAABBCapsule,      cCollisionAABBSphere,      cCollisionAABB,          cCollisionAABBPoint,      cCollisionAABBComposite},
	{cCollisionPointMesh,     cCollisionPointCapsule,     cCollisionPointSphere,     cCollisionPointAABB,     cCollisionPoint,          cCollisionPointComposite},
	{cCollisionCompositeMesh, cCollisionCompositeCapsule, cCollisionCompositeSphere, cCollisionCompositeAABB, cCollisionCompositePoint, cCollisionComposite}
};
__FORCE_INLINE__ return_t cCheckCollision(const collider *const restrict c1, const collider *const restrict c2, cSeparation *const restrict sc, cContact *const restrict cm){
	return cCollisionJumpTable[c1->type][c2->type](&c1->data, &c2->data, sc, cm);
}

// Jump table for separation functions.
return_t (* const cSeparationJumpTable[COLLIDER_TYPE_NUM][COLLIDER_TYPE_NUM])(
	const void *const restrict,
	const void *const restrict,
	const void *const restrict
) = {
	{cSeparationMesh,          cSeparationMeshCapsule,      cSeparationMeshSphere,      cSeparationMeshAABB,      cSeparationMeshPoint,      cSeparationMeshComposite},
	{cSeparationCapsuleMesh,   cSeparationCapsule,          cSeparationCapsuleSphere,   cSeparationCapsuleAABB,   cSeparationCapsulePoint,   cSeparationCapsuleComposite},
	{cSeparationSphereMesh,    cSeparationSphereCapsule,    cSeparationSphere,          cSeparationSphereAABB,    cSeparationSpherePoint,    cSeparationSphereComposite},
	{cSeparationAABBMesh,      cSeparationAABBCapsule,      cSeparationAABBSphere,      cSeparationAABB,          cSeparationAABBPoint,      cSeparationAABBComposite},
	{cSeparationPointMesh,     cSeparationPointCapsule,     cSeparationPointSphere,     cSeparationPointAABB,     cSeparationPoint,          cSeparationPointComposite},
	{cSeparationCompositeMesh, cSeparationCompositeCapsule, cSeparationCompositeSphere, cSeparationCompositeAABB, cSeparationCompositePoint, cSeparationComposite}
};
__FORCE_INLINE__ return_t cCheckSeparation(const collider *const restrict c1, const collider *const restrict c2, const cSeparation *const restrict sc){
	return cSeparationJumpTable[c1->type][c2->type](&c1->data, &c2->data, sc);
}
