#include "object.h"
#include "memoryManager.h"
#include "helpersMath.h"
#include "helpersFileIO.h"
#include "inline.h"
#include <stdio.h>

/****/
#include "moduleTextureWrapper.h"
#include "moduleSkeleton.h"
#include "moduleModel.h"
#include "moduleRenderable.h"
#include "modulePhysics.h"

#define OBJECT_RESOURCE_DIRECTORY_STRING "Resources\\Objects\\"
#define OBJECT_RESOURCE_DIRECTORY_LENGTH 18

void objInit(object *obj){
	obj->name = NULL;
	obj->skl = NULL;
	obj->animationMax = 0;
	obj->animationNum = 0;
	obj->animations = NULL;
	obj->skeletonBodies = NULL;
	obj->skeletonColliders = NULL;
	obj->renderables = 0;
	obj->stateMax = 0;
}

return_t objLoad(object *obj, const char *prgPath, const char *filePath){

	char fullPath[FILE_MAX_PATH_LENGTH];
	const size_t fileLength = strlen(filePath);

	FILE *objInfo;

	objInit(obj);

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
				size_t pathBegin;
				size_t pathLength;
				const char *firstQuote = strchr(line+9, '"');
				const char *secondQuote = NULL;
				if(firstQuote != NULL){
					++firstQuote;
					pathBegin = firstQuote-line;
					secondQuote = strrchr(firstQuote, '"');
				}
				if(secondQuote > firstQuote){
					pathLength = secondQuote-firstQuote;
				}else{
					pathBegin = 9;
					pathLength = lineLength-pathBegin;
				}
				strncpy(&loadPath[0], line+pathBegin, pathLength);
				loadPath[pathLength] = '\0';

				// Check if the skeleton has already been loaded.
				tempSkl = moduleSkeletonFind(&loadPath[0]);
				if(tempSkl != NULL){
					obj->skl = tempSkl;

				// If the skeleton path is surrounded by quotes, try and load it.
				}else{
					tempSkl = moduleSkeletonAllocate();
					if(tempSkl != NULL){
						const return_t r = sklLoad(tempSkl, prgPath, &loadPath[0]);
						if(r < 1){
							// The load failed. Clean up.
							moduleSkeletonFree(tempSkl);
							if(r == -1){
								/** Memory allocation failure. **/
								objDelete(obj);
								fclose(objInfo);
								return -1;
							}
							printf("Error loading object \"%s\": Skeleton \"%s\" at line %u does not exist.\n", fullPath, &loadPath[0], currentLine);
							obj->skl = moduleSkeletonGetDefault();
						}else{
							obj->skl = tempSkl;
						}
					}else{
						/** Memory allocation failure. **/
						objDelete(obj);
						fclose(objInfo);
						return -1;
					}
				}


			// Physics
			}else if(lineLength >= 17 && strncmp(line, "skeletonPhysics ", 16) == 0){
				if(obj->skl != NULL){

					return_t r;

					size_t pathBegin;
					size_t pathLength;
					const char *firstQuote = strchr(line+9, '"');
					const char *secondQuote = NULL;
					if(firstQuote != NULL){
						++firstQuote;
						pathBegin = firstQuote-line;
						secondQuote = strrchr(firstQuote, '"');
					}
					if(secondQuote > firstQuote){
						pathLength = secondQuote-firstQuote;
					}else{
						pathBegin = 9;
						pathLength = lineLength-pathBegin;
					}
					strncpy(&loadPath[0], line+pathBegin, pathLength);
					loadPath[pathLength] = '\0';

					if(obj->skeletonBodies == NULL){
						modulePhysicsRigidBodyFreeArray(&obj->skeletonBodies);
						obj->skeletonBodies = NULL;
					}

					// Load the rigid body from a file.
					r = physRigidBodyLoad(&obj->skeletonBodies, obj->skl, prgPath, loadPath);

					if(r == -1){
						/** Memory allocation failure. **/
						objDelete(obj);
						fclose(objInfo);
						return -1;
					}else if(r == 0){
						modulePhysicsRigidBodyFreeArray(&obj->skeletonBodies);
						obj->skeletonBodies = NULL;
					}

				}else{
					printf("Error loading object \"%s\": Cannot load rigid bodies at line %u when no skeleton has been specified.\n", fullPath, currentLine);
				}


			// Hitboxes
			}else if(lineLength >= 18 && strncmp(line, "skeletonHitboxes ", 17) == 0){
				if(obj->skl != NULL){
					/** Load hitboxes. **/

				}else{
					printf("Error loading object \"%s\": Cannot load hitbox data at line %u when no skeleton has been specified.\n", fullPath, currentLine);
				}


			// Animation
			}else if(lineLength >= 11 && strncmp(line, "animation ", 10) == 0){

				sklAnim *tempSkla;
				size_t pathBegin;
				size_t pathLength;
				const char *firstQuote = strchr(line+9, '"');
				const char *secondQuote = NULL;
				if(firstQuote != NULL){
					++firstQuote;
					pathBegin = firstQuote-line;
					secondQuote = strrchr(firstQuote, '"');
				}
				if(secondQuote > firstQuote){
					pathLength = secondQuote-firstQuote;
				}else{
					pathBegin = 9;
					pathLength = lineLength-pathBegin;
				}
				strncpy(&loadPath[0], line+pathBegin, pathLength);
				loadPath[pathLength] = '\0';

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
							if(r == -1){
								/** Memory allocation failure. **/
								objDelete(obj);
								fclose(objInfo);
								return -1;
							}
							tempSkla = NULL;
						}
					}else{
						/** Memory allocation failure. **/
						objDelete(obj);
						fclose(objInfo);
						return -1;
					}
				}

				if(tempSkla != NULL){

					sklAnim **tempBuffer = memReallocate(obj->animations, (obj->animationNum+1)*sizeof(sklAnim *));
					if(tempBuffer == NULL){
						/** Memory allocation failure. **/
						objDelete(obj);
						fclose(objInfo);
						return -1;
					}
					obj->animations = tempBuffer;
					obj->animations[obj->animationNum] = tempSkla;
					++obj->animationNum;

				}else{
					printf("Error loading object \"%s\": Skeletal animation \"%s\" at line %u does not exist.\n", fullPath, &loadPath[0], currentLine);
				}


			// Animation capacity
			}else if(lineLength >= 20 && strncmp(line, "animationCapacity ", 19) == 0){
				obj->animationMax = strtoul(line+19, NULL, 0);


			// Renderable
			}else if(lineLength >= 14 && strncmp(line, "renderable ", 11) == 0){

				/** Load model and texture. **/
				renderable rndr;
				renderable *rndrNew;

				size_t mdlPathBegin = 11;
				size_t mdlPathLength = 0;
				const char *mdlSecondQuote = NULL;

				rndrInit(&rndr);

				/* Get the model path. */
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

					/* Get the texture wrapper path. */
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

					/* Load the model. */
					strncpy(&loadPath[0], line+mdlPathBegin, mdlPathLength);
					loadPath[mdlPathLength] = '\0';

					// Check if the model has already been loaded.
					tempMdl = moduleModelFind(&loadPath[0]);
					if(tempMdl != NULL){
						rndr.mdl = tempMdl;

					// If the model path is surrounded by quotes, try and load it.
					}else{
						tempMdl = moduleModelAllocate();
						if(tempMdl != NULL){
							const return_t r = mdlLoad(tempMdl, prgPath, &loadPath[0]);
							if(r == -1){
								/** Memory allocation failure. **/
								moduleModelFree(tempMdl);
								objDelete(obj);
								fclose(objInfo);
								return -1;
							}else if(r > 0){
								rndr.mdl = tempMdl;
							}
						}else{
							/** Memory allocation failure. **/
							objDelete(obj);
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

						/* Load the texture wrapper. */
						strncpy(&loadPath[0], line+texPathBegin, texPathLength);
						loadPath[texPathLength] = '\0';

						// Check if the texture wrapper has already been loaded.
						tempTw = moduleTextureWrapperFind(&loadPath[0]);
						if(tempTw != NULL){
							rndr.tw = tempTw;

						// If the texture wrapper path is surrounded by quotes, try and load it.
						}else{
							tempTw = moduleTextureWrapperAllocate();
							if(tempTw != NULL){
								const return_t r = twLoad(tempTw, prgPath, &loadPath[0]);
								if(r == -1){
									/** Memory allocation failure. **/
									moduleTextureWrapperFree(tempTw);
									objDelete(obj);
									fclose(objInfo);
									return -1;
								}else if(r > 0){
									rndr.tw = tempTw;
								}
							}else{
								/** Memory allocation failure. **/
								objDelete(obj);
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

				/* Add the renderable. */
				rndrNew = moduleRenderableAppend(&obj->renderables);
				if(rndrNew == NULL){
					/** Memory allocation failure. **/
					objDelete(obj);
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
	if(obj->skl == NULL){
		//printf("Error loading object: No skeleton was loaded.\n");
		obj->skl = moduleSkeletonGetDefault();
	}

	// If no renderables were loaded, load the default one.
	if(obj->renderables == NULL){
		printf("Error loading object \"%s\": No renderables were loaded.\n", fullPath);
		moduleRenderableAppend(&obj->renderables);
		if(obj->renderables == NULL){
			/** Memory allocation failure. **/
			objDelete(obj);
			return -1;
		}
		obj->renderables->mdl = moduleModelGetDefault();
		obj->renderables->tw = moduleTextureWrapperGetDefault();
	}

	// Generate a name based off the file path.
	obj->name = fileGenerateResourceName(filePath, fileLength);
	if(obj->name == NULL){
		/** Memory allocation failure. **/
		objDelete(obj);
		return -1;
	}

	return 1;

}

void objDelete(object *obj){
	if(obj->name != NULL){
		memFree(obj->name);
	}
	if(obj->animations != NULL){
		memFree(obj->animations);
	}
	if(obj->skl != NULL){
		if(obj->skeletonBodies != NULL){
			modulePhysicsRigidBodyFreeArray(&obj->skeletonBodies);
		}
		if(obj->skeletonColliders != NULL){
			colliderArray *ca = obj->skeletonColliders;
			colliderArray *caLast = &ca[obj->skl->boneNum];
			for(; ca < caLast; ++ca){
				cArrayDelete(ca);
			}
			free(obj->skeletonColliders);
		}
	}
	if(obj->renderables != NULL){
		moduleRenderableFreeArray(&obj->renderables);
	}
}

return_t objiInit(objInstance *obji){
	obji->base = NULL;
	obji->configuration = NULL;
	obji->stateMax = 0;
	obji->stateNum = 0;
	obji->state.skeleton = NULL;
	obji->state.previous = NULL;
	obji->oldestStatePrevious = &obji->state.previous;
	obji->skeletonBodies = NULL;
	obji->skeletonColliders = NULL;
	obji->renderables = NULL;
	return skliInit(&obji->skeletonData, NULL, 0);
}

void objiDelete(objInstance *obji){
	objiState *state = obji->state.previous;
	if(obji->configuration != NULL){
		memFree(obji->configuration);
	}
	while(state != NULL){
		objiState *next = state->previous;
		memFree(state);
		state = next;
	}
	if(obji->skeletonBodies != NULL){
		modulePhysicsRigidBodyInstanceFreeArray(&obji->skeletonBodies);
	}
	if(obji->skeletonColliders != NULL){
		colliderArray *ca = obji->skeletonColliders;
		colliderArray *caLast = &ca[obji->skeletonData.skl->boneNum];
		for(; ca < caLast; ++ca){
			cArrayDelete(ca);
		}
		free(obji->skeletonColliders);
	}
	if(obji->renderables != NULL){
		moduleRenderableInstanceFreeArray(&obji->renderables);
	}
	skliDelete(&obji->skeletonData);
}

return_t objiInstantiate(objInstance *obji, object *base){

	renderable *j;

	if(skliInit(&obji->skeletonData, base->skl, base->animationMax) == -1){
		/** Memory allocation failure. **/
		return -1;
	}

	if(base->skl->boneNum != 0){

		bone *b;
		bone *bLast;

		/* Allocate memory for the object instance. */
		obji->configuration = memAllocate(3 * base->skl->boneNum * sizeof(bone));
		if(obji->configuration == NULL){
			/** Memory allocation failure. **/
			skliDelete(&obji->skeletonData);
			return -1;
		}
		obji->state.skeleton = &obji->configuration[base->skl->boneNum];

		/* Allocate memory for and initialize the colliders if necessary. */
		if(base->skeletonColliders != NULL){

			colliderArray *caBase = base->skeletonColliders;
			colliderArray *ca = obji->skeletonColliders;
			colliderArray *lastCA = &ca[base->skl->boneNum];

			obji->skeletonColliders = malloc(base->skl->boneNum * sizeof(colliderArray));
			if(obji->skeletonColliders == NULL){
				/** Memory allocation failure. **/
				memFree(obji->configuration);
				skliDelete(&obji->skeletonData);
				return -1;
			}

			while(ca < lastCA){
				const size_t arraySize = caBase->colliderNum * sizeof(collider);
				ca->colliders = malloc(arraySize);
				if(ca->colliders == NULL){
					/** Memory allocation failure. **/
					break;
				}
				ca->colliderNum = caBase->colliderNum;
				memcpy(ca->colliders, caBase->colliders, arraySize);
				++caBase;
				++ca;
			}

			if(ca < lastCA){
				/** Memory allocation failure. **/
				while(ca > obji->skeletonColliders){
					--ca;
					cArrayDelete(ca);
				}
				free(obji->skeletonColliders);
				memFree(obji->configuration);
				skliDelete(&obji->skeletonData);
				return -1;
			}

		}else{
			obji->skeletonColliders = NULL;
		}

		// Initialize each bone.
		b = obji->configuration;
		bLast = &b[base->skl->boneNum];
		for(; b < bLast; ++b){
			boneInit(b);
		}

		/* Allocate memory for and initialize the rigid bodies if necessary. */
		if(base->skeletonBodies != NULL){

			physRigidBody *bodyBase = base->skeletonBodies;
			physRBInstance *bodyInstance;

			do {

				bodyInstance = modulePhysicsRigidBodyInstanceAppend(&obji->skeletonBodies);
				if(
					bodyInstance == NULL ||
					physRBIInstantiate(bodyInstance, bodyBase, &obji->state.skeleton[bodyBase->id]) == -1
				){
					/** Memory allocation failure. **/
					memFree(obji->configuration);
					skliDelete(&obji->skeletonData);
					modulePhysicsRigidBodyInstanceFreeArray(&obji->skeletonBodies);
					return -1;
				}

				/** Figure out constraint loading. **/
				///bodyInstance->flags = PHYSICS_BODY_INACTIVE;
				///bodyInstance->constraintNum = base->skeletonConstraintNum[i];
				///bodyInstance->constraints = base->skeletonConstraints[i];

				bodyBase = modulePhysicsRigidBodyNext(bodyBase);

			} while(bodyBase != NULL);

		}else{
			obji->skeletonBodies = NULL;
		}

	}

	// Instantiate each renderable.
	j = base->renderables;
	while(j != NULL){
		rndrInstance *resource = moduleRenderableInstanceAppend(&obji->renderables);
		if(resource == NULL){
			/** Memory allocation failure. **/
			objiDelete(obji);
			return -1;
		}
		rndriInstantiate(resource, j);
		j = moduleRenderableNext(j);
	}

	obji->base = base;
	obji->stateMax = base->stateMax;
	obji->stateNum = 0;
	obji->state.previous = NULL;
	obji->oldestStatePrevious = &obji->state.previous;
	return 1;

}

static __FORCE_INLINE__ return_t objiStateAllocate(objInstance *obji){
	objiState *state = memAllocate(sizeof(objiState) + obji->skeletonData.skl->boneNum * sizeof(bone));
	if(state != NULL){
		state->skeleton = (bone *)((byte_t *)state + sizeof(objiState));
		state->previous = NULL;
		*obji->oldestStatePrevious = state;
		obji->oldestStatePrevious = &state->previous;
		++obji->stateNum;
		return 1;
	}
	return -1;
}

return_t objiStatePreallocate(objInstance *obji){
	while(obji->stateNum < obji->stateMax){
		if(objiStateAllocate(obji) == -1){
			return -1;
		}
	}
	return 1;
}

static __FORCE_INLINE__ void objiStateCopyBone(objInstance *obji, const boneIndex_t i){

	objiState *state = &obji->state;
	bone last = state->skeleton[i];

	while(state->previous != NULL){

		const bone swap = state->previous->skeleton[i];
		state->previous->skeleton[i] = last;
		last = swap;
		state = state->previous;

	}

}

return_t objiNewRenderable(objInstance *obji, model *mdl, textureWrapper *tw){
	/* Allocate room for a new renderable and initialize it. */
	rndrInstance *rndr = moduleRenderableInstanceAppend(&obji->renderables);
	if(rndr == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	rndr->mdl = mdl;
	twiInit(&rndr->twi, tw);
	return 1;
}

return_t objiNewRenderableFromBase(objInstance *obji, const renderable *rndr){
	/* Allocate room for a new renderable and initialize it. */
	rndrInstance *rndrNew = moduleRenderableInstanceAppend(&obji->renderables);
	if(rndrNew == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	rndriInstantiate(rndrNew, rndr);
	return 1;
}

return_t objiNewRenderableFromInstance(objInstance *obji, const rndrInstance *rndr){
	/* Allocate room for a new renderable and initialize it. */
	rndrInstance *rndrNew = moduleRenderableInstanceAppend(&obji->renderables);
	if(rndrNew == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	rndrNew->mdl = rndr->mdl;
	twiInit(&rndrNew->twi, rndr->twi.tw);
	return 1;
}

return_t objiInitSkeleton(objInstance *obji, skeleton *skl){
	bone *bLast;
	bone *tempBuffer = memAllocate(3 * skl->boneNum * sizeof(bone));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	if(obji->configuration != NULL){
		memFree(obji->configuration);
	}
	if(obji->skeletonBodies != NULL){
		modulePhysicsRigidBodyInstanceFreeArray(&obji->skeletonBodies);
	}
	if(obji->skeletonColliders != NULL){
		free(obji->skeletonColliders);
	}
	obji->configuration = tempBuffer;
	obji->state.skeleton = &tempBuffer[skl->boneNum];
	bLast = &tempBuffer[skl->boneNum];
	for(; tempBuffer < bLast; ++tempBuffer){
		boneInit(tempBuffer);
	}
	skliDelete(&obji->skeletonData);
	obji->skeletonData.skl = skl;
	return 1;
}

/**return_t objInitPhysics(object *obji){
	if(obji->skl != NULL){
		size_t i;
		physRBInstance *tempBuffer = malloc(obji->skl->boneNum*sizeof(physRBInstance));
		if(tempBuffer == NULL){
			** Memory allocation failure. **
			return 0;
		}
		obji->skeletonBodies = tempBuffer;
		for(i = 0; i < obji->skl->boneNum; ++i){
			physRBIInit(&obji->skeletonBodies[i]);
		}
	}
	return 1;
}

void objiBoneSetPhysicsFlags(objInstance *obji, const boneIndex_t boneID, const flags_t flags){

	if(obji->skeletonBodies != NULL){

		obji->skeletonBodies[boneID].flags = flags;

		if(flagsAreSet(flags, PHYSICS_BODY_SIMULATE)){

			// Replace PHYSICS_BODY_SIMULATE with PHYSICS_BODY_INITIALIZE.
			flagsUnset(obji->skeletonBodies[boneID].flags, PHYSICS_BODY_SIMULATE);
			flagsSet(obji->skeletonBodies[boneID].flags, PHYSICS_BODY_INITIALIZE);

		}

	}

}**/

sklAnim *objiGetAnimation(const objInstance *obji, const animIndex_t id){
	if(obji->base != NULL){
		if(obji->base->animationNum > id){
			return obji->base->animations[id];
		}
	}
	return NULL;
}

sklAnim *objiFindAnimation(const objInstance *obji, const char *name){
	if(obji->base != NULL){
		sklAnim **anim = obji->base->animations;
		sklAnim **animLast = &anim[obji->base->animationNum];
		for(; anim < animLast; ++anim){
			if(strcmp((*anim)->name, name) == 0){
				return *anim;
			}
		}
	}
	return NULL;
}

/**__FORCE_INLINE__ void objiSetAnimationType(objInstance *obji, const animIndex_t slot, const flags_t additive){
	skliSetAnimationType(&obji->skeletonData, slot, additive);
}

__FORCE_INLINE__ return_t objiChangeAnimation(objInstance *obji, const animIndex_t slot, sklAnim *anim, const frameIndex_t frame, const float blendTime){
	return skliChangeAnimation(&obji->skeletonData, slot, anim, frame, blendTime);
}

__FORCE_INLINE__ void objiClearAnimation(objInstance *obji, const animIndex_t slot){
	skliClearAnimation(&obji->skeletonData, slot);
}

__FORCE_INLINE__ void objiApplyLinearForce(objInstance *obji, const boneIndex_t boneID, const vec3 *F){
	physRBIApplyLinearForce(&obji->skeletonBodies[boneID], F);
}

__FORCE_INLINE__ void objiApplyAngularForceGlobal(objInstance *obji, const boneIndex_t boneID, const vec3 *F, const vec3 *r){
	physRBIApplyAngularForceGlobal(&obji->skeletonBodies[boneID], F, r);
}

__FORCE_INLINE__ void objiApplyForceGlobal(objInstance *obji, const boneIndex_t boneID, const vec3 *F, const vec3 *r){
	physRBIApplyForceGlobal(&obji->skeletonBodies[boneID], F, r);
}

void objiApplyForceAtGlobalPoint(objInstance *obji, const size_t boneID, const vec3 *F, const vec3 *r){
	physRBIApplyForceAtGlobalPoint(&obji->skeletonBodies[boneID], F, r);
}
void objiAddLinearVelocity(objInstance *obji, const size_t boneID, const float x, const float y, const float z){
	physRBIAddLinearVelocity(&obji->skeletonBodies[boneID], x, y, z);
}
void objiApplyLinearImpulse(objInstance *obji, const size_t boneID, const float x, const float y, const float z){
	physRBIApplyLinearImpulse(&obji->skeletonBodies[boneID], x, y, z);
}
void objiAddAngularVelocity(objInstance *obji, const size_t boneID, const float angle, const float x, const float y, const float z){
	physRBIAddAngularVelocity(&obji->skeletonBodies[boneID], angle, x, y, z);
}**/

return_t objiUpdate(objInstance *obji, physicsSolver *solver, const float elapsedTime, const float dt){

	boneIndex_t i;
	rndrInstance *j;
	sklNode *sklBone = obji->skeletonData.skl->bones;
	bone *sklState = obji->state.skeleton;
	bone *configuration = obji->configuration;
	physRBInstance *body = obji->skeletonBodies;

	// If we can create a new previous state, do so.
	if(obji->stateNum < obji->stateMax){
		if(objiStateAllocate(obji) == -1){
			return -1;
		}
	}

	// Update each skeletal animation.
	skliUpdateAnimations(&obji->skeletonData, elapsedTime, 1.f);

	/* Update the object's skeleton. */
	for(i = 0; i < obji->skeletonData.skl->boneNum; ++i, ++sklBone, ++sklState, ++configuration){

		const return_t isRoot = (i == sklBone->parent) || (sklBone->parent >= obji->skeletonData.skl->boneNum);

		// Update the previous states.
		objiStateCopyBone(obji, i);

		if(body != NULL && body->local->id == i && flagsAreSet(body->flags, (PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_SIMULATE))){

			/*
			** Simulate the body attached to the bone.
			*/

			if(flagsAreSet(body->flags, PHYSICS_BODY_INITIALIZE)){

				// Generate a new animated bone state.
				//obji->skeletonState[0][i] = obji->configuration[i];
				*sklState = *configuration;
				skliGenerateBoneState(&obji->skeletonData, i, sklBone->name, sklState);
				//skliGenerateBoneState(&obji->skeletonData, &obji->skeletonState[i2], i);
				//skliGenerateBoneState(&obji->animationData, &obji->skeletonState[0][i], i);

				// Set the body's state to that of its bone when the bone starts being simulated.
				/** This should be removed eventually if possible. **/
				flagsUnset(body->flags, PHYSICS_BODY_INITIALIZE);
				flagsSet(body->flags, PHYSICS_BODY_SIMULATE);
				//body->configuration[0] = obji->skeletonState[0][i];
				//body->configuration[1] = obji->skeletonState[1][i];

			}

			/** TEMPORARILY ADD GRAVITY. **/
			vec3 force; vec3Set(&force, 0.f, -98.0665f, 0.f);
			physRBIApplyLinearForce(body, &force);

			if(flagsAreSet(body->flags, PHYSICS_BODY_COLLIDE)){
				// Only update the body's collision mesh
				// and add it to the solver if it is set
				// to interact with other bodies.
				/** Only update AABB? **/
				physRBInstance **tempBody = physSolverAllocate(solver);
				if(tempBody == NULL){
					/** Memory allocation failure. **/
					return -1;
				}
				physRBIIntegrateLeapfrogVelocity(body, dt);
				physRBIUpdateCollisionMesh(body);
				*tempBody = body;
			}else{
				// If the body is not interacting with any
				// other bodies, integrate everything
				// instead of just the velocity.
				physRBIIntegrateLeapfrog(body, dt);
			}

			/** TEMPORARILY SET THE BONE STATE. **/
			//obji->skeletonState[0][i] = body->configuration[0];

			body = modulePhysicsRigidBodyInstanceNext(body);

		}else{

			/*
			** Apply animation transformations.
			*/

			// Apply the object skeleton's bind offsets.
			/*obji->skeletonState[0][i].position.x += obji->skl->bones[i].defaultState.position.x;
			obji->skeletonState[0][i].position.y += obji->skl->bones[i].defaultState.position.y;
			obji->skeletonState[0][i].position.z += obji->skl->bones[i].defaultState.position.z;*/

			// Generate a new animated bone state.
			//obji->skeletonState[0][i] = obji->configuration[i];
			/** Should configurations be optional? **/
			*sklState = *configuration;
			skliGenerateBoneState(&obji->skeletonData, i, sklBone->name, sklState);
			//skliGenerateBoneState(&obji->skeletonData, &obji->skeletonState[i2], i);
			//skliGenerateBoneState(&obji->animationData, &obji->skeletonState[0][i], i);

			// Apply the object skeleton's bind offsets.
			/*boneTransformAppendPositionVec(&obji->skeletonState[0][i],
			                               obji->skl->bones[i].defaultState.position.x,
			                               obji->skl->bones[i].defaultState.position.y,
			                               obji->skl->bones[i].defaultState.position.z,
			                               &obji->skeletonState[0][i].position);*/
			boneTransformAppendPositionVec(sklState,
			                               sklBone->defaultState.position.x,
			                               sklBone->defaultState.position.y,
			                               sklBone->defaultState.position.z,
			                               &sklState->position);

			// Apply the parent's transformations to each bone.
			if(!isRoot){
				//boneTransformAppend(&obji->skeletonState[0][obji->skl->bones[i].parent], &obji->skeletonState[0][i], &obji->skeletonState[0][i]);
				boneTransformAppend(&obji->state.skeleton[sklBone->parent], sklState, sklState);
			}

			if(body != NULL && body->local->id == i){
				if(flagsAreSet(body->flags, PHYSICS_BODY_COLLIDE)){
					// Only update the body's collision mesh
					// and add it to the solver if it is set
					// to interact with other bodies.
					/** Only update AABB? **/
					physRBInstance **tempBody = physSolverAllocate(solver);
					if(tempBody == NULL){
						/** Memory allocation failure. **/
						return -1;
					}
					*tempBody = body;
				}
				body = modulePhysicsRigidBodyInstanceNext(body);
			}

		}

		// Apply billboarding to the root bone if necessary.
		//if(isRoot && flagsAreSet(obji->tempRndrConfig.flags, (RNDR_BILLBOARD_X | RNDR_BILLBOARD_Y | RNDR_BILLBOARD_Z))){
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

	/* Update each of the object's renderables. */
	j = obji->renderables;
	while(j != NULL){
		rndriUpdate(j, elapsedTime);
		j = moduleRenderableInstanceNext(j);
	}

	return 1;

}

void objiBoneLastState(objInstance *obji, const float dt){
	//
}

gfxRenderGroup_t objiRenderGroup(const objInstance *obji, const float interpT){

	/*
	** Check if the object will have
	** any translucent renderables.
	*/

	float totalAlpha = 0.f;
	rndrInstance *i = obji->renderables;

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

void objiGenerateSprite(const objInstance *obji, const rndrInstance *rndr, const float interpT, const float *texFrag, vertex *vertices){

	/* Generate the base sprite. */
	const float left   = -0.5f; /// - obji->tempRndrConfig.pivot.render.x;
	const float top    = -0.5f; /// - obji->tempRndrConfig.pivot.render.y;
	const float right  =  0.5f; /// - obji->tempRndrConfig.pivot.render.x;
	const float bottom =  0.5f; /// - obji->tempRndrConfig.pivot.render.y;
	const float z      =  0.f;  /// - obji->tempRndrConfig.pivot.render.z;
	const bone *current  = obji->state.skeleton;
	const bone *previous = (obji->state.previous == NULL ? current : obji->state.previous->skeleton);
	bone transform;

	// Create the top left vertex.
	vertices[0].position.x = left;
	vertices[0].position.y = top;
	vertices[0].position.z = z;
	vertices[0].u = 0.f;
	vertices[0].v = 0.f;
	vertices[0].normal.x = 0.f;
	vertices[0].normal.y = 0.f;
	vertices[0].normal.z = 0.f;
	vertices[0].bIDs[0] = -1;
	vertices[0].bIDs[1] = -1;
	vertices[0].bIDs[2] = -1;
	vertices[0].bIDs[3] = -1;
	vertices[0].bWeights[0] = 0.f;
	vertices[0].bWeights[1] = 0.f;
	vertices[0].bWeights[2] = 0.f;
	vertices[0].bWeights[3] = 0.f;

	// Create the top right vertex.
	vertices[1].position.x = right;
	vertices[1].position.y = top;
	vertices[1].position.z = z;
	vertices[1].u = 1.f;
	vertices[1].v = 0.f;
	vertices[1].normal.x = 0.f;
	vertices[1].normal.y = 0.f;
	vertices[1].normal.z = 0.f;
	vertices[1].bIDs[0] = -1;
	vertices[1].bIDs[1] = -1;
	vertices[1].bIDs[2] = -1;
	vertices[1].bIDs[3] = -1;
	vertices[1].bWeights[0] = 0.f;
	vertices[1].bWeights[1] = 0.f;
	vertices[1].bWeights[2] = 0.f;
	vertices[1].bWeights[3] = 0.f;

	// Create the bottom left vertex.
	vertices[2].position.x = left;
	vertices[2].position.y = bottom;
	vertices[2].position.z = z;
	vertices[2].u = 0.f;
	vertices[2].v = -1.f;  // Flip the y dimension so the image isn't upside down.
	vertices[2].normal.x = 0.f;
	vertices[2].normal.y = 0.f;
	vertices[2].normal.z = 0.f;
	vertices[2].bIDs[0] = -1;
	vertices[2].bIDs[1] = -1;
	vertices[2].bIDs[2] = -1;
	vertices[2].bIDs[3] = -1;
	vertices[2].bWeights[0] = 0.f;
	vertices[2].bWeights[1] = 0.f;
	vertices[2].bWeights[2] = 0.f;
	vertices[2].bWeights[3] = 0.f;

	// Create the bottom right vertex.
	vertices[3].position.x = right;
	vertices[3].position.y = bottom;
	vertices[3].position.z = z;
	vertices[3].u = 1.f;
	vertices[3].v = -1.f;  // Flip the y dimension so the image isn't upside down.
	vertices[3].normal.x = 0.f;
	vertices[3].normal.y = 0.f;
	vertices[3].normal.z = 0.f;
	vertices[3].bIDs[0] = -1;
	vertices[3].bIDs[1] = -1;
	vertices[3].bIDs[2] = -1;
	vertices[3].bIDs[3] = -1;
	vertices[3].bWeights[0] = 0.f;
	vertices[3].bWeights[1] = 0.f;
	vertices[3].bWeights[2] = 0.f;
	vertices[3].bWeights[3] = 0.f;

	/* Generate a transformation for the sprite and transform each vertex. */
	/** Optimize? **/
	//boneInterpolate(&obji->skeletonState[1][0], &obji->skeletonState[0][0], interpT, &transform);
	boneInterpolate(previous, current, interpT, &transform);
	transform.scale.x *= twiGetFrameWidth(&rndr->twi) * twiGetTexWidth(&rndr->twi);
	transform.scale.y *= twiGetFrameHeight(&rndr->twi) * twiGetTexHeight(&rndr->twi);
	vertTransform(&vertices[0], &transform.position, &transform.orientation, &transform.scale);
	vertTransform(&vertices[1], &transform.position, &transform.orientation, &transform.scale);
	vertTransform(&vertices[2], &transform.position, &transform.orientation, &transform.scale);
	vertTransform(&vertices[3], &transform.position, &transform.orientation, &transform.scale);

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
