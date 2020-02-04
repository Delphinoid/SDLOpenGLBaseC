#include "graphicsManager.h"
#include "object.h"
#include "texture.h"
#include "model.h"
#include "renderable.h"
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
#include "moduleRenderable.h"
#include "modulePhysics.h"

#define OBJECT_RESOURCE_DIRECTORY_STRING "Resources"FILE_PATH_DELIMITER_STRING"Objects"FILE_PATH_DELIMITER_STRING
#define OBJECT_RESOURCE_DIRECTORY_LENGTH 18

void objBaseInit(objectBase *const __RESTRICT__ base){
	base->name = NULL;
	base->skl = NULL;
	base->animationAllocate = 0;
	base->animationNum = 0;
	base->animations = NULL;
	base->skeletonBodies = NULL;
	base->skeletonColliders = NULL;
	base->renderables = 0;
	base->stateMax = 0;
}

return_t objBaseLoad(objectBase *const __RESTRICT__ base, const char *const __RESTRICT__ prgPath, const char *const __RESTRICT__ filePath){

	char fullPath[FILE_MAX_PATH_LENGTH];
	const size_t fileLength = strlen(filePath);

	FILE *__RESTRICT__ objInfo;

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
				fileParseResourcePath(&loadPath[0], NULL, line+9, lineLength);

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
					fileParseResourcePath(&loadPath[0], NULL, line+16, lineLength);

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
				fileParseResourcePath(&loadPath[0], NULL, line+10, lineLength);

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
				base->animationAllocate = strtoul(line+19, NULL, 0);


			// Renderable
			}else if(lineLength >= 14 && strncmp(line, "renderable ", 11) == 0){

				/** Load model and texture. **/
				renderableBase rndr;
				renderableBase *rndrNew;

				size_t mdlPathBegin = 11;
				size_t mdlPathLength = 0;
				const char *mdlSecondQuote = NULL;

				rndrBaseInit(&rndr);

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
					memcpy(&loadPath[0], line+mdlPathBegin, mdlPathLength);
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
							if(r <= 0){
								moduleModelFree(tempMdl);
								if(r < 0){
									/** Memory allocation failure. **/
									objBaseDelete(base);
									fclose(objInfo);
									return -1;
								}
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
						rndr.mdl = &g_mdlDefault;
					}

					if(texPathLength > 0){

						textureWrapper *tempTw = NULL;

						// Load the texture wrapper.
						memcpy(&loadPath[0], line+texPathBegin, texPathLength);
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
								if(r <= 0){
									moduleTextureWrapperFree(tempTw);
									if(r < 0){
										/** Memory allocation failure. **/
										objBaseDelete(base);
										fclose(objInfo);
										return -1;
									}
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
							rndr.tw = &g_twDefault;
						}

					}else{
						printf("Error loading object \"%s\": Could not parse texture wrapper for renderable at line %u.\n", fullPath, currentLine);
						rndr.tw = &g_twDefault;
					}

				}else{
					printf("Error loading object \"%s\": Could not parse model for renderable at line %u.\n", fullPath, currentLine);
					rndr.mdl = &g_mdlDefault;
					rndr.tw = &g_twDefault;
				}

				// Add the renderable.
				rndrNew = moduleRenderableBaseAppend(&base->renderables);
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
		base->skl = &g_sklDefault;
	}

	// If no renderables were loaded, load the default one.
	if(base->renderables == NULL){
		printf("Error loading object \"%s\": No renderables were loaded.\n", fullPath);
		moduleRenderableBaseAppend(&base->renderables);
		if(base->renderables == NULL){
			/** Memory allocation failure. **/
			objBaseDelete(base);
			return -1;
		}
		base->renderables->mdl = &g_mdlDefault;
		base->renderables->tw = &g_twDefault;
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

void objBaseDelete(objectBase *const __RESTRICT__ base){
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
		moduleRenderableBaseFreeArray(&base->renderables);
	}
}

return_t objInit(object *const __RESTRICT__ obj){
	obj->base = NULL;
	obj->configuration = NULL;
	obj->stateMax = 0;
	obj->stateNum = 0;
	obj->state.configuration = NULL;
	obj->state.previous = NULL;
	obj->oldestStatePrevious = &obj->state.previous;
	obj->skeletonBodies = NULL;
	obj->skeletonColliders = NULL;
	obj->renderables = NULL;
	return skliInit(&obj->skeletonData, NULL, 0);
}

void objDelete(object *const __RESTRICT__ obj){
	objectState *state = obj->state.previous;
	if(obj->configuration != NULL){
		memFree(obj->configuration);
	}
	while(state != NULL){
		objectState *next = state->previous;
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
		moduleRenderableFreeArray(&obj->renderables);
	}
	skliDelete(&obj->skeletonData);
}

return_t objInstantiate(object *const __RESTRICT__ obj, const objectBase *const __RESTRICT__ base){

	renderableBase *j;

	if(skliInit(&obj->skeletonData, base->skl, base->animationAllocate) < 0){
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
		obj->state.configuration = &obj->configuration[base->skl->boneNum];

		// Allocate memory for and initialize the colliders if necessary.
		if(base->skeletonColliders != NULL){

			collider *cLocal = base->skeletonColliders;
			collider *c = obj->skeletonColliders;
			const collider *const cLast = &c[base->skl->boneNum];

			/** Why do I still use malloc here? Was this incomplete? **/
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
					physRigidBodyInstantiate(bodyInstance, bodyBase) < 0
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
		renderable *const resource = moduleRenderableAppend(&obj->renderables);
		if(resource == NULL){
			/** Memory allocation failure. **/
			objDelete(obj);
			return -1;
		}
		rndrInstantiate(resource, j);
		j = moduleRenderableBaseNext(j);
	}

	obj->base = base;
	obj->stateMax = base->stateMax;
	obj->stateNum = 0;
	obj->state.previous = NULL;
	obj->oldestStatePrevious = &obj->state.previous;
	return 1;

}

return_t objNewRenderable(object *const __RESTRICT__ obj, model *const __RESTRICT__ mdl, textureWrapper *const __RESTRICT__ tw){
	// Allocate room for a new renderable and initialize it.
	renderable *const rndr = moduleRenderableAppend(&obj->renderables);
	if(rndr == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	rndr->mdl = mdl;
	twiInit(&rndr->twi, tw);
	return 1;
}

return_t objNewRenderableFromBase(object *const __RESTRICT__ obj, const renderableBase *const __RESTRICT__ rndr){
	// Allocate room for a new renderable and initialize it.
	renderable *const rndrNew = moduleRenderableAppend(&obj->renderables);
	if(rndrNew == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	rndrInstantiate(rndrNew, rndr);
	return 1;
}

return_t objNewRenderableFromInstance(object *const __RESTRICT__ obj, const renderable *rndr){
	// Allocate room for a new renderable and initialize it.
	renderable *const rndrNew = moduleRenderableAppend(&obj->renderables);
	if(rndrNew == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	rndrNew->mdl = rndr->mdl;
	twiInit(&rndrNew->twi, rndr->twi.tw);
	return 1;
}

return_t objInitSkeleton(object *const __RESTRICT__ obj, const skeleton *const __RESTRICT__ skl){
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
	obj->state.configuration = &tempBuffer[skl->boneNum];
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
			// Memory allocation failure. **
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
	physRigidBody *body = obj->skeletonBodies;

	for(i = 0; i < boneID; ++i){
		if(body != NULL){
			if(body->base->id == i){
				++body;
			}
		}else{
			return NULL;
		}
	}

	if(body != NULL && body->base->id == boneID){
		return body;
	}

	return NULL;

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

return_t objTick(object *const __RESTRICT__ obj, physIsland *const __RESTRICT__ island, const float elapsedTime){

	boneIndex_t i;
	renderable *j;
	sklNode *sklBone = obj->skeletonData.skl->bones;
	bone *sklState = obj->state.configuration;
	bone *configuration = obj->configuration;
	physRigidBody *body = obj->skeletonBodies;

	// If we can create a new previous state, do so.
	if(obj->stateNum < obj->stateMax){
		if(objStateAllocate(&obj->oldestStatePrevious, &obj->skeletonData) < 0){
			return -1;
		}
		++obj->stateNum;
	}

	// Update each skeletal animation.
	skliTick(&obj->skeletonData, elapsedTime, 1.f);

	// Update the object's skeleton.
	for(i = 0; i < obj->skeletonData.skl->boneNum; ++i, ++sklBone, ++sklState, ++configuration){

		const unsigned int isRoot = (i == sklBone->parent) || (sklBone->parent >= obj->skeletonData.skl->boneNum);

		// Update the previous states.
		objStateCopyBone(&obj->state, i);

		if(body != NULL && body->base->id == i && physRigidBodyIsSimulated(body)){

			// Simulate the body attached to the bone.

			/** TEMPORARILY ADD GRAVITY. **/
			const vec3 gravity = {.x = 0.f, .y = -9.80665f * body->mass, .z = 0.f};
			physRigidBodyApplyLinearForce(body, gravity);

			if(physRigidBodyIsUninitialized(body)){

				// Generate a new animated bone state.
				// This will later be copied into sklState.
				body->configuration = *configuration;
				skliGenerateBoneState(&obj->skeletonData, i, sklBone->name, &body->configuration);

				// Initialize the body's moment of inertia and centroid.
				physRigidBodyCentroidFromPosition(body);

				// Remove the body's "uninitialized" flag.
				physRigidBodySetInitializedFull(body);

			}

			// Update the position from the centroid.
			physRigidBodyUpdateConfiguration(body);

			// Copy the new bone state, as modified
			// by the body, into the object.
			*configuration = body->configuration;
			*sklState = *configuration;

			// Add the body to the physics island
			// and update all of its colliders.
			if(physRigidBodyUpdateColliders(body, island) < 0){
				/** Memory allocation failure. **/
				return -1;
			}

			body = modulePhysicsRigidBodyNext(body);

		}else{

			// Apply animation transformations.

			/** Should configurations be optional? **/

			// Apply configuration and the skeleton's bind transform.
			// We can just set sklState to configuration we don't want
			// global bone configurations. Doing this may break more
			// complex physics objects, however. If this is removed,
			// the transformation loop before rendering can also be
			// safely removed.
			*sklState = boneTransformAppend(*configuration, sklBone->defaultState);

			// Generate a new animated bone state.
			skliGenerateBoneState(&obj->skeletonData, i, sklBone->name, sklState);

			// Apply the parent's transformations to each bone.
			if(!isRoot){
				*sklState = boneTransformAppend(obj->state.configuration[sklBone->parent], *sklState);
			}

			if(body != NULL && body->base->id == i){

				// Copy the bone state over to the body.
				body->configuration = *sklState;

				// Initialize the body's moment of inertia and centroid.
				physRigidBodyCentroidFromPosition(body);

				// Set the body's "uninitialized" flag.
				physRigidBodySetUninitialized(body);

				// Update the body's colliders.
				if(physRigidBodyUpdateColliders(body, island) < 0){
					/** Memory allocation failure. **/
					return -1;
				}
				body = modulePhysicsRigidBodyNext(body);

			}

		}

	}

	// Update each of the object's renderables.
	j = obj->renderables;
	while(j != NULL){
		rndrTick(j, elapsedTime);
		j = moduleRenderableNext(j);
	}

	return 1;

}

void objBoneLastState(object *const __RESTRICT__ obj, const float dt){
	//
}

void objGenerateSprite(const object *const __RESTRICT__ obj, const renderable *const __RESTRICT__ rndr, const float interpT, const float *const __RESTRICT__ texFrag, vertex *const __RESTRICT__ vertices){

	// Generate the base sprite.
	const float left   = -0.5f;
	const float top    = -0.5f;
	const float right  =  0.5f;
	const float bottom =  0.5f;
	const float z      =  0.f;
	const bone *const current  = obj->state.configuration;
	const bone *const previous = (obj->state.previous == NULL ? current : obj->state.previous->configuration);
	bone transform;

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
	///boneInterpolate(&obj->skeletonState[1][0], &obj->skeletonState[0][0], interpT, &transform);
	transform = boneInterpolate(*previous, *current, interpT);
	transform.scale.x *= twiFrameWidth(&rndr->twi) * twiTextureWidth(&rndr->twi);
	transform.scale.y *= twiFrameHeight(&rndr->twi) * twiTextureHeight(&rndr->twi);
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

gfxRenderGroup_t objRenderGroup(const object *const __RESTRICT__ obj, const float interpT){

	// Check if the object will have
	// any translucent renderables.

	float totalAlpha = 0.f;
	const renderable *i = obj->renderables;

	while(i != NULL){

		if(twiTranslucent(&i->twi)){

			// The object contains translucency.
			return GFX_RNDR_GROUP_TRANSLUCENT;

		// Treat dithered renderables as opaque.
		// The check should eventually be moved up here once rndrAlpha is removed.
		}else{/// if(flagsAreUnset(i->state.flags, RENDERABLE_STATE_ALPHA_DITHER)){

			/** We have to calculate the alpha again when rendering. Maybe avoid this? **/
			const float alpha = rndrStateAlpha(&i->state, interpT);
			if(alpha > 0.f && alpha < 1.f && flagsAreUnset(i->state.flags, RENDERABLE_STATE_ALPHA_DITHER)){
				// The object contains translucency.
				return GFX_RNDR_GROUP_TRANSLUCENT;
			}
			totalAlpha += alpha;

		}

		i = moduleRenderableNext(i);

	}

	if(totalAlpha <= 0.f){
		// The model is fully transparent.
		return GFX_RNDR_GROUP_UNKNOWN;
	}

	// The model is fully opaque.
	return GFX_RNDR_GROUP_OPAQUE;

}

void objRender(const object *const __RESTRICT__ obj, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float distance, const float interpT){

	const renderable *currentRndr = obj->renderables;

	mat4 *transformCurrent = gfxMngr->shdrData.skeletonTransformState;
	const bone *bCurrent = obj->state.configuration;
	const bone *bPrevious = (obj->state.previous == NULL ? bCurrent : obj->state.previous->configuration);

	boneIndex_t boneNum = obj->skeletonData.skl->boneNum;
	sklNode *nLayout = obj->skeletonData.skl->bones;
	bone *bAccumulator = gfxMngr->shdrData.skeletonBindAccumulator;

	vec3 centroid = {.x = 0.f, .y = 0.f, .z = 0.f};

	boneIndex_t i;

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
	for(i = 0; i < boneNum; ++bCurrent, ++bPrevious, ++transformCurrent, ++nLayout, ++bAccumulator, ++i){

		// Interpolate between bone states.
		const bone state = boneInterpolate(*bPrevious, *bCurrent, interpT);

		// If the bone has a parent, add its inverse bind position,
		// otherwise just use the current bone's inverse bind position.
		if(nLayout->parent != i){
			*bAccumulator = boneTransformAppend(boneInvert(nLayout->defaultState), gfxMngr->shdrData.skeletonBindAccumulator[nLayout->parent]);
		}else{
			*bAccumulator = boneInvert(nLayout->defaultState);
			centroid = state.position;
		}

		// Add the inverse bind offsets to the bone state and
		// convert it to a transformation matrix for the shader.
		*transformCurrent = boneMatrix(
			boneTransformAppend(state, *bAccumulator)
		);

	}

	// Draw each renderable.
	while(currentRndr != NULL){

		rndrRender(currentRndr, obj->skeletonData.skl, gfxMngr, cam, distance, centroid, interpT);
		currentRndr = moduleRenderableNext(currentRndr);

	}

}