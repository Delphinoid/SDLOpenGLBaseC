#include "moduleModel.h"
#include "moduleSettings.h"
#include "model.h"
#include "sprite.h"
#include "memoryManager.h"
#include <string.h>

#define RESOURCE_DEFAULT_MODEL_BASE_SIZE sizeof(modelBase)
#define RESOURCE_DEFAULT_MODEL_SIZE sizeof(model)

memoryPool __g_ModelBaseResourceArray;  // Contains modelBases.
memorySLink __g_ModelResourceArray;  // Contains models.

return_t moduleModelResourcesInit(){
	void *memory;
	if(sprGenerateStateBuffer() < 0 || meshDefaultInit() < 0 || sprDefaultInit() < 0 || meshBillboardInit() < 0){
		return -1;
	}
	///mdlBaseDefaultInit(); mdlBaseSpriteInit(); mdlBaseBillboardInit();
	memory = memAllocate(
		memPoolAllocationSize(
			NULL,
			RESOURCE_DEFAULT_MODEL_BASE_SIZE,
			RESOURCE_DEFAULT_MODEL_BASE_NUM
		)
	);
	if(memPoolCreate(&__g_ModelBaseResourceArray, memory, RESOURCE_DEFAULT_MODEL_BASE_SIZE, RESOURCE_DEFAULT_MODEL_BASE_NUM) == NULL){
		return -1;
	}
	memory = memAllocate(
		memSLinkAllocationSize(
			NULL,
			RESOURCE_DEFAULT_MODEL_SIZE,
			RESOURCE_DEFAULT_MODEL_NUM
		)
	);
	if(memSLinkCreate(&__g_ModelResourceArray, memory, RESOURCE_DEFAULT_MODEL_SIZE, RESOURCE_DEFAULT_MODEL_NUM) == NULL){
		return -1;
	}
	return 1;
}
void moduleModelResourcesReset(){
	memoryRegion *region;
	moduleModelBaseClear();
	region = __g_ModelBaseResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__g_ModelBaseResourceArray.region->next = NULL;
	moduleModelClear();
	region = __g_ModelResourceArray.region->next;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	__g_ModelResourceArray.region->next = NULL;
}
void moduleModelResourcesDelete(){
	memoryRegion *region;
	moduleModelBaseClear();
	region = __g_ModelBaseResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	moduleModelClear();
	region = __g_ModelResourceArray.region;
	while(region != NULL){
		memoryRegion *next = memAllocatorNext(region);
		memFree(region->start);
		region = next;
	}
	mdlBaseDelete(&g_mdlDefault);
	mdlBaseDelete(&g_mdlSprite);
	mdlBaseDelete(&g_mdlBillboard);
	sprDeleteStateBuffer();
}

__HINT_INLINE__ modelBase *moduleModelBaseAllocateStatic(){
	return memPoolAllocate(&__g_ModelBaseResourceArray);
}
__HINT_INLINE__ modelBase *moduleModelBaseAllocate(){
	modelBase *r = moduleModelBaseAllocateStatic();
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memPoolAllocationSize(
				NULL,
				RESOURCE_DEFAULT_MODEL_BASE_SIZE,
				RESOURCE_DEFAULT_MODEL_BASE_NUM
			)
		);
		if(memPoolExtend(&__g_ModelBaseResourceArray, memory, RESOURCE_DEFAULT_MODEL_BASE_SIZE, RESOURCE_DEFAULT_MODEL_BASE_NUM)){
			r = moduleModelBaseAllocateStatic();
		}
	}
	return r;
}
__HINT_INLINE__ void moduleModelBaseFree(modelBase *const __RESTRICT__ resource){
	mdlBaseDelete(resource);
	memPoolFree(&__g_ModelBaseResourceArray, (void *)resource);
}
modelBase *moduleModelBaseFind(const char *const __RESTRICT__ name, const size_t nameLength){

	if(strncmp(name, g_mdlDefault.name, nameLength) == 0){
		return &g_mdlDefault;
	}else if(strncmp(name, g_mdlSprite.name, nameLength) == 0){
		return &g_mdlSprite;
	}else if(strncmp(name, g_mdlBillboard.name, nameLength) == 0){
		return &g_mdlBillboard;
	}

	MEMORY_POOL_LOOP_BEGIN(__g_ModelBaseResourceArray, i, modelBase *);

		// Compare the resources' names.
		if(strncmp(name, i->name, nameLength) == 0){
			return i;
		}

	MEMORY_POOL_LOOP_END(__g_ModelBaseResourceArray, i, return NULL;);

	return NULL;

}
void moduleModelBaseClear(){

	MEMORY_POOL_LOOP_BEGIN(__g_ModelBaseResourceArray, i, modelBase *);

		moduleModelBaseFree(i);
		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_INACTIVE_CASE(i);

		memPoolDataSetFlags(i, MEMORY_POOL_BLOCK_INVALID);

	MEMORY_POOL_LOOP_END(__g_ModelBaseResourceArray, i, return;);

}

__HINT_INLINE__ model *moduleModelAppendStatic(model **const __RESTRICT__ array){
	return memSLinkAppend(&__g_ModelResourceArray, (void **)array);
}
__HINT_INLINE__ model *moduleModelAppend(model **const __RESTRICT__ array){
	model *r = moduleModelAppendStatic(array);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_MODEL_SIZE,
				RESOURCE_DEFAULT_MODEL_NUM
			)
		);
		if(memSLinkExtend(&__g_ModelResourceArray, memory, RESOURCE_DEFAULT_MODEL_SIZE, RESOURCE_DEFAULT_MODEL_NUM)){
			r = moduleModelAppendStatic(array);
		}
	}
	return r;
}
__HINT_INLINE__ model *moduleModelInsertAfterStatic(model **const __RESTRICT__ array, model *const __RESTRICT__ resource){
	return memSLinkInsertAfter(&__g_ModelResourceArray, (void **)array, (void *)resource);
}
__HINT_INLINE__ model *moduleModelInsertAfter(model **const __RESTRICT__ array, model *const __RESTRICT__ resource){
	model *r = moduleModelInsertAfterStatic(array, resource);
	if(r == NULL){
		// Attempt to extend the allocator.
		void *const memory = memAllocate(
			memSLinkAllocationSize(
				NULL,
				RESOURCE_DEFAULT_MODEL_SIZE,
				RESOURCE_DEFAULT_MODEL_NUM
			)
		);
		if(memSLinkExtend(&__g_ModelResourceArray, memory, RESOURCE_DEFAULT_MODEL_SIZE, RESOURCE_DEFAULT_MODEL_NUM)){
			r = moduleModelInsertAfterStatic(array, resource);
		}
	}
	return r;
}
__HINT_INLINE__ model *moduleModelNext(const model *const __RESTRICT__ i){
	return (model *)memSLinkDataGetNext(i);
}
__HINT_INLINE__ void moduleModelFree(model **const __RESTRICT__ array, model *const __RESTRICT__ resource, const model *const __RESTRICT__ previous){
	mdlDelete(resource);
	memSLinkFree(&__g_ModelResourceArray, (void **)array, (void *)resource, (const void *)previous);
}
void moduleModelFreeArray(model **const __RESTRICT__ array){
	model *resource = *array;
	while(resource != NULL){
		moduleModelFree(array, resource, NULL);
		resource = *array;
	}
}
void moduleModelClear(){

	MEMORY_SLINK_LOOP_BEGIN(__g_ModelResourceArray, i, model *);

		moduleModelFree(NULL, i, NULL);
		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_INACTIVE_CASE(i);

		memSLinkDataSetFlags(i, MEMORY_SLINK_BLOCK_INVALID);

	MEMORY_SLINK_LOOP_END(__g_ModelResourceArray, i, return;);

}
