#include "object.h"
#include "helpersMath.h"
#include "helpersFileIO.h"
#include "inline.h"
#include <stdio.h>

/****/
#include "moduleTextureWrapper.h"
#include "moduleSkeleton.h"
#include "moduleModel.h"

void objInit(object *obj){
	obj->name = NULL;
	obj->skl = NULL;
	obj->animationNum = 0;
	obj->animations = NULL;
	obj->animationCapacity = 0;
	obj->skeletonBodies = NULL;
	///obj->skeletonBodyFlags = NULL;
	///obj->skeletonConstraintNum = NULL;
	obj->skeletonConstraints = NULL;
	obj->skeletonHitboxes = NULL;
	obj->renderableNum = 0;
	obj->renderables = 0;
	obj->stateNum = 0;
}

return_t objLoad(object *obj, const char *prgPath, const char *filePath,
                 cVector *allTextures, cVector *allTexWrappers, cVector *allSkeletons,
                 cVector *allSklAnimations, cVector *allModels){

	char fullPath[FILE_MAX_PATH_LENGTH];
	const size_t fileLength = strlen(filePath);

	FILE *objInfo;

	objInit(obj);

	fileGenerateFullPath(&fullPath[0], prgPath, strlen(prgPath), filePath, fileLength);
	objInfo = fopen(&fullPath[0], "r");

	if(objInfo != NULL){

		char lineFeed[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		fileLine_t currentLine = 0;  // Current file line being read.

		char loadPath[FILE_MAX_PATH_LENGTH];

		while(fileParseNextLine(objInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			++currentLine;

			// Name
			if(lineLength >= 6 && strncmp(line, "name ", 5) == 0){
				while(line[5] == ' ' || line[5] == '\t'){
					++line;
					--lineLength;
				}
				if(line[5] == '"' && line[lineLength-1] == '"'){
					++line;
					lineLength -= 2;
				}
				obj->name = malloc((lineLength-4) * sizeof(char));
				if(obj->name == NULL){
					/** Memory allocation failure. **/
					objDelete(obj);
					fclose(objInfo);
					return -1;
				}
				strncpy(obj->name, line+5, lineLength-5);
				obj->name[lineLength-5] = '\0';


			// Skeleton
			}else if(lineLength >= 10 && strncmp(line, "skeleton ", 9) == 0){

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
							printf("Error loading object \"%s\": Skeleton \"%s\" at line %u does not exist.\n", &fullPath[0], &loadPath[0], currentLine);
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
					physRigidBody *tempBuffer1;
					///flags_t *tempBuffer2;
					///physConstraintIndex_t *tempBuffer3;
					physConstraint **tempBuffer4;
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

					///tempBuffer1 = modulePhysicsRigidBodyAppend(obj->skeletonBodies); ///realloc(obj->skeletonBodies, obj->skl->boneNum*sizeof(physRigidBody));
					if(tempBuffer1 == NULL){
						/** Memory allocation failure. **/
						objDelete(obj);
						fclose(objInfo);
						return -1;
					}
					/**obj->skeletonBodies = tempBuffer1;
					tempBuffer2 = realloc(obj->skeletonBodyFlags, obj->skl->boneNum*sizeof(flags_t));
					if(tempBuffer2 == NULL){
						** Memory allocation failure. **
						objDelete(obj);
						fclose(objInfo);
						return -1;
					}
					obj->skeletonBodyFlags = tempBuffer2;
					tempBuffer3 = realloc(obj->skeletonConstraintNum, obj->skl->boneNum*sizeof(physConstraintIndex_t));
					if(tempBuffer3 == NULL){
						** Memory allocation failure. **
						objDelete(obj);
						fclose(objInfo);
						return -1;
					}
					obj->skeletonConstraintNum = tempBuffer3;**/
					tempBuffer4 = realloc(obj->skeletonConstraints, obj->skl->boneNum*sizeof(physConstraint *));
					if(tempBuffer4 == NULL){
						/** Memory allocation failure. **/
						objDelete(obj);
						fclose(objInfo);
						return -1;
					}
					obj->skeletonConstraints = tempBuffer4;

					/**r = physRigidBodyLoad(obj->skeletonBodies, PHYS_BODY_INACTIVE,
					                      obj->skeletonConstraints,
					                      obj->skl, prgPath, loadPath);**/
					if(r == -1){
						/** Memory allocation failure. **/
						objDelete(obj);
						fclose(objInfo);
						return -1;
					}else if(r == 0){
						///modulePhysicsRigidBodyFree(obj->skeletonBodies);
						///obj->skeletonConstraints  // Free each constraint.
						///free(obj->skeletonBodyFlags);
						///free(obj->skeletonConstraintNum);
						free(obj->skeletonConstraints);
						obj->skeletonBodies = NULL;
						///obj->skeletonBodyFlags = NULL;
						///obj->skeletonConstraintNum = NULL;
						obj->skeletonConstraints = NULL;
					}

				}else{
					printf("Error loading object \"%s\": Cannot load rigid bodies at line %u when no skeleton has been specified.\n", &fullPath[0], currentLine);
				}


			// Hitboxes
			}else if(lineLength >= 18 && strncmp(line, "skeletonHitboxes ", 17) == 0){
				if(obj->skl != NULL){
					/** Load hitboxes. **/

				}else{
					printf("Error loading object \"%s\": Cannot load hitbox data at line %u when no skeleton has been specified.\n", &fullPath[0], currentLine);
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

					// Allocate room for the new animation pointer if necessary.
					if(obj->animationNum == obj->animationCapacity){
						const size_t tempCapacity = obj->animationCapacity + 1;
						sklAnim **tempBuffer = realloc(obj->animations, tempCapacity*sizeof(sklAnim *));
						if(tempBuffer == NULL){
							/** Memory allocation failure. **/
							objDelete(obj);
							fclose(objInfo);
							return -1;
						}
						obj->animationCapacity = tempCapacity;
						obj->animations = tempBuffer;
					}
					obj->animations[obj->animationNum] = tempSkla;
					++obj->animationNum;

				}else{
					printf("Error loading object \"%s\": Skeletal animation \"%s\" at line %u does not exist.\n", &fullPath[0], &loadPath[0], currentLine);
				}


			// Animation capacity
			}else if(lineLength >= 20 && strncmp(line, "animationCapacity ", 19) == 0){
				obj->animationCapacity = strtoul(line+19, NULL, 0);


			// Renderable
			}else if(lineLength >= 14 && strncmp(line, "renderable ", 11) == 0){

				/** Load model and texture. **/
				renderable *tempBuffer;
				renderable rndr;

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
						printf("Error loading object \"%s\": Model \"%s\" at line %u does not exist.\n", &fullPath[0], &loadPath[0], currentLine);
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
							printf("Error loading object \"%s\": Texture wrapper \"%s\" at line %u does not exist.\n", &fullPath[0], &loadPath[0], currentLine);
							rndr.tw = moduleTextureWrapperGetDefault();
						}

					}else{
						printf("Error loading object \"%s\": Could not parse texture wrapper for renderable at line %u.\n", &fullPath[0], currentLine);
						rndr.tw = moduleTextureWrapperGetDefault();
					}

				}else{
					printf("Error loading object \"%s\": Could not parse model for renderable at line %u.\n", &fullPath[0], currentLine);
					rndr.mdl = (model *)cvGet(allModels, 0);
					rndr.tw = moduleTextureWrapperGetDefault();
				}

				/* Add the renderable. */
				tempBuffer = realloc(obj->renderables, (obj->renderableNum+1)*sizeof(renderable));
				if(tempBuffer == NULL){
					/** Memory allocation failure. **/
					objDelete(obj);
					fclose(objInfo);
					return -1;
				}
				tempBuffer[obj->renderableNum] = rndr;
				obj->renderables = tempBuffer;
				++obj->renderableNum;

			}

		}

		fclose(objInfo);

	}else{
		printf("Error loading object \"%s\": Could not open file.\n", &fullPath[0]);
		return 0;
	}

	// If no skeleton was loaded, load the default one.
	if(obj->skl == NULL){
		//printf("Error loading object: No skeleton was loaded.\n");
		obj->skl = (skeleton *)cvGet(allSkeletons, 0);
	}

	// If no renderables were loaded, load the default one.
	if(obj->renderableNum == 0){
		printf("Error loading object \"%s\": No renderables were loaded.\n", &fullPath[0]);
		renderable *tempBuffer = realloc(obj->renderables, (obj->renderableNum+1)*sizeof(renderable));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			objDelete(obj);
			return -1;
		}
		tempBuffer[obj->renderableNum].mdl = (model *)cvGet(allModels, 0);
		tempBuffer[obj->renderableNum].tw = (textureWrapper *)cvGet(allTexWrappers, 0);
		obj->renderables = tempBuffer;
		++obj->renderableNum;
	}

	// If no name was given, generate one based off the file path.
	if(obj->name == NULL || obj->name[0] == '\0'){
		if(obj->name != NULL){
			free(obj->name);
		}
		obj->name = malloc((fileLength+1)*sizeof(char));
		if(obj->name == NULL){
			/** Memory allocation failure. **/
			objDelete(obj);
			return -1;
		}
		memcpy(obj->name, filePath, fileLength);
		obj->name[fileLength] = '\0';
	}

	return 1;

}

void objDelete(object *obj){
	boneIndex_t i;
	if(obj->name != NULL){
		free(obj->name);
	}
	if(obj->animations != NULL){
		free(obj->animations);
	}
	if(obj->skl != NULL){
		if(obj->skeletonBodies != NULL){
			for(i = 0; i < obj->skl->boneNum; ++i){
				physRigidBodyDelete(&obj->skeletonBodies[i]);
			}
			free(obj->skeletonBodies);
		}
		/**if(obj->skeletonBodyFlags != NULL){
			free(obj->skeletonBodyFlags);
		}
		if(obj->skeletonConstraintNum != NULL){
			free(obj->skeletonConstraintNum);
		}**/
		if(obj->skeletonConstraints != NULL){
			for(i = 0; i < obj->skl->boneNum; ++i){
				free(obj->skeletonConstraints[i]);
			}
			free(obj->skeletonConstraints);
		}
		if(obj->skeletonHitboxes != NULL){
			for(i = 0; i < obj->skl->boneNum; ++i){
				hbArrayDelete(&obj->skeletonHitboxes[i]);
			}
			free(obj->skeletonHitboxes);
		}
	}
	if(obj->renderables != NULL){
		free(obj->renderables);
	}
}

return_t objiInit(objInstance *obji){
	obji->base = NULL;
	//rndrConfigInit(&((objInstance *)obji)->tempRndrConfig);
	//((objInstance *)obji)->skl = NULL;
	obji->configuration = NULL;
	obji->stateNum = 0;
	obji->state.skeleton = NULL;
	obji->state.previous = NULL;
	obji->oldestStatePrevious = &obji->state.previous;
	//((objInstance *)obji)->skeletonState[1] = NULL;
	obji->skeletonPhysics = NULL;
	obji->skeletonHitboxes = NULL;
	obji->renderableNum = 0;
	obji->renderables = NULL;
	return skliInit(&obji->skeletonData, NULL, 0);

	/*((object *)obji)->skl = NULL;
	((object *)obji)->configuration = NULL;
	((object *)obji)->skeletonState[0] = NULL;
	((object *)obji)->skeletonState[1] = NULL;
	((object *)obji)->renderableNum = 0;
	((object *)obji)->renderables = NULL;
	((object *)obji)->physicsSimulate = 0;
	((object *)obji)->skeletonPhysics = NULL;
	return skliInit(&((object *)obji)->animationData, 0);** &&
	       kcInit(  &((object *)obji)->animationData, 0) &&
	       hbInit(  &((object *)obji)->animationData, 0);**/
}

/** FIX FOR PHYSICS OBJECTS **/
return_t objiStateCopy(void *o, void *c){

	/* Resize the skeleton state arrays, if necessary, and copy everything over. */
	if(((objInstance *)o)->skeletonData.skl != NULL){

		boneIndex_t i;

		// Check if the skeleton state arrays need to be resized.
		if(((objInstance *)c)->skeletonData.skl == NULL || ((objInstance *)c)->skeletonData.skl->boneNum != ((objInstance *)o)->skeletonData.skl->boneNum){

			// We need to allocate more or less memory so that
			// the memory allocated for both custom states match.
			//const size_t arraySizeS = ((objInstance *)o)->skl->boneNum*sizeof(bone);
			bone *tempBuffer1;
			//bone *tempBuffer2;
			//bone *tempBuffer3;
			physRBInstance *tempBuffer4;
			hbArray *tempBuffer5;

			tempBuffer1 = malloc(3*((objInstance *)o)->skeletonData.skl->boneNum*sizeof(bone));
			if(tempBuffer1 == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			/*tempBuffer2 = malloc(arraySizeS);
			if(tempBuffer2 == NULL){
				** Memory allocation failure. **
				free(tempBuffer1);
				return -1;
			}
			tempBuffer3 = malloc(arraySizeS);
			if(tempBuffer3 == NULL){
				** Memory allocation failure. **
				free(tempBuffer2);
				free(tempBuffer1);
				return -1;
			}*/
			tempBuffer4 = malloc(((objInstance *)o)->skeletonData.skl->boneNum*sizeof(physRBInstance));
			if(tempBuffer4 == NULL){
				/** Memory allocation failure. **/
				//free(tempBuffer3);
				//free(tempBuffer2);
				free(tempBuffer1);
				return -1;
			}
			tempBuffer5 = malloc(((objInstance *)o)->skeletonData.skl->boneNum*sizeof(hbArray));
			if(tempBuffer5 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer4);
				//free(tempBuffer3);
				//free(tempBuffer2);
				free(tempBuffer1);
				return -1;
			}

			if(((objInstance *)c)->configuration != NULL){
				free(((objInstance *)c)->configuration);
			}
			/*if(((objInstance *)c)->skeletonState[0] != NULL){
				free(((objInstance *)c)->skeletonState[0]);
			}
			if(((objInstance *)c)->skeletonState[1] != NULL){
				free(((objInstance *)c)->skeletonState[1]);
			}*/
			if(((objInstance *)c)->skeletonPhysics != NULL){
				free(((objInstance *)c)->skeletonPhysics);
			}
			if(((objInstance *)c)->skeletonHitboxes != NULL){
				free(((objInstance *)c)->skeletonHitboxes);
			}

			((objInstance *)c)->configuration    = tempBuffer1;
			((objInstance *)c)->state.skeleton   = &tempBuffer1[((objInstance *)o)->skeletonData.skl->boneNum];
			//((objInstance *)c)->skeletonState[1] = tempBuffer3;
			((objInstance *)c)->skeletonPhysics  = tempBuffer4;
			((objInstance *)c)->skeletonHitboxes = tempBuffer5;

		}

		// Copy each member of the arrays.
		for(i = 0; i < ((objInstance *)o)->skeletonData.skl->boneNum; ++i){
			if(physRBIStateCopy(&((objInstance *)o)->skeletonPhysics[i], &((objInstance *)c)->skeletonPhysics[i]) == -1){
				/** Memory allocation failure. **/
				break;
			}
			((objInstance *)c)->configuration[i]     = ((objInstance *)o)->configuration[i];
			((objInstance *)c)->state.skeleton[i]    = ((objInstance *)o)->state.skeleton[i];
			//((objInstance *)c)->skeletonState[i+i+1] = ((objInstance *)o)->skeletonState[i+i+1];
			((objInstance *)c)->skeletonHitboxes[i]  = ((objInstance *)o)->skeletonHitboxes[i];
		}
		if(i < ((objInstance *)o)->skeletonData.skl->boneNum){
			/** Memory allocation failure. **/
			while(i > 0){
				--i;
				physRBIDelete(&((objInstance *)c)->skeletonPhysics[i]);
			}
			return -1;
		}

	}else{
		// Skeleton is NULL, free and reset everything.
		if(((objInstance *)c)->configuration != NULL){
			free(((objInstance *)c)->configuration);
			((objInstance *)c)->configuration = NULL;
		}
		if(((objInstance *)c)->state.skeleton != NULL){
			//free(((objInstance *)c)->skeletonState[0]);
			((objInstance *)c)->state.skeleton = NULL;
		}
		/*if(((objInstance *)c)->skeletonState[1] != NULL){
			free(((objInstance *)c)->skeletonState[1]);
			((objInstance *)c)->skeletonState[1] = NULL;
		}*/
		if(((objInstance *)c)->skeletonPhysics != NULL){
			free(((objInstance *)c)->skeletonPhysics);
			((objInstance *)c)->skeletonPhysics = NULL;
		}
		if(((objInstance *)c)->skeletonHitboxes != NULL){
			free(((objInstance *)c)->skeletonHitboxes);
			((objInstance *)c)->skeletonHitboxes = NULL;
		}
	}
	//((objInstance *)c)->skl = ((objInstance *)o)->skl;

	/* Copy the renderables array. */
	if(((objInstance *)o)->renderableNum > 0 && ((objInstance *)o)->renderables != NULL){

		size_t arraySizeR = ((objInstance *)o)->renderableNum*sizeof(rndrInstance);

		// Check if the renderable array needs to be resized.
		if(((objInstance *)c)->renderables == NULL || ((objInstance *)c)->renderableNum != ((objInstance *)o)->renderableNum){

			rndrInstance *tempBuffer = malloc(arraySizeR);
			if(tempBuffer == NULL){
				/** Memory allocation failure. **/
				return -1;
			}

			if(((objInstance *)c)->renderables != NULL){
				free(((objInstance *)c)->renderables);
			}

			((objInstance *)c)->renderables = tempBuffer;

		}

		memcpy(((objInstance *)c)->renderables, ((objInstance *)o)->renderables, arraySizeR);

	}else{
		if(((objInstance *)c)->renderables != NULL){
			free(((objInstance *)c)->renderables);
			((objInstance *)c)->renderables = NULL;
		}
		((objInstance *)c)->renderableNum = 0;
	}

	/* Copy the rest of the members. */
	((objInstance *)c)->base = ((objInstance *)o)->base;
	//rndrConfigStateCopy(&((objInstance *)o)->tempRndrConfig, &((objInstance *)c)->tempRndrConfig);

	/* Copy the more complex data types. */
	return skliStateCopy(&((objInstance *)o)->skeletonData, &((objInstance *)c)->skeletonData);

	/* Copy configuration. *
	rndrConfigStateCopy(&((object *)o)->tempRndrConfig, &((object *)c)->tempRndrConfig);

	* Resize the skeleton state arrays, if necessary, and copy everything over. *
	if(((object *)o)->skl != NULL){
		// Check if the skeleton state arrays need to be resized.
		size_t arraySizeS = ((object *)o)->skl->boneNum*sizeof(bone);
		if(((object *)c)->skl == NULL || ((object *)c)->skl->boneNum != ((object *)o)->skl->boneNum){
			// We need to allocate more or less memory so that
			// the memory allocated for both custom states match.
			bone *tempBuffer3;
			bone *tempBuffer2;
			bone *tempBuffer1 = malloc(arraySizeS);
			if(tempBuffer1 == NULL){
				** Memory allocation failure. **
				return 0;
			}
			tempBuffer2 = malloc(arraySizeS);
			if(tempBuffer2 == NULL){
				** Memory allocation failure. **
				free(tempBuffer1);
				return 0;
			}
			tempBuffer3 = malloc(arraySizeS);
			if(tempBuffer3 == NULL){
				** Memory allocation failure. **
				free(tempBuffer2);
				free(tempBuffer1);
				return 0;
			}
			if(((object *)o)->skeletonPhysics != NULL){
				// Reallocate the physics skeleton.
				size_t arraySizeP = ((object *)o)->skl->boneNum*sizeof(physRBInstance);
				physRBInstance *tempBuffer4 = malloc(arraySizeP);
				if(tempBuffer4 == NULL){
					** Memory allocation failure. **
					free(tempBuffer3);
					free(tempBuffer2);
					free(tempBuffer1);
					return 0;
				}
				if(((object *)c)->skeletonPhysics != NULL){
					free(((object *)c)->skeletonPhysics);
				}
				((object *)c)->skeletonPhysics = tempBuffer4;
				memcpy(((object *)c)->skeletonPhysics, ((object *)o)->skeletonPhysics, arraySizeS);
			}
			if(((object *)c)->configuration != NULL){
				free(((object *)c)->configuration);
			}
			if(((object *)c)->skeletonState[0] != NULL){
				free(((object *)c)->skeletonState[0]);
			}
			if(((object *)c)->skeletonState[1] != NULL){
				free(((object *)c)->skeletonState[1]);
			}
			((object *)c)->configuration    = tempBuffer1;
			((object *)c)->skeletonState[0] = tempBuffer2;
			((object *)c)->skeletonState[1] = tempBuffer3;
		}
		memcpy(((object *)c)->configuration,    ((object *)o)->configuration,    arraySizeS);
		memcpy(((object *)c)->skeletonState[0], ((object *)o)->skeletonState[0], arraySizeS);
		memcpy(((object *)c)->skeletonState[1], ((object *)o)->skeletonState[1], arraySizeS);
	}else{
		((object *)c)->configuration    = NULL;
		((object *)c)->skeletonState[0] = NULL;
		((object *)c)->skeletonState[1] = NULL;
		((object *)c)->skeletonPhysics  = NULL;
	}
	if(((object *)o)->skeletonPhysics == NULL && ((object *)c)->skeletonPhysics != NULL){
		free(((object *)c)->skeletonPhysics);
	}
	((object *)c)->skl = ((object *)o)->skl;
	((object *)c)->physicsSimulate = ((object *)o)->physicsSimulate;

	* Resize the renderables array, if necessary, and copy everything over. *
	if(((object *)c)->renderableNum != ((object *)o)->renderableNum){
		renderable *tempBuffer = malloc(((object *)o)->renderableNum*sizeof(renderable));
		if(tempBuffer == NULL){
			** Memory allocation failure. **
			return 0;
		}
		if(((object *)c)->renderables != NULL){
			free(((object *)c)->renderables);
		}
		((object *)c)->renderables = tempBuffer;
		((object *)c)->renderableNum = ((object *)o)->renderableNum;
	}
	memcpy(((object *)c)->renderables, ((object *)o)->renderables, ((object *)o)->renderableNum*sizeof(renderable));

	* Copy the more complex data types. *
	return skliStateCopy(&((object *)o)->animationData, &((object *)c)->animationData);** &&
	       kcStateCopy(  &((object *)o)->physicsData,   &((object *)c)->physicsData)   &&
	       hbStateCopy(  &((object *)o)->hitboxData,    &((object *)c)->hitboxData);**/
}

void objiResetInterpolation(void *obji){
	//rndrConfigResetInterpolation(&((objInstance *)obji)->tempRndrConfig);
}

void objiDelete(objInstance *obji){
	boneIndex_t i;
	objiState *state = obji->state.previous;
	if(obji->configuration != NULL){
		free(obji->configuration);
	}
	while(state != NULL){
		objiState *next = state->previous;
		memFree(state);
		state = next;
	}
	/*if(((objInstance *)obji)->skeletonState[0] != NULL){
		free(((objInstance *)obji)->skeletonState[0]);
	}
	if(((objInstance *)obji)->skeletonState[1] != NULL){
		free(((objInstance *)obji)->skeletonState[1]);
	}*/
	if(obji->skeletonPhysics != NULL){
		for(i = 0; i < obji->skeletonData.skl->boneNum; ++i){
			physRBIDelete(&obji->skeletonPhysics[i]);
		}
		free(obji->skeletonPhysics);
	}
	if(obji->skeletonHitboxes != NULL){
		for(i = 0; i < obji->skeletonData.skl->boneNum; ++i){
			hbArrayDelete(&obji->skeletonHitboxes[i]);
		}
		free(obji->skeletonHitboxes);
	}
	if(obji->renderables != NULL){
		free(obji->renderables);
	}
	skliDelete(&obji->skeletonData);

	/*if(((object *)obji)->name != NULL){
		free(((object *)obji)->name);
	}
	if(((object *)obji)->configuration != NULL){
		free(((object *)obji)->configuration);
	}
	if(((object *)obji)->skeletonState[0] != NULL){
		free(((object *)obji)->skeletonState[0]);
	}
	if(((object *)obji)->skeletonState[1] != NULL){
		free(((object *)obji)->skeletonState[1]);
	}
	if(((object *)obji)->renderables != NULL){
		for(i = 0; i < ((object *)obji)->renderableNum; ++i){
			rndrDelete(&((object *)obji)->renderables[i]);
		}
		free(((object *)obji)->renderables);
	}
	skliDelete(&((object *)obji)->animationData);*/
	/**kcDelete(&((object *)obji)->physicsData);
	hbDelete(&((object *)obji)->hitboxData);**/
}

return_t objiInstantiate(objInstance *obji, object *base){

	renderableIndex_t j;

	obji->renderables = malloc(base->renderableNum * sizeof(rndrInstance));
	if(obji->renderables == NULL){
		/** Memory allocation failure. **/
		return -1;
	}

	if(skliInit(&obji->skeletonData, base->skl, base->animationCapacity) == -1){
		/** Memory allocation failure. **/
		free(obji->renderables);
		return -1;
	}

	if(base->skl->boneNum != 0){

		boneIndex_t i;

		/* Allocate memory for the object instance. */
		obji->configuration = malloc(3 * base->skl->boneNum * sizeof(bone));
		if(obji->configuration == NULL){
			/** Memory allocation failure. **/
			skliDelete(&obji->skeletonData);
			free(obji->renderables);
			return -1;
		}
		obji->state.skeleton = &obji->configuration[base->skl->boneNum];
		//obji->skeletonState[1] = &obji->configuration[base->skl->boneNum*2];

		/* Allocate memory for and initialize the hitboxes if necessary. */
		if(base->skeletonHitboxes != NULL){

			obji->skeletonHitboxes = malloc(base->skl->boneNum * sizeof(hbArray));
			if(obji->skeletonHitboxes == NULL){
				/** Memory allocation failure. **/
				free(obji->configuration);
				skliDelete(&obji->skeletonData);
				free(obji->renderables);
				return -1;
			}

			for(i = 0; i < base->skl->boneNum; ++i){
				const size_t arraySize = base->skeletonHitboxes[i].hitboxNum * sizeof(hitbox);
				obji->skeletonHitboxes[i].hitboxes = malloc(arraySize);
				if(obji->skeletonHitboxes[i].hitboxes == NULL){
					/** Memory allocation failure. **/
					break;
				}
				obji->skeletonHitboxes[i].hitboxNum = base->skeletonHitboxes[i].hitboxNum;
				memcpy(obji->skeletonHitboxes[i].hitboxes, base->skeletonHitboxes[i].hitboxes, arraySize);
			}

			while(i < base->skl->boneNum){
				/** Memory allocation failure. **/
				while(i > 0){
					--i;
					hbArrayDelete(&obji->skeletonHitboxes[i]);
				}
				free(obji->skeletonHitboxes);
				free(obji->configuration);
				skliDelete(&obji->skeletonData);
				free(obji->renderables);
				return -1;
			}

		}else{
			obji->skeletonHitboxes = NULL;
		}

		/* Allocate memory for and initialize the rigid bodies if necessary. */
		if(base->skeletonBodies != NULL){

			boneIndex_t i;

			obji->skeletonPhysics = malloc(base->skl->boneNum * sizeof(physRBInstance));
			if(obji->skeletonPhysics == NULL){
				/** Memory allocation failure. **/
				free(obji->configuration);
				skliDelete(&obji->skeletonData);
				free(obji->renderables);
				return -1;
			}

			for(i = 0; i < base->skl->boneNum; ++i){
				if(physRBIInstantiate(&obji->skeletonPhysics[i],  &base->skeletonBodies[i], &obji->state.skeleton[i]) == -1){
					/** Memory allocation failure. **/
					break;
				}
				/** Figure out constraint loading. **/
				///obji->skeletonPhysics[i].flags = base->skeletonBodyFlags[i];
				///obji->skeletonPhysics[i].constraintNum = base->skeletonConstraintNum[i];
				obji->skeletonPhysics[i].constraints = base->skeletonConstraints[i];
				boneInit(&obji->configuration[i]);
			}
			if(i < base->skl->boneNum){
				/** Memory allocation failure. **/
				while(i > 0){
					--i;
					physRBIDelete(&obji->skeletonPhysics[i]);
				}
				if(obji->skeletonHitboxes != NULL){
					for(i = 0; i < base->skl->boneNum; ++i){
						hbArrayDelete(&obji->skeletonHitboxes[i]);
					}
					free(obji->skeletonHitboxes);
				}
				free(obji->skeletonPhysics);
				free(obji->configuration);
				skliDelete(&obji->skeletonData);
				free(obji->renderables);
				return -1;
			}

		}else{
			obji->skeletonPhysics = NULL;
			for(i = 0; i < base->skl->boneNum; ++i){
				boneInit(&obji->configuration[i]);
			}
		}

	}

	for(j = 0; j < base->renderableNum; ++j){
		rndriInstantiate(&obji->renderables[j], &base->renderables[j]);
	}

	/** Remove the following line. **/
	//rndrConfigInit(&obji->tempRndrConfig);
	obji->base = base;
	//obji->skl = base->skl;
	obji->renderableNum = base->renderableNum;
	obji->stateNum = 0;
	obji->state.previous = NULL;
	obji->oldestStatePrevious = &obji->state.previous;
	return 1;

}

static __FORCE_INLINE__ return_t objiStateAllocate(objInstance *obji){
	objiState *state = memAllocate(sizeof(objiState) + obji->base->skl->boneNum * sizeof(bone));
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
	while(obji->stateNum < obji->base->stateNum){
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
	rndrInstance *tempBuffer = realloc(obji->renderables, (obji->renderableNum+1)*sizeof(rndrInstance));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	obji->renderables = tempBuffer;
	obji->renderables[obji->renderableNum].mdl = mdl;
	twiInit(&obji->renderables[obji->renderableNum].twi, tw);
	++obji->renderableNum;
	return 1;
}

return_t objiNewRenderableFromBase(objInstance *obji, const renderable *rndr){
	/* Allocate room for a new renderable and initialize it. */
	rndrInstance *tempBuffer = realloc(obji->renderables, (obji->renderableNum+1)*sizeof(rndrInstance));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	obji->renderables = tempBuffer;
	rndriInstantiate(&obji->renderables[obji->renderableNum], rndr);
	++obji->renderableNum;
	return 1;
}

return_t objiNewRenderableFromInstance(objInstance *obji, const rndrInstance *rndr){
	/* Allocate room for a new renderable and initialize it. */
	rndrInstance *tempBuffer = realloc(obji->renderables, (obji->renderableNum+1)*sizeof(rndrInstance));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	obji->renderables = tempBuffer;
	obji->renderables[obji->renderableNum].mdl = rndr->mdl;
	twiInit(&obji->renderables[obji->renderableNum].twi, rndr->twi.tw);
	++obji->renderableNum;
	return 1;
}

return_t objiDeleteRenderable(objInstance *obji, const renderableIndex_t id){
	/* Remove a specified renderable. */
	if(obji->renderableNum > 0){
		renderableIndex_t i, write;
		renderableIndex_t arraySize = (obji->renderableNum-1)*sizeof(rndrInstance);
		rndrInstance *tempBuffer = malloc(arraySize);
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		if(id >= obji->renderableNum-1){
			memcpy(tempBuffer, obji->renderables, arraySize);
		}else{
			for(i = 0; i < obji->renderableNum; ++i){
				write = i - (i > id);
				tempBuffer[write] = obji->renderables[i];
			}
		}
		free(obji->renderables);
		obji->renderables = tempBuffer;
		--obji->renderableNum;
	}
	return 1;
}

return_t objiInitSkeleton(objInstance *obji, skeleton *skl){
	boneIndex_t i;
	//size_t arraySizeS = skl->boneNum*sizeof(bone);
	//bone *tempBuffer3;
	//bone *tempBuffer2;
	bone *tempBuffer = malloc(3 * skl->boneNum * sizeof(bone));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	/*tempBuffer2 = malloc(arraySizeS);
	if(tempBuffer2 == NULL){
		** Memory allocation failure. **
		free(tempBuffer1);
		return -1;
	}
	tempBuffer3 = malloc(arraySizeS);
	if(tempBuffer3 == NULL){
		** Memory allocation failure. **
		free(tempBuffer2);
		free(tempBuffer1);
		return -1;
	}*/
	if(obji->configuration != NULL){
		free(obji->configuration);
	}
	if(obji->skeletonPhysics != NULL){
		free(obji->skeletonPhysics);
	}
	if(obji->skeletonHitboxes != NULL){
		free(obji->skeletonHitboxes);
	}
	//obji->skl = skl;
	obji->configuration = tempBuffer;
	obji->state.skeleton = &tempBuffer[skl->boneNum];
	//obji->skeletonState[1] = &tempBuffer[obji->base->skl->boneNum*2];
	//obji->skeletonState[0] = tempBuffer2;
	//obji->skeletonState[1] = tempBuffer3;
	for(i = 0; i < skl->boneNum; ++i){
		boneInit(&obji->configuration[i]);
	}
	skliDelete(&obji->skeletonData);
	obji->skeletonData.skl = skl;
	return 1;
}

/*return_t objInitPhysics(object *obji){
	if(obji->skl != NULL){
		size_t i;
		physRBInstance *tempBuffer = malloc(obji->skl->boneNum*sizeof(physRBInstance));
		if(tempBuffer == NULL){
			** Memory allocation failure. **
			return 0;
		}
		obji->skeletonPhysics = tempBuffer;
		for(i = 0; i < obji->skl->boneNum; ++i){
			physRBIInit(&obji->skeletonPhysics[i]);
		}
	}
	return 1;
}*/

void objiBoneSetPhysicsFlags(objInstance *obji, const boneIndex_t boneID, const flags_t flags){

	if(obji->skeletonPhysics != NULL){

		obji->skeletonPhysics[boneID].flags = flags;

		if(flagsAreSet(flags, PHYS_BODY_SIMULATE)){

			// Replace PHYS_BODY_SIMULATE with PHYS_BODY_INITIALIZE.
			flagsUnset(obji->skeletonPhysics[boneID].flags, PHYS_BODY_SIMULATE);
			flagsSet(obji->skeletonPhysics[boneID].flags, PHYS_BODY_INITIALIZE);

		}

	}

}

sklAnim *objiGetAnimation(objInstance *obji, const animIndex_t id){
	if(obji->base != NULL){
		if(obji->base->animationNum > id){
			return obji->base->animations[id];
		}
	}
	return NULL;
}

sklAnim *objiFindAnimation(const objInstance *obji, const char *name){
	if(obji->base != NULL){
		animIndex_t i;
		for(i = 0; i < obji->base->animationNum; ++i){
			if(strcmp(obji->base->animations[i]->name, name) == 0){
				return obji->base->animations[i];
			}
		}
	}
	return NULL;
}

__FORCE_INLINE__ void objiSetAnimationType(objInstance *obji, const animIndex_t slot, const flags_t additive){
	skliSetAnimationType(&obji->skeletonData, slot, additive);
}

__FORCE_INLINE__ return_t objiChangeAnimation(objInstance *obji, const animIndex_t slot, sklAnim *anim, const frameIndex_t frame, const float blendTime){
	return skliChangeAnimation(&obji->skeletonData, slot, anim, frame, blendTime);
}

__FORCE_INLINE__ void objiClearAnimation(objInstance *obji, const animIndex_t slot){
	skliClearAnimation(&obji->skeletonData, slot);
}

__FORCE_INLINE__ void objiApplyLinearForce(objInstance *obji, const boneIndex_t boneID, const vec3 *F){
	physRBIApplyLinearForce(&obji->skeletonPhysics[boneID], F);
}

__FORCE_INLINE__ void objiApplyAngularForceGlobal(objInstance *obji, const boneIndex_t boneID, const vec3 *F, const vec3 *r){
	physRBIApplyAngularForceGlobal(&obji->skeletonPhysics[boneID], F, r);
}

__FORCE_INLINE__ void objiApplyForceGlobal(objInstance *obji, const boneIndex_t boneID, const vec3 *F, const vec3 *r){
	physRBIApplyForceGlobal(&obji->skeletonPhysics[boneID], F, r);
}

/*void objiApplyForceAtGlobalPoint(objInstance *obji, const size_t boneID, const vec3 *F, const vec3 *r){
	physRBIApplyForceAtGlobalPoint(&obji->skeletonPhysics[boneID], F, r);
}
void objiAddLinearVelocity(objInstance *obji, const size_t boneID, const float x, const float y, const float z){
	physRBIAddLinearVelocity(&obji->skeletonPhysics[boneID], x, y, z);
}
void objiApplyLinearImpulse(objInstance *obji, const size_t boneID, const float x, const float y, const float z){
	physRBIApplyLinearImpulse(&obji->skeletonPhysics[boneID], x, y, z);
}
void objiAddAngularVelocity(objInstance *obji, const size_t boneID, const float angle, const float x, const float y, const float z){
	physRBIAddAngularVelocity(&obji->skeletonPhysics[boneID], angle, x, y, z);
}*/

return_t objiUpdate(objInstance *obji, physicsSolver *solver, const float elapsedTime, const float dt){

	return_t r = 1;

	boneIndex_t i;
	renderableIndex_t j;

	// If we can create a new previous state, do so.
	if(obji->stateNum < obji->base->stateNum){
		r = objiStateAllocate(obji);
	}

	// Update each skeletal animation.
	skliUpdateAnimations(&obji->skeletonData, elapsedTime, 1.f);

	/* Update the object's skeleton. */
	for(i = 0; i < obji->skeletonData.skl->boneNum; ++i){

		const return_t isRoot = (i == obji->skeletonData.skl->bones[i].parent) ||
		                        (obji->skeletonData.skl->bones[i].parent >= obji->skeletonData.skl->boneNum);

		// Update the previous states.
		objiStateCopyBone(obji, i);

		if(obji->skeletonPhysics != NULL && flagsAreSet(obji->skeletonPhysics[i].flags, (PHYS_BODY_INITIALIZE | PHYS_BODY_SIMULATE))){

			/*
			** Simulate the body attached to the bone.
			*/

			if(flagsAreSet(obji->skeletonPhysics[i].flags, PHYS_BODY_INITIALIZE)){

				// Generate a new animated bone state.
				//obji->skeletonState[0][i] = obji->configuration[i];
				obji->state.skeleton[i] = obji->configuration[i];
				skliGenerateBoneState(&obji->skeletonData, i, obji->skeletonData.skl->bones[i].name, &obji->state.skeleton[i]);
				//skliGenerateBoneState(&obji->skeletonData, &obji->skeletonState[i2], i);
				//skliGenerateBoneState(&obji->animationData, &obji->skeletonState[0][i], i);

				// Set the body's state to that of its bone when the bone starts being simulated.
				/** This should be removed eventually if possible. **/
				flagsUnset(obji->skeletonPhysics[i].flags, PHYS_BODY_INITIALIZE);
				flagsSet(obji->skeletonPhysics[i].flags, PHYS_BODY_SIMULATE);
				//obji->skeletonPhysics[i].configuration[0] = obji->skeletonState[0][i];
				//obji->skeletonPhysics[i].configuration[1] = obji->skeletonState[1][i];

			}

			/** TEMPORARILY ADD GRAVITY. **/
			vec3 force; vec3Set(&force, 0.f, -98.0665f, 0.f);
			objiApplyLinearForce(obji, 0, &force);

			if(flagsAreSet(obji->skeletonPhysics[i].flags, PHYS_BODY_COLLIDE)){
				// Only update the body's collision mesh
				// and add it to the solver if it is set
				// to interact with other bodies.
				/** Only update AABB? **/
				physRBIIntegrateLeapfrogVelocity(&obji->skeletonPhysics[i], dt);
				physRBIUpdateCollisionMesh(&obji->skeletonPhysics[i]);
				physSolverAddBody(solver, &obji->skeletonPhysics[i]);
			}else{
				// If the body is not interacting with any
				// other bodies, integrate everything
				// instead of just the velocity.
				physRBIIntegrateLeapfrog(&obji->skeletonPhysics[i], dt);
			}

			/** TEMPORARILY SET THE BONE STATE. **/
			//obji->skeletonState[0][i] = obji->skeletonPhysics[i].configuration[0];

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
			obji->state.skeleton[i] = obji->configuration[i];
			skliGenerateBoneState(&obji->skeletonData, i, obji->skeletonData.skl->bones[i].name, &obji->state.skeleton[i]);
			//skliGenerateBoneState(&obji->skeletonData, &obji->skeletonState[i2], i);
			//skliGenerateBoneState(&obji->animationData, &obji->skeletonState[0][i], i);

			// Apply the object skeleton's bind offsets.
			/*boneTransformAppendPositionVec(&obji->skeletonState[0][i],
			                               obji->skl->bones[i].defaultState.position.x,
			                               obji->skl->bones[i].defaultState.position.y,
			                               obji->skl->bones[i].defaultState.position.z,
			                               &obji->skeletonState[0][i].position);*/
			boneTransformAppendPositionVec(&obji->state.skeleton[i],
			                               obji->skeletonData.skl->bones[i].defaultState.position.x,
			                               obji->skeletonData.skl->bones[i].defaultState.position.y,
			                               obji->skeletonData.skl->bones[i].defaultState.position.z,
			                               &obji->state.skeleton[i].position);

			// Apply the parent's transformations to each bone.
			if(!isRoot){
				//boneTransformAppend(&obji->skeletonState[0][obji->skl->bones[i].parent], &obji->skeletonState[0][i], &obji->skeletonState[0][i]);
				const boneIndex_t p = obji->skeletonData.skl->bones[i].parent;
				boneTransformAppend(&obji->state.skeleton[p], &obji->state.skeleton[i], &obji->state.skeleton[i]);
			}

			if(obji->skeletonPhysics != NULL && flagsAreSet(obji->skeletonPhysics[i].flags, PHYS_BODY_COLLIDE)){
				// Only update the body's collision mesh
				// and add it to the solver if it is set
				// to interact with other bodies.
				/** Only update AABB? **/
				physRBIUpdateCollisionMesh(&obji->skeletonPhysics[i]);
				physSolverAddBody(solver, &obji->skeletonPhysics[i]);
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

	/* Update each of the object's texture wrappers. */
	for(j = 0; j < obji->renderableNum; ++j){
		rndriUpdate(&obji->renderables[j], elapsedTime);
	}

	return r;

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

	if(obji->renderableNum > 0){

		renderableIndex_t i;
		for(i = 0; i < obji->renderableNum; ++i){
			if(twiContainsTranslucency(&obji->renderables[i].twi)){

				// The object contains translucency.
				return GFX_RENDER_GROUP_TRANSLUCENT;

			}else{

				const float alpha = floatLerp(
					obji->renderables[i].alphaPrevious,
					obji->renderables[i].alpha,
					interpT
				);
				if(alpha > 0.f && alpha < 1.f){
					// The object contains translucency.
					return GFX_RENDER_GROUP_TRANSLUCENT;
				}
				totalAlpha += alpha;

			}
		}

	}

	if(totalAlpha == 0.f){
		// The model is fully transparent.
		return GFX_RENDER_GROUP_UNKNOWN;
	}

	// The model is fully opaque.
	return GFX_RENDER_GROUP_OPAQUE;

}

void objiGenerateSprite(const objInstance *obji, const renderableIndex_t rndr, const float interpT, const float *texFrag, vertex *vertices){

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
	transform.scale.x *= twiGetFrameWidth(&obji->renderables[rndr].twi) * twiGetTexWidth(&obji->renderables[rndr].twi);
	transform.scale.y *= twiGetFrameHeight(&obji->renderables[rndr].twi) * twiGetTexHeight(&obji->renderables[rndr].twi);
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
