#include "collision.h"
#include "memoryManager.h"

void cCompositeInit(cComposite *const restrict cc){
	cc->colliders = NULL;
	cc->colliderNum = 0;
}

void cCompositeDelete(cComposite *const restrict cc){
	if(cc->colliders != NULL){
		memFree(cc->colliders);
	}
}