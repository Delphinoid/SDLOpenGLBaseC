#include "collider.h"
#include "memoryManager.h"
#include <string.h>

void cInit(collider *const __RESTRICT__ c, const colliderType_t type){
	c->type = type;
	c->flags = 0;
}


/** The lines below should eventually be removed. **/
#define cCapsuleInstantiate NULL
#define cSphereInstantiate  NULL
#define cAABBInstantiate    NULL
#define cPointInstantiate   NULL

return_t (* const cInstantiateJumpTable[COLLIDER_TYPE_NUM])(
	void *const instance,
	const void *const local
) = {
	cHullInstantiate,
	cCapsuleInstantiate,
	cSphereInstantiate,
	cAABBInstantiate,
	cPointInstantiate,
	cCompositeInstantiate
};
__FORCE_INLINE__ return_t cInstantiate(collider *const instance, const collider *const local){
	// Instantiates a collider by performing a copy.
	instance->type = local->type;
	instance->flags = local->flags | COLLIDER_INSTANCE;
	return cInstantiateJumpTable[local->type](&instance->data, &local->data);
}


/** The lines below should eventually be removed. **/
#define cCapsuleTransform NULL
#define cSphereTransform  NULL
#define cAABBTransform    NULL
#define cPointTransform   NULL

cAABB (* const cTransformJumpTable[COLLIDER_TYPE_NUM])(
	void *const instance,
	const vec3 instanceCentroid,
	const void *const local,
	const vec3 localCentroid,
	const transform configuration
) = {
	cHullTransform,
	cCapsuleTransform,
	cSphereTransform,
	cAABBTransform,
	cPointTransform,
	cCompositeTransform
};
__FORCE_INLINE__ cAABB cTransform(collider *const instance, const vec3 instanceCentroid, const collider *const local, const vec3 localCentroid, const transform configuration){

	// Updates the collider by transforming it.
	return cTransformJumpTable[instance->type](&instance->data, instanceCentroid, &local->data, localCentroid, configuration);

}


void cDelete(collider *const __RESTRICT__ c){
	// Handle deletions for base and instance colliders.
	if(flagsAreSet(c->flags, COLLIDER_INSTANCE)){
		if(c->type == COLLIDER_TYPE_HULL){
			cHullDelete((cHull *)&c->data);
		}else if(c->type == COLLIDER_TYPE_COMPOSITE){
			cCompositeDelete((cComposite *)&c->data);
		}
	}else{
		if(c->type == COLLIDER_TYPE_HULL){
			cHullDeleteBase((cHull *)&c->data);
		}else if(c->type == COLLIDER_TYPE_COMPOSITE){
			cCompositeDeleteBase((cComposite *)&c->data);
		}
	}
}