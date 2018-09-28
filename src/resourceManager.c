#include "resourceManager.h"
#include "object.h"

#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
return_t resMngrInit(resourceManager *resMngr, memoryManager *memMngr){
#else
return_t resMngrInit(resourceManager *resMngr){
#endif

	/*
	** A general-purpose initializer
	** for the resource manager.
	*/

	//byte_t *i;

	/* Shared data. */
	/*resMngr->resourceIDs = memMngrArenaNewTree(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_IDENTIFIER_SIZE,
		RESOURCE_DEFAULT_IDENTIFIER_NUM
	);
	if(resMngr->resourceIDs == NULL){
		return -1;
	}

	// Shared mesh data.
	resMngr->meshes = memMngrArenaNewTree(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_MESH_SIZE,
		RESOURCE_DEFAULT_MESH_NUM
	);
	if(resMngr->meshes == NULL){
		return -1;
	}*

	// Shared hitbox data.
	resMngr->hitboxes = memMngrArenaNewSLink(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_HITBOX_SIZE,
		RESOURCE_DEFAULT_HITBOX_NUM
	);
	if(resMngr->hitboxes == NULL){
		return -1;
	}

	// Shared physics data.
	resMngr->colliders = memMngrArenaNewSLink(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_COLLIDER_SIZE,
		RESOURCE_DEFAULT_COLLIDER_NUM
	);
	if(resMngr->colliders == NULL){
		return -1;
	}


	/* Uninstantiated data. *
	// Uninstantiated object data.
	resMngr->objects = memMngrArenaNewList(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_OBJECT_SIZE,
		RESOURCE_DEFAULT_OBJECT_NUM
	);
	if(resMngr->objects == NULL){
		return -1;
	}

	// Uninstantiated texture data.
	resMngr->textures = memMngrArenaNewList(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_TEXTURE_SIZE,
		RESOURCE_DEFAULT_TEXTURE_NUM
	);
	if(resMngr->textures == NULL){
		return -1;
	}
	resMngr->textureWrappers = memMngrArenaNewList(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE,
		RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM
	);
	if(resMngr->textureWrappers == NULL){
		return -1;
	}
	/*resMngr->textureWrapperData = memMngrArenaNewTree(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_TEXTURE_WRAPPER_DATA_SIZE,
		RESOURCE_DEFAULT_TEXTURE_WRAPPER_DATA_NUM
	);
	if(resMngr->textureWrapperData == NULL){
		return -1;
	}*

	// Uninstantiated model data.
	resMngr->models = memMngrArenaNewList(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_MODEL_SIZE,
		RESOURCE_DEFAULT_MODEL_NUM
	);
	if(resMngr->models == NULL){
		return -1;
	}

	// Uninstantiated renderable data.
	resMngr->renderables = memMngrArenaNewList(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_RENDERABLE_SIZE,
		RESOURCE_DEFAULT_RENDERABLE_NUM
	);
	if(resMngr->renderables == NULL){
		return -1;
	}

	// Uninstantiated skeleton data.
	resMngr->skeletons = memMngrArenaNewList(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_SKELETON_SIZE,
		RESOURCE_DEFAULT_SKELETON_NUM
	);
	if(resMngr->skeletons == NULL){
		return -1;
	}
	/*resMngr->skeletonData = memMngrArenaNewTree(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_SKELETON_DATA_SIZE,
		RESOURCE_DEFAULT_SKELETON_DATA_NUM
	);
	if(resMngr->skeletonData == NULL){
		return -1;
	}*
	resMngr->skeletalAnimations = memMngrArenaNewList(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM
	);
	if(resMngr->skeletalAnimations == NULL){
		return -1;
	}
	/*resMngr->skeletalAnimationData = memMngrArenaNewTree(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_DATA_SIZE,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_DATA_NUM
	);
	if(resMngr->skeletalAnimationData == NULL){
		return -1;
	}*

	// Uninstantiated physics data.
	resMngr->rigidBodies = memMngrArenaNewList(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_RIGID_BODY_SIZE,
		RESOURCE_DEFAULT_RIGID_BODY_NUM
	);
	if(resMngr->rigidBodies == NULL){
		return -1;
	}


	/* Instantiated data. *
	// Instantiated object data.
	resMngr->objectInstances = memMngrArenaNewPool(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_OBJECT_INSTANCE_SIZE,
		RESOURCE_DEFAULT_OBJECT_INSTANCE_NUM
	);
	if(resMngr->objectInstances == NULL){
		return -1;
	}
	/*resMngr->objectInstanceData = memMngrArenaNewTree(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_OBJECT_INSTANCE_DATA_SIZE,
		RESOURCE_DEFAULT_OBJECT_INSTANCE_DATA_NUM
	);
	if(resMngr->objectInstanceData == NULL){
		return -1;
	}*

	// Instantiated renderable data.
	resMngr->renderableInstances = memMngrArenaNewSLink(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE,
		RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM
	);
	if(resMngr->renderableInstances == NULL){
		return -1;
	}

	// Instantiated skeleton data.
	resMngr->skeletalAnimationInstances = memMngrArenaNewSLink(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM
	);
	if(resMngr->skeletalAnimationInstances == NULL){
		return -1;
	}
	resMngr->skeletalAnimationFragments = memMngrArenaNewSLink(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM
	);
	if(resMngr->skeletalAnimationFragments == NULL){
		return -1;
	}

	// Instantiated hitbox data.
	resMngr->hitboxArrayInstances = memMngrArenaNewSLink(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_HITBOX_ARRAY_SIZE,
		RESOURCE_DEFAULT_HITBOX_ARRAY_NUM
	);
	if(resMngr->hitboxArrayInstances == NULL){
		return -1;
	}

	// Instantiated physics data.
	resMngr->rigidBodyInstances = memMngrArenaNewSLink(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE,
		RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM
	);
	if(resMngr->rigidBodyInstances == NULL){
		return -1;
	}
	resMngr->constraintInstances = memMngrArenaNewSLink(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_CONSTRAINT_SIZE,
		RESOURCE_DEFAULT_CONSTRAINT_NUM
	);
	if(resMngr->constraintInstances == NULL){
		return -1;
	}
	resMngr->collisionInstances = memMngrArenaNewSLink(
		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		memMngr,
		#endif
		NULL,
		RESOURCE_DEFAULT_COLLISION_SIZE,
		RESOURCE_DEFAULT_COLLISION_NUM
	);
	if(resMngr->collisionInstances == NULL){
		return -1;
	}

	// Initialize each object.
	i = memListFirst(resMngr->objects);
	while(i < memListEnd(resMngr->objects)){
		objInit((object *)i);
		memListBlockNext(resMngr->objects, i);
	}

	// Initialize each texture.
	i = memListFirst(resMngr->textures);
	while(i < memListEnd(resMngr->textures)){
		tInit((texture *)i);
		memListBlockNext(resMngr->textures, i);
	}

	// Initialize each texture wrapper.
	i = memListFirst(resMngr->textureWrappers);
	while(i < memListEnd(resMngr->textureWrappers)){
		twInit((textureWrapper *)i);
		memListBlockNext(resMngr->textureWrappers, i);
	}

	// Initialize each model.
	i = memListFirst(resMngr->models);
	while(i < memListEnd(resMngr->models)){
		mdlInit((model *)i);
		memListBlockNext(resMngr->models, i);
	}

	// Initialize each renderable.
	i = memListFirst(resMngr->renderables);
	while(i < memListEnd(resMngr->renderables)){
		rndrInit((renderable *)i);
		memListBlockNext(resMngr->renderables, i);
	}

	// Initialize each skeleton.
	i = memListFirst(resMngr->skeletons);
	while(i < memListEnd(resMngr->skeletons)){
		sklInit((skeleton *)i);
		memListBlockNext(resMngr->skeletons, i);
	}

	// Initialize each skeletal animation.
	i = memListFirst(resMngr->skeletalAnimations);
	while(i < memListEnd(resMngr->skeletalAnimations)){
		sklaInit((sklAnim *)i);
		memListBlockNext(resMngr->skeletalAnimations, i);
	}

	// Initialize each rigid body.
	i = memListFirst(resMngr->rigidBodies);
	while(i < memListEnd(resMngr->rigidBodies)){
		physRigidBodyInit((physRigidBody *)i);
		memListBlockNext(resMngr->rigidBodies, i);
	}*/


	#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
	resMngr->memMngr = memMngr;
	#endif
	return 1;

}

return_t resMngrCreateDefaultResources(resourceManager *resMngr){

	//char *defaultName;
	texture *defaultTexture;
	textureWrapper *defaultTextureWrapper;
	skeleton *defaultSkeleton;
	model *defaultModel;

	/*defaultName = (char *)memTreeAllocate(&resMngr->memMngr->allocator, 8*sizeof(char));
	if(defaultName == NULL){
		return -1;
	}
	defaultName[0] = 'd';
	defaultName[1] = 'e';
	defaultName[2] = 'f';
	defaultName[3] = 'a';
	defaultName[4] = 'u';
	defaultName[5] = 'l';
	defaultName[6] = 't';
	defaultName[7] = '\0';*/

	/*defaultTexture = (texture *)memListAllocate(&resMngr->textures);
	if(defaultTexture == NULL){
		return -1;
	}
	tDefault(defaultTexture);

	defaultTextureWrapper = (textureWrapper *)memListAllocate(&resMngr->textureWrappers);
	if(defaultTexture == NULL){
		return -1;
	}
	twDefault(defaultTextureWrapper);*/

	/*defaultSkeleton = (skeleton *)memListAllocate(resMngr->skeletons);
	if(defaultSkeleton == NULL){
		return -1;
	}
	sklDefault(defaultSkeleton, resMngr);

	defaultModel = (model *)memListAllocate(resMngr->models);
	if(defaultTexture == NULL){
		return -1;
	}
	mdlDefault(defaultModel, resMngr);*/

	return 1;

}

void resMngrDelete(resourceManager *resMngr){

	/* Shared data. */
	//memTreeDelete(&resMngr->resourceIDs);

	// Shared mesh data.
	//memTreeDelete(&resMngr->meshes);
	// Shared hitbox data.
	/*memSLinkDelete(&resMngr->hitboxes);
	// Shared physics data.
	memSLinkDelete(&resMngr->colliders);


	/* Uninstantiated data. *
	// Uninstantiated object data.
	memListDelete(&resMngr->objects);

	// Uninstantiated texture data.
	memListDelete(&resMngr->textures);
	memListDelete(&resMngr->textureWrappers);
	//memTreeDelete(&resMngr->textureWrapperData);

	// Uninstantiated model data.
	memListDelete(&resMngr->models);

	// Uninstantiated renderable data.
	memListDelete(&resMngr->renderables);

	// Uninstantiated skeleton data.
	memListDelete(&resMngr->skeletons);
	//memTreeDelete(&resMngr->skeletonData);
	memListDelete(&resMngr->skeletalAnimations);
	//memTreeDelete(&resMngr->skeletalAnimationData);

	// Uninstantiated physics data.
	memListDelete(&resMngr->rigidBodies);


	/* Instantiated data. *
	// Instantiated object data.
	memPoolDelete(&resMngr->objectInstances);
	//memTreeDelete(&resMngr->objectInstanceData);

	// Instantiated renderable data.
	memSLinkDelete(&resMngr->renderableInstances);

	// Instantiated skeleton data.
	memSLinkDelete(&resMngr->skeletalAnimationInstances);
	memSLinkDelete(&resMngr->skeletalAnimationFragments);

	// Instantiated hitbox data.
	memSLinkDelete(&resMngr->hitboxArrayInstances);

	// Instantiated physics data.
	memSLinkDelete(&resMngr->rigidBodyInstances);
	memSLinkDelete(&resMngr->constraintInstances);
	memSLinkDelete(&resMngr->collisionInstances);*/

}
