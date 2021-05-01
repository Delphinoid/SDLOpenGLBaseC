#include "graphicsManager.h"
#include "object.h"
#include "texture.h"
#include "model.h"
#include "collider.h"
#include "physicsRigidBody.h"
#include "memoryManager.h"
#include "helpersMath.h"
#include "helpersFileIO.h"
#include "vertex.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/****/
#include "moduleTextureWrapper.h"
#include "moduleSkeleton.h"
#include "moduleModel.h"
#include "modulePhysics.h"

/****/
#include "graphicsDebug.h"
#include "camera.h"

#define OBJECT_RESOURCE_DIRECTORY_STRING FILE_PATH_RESOURCE_DIRECTORY_SHARED"Resources"FILE_PATH_DELIMITER_STRING"Objects"FILE_PATH_DELIMITER_STRING
#define OBJECT_RESOURCE_DIRECTORY_LENGTH 20

void objBaseInit(objectBase *const __RESTRICT__ base){
	memset(base, 0, sizeof(objectBase));
}

return_t objBaseLoad(objectBase *const __RESTRICT__ base, const char *const __RESTRICT__ filePath, const size_t filePathLength){

	char fullPath[FILE_MAX_PATH_LENGTH];

	FILE *__RESTRICT__ objInfo;

	objBaseInit(base);

	fileGenerateFullPath(fullPath, OBJECT_RESOURCE_DIRECTORY_STRING, OBJECT_RESOURCE_DIRECTORY_LENGTH, filePath, filePathLength);
	objInfo = fopen(fullPath, "r");

	if(objInfo != NULL){

		char lineFeed[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		fileLine_t currentLine = 0;  // Current file line being read.

		char loadPath[FILE_MAX_PATH_LENGTH];
		size_t loadPathLength;

		while(fileParseNextLine(objInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			++currentLine;

			// Skeleton
			if(lineLength >= 10 && strncmp(line, "skeleton ", 9) == 0){

				skeleton *tempSkl;
				loadPathLength = fileParseResourcePath(loadPath, line+9, lineLength-9);

				// Check if the skeleton has already been loaded.
				tempSkl = moduleSkeletonFind(loadPath, loadPathLength);
				if(tempSkl != NULL){
					base->skl = tempSkl;

				// If the skeleton path is surrounded by quotes, try and load it.
				}else{
					tempSkl = moduleSkeletonAllocate();
					if(tempSkl != NULL){
						const return_t r = sklLoad(tempSkl, loadPath, loadPathLength);
						if(r < 1){
							// The load failed. Clean up.
							moduleSkeletonFree(tempSkl);
							if(r < 0){
								/** Memory allocation failure. **/
								objBaseDelete(base);
								fclose(objInfo);
								return -1;
							}
							printf("Error loading object \"%s\": Skeleton \"%s\" at line %u does not exist.\n", fullPath, loadPath, currentLine);
							base->skl = &g_sklDefault;
						}else{
							base->skl = tempSkl;
						}
					}else{
						/** Memory allocation failure. **/
						objBaseDelete(base);
						fclose(objInfo);
						return -1;
					}
				}


			// Physics
			}else if(lineLength >= 17 && strncmp(line, "skeletonPhysics ", 16) == 0){
				if(base->skl != NULL){

					return_t r;
					loadPathLength = fileParseResourcePath(loadPath, line+16, lineLength-16);

					// Load the rigid body from a file.
					r = physRigidBodyBaseLoad(
						&base->skeletonBodies, &base->skeletonBodyIDs, &base->skeletonBodyNum,
						base->skl, loadPath, loadPathLength
					);

					if(r < 0){
						/** Memory allocation failure. **/
						objBaseDelete(base);
						fclose(objInfo);
						return -1;
					}else if(r == 0){
						modulePhysicsRigidBodyBaseFreeArray(&base->skeletonBodies);
						base->skeletonBodies = NULL;
					}

				}else{
					printf("Error loading object \"%s\": Cannot load rigid bodies at line %u when no skeleton has been specified.\n", fullPath, currentLine);
				}


			// Hitboxes
			}else if(lineLength >= 18 && strncmp(line, "skeletonHitboxes ", 17) == 0){
				if(base->skl != NULL){
					/** Load hitboxes. **/

				}else{
					printf("Error loading object \"%s\": Cannot load hitbox data at line %u when no skeleton has been specified.\n", fullPath, currentLine);
				}


			// Animation
			}else if(lineLength >= 11 && strncmp(line, "animation ", 10) == 0){

				sklAnim *tempSkla;
				loadPathLength = fileParseResourcePath(loadPath, line+10, lineLength-10);

				// Check if the animation has already been loaded.
				tempSkla = moduleSkeletonAnimationFind(loadPath, loadPathLength);

				// If the animation path is surrounded by quotes, try and load it.
				if(tempSkla == NULL){
					tempSkla = moduleSkeletonAnimationAllocate();
					if(tempSkla != NULL){
						const return_t r = sklaLoad(tempSkla, loadPath, loadPathLength);
						if(r < 1){
							// The load failed. Clean up.
							moduleSkeletonAnimationFree(tempSkla);
							if(r < 0){
								/** Memory allocation failure. **/
								objBaseDelete(base);
								fclose(objInfo);
								return -1;
							}
							tempSkla = NULL;
						}
					}else{
						/** Memory allocation failure. **/
						objBaseDelete(base);
						fclose(objInfo);
						return -1;
					}
				}

				if(tempSkla != NULL){

					sklAnim **const tempBuffer = memReallocate(base->animations, (base->animationNum+1)*sizeof(sklAnim *));
					if(tempBuffer == NULL){
						/** Memory allocation failure. **/
						objBaseDelete(base);
						fclose(objInfo);
						return -1;
					}
					base->animations = tempBuffer;
					base->animations[base->animationNum] = tempSkla;
					++base->animationNum;

				}else{
					printf("Error loading object \"%s\": Skeletal animation \"%s\" at line %u does not exist.\n", fullPath, loadPath, currentLine);
				}


			// Animation capacity
			}else if(lineLength >= 20 && strncmp(line, "animationCapacity ", 19) == 0){
				base->animationAllocate = strtoul(line+19, NULL, 0);


			// Renderable
			}else if(lineLength >= 14 && strncmp(line, "renderable ", 11) == 0){

				modelBase *tempMdl;
				loadPathLength = fileParseResourcePath(loadPath, line+11, lineLength-11);

				// Check if the model has already been loaded.
				tempMdl = moduleModelBaseFind(loadPath, loadPathLength);
				if(tempMdl == NULL){
					// If the model path is surrounded by quotes, try and load it.
					tempMdl = moduleModelBaseAllocate();
					if(tempMdl != NULL){
						const return_t r = mdlBaseLoad(tempMdl, loadPath, loadPathLength);
						if(r < 1){
							// The load failed. Clean up.
							moduleModelBaseFree(tempMdl);
							if(r < 0){
								/** Memory allocation failure. **/
								objBaseDelete(base);
								fclose(objInfo);
								return -1;
							}
							printf("Error loading object \"%s\": Model \"%s\" at line %u does not exist.\n", fullPath, loadPath, currentLine);
							tempMdl = &g_mdlDefault;
						}
					}else{
						/** Memory allocation failure. **/
						objBaseDelete(base);
						fclose(objInfo);
						return -1;
					}
				}

				base->models = memReallocate(base->models, (base->modelNum+1)*sizeof(model *));
				if(base->models == NULL){
					/** Memory allocation failure. **/
					objBaseDelete(base);
					fclose(objInfo);
					return -1;
				}
				base->models[base->modelNum] = tempMdl;
				++base->modelNum;

			}

		}

		fclose(objInfo);

	}else{
		printf("Error loading object \"%s\": Could not open file.\n", fullPath);
		return 0;
	}

	// If no skeleton was loaded, load the default one.
	if(base->skl == NULL){
		//printf("Error loading object: No skeleton was loaded.\n");
		base->skl = &g_sklDefault;
	}

	// If no renderables were loaded, load the default one.
	if(base->models == NULL){
		printf("Error loading object \"%s\": No renderables were loaded.\n", fullPath);
		base->modelNum = 1;
		base->models = memAllocate(base->modelNum*sizeof(model *));
		if(base->models == NULL){
			/** Memory allocation failure. **/
			objBaseDelete(base);
			return -1;
		}
		base->models[0] = &g_mdlDefault;
	}

	// Generate a name based off the file path.
	base->name = fileGenerateResourceName(filePath, filePathLength);
	if(base->name == NULL){
		/** Memory allocation failure. **/
		objBaseDelete(base);
		return -1;
	}

	return 1;

}

void objBaseDelete(objectBase *const __RESTRICT__ base){
	if(base->name != NULL){
		memFree(base->name);
	}
	if(base->animations != NULL){
		memFree(base->animations);
	}
	if(base->skl != NULL){
		if(base->skeletonBodyIDs != NULL){
			memFree(base->skeletonBodyIDs);
		}
		if(base->skeletonBodies != NULL){
			modulePhysicsRigidBodyBaseFreeArray(&base->skeletonBodies);
		}
		if(base->skeletonColliders != NULL){
			collider *c = base->skeletonColliders;
			const collider *const cLast = &c[base->skl->boneNum];
			for(; c < cLast; ++c){
				cDelete(c);
			}
			memFree(base->skeletonColliders);
		}
	}
	if(base->models != NULL){
		memFree(base->models);
	}
}

return_t objInit(object *const __RESTRICT__ obj){
	obj->base = NULL;
	obj->configuration = NULL;
	obj->skeletonBodyNum = 0;
	obj->skeletonColliderNum = 0;
	obj->stateMax = 0;
	obj->stateNum = 0;
	obj->state.configuration = NULL;
	obj->state.previous = NULL;
	obj->oldestStatePrevious = &obj->state.previous;
	obj->skeletonBodyIDs = NULL;
	obj->skeletonBodies = NULL;
	obj->skeletonColliders = NULL;
	obj->models = NULL;
	return skliInit(&obj->skeletonData, NULL, 0);
}

void objDelete(object *const __RESTRICT__ obj){
	objState *state = obj->state.previous;
	boneIndex_t id = obj->skeletonBodyNum;
	if(obj->configuration != NULL){
		// This frees skeletonBodyIDs and skeletonColliders as well.
		memFree(obj->configuration);
	}
	while(state != NULL){
		objState *next = state->previous;
		memFree(state);
		state = next;
	}
	while(id > 0){
		modulePhysicsRigidBodyFree(&obj->skeletonBodies, obj->skeletonBodies);
		--id;
	}
	if(obj->skeletonColliders != NULL){
		collider *c = obj->skeletonColliders;
		const collider *const cLast = &c[obj->skeletonData.skl->boneNum];
		for(; c < cLast; ++c){
			cDelete(c);
		}
	}
	if(obj->models != NULL){
		moduleModelFreeArray(&obj->models);
	}
	skliDelete(&obj->skeletonData);
}

return_t objInstantiate(object *const __RESTRICT__ obj, const objectBase *const __RESTRICT__ base){

	uintptr_t offset_bone = 0;
	uintptr_t offset_collider = 0;
	uintptr_t offset_body = 0;
	byte_t *buffer;
	const modelBase **m = base->models;
	const modelBase **const mLast = &m[base->modelNum];

	if(base->skl->boneNum > 0){
		// Allocate memory for the configuration and current state skeletons.
		offset_bone = base->skl->boneNum * sizeof(transform);
	}
	if(base->skeletonColliderNum > 0){
		// Allocate memory for the collider array.
		offset_collider = base->skeletonColliderNum * sizeof(collider);
	}
	if(base->skeletonBodyNum > 0){
		// Allocate memory for the rigid body ID array.
		offset_body = base->skeletonBodyNum * sizeof(boneIndex_t);
	}

	// Allocate a single block of memory for everything.
	buffer = memAllocate((offset_bone << 1) + offset_collider + offset_body);
	if(buffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}

	// Set up the object's skeleton arrays.
	if(base->skl->boneNum > 0){

		transform *b = (transform *)buffer;

		// Set up the configuration skeleton.
		obj->configuration = b;
		buffer += offset_bone;

		// Initialize each bone.
		for(; b < (transform *)buffer; ++b){
			tfInit(b);
		}

		// Set up the current state skeleton.
		obj->state.configuration = (transform *)buffer;
		buffer += offset_bone;

	}else{
		obj->configuration = NULL;
		obj->state.configuration = NULL;
	}

	// Set up the object's collider array.
	if(base->skeletonColliderNum > 0){

		collider *cLocal = base->skeletonColliders;
		collider *c = (collider *)buffer;

		// Set the collider array pointer.
		obj->skeletonColliders = c;
		buffer += offset_collider;

		// Instantiate each collider.
		for(; c < (collider *)buffer; ++c, ++cLocal){
			if(cInstantiate(c, cLocal) < 0){
				/** Memory allocation failure. **/
				while(c > obj->skeletonColliders){
					--c;
					cDelete(c);
				}
				memFree(obj->configuration);
				skliDelete(&obj->skeletonData);
				return -1;
			}
		}

	}else{
		obj->skeletonColliders = NULL;
	}

	// Set up the object's rigid body arrays.
	if(base->skeletonBodyNum > 0){

		const physRigidBodyBase *bodyBase = base->skeletonBodies;
		physRigidBody *bodyInstance;

		// Set the body ID array pointer and copy over the IDs.
		obj->skeletonBodyIDs = (boneIndex_t *)buffer;
		memcpy(buffer, base->skeletonBodyIDs, offset_body);

		do {

			bodyInstance = modulePhysicsRigidBodyAppend(&obj->skeletonBodies);
			if(
				bodyInstance == NULL ||
				physRigidBodyInstantiate(bodyInstance, bodyBase) < 0
			){
				/** Memory allocation failure. **/
				boneIndex_t id = obj->skeletonBodyNum;
				while(id > 0){
					modulePhysicsRigidBodyFree(&obj->skeletonBodies, obj->skeletonBodies);
					--id;
				}
				if(obj->skeletonColliders != NULL){
					collider *c = obj->skeletonColliders;
					while(c < (collider *)buffer){
						cDelete(c);
						++c;
					}
				}
				memFree(obj->configuration);
				skliDelete(&obj->skeletonData);
				return -1;
			}

			/** Figure out constraint loading. **/
			///bodyInstance->flags = PHYSICS_BODY_INACTIVE;
			///bodyInstance->constraintNum = base->skeletonJointNum[i];
			///bodyInstance->constraints = base->skeletonJoints[i];

			bodyBase = modulePhysicsRigidBodyBaseNext(bodyBase);

		} while(bodyBase != NULL);

		buffer += offset_body;

	}else{
		obj->skeletonBodyIDs = NULL;
	}

	// Allocate the animated skeleton instance.
	if(skliInit(&obj->skeletonData, base->skl, base->animationAllocate) < 0){
		/** Memory allocation failure. **/
		memFree(buffer);
		return -1;
	}

	// Instantiate each renderable.
	while(m < mLast){
		model *const resource = moduleModelAppend(&obj->models);
		if(resource == NULL){
			/** Memory allocation failure. **/
			memFree(buffer);
			skliDelete(&obj->skeletonData);
			return -1;
		}
		mdlInstantiate(resource, *m);
		++m;
	}

	obj->base = base;
	obj->skeletonBodyNum = base->skeletonBodyNum;
	obj->skeletonColliderNum = base->skeletonColliderNum;
	obj->stateMax = base->stateMax;
	obj->stateNum = 0;
	obj->state.previous = NULL;
	obj->oldestStatePrevious = &obj->state.previous;
	return 1;

}

return_t objNewModelFromBase(object *const __RESTRICT__ obj, const modelBase *const __RESTRICT__ mdl){
	// Allocate room for a new renderable and initialize it.
	model *const mdlNew = moduleModelAppend(&obj->models);
	if(mdlNew == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	mdlInstantiate(mdlNew, mdl);
	return 1;
}

/**return_t objInitSkeleton(object *const __RESTRICT__ obj, const skeleton *const __RESTRICT__ skl){
	bone *tempBuffer = memAllocate(2 * skl->boneNum * sizeof(bone));
	const bone *const bLast = &tempBuffer[skl->boneNum];
	if(tempBuffer == NULL){
		** Memory allocation failure. **
		return -1;
	}
	if(obj->configuration != NULL){
		memFree(obj->configuration);
	}
	if(obj->skeletonBodies != NULL){
		boneIndex_t id = obj->skeletonBodyNum;
		while(id > 0){
			modulePhysicsRigidBodyFree(&obj->skeletonBodies, obj->skeletonBodies);
			--id;
		}
		obj->skeletonBodyIDs = NULL;
		obj->skeletonBodies = NULL;
		obj->skeletonBodyNum = 0;
	}
	if(obj->skeletonColliders != NULL){
		free(obj->skeletonColliders);
	}
	obj->configuration = tempBuffer;
	obj->state.configuration = &tempBuffer[skl->boneNum];
	for(; tempBuffer < bLast; ++tempBuffer){
		tfInit(tempBuffer);
	}
	skliDelete(&obj->skeletonData);
	obj->skeletonData.skl = skl;
	return 1;
}**/

/**return_t objInitPhysics(object *obj){
	if(obj->skl != NULL){
		size_t i;
		physRigidBody *const tempBuffer = malloc(obj->skl->boneNum*sizeof(physRigidBody));
		if(tempBuffer == NULL){
			** Memory allocation failure. **
			return 0;
		}
		obj->skeletonBodies = tempBuffer;
		for(i = 0; i < obj->skl->boneNum; ++i){
			physRigidBodyInit(&obj->skeletonBodies[i]);
		}
	}
	return 1;
}**/

physRigidBody *objBoneGetPhysicsBody(const object *const __RESTRICT__ obj, const boneIndex_t boneID){

	// Finds the body attached to the
	// specified bone, if one exists.

	boneIndex_t i;
	const boneIndex_t *id = obj->skeletonBodyIDs;
	const boneIndex_t *const idLast = &id[obj->skeletonBodyNum];
	physRigidBody *body = obj->skeletonBodies;

	for(i = 0; i < boneID; ++i){
		if(body != NULL){
			if(*id == i){
				// Get the next body.
				++id;
				if(id >= idLast){
					body = NULL;
				}else{
					body = modulePhysicsRigidBodyNext(body);
				}
			}
		}else{
			return NULL;
		}
	}

	if(body != NULL && *id == boneID){
		return body;
	}

	return NULL;

}

void objPhysicsPrepare(object *const __RESTRICT__ obj){

	// Prepare all physics bodies.
	boneIndex_t i;
	sklNode *sklBone = obj->skeletonData.skl->bones;
	transform *configuration = obj->configuration;
	const boneIndex_t *id = obj->skeletonBodyIDs;
	const boneIndex_t *const idLast = &id[obj->skeletonBodyNum];
	physRigidBody *body = obj->skeletonBodies;

	// Configuration and animated bone state accumulators.
	/** Might have to use memAllocate here to prevent stack overflows, ~10 KB is a pretty significant chunk of the stack. **/
	transform accumulators[SKELETON_MAX_BONE_NUM<<1];
	transform *cAccumulator = &accumulators[0];
	transform *sAccumulator = &accumulators[obj->skeletonData.skl->boneNum];

	// Update the object's skeleton.
	for(i = 0; i < obj->skeletonData.skl->boneNum; ++i, ++sklBone, ++configuration, ++cAccumulator, ++sAccumulator){

		/** Split the root into a separate case. **/
		const unsigned int isRoot = (i == sklBone->parent) || (sklBone->parent >= obj->skeletonData.skl->boneNum);

		// Generate a new animated bone state.
		*sAccumulator = skliGenerateBoneState(&obj->skeletonData, i, sklBone->name, sklBone->defaultState);

		// Apply the parent's transformations to each bone.
		// Only do this if we are not on the root bone, of course.
		// We do, however, need to accumulate the configurations for every bone.
		if(!isRoot){
			*cAccumulator = tfAppend(accumulators[sklBone->parent], *configuration);
			*sAccumulator = tfAppend(accumulators[obj->skeletonData.skl->boneNum+sklBone->parent], *sAccumulator);
		}else{
			*cAccumulator = *configuration;
		}

		if(id < idLast && *id == i){

			if(physRigidBodyIsSimulated(body)){

				// Apply the accumulated bone state to the accumulated bone configurations.
				// Accumulating the transformations allows us to do everything in one loop
				// rather than splitting this into two separate ones.
				body->configuration = tfAppend(*cAccumulator, *sAccumulator);

				// Initialize the body's moment of inertia and centroid.
				physRigidBodyCentroidFromPosition(body);

			}
			if(physRigidBodyIsCollidable(body)){
				// Remember to add the body's collider to the island.
				body->flags |= PHYSICS_BODY_TRANSFORMED;
			}

			// Get the next body.
			++id;
			body = modulePhysicsRigidBodyNext(body);

		}

	}

}

void objPhysicsBodySimulate(object *const __RESTRICT__ obj, const boneIndex_t boneID){
	physRigidBody *body = objBoneGetPhysicsBody(obj, boneID);
	if(body != NULL){
		// Start at the animated bone state.
		body->configuration = obj->state.configuration[boneID];
		// Initialize the body's moment of inertia and centroid.
		physRigidBodyCentroidFromPosition(body);
		// Remember to add the body's collider to the island if necessary.
		if(physRigidBodyIsCollidable(body)){
			body->flags |= PHYSICS_BODY_TRANSFORMED;
		}
	}
}

void objPhysicsBodySuspend(object *const __RESTRICT__ obj, const boneIndex_t boneID){
	physRigidBody *body = objBoneGetPhysicsBody(obj, boneID);
	if(body != NULL){
		flagsUnset(body->flags, PHYSICS_BODY_SIMULATE);
	}
}

/**void objBoneScale(const object *const __RESTRICT__ obj, const boneIndex_t boneID, const vec3 scale){

	physRigidBody *const body = objBoneGetPhysicsBody(obj, boneID);
	if(body != NULL){
		body->configuration.scale = scale;
	}
	obj->configuration[boneID].scale = scale;

}**/

/**void objBoneSetPhysicsFlags(object *obj, const boneIndex_t boneID, const flags_t flags){

	if(obj->skeletonBodies != NULL){

		obj->skeletonBodies[boneID].flags = flags;

		if(flagsAreSet(flags, PHYSICS_BODY_SIMULATE)){

			// Replace PHYSICS_BODY_SIMULATE with PHYSICS_BODY_INITIALIZE.
			flagsUnset(obj->skeletonBodies[boneID].flags, PHYSICS_BODY_SIMULATE);
			flagsSet(obj->skeletonBodies[boneID].flags, PHYSICS_BODY_INITIALIZE);

		}

	}

}**/

sklAnim *objGetAnimation(const object *const __RESTRICT__ obj, const animIndex_t id){
	if(obj->base != NULL){
		if(obj->base->animationNum > id){
			return obj->base->animations[id];
		}
	}
	return NULL;
}

sklAnim *objFindAnimation(const object *const __RESTRICT__ obj, const char *const __RESTRICT__ name){
	if(obj->base != NULL){
		sklAnim **anim = obj->base->animations;
		sklAnim **const animLast = &anim[obj->base->animationNum];
		for(; anim < animLast; ++anim){
			if(strcmp((*anim)->name, name) == 0){
				return *anim;
			}
		}
	}
	return NULL;
}

/**__FORCE_INLINE__ void objSetAnimationType(object *obj, const animIndex_t slot, const flags_t additive){
	skliSetAnimationType(&obj->skeletonData, slot, additive);
}

__FORCE_INLINE__ return_t objChangeAnimation(object *obj, const animIndex_t slot, sklAnim *anim, const frameIndex_t frame, const float blendTime){
	return skliChangeAnimation(&obj->skeletonData, slot, anim, frame, blendTime);
}

__FORCE_INLINE__ void objClearAnimation(object *obj, const animIndex_t slot){
	skliClearAnimation(&obj->skeletonData, slot);
}

__FORCE_INLINE__ void objApplyLinearForce(object *obj, const boneIndex_t boneID, const vec3 *F){
	physRigidBodyApplyLinearForce(&obj->skeletonBodies[boneID], F);
}

__FORCE_INLINE__ void objApplyAngularForceGlobal(object *obj, const boneIndex_t boneID, const vec3 *F, const vec3 *r){
	physRigidBodyApplyAngularForceGlobal(&obj->skeletonBodies[boneID], F, r);
}

__FORCE_INLINE__ void objApplyForceGlobal(object *obj, const boneIndex_t boneID, const vec3 *F, const vec3 *r){
	physRigidBodyApplyForceGlobal(&obj->skeletonBodies[boneID], F, r);
}

void objApplyForceAtGlobalPoint(object *obj, const size_t boneID, const vec3 *F, const vec3 *r){
	physRigidBodyApplyForceAtGlobalPoint(&obj->skeletonBodies[boneID], F, r);
}
void objAddLinearVelocity(object *obj, const size_t boneID, const float x, const float y, const float z){
	physRigidBodyAddLinearVelocity(&obj->skeletonBodies[boneID], x, y, z);
}
void objApplyLinearImpulse(object *obj, const size_t boneID, const float x, const float y, const float z){
	physRigidBodyApplyLinearImpulse(&obj->skeletonBodies[boneID], x, y, z);
}
void objAddAngularVelocity(object *obj, const size_t boneID, const float angle, const float x, const float y, const float z){
	physRigidBodyAddAngularVelocity(&obj->skeletonBodies[boneID], angle, x, y, z);
}**/


return_t objTick(object *const __RESTRICT__ obj, const float dt_ms){

	boneIndex_t i;
	model *j;
	sklNode *sklBone = obj->skeletonData.skl->bones;
	transform *sklState = obj->state.configuration;
	transform *configuration = obj->configuration;
	const boneIndex_t *id = obj->skeletonBodyIDs;
	const boneIndex_t *const idLast = &id[obj->skeletonBodyNum];
	physRigidBody *body = obj->skeletonBodies;

	// Configuration and animated bone state accumulators.
	/** Might have to use memAllocate here to prevent stack overflows, ~10 KB is a pretty significant chunk of the stack. **/
	transform accumulators[SKELETON_MAX_BONE_NUM<<1];
	transform *cAccumulator = &accumulators[0];
	transform *sAccumulator = &accumulators[obj->skeletonData.skl->boneNum];

	// If we can create a new previous state, do so.
	if(obj->stateNum < obj->stateMax){
		if(objStateAllocate(&obj->oldestStatePrevious, &obj->skeletonData) < 0){
			return -1;
		}
		++obj->stateNum;
	}

	// Update each skeletal animation.
	skliTick(&obj->skeletonData, dt_ms, 1.f);

	// Update the object's skeleton.
	for(i = 0; i < obj->skeletonData.skl->boneNum; ++i, ++sklBone, ++sklState, ++configuration, ++cAccumulator, ++sAccumulator){

		// Update the previous states.
		objStateCopyBone(&obj->state, i);

		if(id < idLast && *id == i && physRigidBodyIsSimulated(body)){

			// Simulate the body attached to the bone.

			// Update the position from the centroid.
			physRigidBodyUpdateConfiguration(body);

			// Copy the new bone state, as modified
			// by the body, into the object.
			*configuration = body->configuration;
			*sklState = *configuration;

			// Update the accumulators.
			*cAccumulator = *configuration;
			*sAccumulator = tfIdentity();

			// Get the next body.
			++id;
			body = modulePhysicsRigidBodyNext(body);

		}else{

			// Apply animation transformations.

			/** Split the root into a separate case. **/
			const unsigned int isRoot = (i == sklBone->parent) || (sklBone->parent >= obj->skeletonData.skl->boneNum);

			// Generate a new animated bone state.
			*sAccumulator = skliGenerateBoneState(
				&obj->skeletonData, i, sklBone->name,
				sklBone->defaultState
			);

			// Apply the parent's transformations to each bone.
			// Only do this if we are not on the root bone, of course.
			// We do, however, need to accumulate the configurations for every bone.
			if(!isRoot){
				*cAccumulator = tfAppend(accumulators[sklBone->parent], *configuration);
				*sAccumulator = tfAppend(accumulators[obj->skeletonData.skl->boneNum+sklBone->parent], *sAccumulator);
			}else{
				*cAccumulator = *configuration;
			}

			// Apply the accumulated bone state to the accumulated bone configurations.
			// Accumulating the transformations allows us to do everything in one loop
			// rather than splitting this into two separate ones.
			*sklState = tfAppend(*cAccumulator, *sAccumulator);

			if(id < idLast && *id == i){
				// Copy the bone state over to the body.
				body->configuration = *sklState;
				// Initialize the body's moment of inertia and centroid.
				physRigidBodyCentroidFromPosition(body);
				// Get the next body.
				++id;
				body = modulePhysicsRigidBodyNext(body);
			}

		}

	}

	// Update each of the object's renderables.
	j = obj->models;
	while(j != NULL){
		mdlTick(j, dt_ms);
		j = moduleModelNext(j);
	}

	return 1;

}

void objBoneLastState(object *const __RESTRICT__ obj, const float dt_s){
	//
}

void objGenerateSprite(const object *const __RESTRICT__ obj, const model *const __RESTRICT__ mdl, const float interpT, const float *const __RESTRICT__ texFrag, vertex *const __RESTRICT__ vertices){

	// Generate the base sprite.
	const float left   = -0.5f;
	const float top    = -0.5f;
	const float right  =  0.5f;
	const float bottom =  0.5f;
	const float z      =  0.f;
	const transform *const current  = obj->state.configuration;
	const transform *const previous = (obj->state.previous == NULL ? current : obj->state.previous->configuration);
	transform tf;

	// Create the top left vertex.
	vertices[0].position.x = left;
	vertices[0].position.y = top;
	vertices[0].position.z = z;
	vertices[0].u = 0.f;
	vertices[0].v = 0.f;
	vec3ZeroP(&vertices[0].normal);
	vertices[0].bIDs[0] = -1;
	vertices[0].bIDs[1] = -1;
	vertices[0].bIDs[2] = -1;
	vertices[0].bIDs[3] = -1;
	memset(&vertices[0].bWeights[0], 0, sizeof(float)*VERTEX_MAX_BONES);

	// Create the top right vertex.
	vertices[1].position.x = right;
	vertices[1].position.y = top;
	vertices[1].position.z = z;
	vertices[1].u = 1.f;
	vertices[1].v = 0.f;
	vec3ZeroP(&vertices[0].normal);
	vertices[1].bIDs[0] = -1;
	vertices[1].bIDs[1] = -1;
	vertices[1].bIDs[2] = -1;
	vertices[1].bIDs[3] = -1;
	memset(&vertices[0].bWeights[0], 0, sizeof(float)*VERTEX_MAX_BONES);

	// Create the bottom left vertex.
	vertices[2].position.x = left;
	vertices[2].position.y = bottom;
	vertices[2].position.z = z;
	vertices[2].u = 0.f;
	vertices[2].v = -1.f;  // Flip the y dimension so the image isn't upside down.
	vec3ZeroP(&vertices[0].normal);
	vertices[2].bIDs[0] = -1;
	vertices[2].bIDs[1] = -1;
	vertices[2].bIDs[2] = -1;
	vertices[2].bIDs[3] = -1;
	memset(&vertices[0].bWeights[0], 0, sizeof(float)*VERTEX_MAX_BONES);

	// Create the bottom right vertex.
	vertices[3].position.x = right;
	vertices[3].position.y = bottom;
	vertices[3].position.z = z;
	vertices[3].u = 1.f;
	vertices[3].v = -1.f;  // Flip the y dimension so the image isn't upside down.
	vec3ZeroP(&vertices[0].normal);
	vertices[3].bIDs[0] = -1;
	vertices[3].bIDs[1] = -1;
	vertices[3].bIDs[2] = -1;
	vertices[3].bIDs[3] = -1;
	memset(&vertices[0].bWeights[0], 0, sizeof(float)*VERTEX_MAX_BONES);

	// Generate a transformation for the sprite and transform each vertex.
	/** Optimize? **/
	///tfInterpolate(&obj->skeletonState[1][0], &obj->skeletonState[0][0], interpT, &transform);
	tf = tfInterpolate(*previous, *current, interpT);
	tf.scale.x *= twiFrameWidth(&mdl->twi) * twiTextureWidth(&mdl->twi);
	tf.scale.y *= twiFrameHeight(&mdl->twi) * twiTextureHeight(&mdl->twi);
	vertTransform(&vertices[0], tf.position, tf.orientation, tf.scale);
	vertTransform(&vertices[1], tf.position, tf.orientation, tf.scale);
	vertTransform(&vertices[2], tf.position, tf.orientation, tf.scale);
	vertTransform(&vertices[3], tf.position, tf.orientation, tf.scale);

	// We can't pass unique textureFragment values for each individual sprite when batching. Therefore,
	// we have to do the offset calculations for each vertex UV here instead of in the shader.
	vertices[0].u = vertices[0].u * texFrag[2] + texFrag[0];
	vertices[0].v = vertices[0].v * texFrag[3] + texFrag[1];
	vertices[1].u = vertices[1].u * texFrag[2] + texFrag[0];
	vertices[1].v = vertices[1].v * texFrag[3] + texFrag[1];
	vertices[2].u = vertices[2].u * texFrag[2] + texFrag[0];
	vertices[2].v = vertices[2].v * texFrag[3] + texFrag[1];
	vertices[3].u = vertices[3].u * texFrag[2] + texFrag[0];
	vertices[3].v = vertices[3].v * texFrag[3] + texFrag[1];

}

gfxRenderGroup_t objRenderGroup(const object *const __RESTRICT__ obj, const float interpT){

	// Check if the object will have
	// any translucent renderables.

	float totalAlpha = 0.f;
	const model *i = obj->models;

	while(i != NULL){

		if(twiTranslucent(&i->twi)){

			// The object contains translucency.
			return GFX_RNDR_GROUP_TRANSLUCENT;

		// Treat dithered renderables as opaque.
		// The check should eventually be moved up here once rndrAlpha is removed.
		}else{/// if(flagsAreUnset(i->state.flags, RENDERABLE_STATE_ALPHA_DITHER)){

			/** We have to calculate the alpha again when rendering. Maybe avoid this? **/
			const float alpha = mdlStateAlpha(&i->state, interpT);
			if(alpha > 0.f && alpha < 1.f){
				// The object contains translucency.
				return GFX_RNDR_GROUP_TRANSLUCENT;
			}
			totalAlpha += alpha;

		}

		i = moduleModelNext(i);

	}

	if(totalAlpha <= 0.f){
		// The model is fully transparent.
		return GFX_RNDR_GROUP_UNKNOWN;
	}

	// The model is fully opaque.
	return GFX_RNDR_GROUP_OPAQUE;

}

void objRender(const object *const __RESTRICT__ obj, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float distance, const float interpT){

	const model *currentMdl = obj->models;
	boneIndex_t boneNum = obj->skeletonData.skl->boneNum;
	vec3 centroid = {.x = 0.f, .y = 0.f, .z = 0.f};

	// Generate a transformation matrix for each bone. Steps:
	//     1. Accumulate inverse bind offsets for the bone and each predecessor.
	//     2. Interpolate between the previous and current animation frames.
	//     3. Apply the accumulated inverse bind offsets.
	//     4. Write to the transform state array.
	//
	// The inverse bind offsets are used to convert the global skeleton state
	// used in updates to the local model space in preparation for rendering.
	//
	// Consider a rigged model in some reference position. When the bone
	// transformations are identity transforms, the model will maintain this
	// reference position correctly. However, we don't want the configuration
	// of every bone to be identity when we access them during updates, and
	// this would not work with physics objects (among other things). So before
	// updates we must transform them into global space based on their bind
	// positions, and then transform them back before passing their transforms
	// to the shader.
	if(boneNum > 0){

		boneIndex_t i = 0;

		mat4 *transformCurrent = gfxMngr->shdrData.skeletonTransformState;
		const transform *bCurrent = obj->state.configuration;
		const transform *bPrevious = (obj->state.previous == NULL ? bCurrent : obj->state.previous->configuration);

		transform state;
		sklNode *sklBone = obj->skeletonData.skl->bones;
		transform *bAccumulator = gfxMngr->shdrData.skeletonBindAccumulator;

		// Configuration and animated bone state accumulators.
		/** Might have to use memAllocate here to prevent stack overflows, ~5 KB is a pretty significant chunk of the stack. **/
		transform accumulators[SKELETON_MAX_BONE_NUM];
		transform *cAccumulator = &accumulators[0];

		///vec3 gfxDebugBonePositions[SKELETON_MAX_BONE_NUM];
		///boneIndex_t gfxDebugBoneParents[SKELETON_MAX_BONE_NUM];

		// Handle the root separately.
		state = tfInterpolate(*bPrevious, *bCurrent, interpT);
		centroid = state.position;

		///gfxDebugBonePositions[i] = state.position;
		///gfxDebugBoneParents[i] = 0;

		// Calculate the root inverse bind pose.
		/// Originally used the old tfInverse function.
		*bAccumulator = tfInverse(sklBone->defaultState);
		bAccumulator->position = quatRotateVec3FastApproximate(bAccumulator->orientation, bAccumulator->position);

		// Accumulate the object configurations and undo the configuration transformation.
		// Bones cannot fully represent an arbitrary configuration state, so we need to
		// use matrices. We still want the global bone positions during updates, though.
		*cAccumulator = obj->configuration[i];
		state = tfPrepend(tfInverse(*cAccumulator), state);

		// Compute the global bone states. Then add the inverse bind offsets
		// and convert it to a transformation matrix for the shader.
		*transformCurrent = mat4MMultM(
			mat4MMultM(tfMatrix(*cAccumulator), tfMatrix(state)),
			tfMatrix(*bAccumulator)
		);

		// Handle the rest of the bones.
		while(i < boneNum){

			++bCurrent, ++bPrevious, ++transformCurrent, ++sklBone, ++bAccumulator, ++cAccumulator, ++i;

			// Add the parent's inverse bind position.
			/// Originally used the old tfInverse function.
			state = tfInverse(sklBone->defaultState);
			state.position = quatRotateVec3FastApproximate(state.orientation, state.position);
			*bAccumulator = tfAppend(state, gfxMngr->shdrData.skeletonBindAccumulator[sklBone->parent]);

			// Interpolate between bone states.
			state = tfInterpolate(*bPrevious, *bCurrent, interpT);

			///gfxDebugBonePositions[i] = state.position;
			///gfxDebugBoneParents[i] = sklBone->parent;

			// Accumulate the object configurations and undo the configuration transformation.
			// Bones cannot fully represent an arbitrary configuration state, so we need to
			// use matrices. We still want the global bone positions during updates, though.
			*cAccumulator = tfAppend(accumulators[sklBone->parent], obj->configuration[i]);
			state = tfPrepend(tfInverse(*cAccumulator), state);

			// Compute the global bone states. Then add the inverse bind offsets
			// and convert it to a transformation matrix for the shader.
			*transformCurrent = mat4MMultM(
				mat4MMultM(tfMatrix(*cAccumulator), tfMatrix(state)),
				tfMatrix(*bAccumulator)
			);

		}

		// Draw the skeleton for debugging.
		///gfxDebugDrawSkeleton(
		///	gfxDebugBonePositions, gfxDebugBoneParents, boneNum,
		///	gfxDebugInfoInit(GL_LINE, vec3New(0.f, 1.f, 0.f)), &cam->viewProjectionMatrix
		///);

	}

	// Draw each renderable.
	while(currentMdl != NULL){
		mdlRender(currentMdl, obj->skeletonData.skl, gfxMngr, cam, distance, centroid, interpT);
		currentMdl = moduleModelNext(currentMdl);
	}

}