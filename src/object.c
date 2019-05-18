#include "object.h"
#include "memoryManager.h"
#include "helpersMath.h"
#include "helpersFileIO.h"
#include "inline.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/****/
#include "moduleTextureWrapper.h"
#include "moduleSkeleton.h"
#include "moduleModel.h"
#include "moduleRenderable.h"
#include "modulePhysics.h"

#define OBJECT_RESOURCE_DIRECTORY_STRING "Resources"FILE_PATH_DELIMITER_STRING"Objects"FILE_PATH_DELIMITER_STRING
#define OBJECT_RESOURCE_DIRECTORY_LENGTH 18

void objBaseInit(objectBase *const restrict base){
	base->name = NULL;
	base->skl = NULL;
	base->animationMax = 0;
	base->animationNum = 0;
	base->animations = NULL;
	base->skeletonBodies = NULL;
	base->skeletonColliders = NULL;
	base->renderables = 0;
	base->stateMax = 0;
}

return_t objBaseLoad(objectBase *const restrict base, const char *const restrict prgPath, const char *const restrict filePath){

	char fullPath[FILE_MAX_PATH_LENGTH];
	const size_t fileLength = strlen(filePath);

	FILE *restrict objInfo;

	objBaseInit(base);

	fileGenerateFullPath(fullPath, prgPath, strlen(prgPath), OBJECT_RESOURCE_DIRECTORY_STRING, OBJECT_RESOURCE_DIRECTORY_LENGTH, filePath, fileLength);
	objInfo = fopen(fullPath, "r");

	if(objInfo != NULL){

		char lineFeed[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		fileLine_t currentLine = 0;  // Current file line being read.

		char loadPath[FILE_MAX_PATH_LENGTH];

		while(fileParseNextLine(objInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			++currentLine;

			// Skeleton
			if(lineLength >= 10 && strncmp(line, "skeleton ", 9) == 0){

				skeleton *tempSkl;
				fileParseResourcePath(&loadPath[0], NULL, line, lineLength, 9);

				// Check if the skeleton has already been loaded.
				tempSkl = moduleSkeletonFind(&loadPath[0]);
				if(tempSkl != NULL){
					base->skl = tempSkl;

				// If the skeleton path is surrounded by quotes, try and load it.
				}else{
					tempSkl = moduleSkeletonAllocate();
					if(tempSkl != NULL){
						const return_t r = sklLoad(tempSkl, prgPath, &loadPath[0]);
						if(r < 1){
							// The load failed. Clean up.
							moduleSkeletonFree(tempSkl);
							if(r < 0){
								/** Memory allocation failure. **/
								objBaseDelete(base);
								fclose(objInfo);
								return -1;
							}
							printf("Error loading object \"%s\": Skeleton \"%s\" at line %u does not exist.\n", fullPath, &loadPath[0], currentLine);
							base->skl = moduleSkeletonGetDefault();
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
					fileParseResourcePath(&loadPath[0], NULL, line, lineLength, 16);

					// Load the rigid body from a file.
					r = physRigidBodyBaseLoad(&base->skeletonBodies, base->skl, prgPath, loadPath);

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
				fileParseResourcePath(&loadPath[0], NULL, line, lineLength, 10);

				// Check if the animation has already been loaded.
				tempSkla = moduleSkeletonAnimationFind(&loadPath[0]);

				// If the animation path is surrounded by quotes, try and load it.
				if(tempSkla == NULL){
					tempSkla = moduleSkeletonAnimationAllocate();
					if(tempSkla != NULL){
						const return_t r = sklaLoad(tempSkla, prgPath, &loadPath[0]);
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
					printf("Error loading object \"%s\": Skeletal animation \"%s\" at line %u does not exist.\n", fullPath, &loadPath[0], currentLine);
				}


			// Animation capacity
			}else if(lineLength >= 20 && strncmp(line, "animationCapacity ", 19) == 0){
				base->animationMax = strtoul(line+19, NULL, 0);


			// Renderable
			}else if(lineLength >= 14 && strncmp(line, "renderable ", 11) == 0){

				/** Load model and texture. **/
				renderable rndr;
				renderable *rndrNew;

				size_t mdlPathBegin = 11;
				size_t mdlPathLength = 0;
				const char *mdlSecondQuote = NULL;

				rndrInit(&rndr);

				// Get the model path.
				if(line[mdlPathBegin] == '"'){
					++mdlPathBegin;
					mdlSecondQuote = strchr(line+mdlPathBegin, '"');
					if(mdlSecondQuote == NULL){
						mdlSecondQuote = strchr(line+mdlPathBegin, ' ');
					}
				}else{
					mdlSecondQuote = strchr(line+mdlPathBegin, ' ');
				}
				if(mdlSecondQuote != NULL){
					mdlPathLength = mdlSecondQuote-&line[mdlPathBegin];
				}

				if(mdlPathLength > 0){

					model *tempMdl = NULL;

					size_t texPathBegin = mdlPathBegin+mdlPathLength+1;
					size_t texPathLength = 0;
					const char *texSecondQuote = NULL;
					char *delimiter;

					// Get the texture wrapper path.
					if(line[texPathBegin] == ' '){
						++texPathBegin;
					}
					if(line[texPathBegin] == '"'){
						++texPathBegin;
						texSecondQuote = strchr(line+texPathBegin, '"');
						if(texSecondQuote == NULL){
							texSecondQuote = strchr(line+texPathBegin, ' ');
						}
					}else{
						texSecondQuote = line+lineLength;
					}
					if(texSecondQuote != NULL){
						texPathLength = texSecondQuote-line-texPathBegin;
					}

					// Load the model.
					strncpy(&loadPath[0], line+mdlPathBegin, mdlPathLength);
					loadPath[mdlPathLength] = '\0';

					delimiter = loadPath;
					while(*delimiter != '\0'){
						if(*delimiter == FILE_PATH_DELIMITER_CHAR_UNUSED){
							*delimiter = FILE_PATH_DELIMITER_CHAR;
						}
						++delimiter;
					}

					// Check if the model has already been loaded.
					tempMdl = moduleModelFind(&loadPath[0]);
					if(tempMdl != NULL){
						rndr.mdl = tempMdl;

					// If the model path is surrounded by quotes, try and load it.
					}else{
						tempMdl = moduleModelAllocate();
						if(tempMdl != NULL){
							const return_t r = mdlLoad(tempMdl, prgPath, &loadPath[0]);
							if(r < 0){
								/** Memory allocation failure. **/
								moduleModelFree(tempMdl);
								objBaseDelete(base);
								fclose(objInfo);
								return -1;
							}else if(r > 0){
								rndr.mdl = tempMdl;
							}
						}else{
							/** Memory allocation failure. **/
							objBaseDelete(base);
							fclose(objInfo);
							return -1;
						}
					}
					// Use the default model if no model was loaded.
					if(rndr.mdl == NULL){
						printf("Error loading object \"%s\": Model \"%s\" at line %u does not exist.\n", fullPath, &loadPath[0], currentLine);
						rndr.mdl = moduleModelGetDefault();
					}

					if(texPathLength > 0){

						textureWrapper *tempTw = NULL;

						// Load the texture wrapper.
						strncpy(&loadPath[0], line+texPathBegin, texPathLength);
						loadPath[texPathLength] = '\0';

						delimiter = loadPath;
						while(*delimiter != '\0'){
							if(*delimiter == FILE_PATH_DELIMITER_CHAR_UNUSED){
								*delimiter = FILE_PATH_DELIMITER_CHAR;
							}
							++delimiter;
						}

						// Check if the texture wrapper has already been loaded.
						tempTw = moduleTextureWrapperFind(&loadPath[0]);
						if(tempTw != NULL){
							rndr.tw = tempTw;

						// If the texture wrapper path is surrounded by quotes, try and load it.
						}else{
							tempTw = moduleTextureWrapperAllocate();
							if(tempTw != NULL){
								const return_t r = twLoad(tempTw, prgPath, &loadPath[0]);
								if(r < 0){
									/** Memory allocation failure. **/
									moduleTextureWrapperFree(tempTw);
									objBaseDelete(base);
									fclose(objInfo);
									return -1;
								}else if(r > 0){
									rndr.tw = tempTw;
								}
							}else{
								/** Memory allocation failure. **/
								objBaseDelete(base);
								fclose(objInfo);
								return -1;
							}
						}
						// Use the default texture wrapper if no texture wrapper was loaded.
						if(rndr.tw == NULL){
							printf("Error loading object \"%s\": Texture wrapper \"%s\" at line %u does not exist.\n", fullPath, &loadPath[0], currentLine);
							rndr.tw = moduleTextureWrapperGetDefault();
						}

					}else{
						printf("Error loading object \"%s\": Could not parse texture wrapper for renderable at line %u.\n", fullPath, currentLine);
						rndr.tw = moduleTextureWrapperGetDefault();
					}

				}else{
					printf("Error loading object \"%s\": Could not parse model for renderable at line %u.\n", fullPath, currentLine);
					rndr.mdl = moduleModelGetDefault();
					rndr.tw = moduleTextureWrapperGetDefault();
				}

				// Add the renderable.
				rndrNew = moduleRenderableAppend(&base->renderables);
				if(rndrNew == NULL){
					/** Memory allocation failure. **/
					objBaseDelete(base);
					fclose(objInfo);
					return -1;
				}
				*rndrNew = rndr;

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
		base->skl = moduleSkeletonGetDefault();
	}

	// If no renderables were loaded, load the default one.
	if(base->renderables == NULL){
		printf("Error loading object \"%s\": No renderables were loaded.\n", fullPath);
		moduleRenderableAppend(&base->renderables);
		if(base->renderables == NULL){
			/** Memory allocation failure. **/
			objBaseDelete(base);
			return -1;
		}
		base->renderables->mdl = moduleModelGetDefault();
		base->renderables->tw = moduleTextureWrapperGetDefault();
	}

	// Generate a name based off the file path.
	base->name = fileGenerateResourceName(filePath, fileLength);
	if(base->name == NULL){
		/** Memory allocation failure. **/
		objBaseDelete(base);
		return -1;
	}

	return 1;

}

void objBaseDelete(objectBase *const restrict base){
	if(base->name != NULL){
		memFree(base->name);
	}
	if(base->animations != NULL){
		memFree(base->animations);
	}
	if(base->skl != NULL){
		if(base->skeletonBodies != NULL){
			modulePhysicsRigidBodyBaseFreeArray(&base->skeletonBodies);
		}
		if(base->skeletonColliders != NULL){
			collider *c = base->skeletonColliders;
			const collider *const cLast = &c[base->skl->boneNum];
			for(; c < cLast; ++c){
				cDelete(c);
			}
			free(base->skeletonColliders);
		}
	}
	if(base->renderables != NULL){
		moduleRenderableFreeArray(&base->renderables);
	}
}

return_t objInit(object *const restrict obj){
	obj->base = NULL;
	obj->configuration = NULL;
	obj->stateMax = 0;
	obj->stateNum = 0;
	obj->state.skeleton = NULL;
	obj->state.previous = NULL;
	obj->oldestStatePrevious = &obj->state.previous;
	obj->skeletonBodies = NULL;
	obj->skeletonColliders = NULL;
	obj->renderables = NULL;
	return skliInit(&obj->skeletonData, NULL, 0);
}

void objDelete(object *const restrict obj){
	objState *state = obj->state.previous;
	if(obj->configuration != NULL){
		memFree(obj->configuration);
	}
	while(state != NULL){
		objState *next = state->previous;
		memFree(state);
		state = next;
	}
	if(obj->skeletonBodies != NULL){
		modulePhysicsRigidBodyFreeArray(&obj->skeletonBodies);
	}
	if(obj->skeletonColliders != NULL){
		collider *c = obj->skeletonColliders;
		const collider *const cLast = &c[obj->skeletonData.skl->boneNum];
		for(; c < cLast; ++c){
			cDelete(c);
		}
		free(obj->skeletonColliders);
	}
	if(obj->renderables != NULL){
		moduleRenderableInstanceFreeArray(&obj->renderables);
	}
	skliDelete(&obj->skeletonData);
}

return_t objInstantiate(object *const restrict obj, const objectBase *const restrict base){

	renderable *j;

	if(skliInit(&obj->skeletonData, base->skl, base->animationMax) < 0){
		/** Memory allocation failure. **/
		return -1;
	}

	if(base->skl->boneNum != 0){

		// Allocate memory for the object instance.
		obj->configuration = memAllocate(3 * base->skl->boneNum * sizeof(bone));
		if(obj->configuration == NULL){
			/** Memory allocation failure. **/
			skliDelete(&obj->skeletonData);
			return -1;
		}
		obj->state.skeleton = &obj->configuration[base->skl->boneNum];

		// Allocate memory for and initialize the colliders if necessary.
		if(base->skeletonColliders != NULL){

			collider *cLocal = base->skeletonColliders;
			collider *c = obj->skeletonColliders;
			const collider *const cLast = &c[base->skl->boneNum];

			obj->skeletonColliders = malloc(base->skl->boneNum * sizeof(collider));
			if(obj->skeletonColliders == NULL){
				/** Memory allocation failure. **/
				memFree(obj->configuration);
				skliDelete(&obj->skeletonData);
				return -1;
			}

			for(; c < cLast; ++c, ++cLocal){
				if(cInstantiate(c, cLocal) < 0){
					/** Memory allocation failure. **/
					while(c > obj->skeletonColliders){
						--c;
						cDelete(c);
					}
					free(obj->skeletonColliders);
					memFree(obj->configuration);
					skliDelete(&obj->skeletonData);
					return -1;
				}
			}

		}else{
			obj->skeletonColliders = NULL;
		}

		{
			bone *b = obj->configuration;
			const bone *const bLast = &b[base->skl->boneNum];
			// Initialize each bone.
			for(; b < bLast; ++b){
				boneInit(b);
			}
		}

		// Allocate memory for and initialize the rigid bodies if necessary.
		if(base->skeletonBodies != NULL){

			physRigidBodyBase *bodyBase = base->skeletonBodies;
			physRigidBody *bodyInstance;

			do {

				bodyInstance = modulePhysicsRigidBodyAppend(&obj->skeletonBodies);
				if(
					bodyInstance == NULL ||
					physRigidBodyInstantiate(bodyInstance, bodyBase/*, &obj->state.skeleton[bodyBase->id]*/) < 0
				){
					/** Memory allocation failure. **/
					memFree(obj->configuration);
					skliDelete(&obj->skeletonData);
					modulePhysicsRigidBodyFreeArray(&obj->skeletonBodies);
					return -1;
				}

				/** Figure out constraint loading. **/
				///bodyInstance->flags = PHYSICS_BODY_INACTIVE;
				///bodyInstance->constraintNum = base->skeletonJointNum[i];
				///bodyInstance->constraints = base->skeletonJoints[i];

				bodyBase = modulePhysicsRigidBodyBaseNext(bodyBase);

			} while(bodyBase != NULL);

		}else{
			obj->skeletonBodies = NULL;
		}

	}

	// Instantiate each renderable.
	j = base->renderables;
	while(j != NULL){
		rndrInstance *const resource = moduleRenderableInstanceAppend(&obj->renderables);
		if(resource == NULL){
			/** Memory allocation failure. **/
			objDelete(obj);
			return -1;
		}
		rndriInstantiate(resource, j);
		j = moduleRenderableNext(j);
	}

	obj->base = base;
	obj->stateMax = base->stateMax;
	obj->stateNum = 0;
	obj->state.previous = NULL;
	obj->oldestStatePrevious = &obj->state.previous;
	return 1;

}

static __FORCE_INLINE__ return_t objStateAllocate(object *const restrict obj){
	objState *const state = memAllocate(sizeof(objState) + obj->skeletonData.skl->boneNum * sizeof(bone));
	if(state != NULL){
		state->skeleton = (bone *)((byte_t *)state + sizeof(objState));
		state->previous = NULL;
		*obj->oldestStatePrevious = state;
		obj->oldestStatePrevious = &state->previous;
		++obj->stateNum;
		return 1;
	}
	return -1;
}

return_t objStatePreallocate(object *const restrict obj){
	while(obj->stateNum < obj->stateMax){
		if(objStateAllocate(obj) < 0){
			return -1;
		}
	}
	return 1;
}

static __FORCE_INLINE__ void objStateCopyBone(object *const restrict obj, const boneIndex_t i){

	objState *state = &obj->state;
	bone last = state->skeleton[i];

	while(state->previous != NULL){

		const bone swap = state->previous->skeleton[i];
		state->previous->skeleton[i] = last;
		last = swap;
		state = state->previous;

	}

}

return_t objNewRenderable(object *const restrict obj, model *const restrict mdl, textureWrapper *const restrict tw){
	/*
	** Allocate room for a new renderable and initialize it.
	*/
	rndrInstance *const rndr = moduleRenderableInstanceAppend(&obj->renderables);
	if(rndr == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	rndr->mdl = mdl;
	twiInit(&rndr->twi, tw);
	return 1;
}

return_t objNewRenderableFromBase(object *const restrict obj, const renderable *const restrict rndr){
	/*
	** Allocate room for a new renderable and initialize it.
	*/
	rndrInstance *const rndrNew = moduleRenderableInstanceAppend(&obj->renderables);
	if(rndrNew == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	rndriInstantiate(rndrNew, rndr);
	return 1;
}

return_t objNewRenderableFromInstance(object *const restrict obj, const rndrInstance *rndr){
	/*
	** Allocate room for a new renderable and initialize it.
	*/
	rndrInstance *const rndrNew = moduleRenderableInstanceAppend(&obj->renderables);
	if(rndrNew == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	rndrNew->mdl = rndr->mdl;
	twiInit(&rndrNew->twi, rndr->twi.tw);
	return 1;
}

return_t objInitSkeleton(object *const restrict obj, const skeleton *const restrict skl){
	bone *tempBuffer = memAllocate(3 * skl->boneNum * sizeof(bone));
	const bone *const bLast = &tempBuffer[skl->boneNum];
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	if(obj->configuration != NULL){
		memFree(obj->configuration);
	}
	if(obj->skeletonBodies != NULL){
		modulePhysicsRigidBodyFreeArray(&obj->skeletonBodies);
	}
	if(obj->skeletonColliders != NULL){
		free(obj->skeletonColliders);
	}
	obj->configuration = tempBuffer;
	obj->state.skeleton = &tempBuffer[skl->boneNum];
	for(; tempBuffer < bLast; ++tempBuffer){
		boneInit(tempBuffer);
	}
	skliDelete(&obj->skeletonData);
	obj->skeletonData.skl = skl;
	return 1;
}

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
}

void objBoneSetPhysicsFlags(object *obj, const boneIndex_t boneID, const flags_t flags){

	if(obj->skeletonBodies != NULL){

		obj->skeletonBodies[boneID].flags = flags;

		if(flagsAreSet(flags, PHYSICS_BODY_SIMULATE)){

			// Replace PHYSICS_BODY_SIMULATE with PHYSICS_BODY_INITIALIZE.
			flagsUnset(obj->skeletonBodies[boneID].flags, PHYSICS_BODY_SIMULATE);
			flagsSet(obj->skeletonBodies[boneID].flags, PHYSICS_BODY_INITIALIZE);

		}

	}

}**/

sklAnim *objGetAnimation(const object *const restrict obj, const animIndex_t id){
	if(obj->base != NULL){
		if(obj->base->animationNum > id){
			return obj->base->animations[id];
		}
	}
	return NULL;
}

sklAnim *objFindAnimation(const object *const restrict obj, const char *const restrict name){
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

return_t objUpdate(object *const restrict obj, physIsland *const restrict island, const float elapsedTime, const float dt){

	boneIndex_t i;
	rndrInstance *j;
	sklNode *sklBone = obj->skeletonData.skl->bones;
	bone *sklState = obj->state.skeleton;
	bone *configuration = obj->configuration;
	physRigidBody *body = obj->skeletonBodies;

	// If we can create a new previous state, do so.
	if(obj->stateNum < obj->stateMax){
		if(objStateAllocate(obj) < 0){
			return -1;
		}
	}

	// Update each skeletal animation.
	skliUpdateAnimations(&obj->skeletonData, elapsedTime, 1.f);

	// Update the object's skeleton.
	for(i = 0; i < obj->skeletonData.skl->boneNum; ++i, ++sklBone, ++sklState, ++configuration){

		const unsigned int isRoot = (i == sklBone->parent) || (sklBone->parent >= obj->skeletonData.skl->boneNum);

		// Update the previous states.
		objStateCopyBone(obj, i);

		if(body != NULL && body->base->id == i && physRigidBodyIsSimulated(body)){

			/*
			** Simulate the body attached to the bone.
			*/

			/** TEMPORARILY ADD GRAVITY. **/
			const vec3 gravity = {.x = 0.f, .y = -9.80665f * body->mass, .z = 0.f};
			physRigidBodyApplyLinearForce(body, gravity);

			if(physRigidBodyIsUninitialized(body)){

				// Generate a new animated bone state.
				// This will later be copied into sklState.
				body->configuration = *configuration;
				skliGenerateBoneState(&obj->skeletonData, i, sklBone->name, &body->configuration);

				// Initialize the body's moment of inertia and centroid.
				physRigidBodyPrepare(body);

				#if !defined PHYSICS_MODULARIZE_SOLVER && !defined PHYSICS_GAUSS_SEIDEL_SOLVER

				// Only integrate half of the first step
				// for the leapfrog integration scheme.
				physRigidBodyIntegrateVelocity(body, dt*0.5f);

				#endif

				// Remove the body's "uninitialized" flag.
				physRigidBodySetInitialized(body);

			}else{

				#if !defined PHYSICS_MODULARIZE_SOLVER && !defined PHYSICS_GAUSS_SEIDEL_SOLVER

				// Integrate the body's configuration.
				// Done here for the sake of any child bones.
				physRigidBodyIntegrateConfiguration(body, dt);

				// Integrate the body's velocities.
				physRigidBodyIntegrateVelocity(body, dt);

				#endif

				// Update the position from the centroid.
				physRigidBodyUpdateConfiguration(body);

			}

			// Copy the new bone state, as modified
			// by the body, into the object.
			*sklState = body->configuration;

			// Add the body to the physics island
			// and update all of its colliders.
			if(physRigidBodyUpdateColliders(body, island) < 0){
				/** Memory allocation failure. **/
				return -1;
			}

			body = modulePhysicsRigidBodyNext(body);

		}else{

			/*
			** Apply animation transformations.
			*/

			// Generate a new animated bone state.
			/** Should configurations be optional? **/
			*sklState = *configuration;
			skliGenerateBoneState(&obj->skeletonData, i, sklBone->name, sklState);

			// Apply the object skeleton's bind offsets.
			sklState->position = boneTransformAppendPositionVec(
				*sklState,
				sklBone->defaultState.position.x,
				sklBone->defaultState.position.y,
				sklBone->defaultState.position.z
			);

			// Apply the parent's transformations to each bone.
			if(!isRoot){
				*sklState = boneTransformAppend(obj->state.skeleton[sklBone->parent], *sklState);
			}

			if(body != NULL && body->base->id == i){

				// Copy the bone state over to the body.
				body->configuration = *sklState;

				// Update the body's colliders.
				if(physRigidBodyUpdateColliders(body, island) < 0){
					/** Memory allocation failure. **/
					return -1;
				}
				body = modulePhysicsRigidBodyNext(body);

			}

		}

		// Apply billboarding to the root bone if necessary.
		//if(isRoot && flagsAreSet(obj->tempRndrConfig.flags, (RNDR_BILLBOARD_X | RNDR_BILLBOARD_Y | RNDR_BILLBOARD_Z))){
			/** Should work with physics objects? **/
			/** Actually, do this before rendering. **/
			/**
			// If any of the flags apart from RNDR_BILLBOARD_TARGET are set, continue.
			mat4 billboardRotation;
			if(flagsAreSet(rc->flags, RNDR_BILLBOARD_SPRITE)){
				// Use a less accurate but faster method for billboarding.
				vec3 right, up, forward;
				// Use the camera's X, Y and Z axes for cheap sprite billboarding.
				vec3Set(&right,   cam->viewMatrix.m[0][0], cam->viewMatrix.m[0][1], cam->viewMatrix.m[0][2]);
				vec3Set(&up,      cam->viewMatrix.m[1][0], cam->viewMatrix.m[1][1], cam->viewMatrix.m[1][2]);
				vec3Set(&forward, cam->viewMatrix.m[2][0], cam->viewMatrix.m[2][1], cam->viewMatrix.m[2][2]);
				// Lock certain axes if needed.
				if(flagsAreUnset(rc->flags, RNDR_BILLBOARD_X)){
					right.y   = 0.f;
					up.y      = 1.f;
					forward.y = 0.f;
				}
				if(flagsAreUnset(rc->flags, RNDR_BILLBOARD_Y)){
					right.x   = 1.f;
					up.x      = 0.f;
					forward.x = 0.f;
				}
				if(flagsAreUnset(rc->flags, RNDR_BILLBOARD_Z)){
					right.z   = 0.f;
					up.z      = 0.f;
					forward.z = 1.f;
				}
				billboardRotation.m[0][0] = right.x; billboardRotation.m[0][1] = up.x; billboardRotation.m[0][2] = forward.x; billboardRotation.m[0][3] = 0.f;
				billboardRotation.m[1][0] = right.y; billboardRotation.m[1][1] = up.y; billboardRotation.m[1][2] = forward.y; billboardRotation.m[1][3] = 0.f;
				billboardRotation.m[2][0] = right.z; billboardRotation.m[2][1] = up.z; billboardRotation.m[2][2] = forward.z; billboardRotation.m[2][3] = 0.f;
				billboardRotation.m[3][0] = 0.f;     billboardRotation.m[3][1] = 0.f;  billboardRotation.m[3][2] = 0.f;       billboardRotation.m[3][3] = 1.f;
			}else{
				vec3 eye, target, up;
				if(flagsAreSet(rc->flags, RNDR_BILLBOARD_TARGET)){
					eye = rc->target.render;
					target = rc->position.render;
					vec3Set(&up, 0.f, 1.f, 0.f);
					quatRotateVec3(&rc->targetOrientation.render, &up);
				}else if(flagsAreSet(rc->flags, RNDR_BILLBOARD_TARGET_CAMERA)){
					eye = cam->position.render;
					target = rc->position.render;
					up = cam->up.render;
				}else{
					eye = cam->position.render;
					target = cam->target.render;
					up = cam->up.render;
				}
				// Lock certain axes if needed.
				if(flagsAreUnset(rc->flags, RNDR_BILLBOARD_X)){
					target.y = eye.y;
				}
				if(flagsAreUnset(rc->flags, RNDR_BILLBOARD_Y)){
					target.x = eye.x;
				}
				if(flagsAreUnset(rc->flags, RNDR_BILLBOARD_Z)){
					vec3Set(&up, 0.f, 1.f, 0.f);
				}
				mat4RotateToFace(&billboardRotation, &eye, &target, &up);
			}
			mat4MultMByM2(&billboardRotation, transformMatrix);  // Apply billboard rotation
			**/
		//}

	}

	// Update each of the object's renderables.
	j = obj->renderables;
	while(j != NULL){
		rndriUpdate(j, elapsedTime);
		j = moduleRenderableInstanceNext(j);
	}

	return 1;

}

void objBoneLastState(object *const restrict obj, const float dt){
	//
}

gfxRenderGroup_t objRenderGroup(const object *const restrict obj, const float interpT){

	/*
	** Check if the object will have
	** any translucent renderables.
	*/

	float totalAlpha = 0.f;
	const rndrInstance *i = obj->renderables;

	while(i != NULL){

		if(twiContainsTranslucency(&i->twi)){

			// The object contains translucency.
			return GFX_RENDER_GROUP_TRANSLUCENT;

		}else{

			const float alpha = floatLerp(i->alphaPrevious, i->alpha, interpT);
			if(alpha > 0.f && alpha < 1.f){
				// The object contains translucency.
				return GFX_RENDER_GROUP_TRANSLUCENT;
			}
			totalAlpha += alpha;

		}

		i = moduleRenderableInstanceNext(i);

	}

	if(totalAlpha == 0.f){
		// The model is fully transparent.
		return GFX_RENDER_GROUP_UNKNOWN;
	}

	// The model is fully opaque.
	return GFX_RENDER_GROUP_OPAQUE;

}

void objGenerateSprite(const object *const restrict obj, const rndrInstance *const restrict rndr, const float interpT, const float *const restrict texFrag, vertex *const restrict vertices){

	/*
	** Generate the base sprite.
	*/
	const float left   = -0.5f; /// - obj->tempRndrConfig.pivot.render.x;
	const float top    = -0.5f; /// - obj->tempRndrConfig.pivot.render.y;
	const float right  =  0.5f; /// - obj->tempRndrConfig.pivot.render.x;
	const float bottom =  0.5f; /// - obj->tempRndrConfig.pivot.render.y;
	const float z      =  0.f;  /// - obj->tempRndrConfig.pivot.render.z;
	const bone *const current  = obj->state.skeleton;
	const bone *const previous = (obj->state.previous == NULL ? current : obj->state.previous->skeleton);
	bone transform;

	// Create the top left vertex.
	vertices[0].position.x = left;
	vertices[0].position.y = top;
	vertices[0].position.z = z;
	vertices[0].u = 0.f;
	vertices[0].v = 0.f;
	vec3Zero(&vertices[0].normal);
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
	vec3Zero(&vertices[0].normal);
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
	vec3Zero(&vertices[0].normal);
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
	vec3Zero(&vertices[0].normal);
	vertices[3].bIDs[0] = -1;
	vertices[3].bIDs[1] = -1;
	vertices[3].bIDs[2] = -1;
	vertices[3].bIDs[3] = -1;
	memset(&vertices[0].bWeights[0], 0, sizeof(float)*VERTEX_MAX_BONES);

	/*
	** Generate a transformation for the sprite and transform each vertex.
	*/
	/** Optimize? **/
	//boneInterpolate(&obj->skeletonState[1][0], &obj->skeletonState[0][0], interpT, &transform);
	transform = boneInterpolate(*previous, *current, interpT);
	transform.scale.x *= twiGetFrameWidth(&rndr->twi) * twiGetTexWidth(&rndr->twi);
	transform.scale.y *= twiGetFrameHeight(&rndr->twi) * twiGetTexHeight(&rndr->twi);
	vertTransform(&vertices[0], transform.position, transform.orientation, transform.scale);
	vertTransform(&vertices[1], transform.position, transform.orientation, transform.scale);
	vertTransform(&vertices[2], transform.position, transform.orientation, transform.scale);
	vertTransform(&vertices[3], transform.position, transform.orientation, transform.scale);

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
