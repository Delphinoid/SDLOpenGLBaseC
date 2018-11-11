#ifndef MODULERENDERABLE_H
#define MODULERENDERABLE_H

#include "memoryManager.h"
#include "renderable.h"

#define RESOURCE_DEFAULT_RENDERABLE_SIZE sizeof(renderable)
#ifndef RESOURCE_DEFAULT_RENDERABLE_NUM
	#define RESOURCE_DEFAULT_RENDERABLE_NUM RESOURCE_DEFAULT_MODEL_NUM
#endif

#define RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE sizeof(rndrInstance)
#ifndef RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM
	#define RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM 4*RESOURCE_DEFAULT_OBJECT_INSTANCE_NUM
#endif

#ifndef MODULE_RENDERABLE_USE_LOCAL_DEFINITION

extern memoryPool  RenderableResourceArray;          // Contains renderable.
extern memorySLink RenderableInstanceResourceArray;  // Contains rndrInstances.

return_t moduleRenderableInit(){
	void *memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RENDERABLE_SIZE,
			RESOURCE_DEFAULT_RENDERABLE_NUM
		)
	);
	if(memPoolCreate(&RenderableResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_SIZE, RESOURCE_DEFAULT_RENDERABLE_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE,
			RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM
		)
	);
	if(memSLinkCreate(&RenderableInstanceResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE, RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM) == NULL){
		return -1;
	}
	return 1;
}

#else

return_t moduleRenderableInit(
	memoryPool  *RenderableResourceArray,         // Contains renderable.
	memorySLink *RenderableInstanceResourceArray  // Contains rndrInstances.
){
	void *memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RENDERABLE_SIZE,
			RESOURCE_DEFAULT_RENDERABLE_NUM
		)
	);
	if(memPoolCreate(RenderableResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_SIZE, RESOURCE_DEFAULT_RENDERABLE_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE,
			RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM
		)
	);
	if(memSLinkCreate(RenderableInstanceResourceArray, memory, RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE, RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM) == NULL){
		return -1;
	}
	return 1;
}

#endif

#endif
