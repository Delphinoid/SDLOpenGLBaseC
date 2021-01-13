#include "collision.h"
#include "qualifiers.h"
#include <math.h>

return_t cCollisionHull(const void *const __RESTRICT__ c1h, const void *const __RESTRICT__ c2h, void *const __RESTRICT__ sc, cContact *const __RESTRICT__ cm){
	return cHullCollisionSAT((const cHull *const __RESTRICT__)c1h, (const cHull *const __RESTRICT__)c2h, (cHullSeparation *const __RESTRICT__)sc, cm);
}

/** The lines below should eventually be removed. **/
#define cCollisionHullCapsule      NULL
#define cCollisionHullSphere       NULL
#define cCollisionHullAABB         NULL
#define cCollisionHullPoint        NULL
#define cCollisionHullComposite    NULL

#define cCollisionCapsuleHull      NULL
#define cCollisionCapsule          NULL
#define cCollisionCapsuleSphere    NULL
#define cCollisionCapsuleAABB      NULL
#define cCollisionCapsulePoint     NULL
#define cCollisionCapsuleComposite NULL

#define cCollisionSphereHull       NULL
#define cCollisionSphereCapsule    NULL
#define cCollisionSphere           NULL
#define cCollisionSphereAABB       NULL
#define cCollisionSpherePoint      NULL
#define cCollisionSphereComposite  NULL

#define cCollisionAABBHull         NULL
#define cCollisionAABBCapsule      NULL
#define cCollisionAABBSphere       NULL
#define cCollisionAABB             NULL
#define cCollisionAABBPoint        NULL
#define cCollisionAABBComposite    NULL

#define cCollisionPointHull        NULL
#define cCollisionPointCapsule     NULL
#define cCollisionPointSphere      NULL
#define cCollisionPointAABB        NULL
#define cCollisionPoint            NULL
#define cCollisionPointComposite   NULL

#define cCollisionCompositeHull    NULL
#define cCollisionCompositeCapsule NULL
#define cCollisionCompositeSphere  NULL
#define cCollisionCompositeAABB    NULL
#define cCollisionCompositePoint   NULL
#define cCollisionComposite        NULL

// Jump table for collision functions.
return_t (* const cCollisionJumpTable[COLLIDER_TYPE_NUM][COLLIDER_TYPE_NUM])(
	const void *const __RESTRICT__,
	const void *const __RESTRICT__,
	void *const __RESTRICT__,
	cContact *const __RESTRICT__
) = {
	{cCollisionHull,          cCollisionHullCapsule,      cCollisionHullSphere,      cCollisionHullAABB,      cCollisionHullPoint,      cCollisionHullComposite},
	{cCollisionCapsuleHull,   cCollisionCapsule,          cCollisionCapsuleSphere,   cCollisionCapsuleAABB,   cCollisionCapsulePoint,   cCollisionCapsuleComposite},
	{cCollisionSphereHull,    cCollisionSphereCapsule,    cCollisionSphere,          cCollisionSphereAABB,    cCollisionSpherePoint,    cCollisionSphereComposite},
	{cCollisionAABBHull,      cCollisionAABBCapsule,      cCollisionAABBSphere,      cCollisionAABB,          cCollisionAABBPoint,      cCollisionAABBComposite},
	{cCollisionPointHull,     cCollisionPointCapsule,     cCollisionPointSphere,     cCollisionPointAABB,     cCollisionPoint,          cCollisionPointComposite},
	{cCollisionCompositeHull, cCollisionCompositeCapsule, cCollisionCompositeSphere, cCollisionCompositeAABB, cCollisionCompositePoint, cCollisionComposite}
};
__FORCE_INLINE__ return_t cCheckCollision(const collider *const __RESTRICT__ c1, const collider *const __RESTRICT__ c2, cSeparation *const __RESTRICT__ sc, cContact *const __RESTRICT__ cm){
	return cCollisionJumpTable[c1->type][c2->type](&c1->data, &c2->data, sc, cm);
}


return_t cSeparationHull(const void *const __RESTRICT__ c1h, const void *const __RESTRICT__ c2h, const void *const __RESTRICT__ sc){
	return cHullSeparationSAT((const cHull *const __RESTRICT__)c1h, (const cHull *const __RESTRICT__)c2h, (const cHullSeparation *const __RESTRICT__)sc);
}

/** The lines below should eventually be removed. **/
#define cSeparationHullCapsule   NULL
#define cSeparationHullSphere    NULL
#define cSeparationHullAABB      NULL
#define cSeparationHullPoint     NULL
#define cSeparationHullComposite NULL

#define cSeparationCapsuleHull      NULL
#define cSeparationCapsule          NULL
#define cSeparationCapsuleSphere    NULL
#define cSeparationCapsuleAABB      NULL
#define cSeparationCapsulePoint     NULL
#define cSeparationCapsuleComposite NULL

#define cSeparationSphereHull       NULL
#define cSeparationSphereCapsule    NULL
#define cSeparationSphere           NULL
#define cSeparationSphereAABB       NULL
#define cSeparationSpherePoint      NULL
#define cSeparationSphereComposite  NULL

#define cSeparationAABBHull         NULL
#define cSeparationAABBCapsule      NULL
#define cSeparationAABBSphere       NULL
#define cSeparationAABB             NULL
#define cSeparationAABBPoint        NULL
#define cSeparationAABBComposite    NULL

#define cSeparationPointHull        NULL
#define cSeparationPointCapsule     NULL
#define cSeparationPointSphere      NULL
#define cSeparationPointAABB        NULL
#define cSeparationPoint            NULL
#define cSeparationPointComposite   NULL

#define cSeparationCompositeHull    NULL
#define cSeparationCompositeCapsule NULL
#define cSeparationCompositeSphere  NULL
#define cSeparationCompositeAABB    NULL
#define cSeparationCompositePoint   NULL
#define cSeparationComposite        NULL

// Jump table for separation functions.
return_t (* const cSeparationJumpTable[COLLIDER_TYPE_NUM][COLLIDER_TYPE_NUM])(
	const void *const __RESTRICT__,
	const void *const __RESTRICT__,
	const void *const __RESTRICT__
) = {
	{cSeparationHull,          cSeparationHullCapsule,      cSeparationHullSphere,      cSeparationHullAABB,      cSeparationHullPoint,      cSeparationHullComposite},
	{cSeparationCapsuleHull,   cSeparationCapsule,          cSeparationCapsuleSphere,   cSeparationCapsuleAABB,   cSeparationCapsulePoint,   cSeparationCapsuleComposite},
	{cSeparationSphereHull,    cSeparationSphereCapsule,    cSeparationSphere,          cSeparationSphereAABB,    cSeparationSpherePoint,    cSeparationSphereComposite},
	{cSeparationAABBHull,      cSeparationAABBCapsule,      cSeparationAABBSphere,      cSeparationAABB,          cSeparationAABBPoint,      cSeparationAABBComposite},
	{cSeparationPointHull,     cSeparationPointCapsule,     cSeparationPointSphere,     cSeparationPointAABB,     cSeparationPoint,          cSeparationPointComposite},
	{cSeparationCompositeHull, cSeparationCompositeCapsule, cSeparationCompositeSphere, cSeparationCompositeAABB, cSeparationCompositePoint, cSeparationComposite}
};
__FORCE_INLINE__ return_t cCheckSeparation(const collider *const __RESTRICT__ c1, const collider *const __RESTRICT__ c2, const cSeparation *const __RESTRICT__ sc){
	return cSeparationJumpTable[c1->type][c2->type](&c1->data, &c2->data, sc);
}