#ifndef MODULEHITBOX_H
#define MODULEHITBOX_H

#include "memoryManager.h"
#include "hitbox.h"

#define RESOURCE_DEFAULT_HITBOX_SIZE sizeof(hitbox)
#ifndef RESOURCE_DEFAULT_HITBOX_NUM
	#define RESOURCE_DEFAULT_HITBOX_NUM 2*4096
#endif

#define RESOURCE_DEFAULT_HITBOX_ARRAY_SIZE sizeof(hbArray)
#ifndef RESOURCE_DEFAULT_HITBOX_ARRAY_NUM
	#define RESOURCE_DEFAULT_HITBOX_ARRAY_NUM SKL_MAX_BONE_NUM*RESOURCE_DEFAULT_OBJECT_INSTANCE_NUM
#endif

#ifndef MODULE_HITBOX_USE_LOCAL_DEFINITION

extern memorySLink HitboxResourceArray;       // Contains hitboxes.
extern memorySLink HitboxArrayResourceArray;  // Contains hbArrays.

return_t moduleHitboxInit(){
	void *memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_HITBOX_SIZE,
			RESOURCE_DEFAULT_HITBOX_NUM
		)
	);
	if(memSLinkCreate(&HitboxResourceArray, memory, RESOURCE_DEFAULT_HITBOX_SIZE, RESOURCE_DEFAULT_HITBOX_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_HITBOX_ARRAY_SIZE,
			RESOURCE_DEFAULT_HITBOX_ARRAY_NUM
		)
	);
	if(memSLinkCreate(&HitboxArrayResourceArray, memory, RESOURCE_DEFAULT_HITBOX_ARRAY_SIZE, RESOURCE_DEFAULT_HITBOX_ARRAY_NUM) == NULL){
		return -1;
	}
	return 1;
}

#else

return_t moduleHitboxInit(
	memorySLink *HitboxResourceArray,      // Contains hitboxes.
	memorySLink *HitboxArrayResourceArray  // Contains hbArrays.
){
	void *memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_HITBOX_SIZE,
			RESOURCE_DEFAULT_HITBOX_NUM
		)
	);
	if(memSLinkCreate(HitboxResourceArray, memory, RESOURCE_DEFAULT_HITBOX_SIZE, RESOURCE_DEFAULT_HITBOX_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_HITBOX_ARRAY_SIZE,
			RESOURCE_DEFAULT_HITBOX_ARRAY_NUM
		)
	);
	if(memSLinkCreate(HitboxArrayResourceArray, memory, RESOURCE_DEFAULT_HITBOX_ARRAY_SIZE, RESOURCE_DEFAULT_HITBOX_ARRAY_NUM) == NULL){
		return -1;
	}
	return 1;
}

#endif

#endif
