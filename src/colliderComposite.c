#include "collision.h"
#include "memoryManager.h"

void cCompositeInit(cComposite *const __RESTRICT__ c){
	c->colliders = NULL;
	c->colliderNum = 0;
}

return_t cCompositeInstantiate(cComposite *const __RESTRICT__ instance, const cComposite *const __RESTRICT__ local){

	const size_t bufferSize = sizeof(collider)*local->colliderNum;
	collider *tempBuffer = memAllocate(bufferSize);
	if(tempBuffer != NULL){

		collider *c1 = tempBuffer;
		const collider *c2 = local->colliders;
		const collider *const cLast = (const collider *)((const byte_t *)&tempBuffer + bufferSize);

		// Loop through each collider, instantiating them.
		for(; c1 < cLast; ++c1, ++c2){
			if(cInstantiate(c1, c2) < 0){
				/** Memory allocation failure. **/
				while(c1 > tempBuffer){
					--c1;
					cDelete(c1);
				}
				return -1;
			}
		}

		instance->colliders = tempBuffer;
		instance->colliderNum = local->colliderNum;
		return 1;

	}

	/** Memory allocation failure. **/
	return -1;

}

void cCompositeDeleteBase(cComposite *const __RESTRICT__ c){
	if(c->colliders != NULL){
		// Delete every collider that composes
		// the composite collider.
		collider *cCurrent = c->colliders;
		const collider *const cLast = &cCurrent[c->colliderNum];
		for(; cCurrent < cLast; ++cCurrent){
			cDelete(cCurrent);
		}
		memFree(c->colliders);
	}
}

void cCompositeDelete(cComposite *const __RESTRICT__ c){
	if(c->colliders != NULL){
		// Delete every collider that composes
		// the composite collider.
		collider *cCurrent = c->colliders;
		const collider *const cLast = &cCurrent[c->colliderNum];
		for(; cCurrent < cLast; ++cCurrent){
			cDelete(cCurrent);
		}
		memFree(c->colliders);
	}
}