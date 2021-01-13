#include "collision.h"
#include "memoryManager.h"

void cCompositeInit(cComposite *const __RESTRICT__ c){
	c->colliders = NULL;
	c->colliderNum = 0;
}

return_t cCompositeInstantiate(void *const __RESTRICT__ instance, const void *const __RESTRICT__ local){

	const size_t bufferSize = sizeof(collider)*((cComposite *)local)->colliderNum;
	collider *tempBuffer = memAllocate(bufferSize);
	if(tempBuffer != NULL){

		collider *c1 = tempBuffer;
		const collider *c2 = ((cComposite *)local)->colliders;
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

		((cComposite *)instance)->colliders = tempBuffer;
		((cComposite *)instance)->colliderNum = ((cComposite *)local)->colliderNum;
		return 1;

	}

	/** Memory allocation failure. **/
	return -1;

}

cAABB cCompositeTransform(void *const instance, const vec3 instanceCentroid, const void *const local, const vec3 localCentroid, const vec3 position, const quat orientation, const vec3 scale){

	cComposite *const cInstance = instance;
	const cComposite *const cLocal = local;

	collider *c1 = cInstance->colliders;
	collider *c2 = cLocal->colliders;
	const collider *const cLast = &c1[cInstance->colliderNum];

	cAABB tempAABB = {.min.x = 0.f, .min.y = 0.f, .min.z = 0.f, .max.x = 0.f, .max.y = 0.f, .max.z = 0.f};

	// Update each collider and find the total bounding box.
	if(c1 < cLast){

		// First iteration.
		tempAABB = cTransform(c1, instanceCentroid, c2, localCentroid, position, orientation, scale);

		// Remaining iterations.
		for(++c1, ++c2; c1 < cLast; ++c1, ++c2){

			const cAABB colliderAABB = cTransform(c1, instanceCentroid, c2, localCentroid, position, orientation, scale);

			// Update collider minima and maxima.
			// Update aabb.left and aabb.right.
			if(colliderAABB.min.x <= tempAABB.min.x){
				tempAABB.min.x = colliderAABB.min.x;
			}else if(colliderAABB.max.x > tempAABB.max.x){
				tempAABB.max.x = colliderAABB.max.x;
			}
			// Update aabb.bottom and aabb.top.
			if(colliderAABB.min.y <= tempAABB.min.y){
				tempAABB.min.y = colliderAABB.min.y;
			}else if(colliderAABB.max.y > tempAABB.max.y){
				tempAABB.max.y = colliderAABB.max.y;
			}
			// Update aabb.back and aabb.front.
			if(colliderAABB.min.z <= tempAABB.min.z){
				tempAABB.min.z = colliderAABB.min.z;
			}else if(colliderAABB.max.z > tempAABB.max.z){
				tempAABB.max.z = colliderAABB.max.z;
			}

		}

	}

	return tempAABB;

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