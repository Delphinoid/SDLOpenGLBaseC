#include "resourceManager.h"
#include "object.h"

signed char resMngrInit(resourceManager *resMngr, memoryManager *memMngr){

	/*
	** A general-purpose initializer
	** for the resource manager.
	*/

	byte_t *i;

	/* Shared data. */
	/*resMngr->resourceIDs = memMngrArenaNewTree(memMngr, NULL,
		RESOURCE_DEFAULT_IDENTIFIER_SIZE,
		RESOURCE_DEFAULT_IDENTIFIER_NUM
	);
	if(resMngr->resourceIDs == NULL){
		return -1;
	}

	// Shared mesh data.
	resMngr->meshes = memMngrArenaNewTree(memMngr, NULL,
		RESOURCE_DEFAULT_MESH_SIZE,
		RESOURCE_DEFAULT_MESH_NUM
	);
	if(resMngr->meshes == NULL){
		return -1;
	}*/

	// Shared hitbox data.
	resMngr->hitboxes = memMngrArenaNewSLink(memMngr, NULL,
		RESOURCE_DEFAULT_HITBOX_SIZE,
		RESOURCE_DEFAULT_HITBOX_NUM
	);
	if(resMngr->hitboxes == NULL){
		return -1;
	}

	// Shared physics data.
	resMngr->colliders = memMngrArenaNewSLink(memMngr, NULL,
		RESOURCE_DEFAULT_COLLIDER_SIZE,
		RESOURCE_DEFAULT_COLLIDER_NUM
	);
	if(resMngr->colliders == NULL){
		return -1;
	}


	/* Uninstantiated data. */
	// Uninstantiated object data.
	resMngr->objects = memMngrArenaNewList(memMngr, NULL,
		RESOURCE_DEFAULT_OBJECT_SIZE,
		RESOURCE_DEFAULT_OBJECT_NUM
	);
	if(resMngr->objects == NULL){
		return -1;
	}

	// Uninstantiated texture data.
	resMngr->textures = memMngrArenaNewList(memMngr, NULL,
		RESOURCE_DEFAULT_TEXTURE_SIZE,
		RESOURCE_DEFAULT_TEXTURE_NUM
	);
	if(resMngr->textures == NULL){
		return -1;
	}
	resMngr->textureWrappers = memMngrArenaNewList(memMngr, NULL,
		RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE,
		RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM
	);
	if(resMngr->textureWrappers == NULL){
		return -1;
	}
	/*resMngr->textureWrapperData = memMngrArenaNewTree(memMngr, NULL,
		RESOURCE_DEFAULT_TEXTURE_WRAPPER_DATA_SIZE,
		RESOURCE_DEFAULT_TEXTURE_WRAPPER_DATA_NUM
	);
	if(resMngr->textureWrapperData == NULL){
		return -1;
	}*/

	// Uninstantiated model data.
	resMngr->models = memMngrArenaNewList(memMngr, NULL,
		RESOURCE_DEFAULT_MODEL_SIZE,
		RESOURCE_DEFAULT_MODEL_NUM
	);
	if(resMngr->models == NULL){
		return -1;
	}

	// Uninstantiated renderable data.
	resMngr->renderables = memMngrArenaNewList(memMngr, NULL,
		RESOURCE_DEFAULT_RENDERABLE_SIZE,
		RESOURCE_DEFAULT_RENDERABLE_NUM
	);
	if(resMngr->renderables == NULL){
		return -1;
	}

	// Uninstantiated skeleton data.
	resMngr->skeletons = memMngrArenaNewList(memMngr, NULL,
		RESOURCE_DEFAULT_SKELETON_SIZE,
		RESOURCE_DEFAULT_SKELETON_NUM
	);
	if(resMngr->skeletons == NULL){
		return -1;
	}
	/*resMngr->skeletonData = memMngrArenaNewTree(memMngr, NULL,
		RESOURCE_DEFAULT_SKELETON_DATA_SIZE,
		RESOURCE_DEFAULT_SKELETON_DATA_NUM
	);
	if(resMngr->skeletonData == NULL){
		return -1;
	}*/
	resMngr->skeletalAnimations = memMngrArenaNewList(memMngr, NULL,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM
	);
	if(resMngr->skeletalAnimations == NULL){
		return -1;
	}
	/*resMngr->skeletalAnimationData = memMngrArenaNewTree(memMngr, NULL,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_DATA_SIZE,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_DATA_NUM
	);
	if(resMngr->skeletalAnimationData == NULL){
		return -1;
	}*/

	// Uninstantiated physics data.
	resMngr->rigidBodies = memMngrArenaNewList(memMngr, NULL,
		RESOURCE_DEFAULT_RIGID_BODY_SIZE,
		RESOURCE_DEFAULT_RIGID_BODY_NUM
	);
	if(resMngr->rigidBodies == NULL){
		return -1;
	}


	/* Instantiated data. */
	// Instantiated object data.
	resMngr->objectInstances = memMngrArenaNewPool(memMngr, NULL,
		RESOURCE_DEFAULT_OBJECT_INSTANCE_SIZE,
		RESOURCE_DEFAULT_OBJECT_INSTANCE_NUM
	);
	if(resMngr->objectInstances == NULL){
		return -1;
	}
	/*resMngr->objectInstanceData = memMngrArenaNewTree(memMngr, NULL,
		RESOURCE_DEFAULT_OBJECT_INSTANCE_DATA_SIZE,
		RESOURCE_DEFAULT_OBJECT_INSTANCE_DATA_NUM
	);
	if(resMngr->objectInstanceData == NULL){
		return -1;
	}*/

	// Instantiated renderable data.
	resMngr->renderableInstances = memMngrArenaNewSLink(memMngr, NULL,
		RESOURCE_DEFAULT_RENDERABLE_INSTANCE_SIZE,
		RESOURCE_DEFAULT_RENDERABLE_INSTANCE_NUM
	);
	if(resMngr->renderableInstances == NULL){
		return -1;
	}

	// Instantiated skeleton data.
	resMngr->skeletalAnimationInstances = memMngrArenaNewSLink(memMngr, NULL,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM
	);
	if(resMngr->skeletalAnimationInstances == NULL){
		return -1;
	}
	resMngr->skeletalAnimationFragments = memMngrArenaNewSLink(memMngr, NULL,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE,
		RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM
	);
	if(resMngr->skeletalAnimationFragments == NULL){
		return -1;
	}

	// Instantiated hitbox data.
	resMngr->hitboxArrayInstances = memMngrArenaNewSLink(memMngr, NULL,
		RESOURCE_DEFAULT_HITBOX_ARRAY_SIZE,
		RESOURCE_DEFAULT_HITBOX_ARRAY_NUM
	);
	if(resMngr->hitboxArrayInstances == NULL){
		return -1;
	}

	// Instantiated physics data.
	resMngr->rigidBodyInstances = memMngrArenaNewSLink(memMngr, NULL,
		RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_SIZE,
		RESOURCE_DEFAULT_RIGID_BODY_INSTANCE_NUM
	);
	if(resMngr->rigidBodyInstances == NULL){
		return -1;
	}
	resMngr->constraintInstances = memMngrArenaNewSLink(memMngr, NULL,
		RESOURCE_DEFAULT_CONSTRAINT_SIZE,
		RESOURCE_DEFAULT_CONSTRAINT_NUM
	);
	if(resMngr->constraintInstances == NULL){
		return -1;
	}
	resMngr->collisionInstances = memMngrArenaNewSLink(memMngr, NULL,
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
	}

	resMngr->memMngr = memMngr;
	return 1;

}

signed char resMngrCreateDefaultResources(resourceManager *resMngr){

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

	defaultTexture = (texture *)memListAllocate(resMngr->textures);
	if(defaultTexture == NULL){
		return -1;
	}
	tDefault(defaultTexture, resMngr);

	defaultTextureWrapper = (textureWrapper *)memListAllocate(resMngr->textureWrappers);
	if(defaultTexture == NULL){
		return -1;
	}
	twDefault(defaultTextureWrapper, resMngr);

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
	//memTreeFree(&resMngr->memMngr->allocator, resMngr->resourceIDs);

	// Shared mesh data.
	//memTreeFree(&resMngr->memMngr->allocator, resMngr->meshes);
	// Shared hitbox data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->hitboxes);
	// Shared physics data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->colliders);


	/* Uninstantiated data. */
	// Uninstantiated object data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->objects);

	// Uninstantiated texture data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->textures);
	memTreeFree(&resMngr->memMngr->allocator, resMngr->textureWrappers);
	//memTreeFree(&resMngr->memMngr->allocator, resMngr->textureWrapperData);

	// Uninstantiated model data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->models);

	// Uninstantiated renderable data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->renderables);

	// Uninstantiated skeleton data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->skeletons);
	//memTreeFree(&resMngr->memMngr->allocator, resMngr->skeletonData);
	memTreeFree(&resMngr->memMngr->allocator, resMngr->skeletalAnimations);
	//memTreeFree(&resMngr->memMngr->allocator, resMngr->skeletalAnimationData);

	// Uninstantiated physics data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->rigidBodies);


	/* Instantiated data. */
	// Instantiated object data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->objectInstances);
	//memTreeFree(&resMngr->memMngr->allocator, resMngr->objectInstanceData);

	// Instantiated renderable data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->renderableInstances);

	// Instantiated skeleton data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->skeletalAnimationInstances);
	memTreeFree(&resMngr->memMngr->allocator, resMngr->skeletalAnimationFragments);

	// Instantiated hitbox data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->hitboxArrayInstances);

	// Instantiated physics data.
	memTreeFree(&resMngr->memMngr->allocator, resMngr->rigidBodyInstances);
	memTreeFree(&resMngr->memMngr->allocator, resMngr->constraintInstances);
	memTreeFree(&resMngr->memMngr->allocator, resMngr->collisionInstances);

}
