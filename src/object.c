#include "object.h"
#include "helpersMisc.h"
#include <stdio.h>

void objInit(object *obj){
	obj->name = NULL;
	obj->skl = NULL;
	obj->animationNum = 0;
	obj->animations = NULL;
	obj->animationCapacity = 0;
	obj->skeletonBodies = NULL;
	obj->skeletonBodyFlags = NULL;
	obj->skeletonConstraints = NULL;
	obj->skeletonHitboxes = NULL;
	obj->renderableNum = 0;
	obj->renderables = 0;
}

signed char objLoad(object *obj, const char *prgPath, const char *filePath,
                    cVector *allTextures, cVector *allTexWrappers, cVector *allSkeletons,
                    cVector *allSklAnimations, cVector *allModels){

	objInit(obj);

	unsigned int currentLine = 0;  // Current file line being read.


	const size_t pathLen = strlen(prgPath);
	const size_t fileLen = strlen(filePath);
	char *fullPath = malloc((pathLen+fileLen+1) * sizeof(char));
	if(fullPath == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	memcpy(fullPath, prgPath, pathLen);
	memcpy(fullPath+pathLen, filePath, fileLen);
	fullPath[pathLen+fileLen] = '\0';
	FILE *objInfo = fopen(fullPath, "r");
	char lineFeed[1024];
	char *line;
	size_t lineLength;

	if(objInfo != NULL){
		while(fileParseNextLine(objInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			size_t i;
			++currentLine;

			// Name
			if(lineLength >= 6 && strncmp(line, "name ", 5) == 0){
				if(line[5] == '"' && line[lineLength-1] == '"'){
					++line;
					lineLength -= 2;
				}
				obj->name = malloc((lineLength-4) * sizeof(char));
				if(obj->name == NULL){
					/** Memory allocation failure. **/
					objDelete(obj);
					free(fullPath);
					fclose(objInfo);
					return -1;
				}
				strncpy(obj->name, line+5, lineLength-5);
				obj->name[lineLength-5] = '\0';


			// Skeleton
			}else if(lineLength >= 10 && strncmp(line, "skeleton ", 9) == 0){

				signed char skipLoad = 0;
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
				char *sklPath = malloc((pathLength+1) * sizeof(char));
				if(sklPath == NULL){
					/** Memory allocation failure. **/
					objDelete(obj);
					free(fullPath);
					fclose(objInfo);
					return -1;
				}
				strncpy(sklPath, line+pathBegin, pathLength);
				sklPath[pathLength] = '\0';

				/** Should the allSkeletons vector be used? It's not as nice or as modular
				but if the assetHandler system is decided on it'll probably be better. **/
				// Look for skeleton name in allSkeletons.
				for(i = 0; i < allSkeletons->size; ++i){
					skeleton *tempSkl = (skeleton *)cvGet(allSkeletons, i);
					if(strcmp(sklPath, tempSkl->name) == 0){
						obj->skl = (skeleton *)cvGet(allSkeletons, i);
						skipLoad = 1;
						break;
					}
				}
				if(!skipLoad){
					skeleton tempSkl;
					signed char r = sklLoad(&tempSkl, prgPath, sklPath);
					if(r == -1){
						/** Memory allocation failure. **/
						free(sklPath);
						objDelete(obj);
						free(fullPath);
						fclose(objInfo);
						return -1;
					}else if(r > 0){
						cvPush(allSkeletons, (void *)&tempSkl, sizeof(tempSkl));  // Add it to allSkeletons.
						obj->skl = (skeleton *)cvGet(allSkeletons, allSkeletons->size-1);
					}
				}

				// Use the default skeleton if no skeleton was loaded.
				if(obj->skl == NULL){
					printf("Error loading object \"%s\": Skeleton \"%s\" at line %u does not exist.\n", fullPath, sklPath, currentLine);
					obj->skl = (skeleton *)cvGet(allSkeletons, 0);
				}
				free(sklPath);


			// Physics
			}else if(lineLength >= 17 && strncmp(line, "skeletonPhysics ", 16) == 0){
				if(obj->skl != NULL){
					/** Load physics. **/

				}else{
					printf("Error loading object \"%s\": Cannot load kinematics chain at line %u when no skeleton has been specified.\n", fullPath, currentLine);
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

				sklAnim *sklaPointer = NULL;
				signed char skipLoad = 0;
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
				char *animPath = malloc((pathLength+1) * sizeof(char));
				if(animPath == NULL){
					/** Memory allocation failure. **/
					objDelete(obj);
					free(fullPath);
					fclose(objInfo);
					return -1;
				}
				strncpy(animPath, line+pathBegin, pathLength);
				animPath[pathLength] = '\0';

				/** Should the allSklAnimations vector be used? It's not as nice or as modular
				but if the assetHandler system is decided on it'll probably be better. **/
				// Look for animation name in allSklAnimations.
				for(i = 0; i < allSklAnimations->size; ++i){
					sklaPointer = (sklAnim *)cvGet(allSklAnimations, i);
					if(strcmp(animPath, sklaPointer->name) == 0){
						skipLoad = 1;
						break;
					}
				}
				if(!skipLoad){
					sklAnim tempSkla;
					signed char r = sklaLoad(&tempSkla, prgPath, animPath);
					if(r == -1){
						/** Memory allocation failure. **/
						free(animPath);
						objDelete(obj);
						free(fullPath);
						fclose(objInfo);
						return -1;
					}else if(r > 0){
						cvPush(allSklAnimations, (void *)&tempSkla, sizeof(tempSkla));  // Add it to allSklAnimations.
						sklaPointer = (sklAnim *)cvGet(allSklAnimations, allSklAnimations->size-1);
					}
				}

				if(sklaPointer != NULL){

					// Allocate room for the new animation pointer if necessary.
					if(obj->animationNum == obj->animationCapacity){
						size_t tempCapacity = obj->animationCapacity + 1;
						sklAnim **tempBuffer = realloc(obj->animations, tempCapacity*sizeof(sklAnim *));
						if(tempBuffer == NULL){
							/** Memory allocation failure. **/
							free(animPath);
							objDelete(obj);
							free(fullPath);
							fclose(objInfo);
							return -1;
						}
						obj->animationCapacity = tempCapacity;
						obj->animations = tempBuffer;
					}
					obj->animations[obj->animationNum] = sklaPointer;
					++obj->animationNum;

				}else{
					printf("Error loading object \"%s\": Skeletal animation \"%s\" at line %u does not exist.\n", fullPath, animPath, currentLine);
				}
				free(animPath);


			// Animation capacity
			}else if(lineLength >= 20 && strncmp(line, "animationCapacity ", 19) == 0){
				obj->animationCapacity = strtoul(line+19, NULL, 0);


			// Renderable
			}else if(lineLength >= 14 && strncmp(line, "renderable ", 11) == 0){

				/** Load model and texture. **/
				renderable *tempBuffer;

				renderable rndr;
				char *loadPath;
				signed char skipLoad = 0;

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
					loadPath = malloc(((mdlPathLength > texPathLength ? mdlPathLength : texPathLength)+1) * sizeof(char));
					if(loadPath == NULL){
						/** Memory allocation failure. **/
						objDelete(obj);
						free(fullPath);
						fclose(objInfo);
						return -1;
					}
					strncpy(loadPath, line+mdlPathBegin, mdlPathLength);
					loadPath[mdlPathLength] = '\0';
					/** Should the allModels vector be used? It's not as nice or as modular
					but if the assetHandler system is decided on it'll probably be better. **/
					// Look for model name in allModels.
					for(i = 0; i < allModels->size; ++i){
						model *tempMdl = (model *)cvGet(allModels, i);
						if(strcmp(loadPath, tempMdl->name) == 0){
							rndr.mdl = (model *)cvGet(allModels, i);
							skipLoad = 1;
							break;
						}
					}
					if(!skipLoad){
						model tempMdl;
						signed char r = mdlLoad(&tempMdl, prgPath, loadPath, allSkeletons);
						if(r == -1){
							/** Memory allocation failure. **/
							free(loadPath);
							objDelete(obj);
							free(fullPath);
							fclose(objInfo);
							return -1;
						}else if(r > 0){
							cvPush(allModels, (void *)&tempMdl, sizeof(tempMdl));  // Add it to allModels.
							rndr.mdl = (model *)cvGet(allModels, allModels->size-1);
						}
					}
					// Use the default model if no model was loaded.
					if(rndr.mdl == NULL){
						printf("Error loading object \"%s\": Model \"%s\" at line %u does not exist.\n", fullPath, loadPath, currentLine);
						rndr.mdl = (model *)cvGet(allModels, 0);
					}

					if(texPathLength > 0){

						skipLoad = 0;

						/* Load the texture wrapper. */
						strncpy(loadPath, line+texPathBegin, texPathLength);
						loadPath[texPathLength] = '\0';
						/** Should the allTexWrappers vector be used? It's not as nice or as modular
						but if the assetHandler system is decided on it'll probably be better. **/
						// Look for texture wrapper name in allTexWrappers.
						for(i = 0; i < allTexWrappers->size; ++i){
							textureWrapper *tempTw = (textureWrapper *)cvGet(allTexWrappers, i);
							if(strcmp(loadPath, tempTw->name) == 0){
								rndr.tw = (textureWrapper *)cvGet(allTexWrappers, i);
								skipLoad = 1;
								break;
							}
						}
						if(!skipLoad){
							textureWrapper tempTw;
							signed char r = twLoad(&tempTw, prgPath, loadPath, allTextures);
							if(r == -1){
								/** Memory allocation failure. **/
								free(loadPath);
								objDelete(obj);
								free(fullPath);
								fclose(objInfo);
								return -1;
							}else if(r > 0){
								cvPush(allTexWrappers, (void *)&tempTw, sizeof(tempTw));  // Add it to allTexWrappers.
								rndr.tw = (textureWrapper *)cvGet(allTexWrappers, allTexWrappers->size-1);
							}
						}
						// Use the default texture wrapper if no texture wrapper was loaded.
						if(rndr.tw == NULL){
							printf("Error loading object \"%s\": Texture wrapper \"%s\" at line %u does not exist.\n", fullPath, loadPath, currentLine);
							rndr.tw = (textureWrapper *)cvGet(allTexWrappers, 0);
						}

					}else{
						printf("Error loading object \"%s\": Could not parse texture wrapper for renderable at line %u.\n", fullPath, currentLine);
						rndr.tw = (textureWrapper *)cvGet(allTexWrappers, 0);
					}
					free(loadPath);

				}else{
					printf("Error loading object \"%s\": Could not parse model for renderable at line %u.\n", fullPath, currentLine);
					rndr.mdl = (model *)cvGet(allModels, 0);
					rndr.tw = (textureWrapper *)cvGet(allTexWrappers, 0);
				}

				/* Add the renderable. */
				tempBuffer = realloc(obj->renderables, (obj->renderableNum+1)*sizeof(renderable));
				if(tempBuffer == NULL){
					/** Memory allocation failure. **/
					objDelete(obj);
					free(fullPath);
					fclose(objInfo);
					return -1;
				}
				tempBuffer[obj->renderableNum] = rndr;
				obj->renderables = tempBuffer;
				++obj->renderableNum;

			}

		}

		fclose(objInfo);
		free(fullPath);

	}else{
		printf("Error loading object \"%s\": Could not open file.\n", fullPath);
		free(fullPath);
		return 0;
	}

	// If no skeleton was loaded, load the default one.
	if(obj->skl == NULL){
		//printf("Error loading object: No skeleton was loaded.\n");
		obj->skl = (skeleton *)cvGet(allSkeletons, 0);
	}

	// If no renderables were loaded, load the default one.
	if(obj->renderableNum == 0){
		printf("Error loading object \"%s\": No renderables were loaded.\n", fullPath);
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
		obj->name = malloc((fileLen+1)*sizeof(char));
		if(obj->name == NULL){
			/** Memory allocation failure. **/
			objDelete(obj);
			return -1;
		}
		memcpy(obj->name, filePath, fileLen);
		obj->name[fileLen] = '\0';
	}

	return 1;

}

void objDelete(object *obj){
	size_t i;
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
		if(obj->skeletonBodyFlags != NULL){
			free(obj->skeletonBodyFlags);
		}
		if(obj->skeletonConstraints != NULL){
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

signed char objiInit(void *obji){
	((objInstance *)obji)->base = NULL;
	rndrConfigInit(&((objInstance *)obji)->tempRndrConfig);
	((objInstance *)obji)->skl = NULL;
	((objInstance *)obji)->configuration = NULL;
	((objInstance *)obji)->skeletonState[0] = NULL;
	((objInstance *)obji)->skeletonState[1] = NULL;
	((objInstance *)obji)->physicsSimulate = 0;
	((objInstance *)obji)->skeletonPhysics = NULL;
	((objInstance *)obji)->skeletonHitboxes = NULL;
	((objInstance *)obji)->renderableNum = 0;
	((objInstance *)obji)->renderables = NULL;
	return skliInit(&((objInstance *)obji)->animationData, 0);

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
signed char objiStateCopy(void *o, void *c){

	size_t i;

	/* Resize the skeleton state arrays, if necessary, and copy everything over. */
	if(((objInstance *)o)->skl != NULL){

		// Check if the skeleton state arrays need to be resized.
		if(((object *)c)->skl == NULL || ((object *)c)->skl->boneNum != ((object *)o)->skl->boneNum){

			// We need to allocate more or less memory so that
			// the memory allocated for both custom states match.
			size_t arraySizeS = ((objInstance *)o)->skl->boneNum*sizeof(bone);
			bone *tempBuffer1;
			bone *tempBuffer2;
			bone *tempBuffer3;
			physRBInstance *tempBuffer4;
			hbArray *tempBuffer5;

			tempBuffer1 = malloc(arraySizeS);
			if(tempBuffer1 == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			tempBuffer2 = malloc(arraySizeS);
			if(tempBuffer2 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer1);
				return -1;
			}
			tempBuffer3 = malloc(arraySizeS);
			if(tempBuffer3 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer2);
				free(tempBuffer1);
				return -1;
			}
			tempBuffer4 = malloc(((objInstance *)o)->skl->boneNum*sizeof(physRBInstance));
			if(tempBuffer4 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer3);
				free(tempBuffer2);
				free(tempBuffer1);
				return -1;
			}
			tempBuffer5 = malloc(((objInstance *)o)->skl->boneNum*sizeof(hbArray));
			if(tempBuffer5 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer4);
				free(tempBuffer3);
				free(tempBuffer2);
				free(tempBuffer1);
				return -1;
			}

			if(((objInstance *)c)->configuration != NULL){
				free(((objInstance *)c)->configuration);
			}
			if(((objInstance *)c)->skeletonState[0] != NULL){
				free(((objInstance *)c)->skeletonState[0]);
			}
			if(((objInstance *)c)->skeletonState[1] != NULL){
				free(((objInstance *)c)->skeletonState[1]);
			}
			if(((objInstance *)c)->skeletonPhysics != NULL){
				free(((objInstance *)c)->skeletonPhysics);
			}
			if(((objInstance *)c)->skeletonHitboxes != NULL){
				free(((objInstance *)c)->skeletonHitboxes);
			}

			((objInstance *)c)->configuration    = tempBuffer1;
			((objInstance *)c)->skeletonState[0] = tempBuffer2;
			((objInstance *)c)->skeletonState[1] = tempBuffer3;
			((objInstance *)c)->skeletonPhysics  = tempBuffer4;
			((objInstance *)c)->skeletonHitboxes = tempBuffer5;

		}

		// Copy each member of the arrays.
		for(i = 0; i < ((objInstance *)o)->skl->boneNum; ++i){
			if(physRBIStateCopy(&((objInstance *)o)->skeletonPhysics[i], &((objInstance *)c)->skeletonPhysics[i]) == -1){
				/** Memory allocation failure. **/
				break;
			}
			((objInstance *)c)->configuration[i]    = ((objInstance *)o)->configuration[i];
			((objInstance *)c)->skeletonState[0][i] = ((objInstance *)o)->skeletonState[0][i];
			((objInstance *)c)->skeletonState[1][i] = ((objInstance *)o)->skeletonState[1][i];
			((objInstance *)c)->skeletonHitboxes[i] = ((objInstance *)o)->skeletonHitboxes[i];
		}
		if(i < ((objInstance *)o)->skl->boneNum){
			/** Memory allocation failure. **/
			while(i > 0){
				--i;
				physRBIDelete(&((objInstance *)c)->skeletonPhysics[i]);
			}
			return -1;
		}
		((objInstance *)c)->physicsSimulate = ((objInstance *)o)->physicsSimulate;

	}else{
		// Skeleton is NULL, free and reset everything.
		((objInstance *)c)->physicsSimulate = 0;
		if(((objInstance *)c)->configuration != NULL){
			free(((objInstance *)c)->configuration);
			((objInstance *)c)->configuration = NULL;
		}
		if(((objInstance *)c)->skeletonState[0] != NULL){
			free(((objInstance *)c)->skeletonState[0]);
			((objInstance *)c)->skeletonState[0] = NULL;
		}
		if(((objInstance *)c)->skeletonState[1] != NULL){
			free(((objInstance *)c)->skeletonState[1]);
			((objInstance *)c)->skeletonState[1] = NULL;
		}
		if(((objInstance *)c)->skeletonPhysics != NULL){
			free(((objInstance *)c)->skeletonPhysics);
			((objInstance *)c)->skeletonPhysics = NULL;
		}
		if(((objInstance *)c)->skeletonHitboxes != NULL){
			free(((objInstance *)c)->skeletonHitboxes);
			((objInstance *)c)->skeletonHitboxes = NULL;
		}
	}
	((objInstance *)c)->skl = ((objInstance *)o)->skl;

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
	rndrConfigStateCopy(&((objInstance *)o)->tempRndrConfig, &((objInstance *)c)->tempRndrConfig);

	/* Copy the more complex data types. */
	return skliStateCopy(&((objInstance *)o)->animationData, &((objInstance *)c)->animationData);

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
	rndrConfigResetInterpolation(&((objInstance *)obji)->tempRndrConfig);
}

void objiDelete(void *obji){
	size_t i;
	if(((objInstance *)obji)->configuration != NULL){
		free(((objInstance *)obji)->configuration);
	}
	if(((objInstance *)obji)->skeletonState[0] != NULL){
		free(((objInstance *)obji)->skeletonState[0]);
	}
	if(((objInstance *)obji)->skeletonState[1] != NULL){
		free(((objInstance *)obji)->skeletonState[1]);
	}
	if(((objInstance *)obji)->skeletonPhysics != NULL){
		for(i = 0; i < ((objInstance *)obji)->skl->boneNum; ++i){
			physRBIDelete(&((objInstance *)obji)->skeletonPhysics[i]);
		}
		free(((objInstance *)obji)->skeletonPhysics);
	}
	if(((objInstance *)obji)->skeletonHitboxes != NULL){
		for(i = 0; i < ((objInstance *)obji)->skl->boneNum; ++i){
			hbArrayDelete(&((objInstance *)obji)->skeletonHitboxes[i]);
		}
		free(((objInstance *)obji)->skeletonHitboxes);
	}
	if(((objInstance *)obji)->renderables != NULL){
		free(((objInstance *)obji)->renderables);
	}
	skliDelete(&((objInstance *)obji)->animationData);

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

signed char objiInstantiate(objInstance *obji, object *base){

	size_t i;

	obji->renderables = malloc(base->renderableNum * sizeof(rndrInstance));
	if(obji->renderables == NULL){
		/** Memory allocation failure. **/
		return -1;
	}

	if(skliInit(&obji->animationData, base->animationCapacity) == -1){
		/** Memory allocation failure. **/
		free(obji->renderables);
		return -1;
	}

	if(base->skl->boneNum != 0){

		size_t arraySize = base->skl->boneNum * sizeof(bone);

		/* Allocate memory for the object instance. */
		obji->configuration = malloc(arraySize);
		if(obji->configuration == NULL){
			/** Memory allocation failure. **/
			skliDelete(&obji->animationData);
			free(obji->renderables);
			return -1;
		}
		obji->skeletonState[0] = malloc(arraySize);
		if(obji->skeletonState[0] == NULL){
			/** Memory allocation failure. **/
			free(obji->configuration);
			skliDelete(&obji->animationData);
			free(obji->renderables);
			return -1;
		}
		obji->skeletonState[1] = malloc(arraySize);
		if(obji->skeletonState[1] == NULL){
			/** Memory allocation failure. **/
			free(obji->skeletonState[0]);
			free(obji->configuration);
			skliDelete(&obji->animationData);
			free(obji->renderables);
			return -1;
		}

		/* Allocate memory for and initialize the hitboxes if necessary. */
		if(base->skeletonHitboxes != NULL){

			obji->skeletonHitboxes = malloc(base->skl->boneNum * sizeof(hbArray));
			if(obji->skeletonHitboxes == NULL){
				/** Memory allocation failure. **/
				free(obji->skeletonState[1]);
				free(obji->skeletonState[0]);
				free(obji->configuration);
				skliDelete(&obji->animationData);
				free(obji->renderables);
				return -1;
			}

			for(i = 0; i < base->skl->boneNum; ++i){
				arraySize = base->skeletonHitboxes[i].hitboxNum * sizeof(hitbox);
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
				free(obji->skeletonState[1]);
				free(obji->skeletonState[0]);
				free(obji->configuration);
				skliDelete(&obji->animationData);
				free(obji->renderables);
				return -1;
			}

		}else{
			obji->skeletonHitboxes = NULL;
		}

		/* Allocate memory for and initialize the rigid bodies if necessary. */
		obji->physicsSimulate = 0;
		if(base->skeletonBodies != NULL){

			obji->skeletonPhysics = malloc(base->skl->boneNum * sizeof(physRBInstance));
			if(obji->skeletonPhysics == NULL){
				/** Memory allocation failure. **/
				free(obji->skeletonState[1]);
				free(obji->skeletonState[0]);
				free(obji->configuration);
				skliDelete(&obji->animationData);
				free(obji->renderables);
				return -1;
			}

			for(i = 0; i < base->skl->boneNum; ++i){
				boneInit(&obji->configuration[i]);
				if(physRBICreate(&obji->skeletonPhysics[i], &base->skeletonBodies[i]) == -1){
					/** Memory allocation failure. **/
					break;
				}
				/** Figure out constraint loading. **/
				obji->skeletonPhysics[i].flags = base->skeletonBodyFlags[i];
				if((obji->skeletonPhysics[i].flags & (PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_SIMULATE)) > 0){
					++obji->physicsSimulate;
				}
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
				free(obji->skeletonState[1]);
				free(obji->skeletonState[0]);
				free(obji->configuration);
				skliDelete(&obji->animationData);
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

	for(i = 0; i < base->renderableNum; ++i){
		rndriCreate(&obji->renderables[i], &base->renderables[i]);
	}

	/** Remove the following line. **/
	rndrConfigInit(&obji->tempRndrConfig);
	obji->base = base;
	obji->skl = base->skl;
	obji->renderableNum = base->renderableNum;
	return 1;

}

signed char objiNewRenderable(objInstance *obji, model *mdl, textureWrapper *tw){
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

signed char objiNewRenderableFromBase(objInstance *obji, renderable *rndr){
	/* Allocate room for a new renderable and initialize it. */
	rndrInstance *tempBuffer = realloc(obji->renderables, (obji->renderableNum+1)*sizeof(rndrInstance));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	obji->renderables = tempBuffer;
	rndriCreate(&obji->renderables[obji->renderableNum], rndr);
	++obji->renderableNum;
	return 1;
}

signed char objiNewRenderableFromInstance(objInstance *obji, rndrInstance *rndr){
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

signed char objiDeleteRenderable(objInstance *obji, size_t id){
	/* Remove a specified renderable. */
	if(obji->renderableNum > 0){
		size_t i, write;
		size_t arraySize = (obji->renderableNum-1)*sizeof(rndrInstance);
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

signed char objiInitSkeleton(objInstance *obji, skeleton *skl){
	size_t i;
	size_t arraySizeS = skl->boneNum*sizeof(bone);
	bone *tempBuffer3;
	bone *tempBuffer2;
	bone *tempBuffer1 = malloc(arraySizeS);
	if(tempBuffer1 == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	tempBuffer2 = malloc(arraySizeS);
	if(tempBuffer2 == NULL){
		/** Memory allocation failure. **/
		free(tempBuffer1);
		return -1;
	}
	tempBuffer3 = malloc(arraySizeS);
	if(tempBuffer3 == NULL){
		/** Memory allocation failure. **/
		free(tempBuffer2);
		free(tempBuffer1);
		return -1;
	}
	if(obji->configuration != NULL){
		free(obji->configuration);
	}
	if(obji->skeletonState[0] != NULL){
		free(obji->skeletonState[0]);
	}
	if(obji->skeletonState[1] != NULL){
		free(obji->skeletonState[1]);
	}
	if(obji->skeletonPhysics != NULL){
		free(obji->skeletonPhysics);
	}
	if(obji->skeletonHitboxes != NULL){
		free(obji->skeletonHitboxes);
	}
	obji->skl = skl;
	obji->configuration    = tempBuffer1;
	obji->skeletonState[0] = tempBuffer2;
	obji->skeletonState[1] = tempBuffer3;
	for(i = 0; i < skl->boneNum; ++i){
		boneInit(&obji->configuration[i]);
	}
	skliDelete(&obji->animationData);
	return 1;
}

/*signed char objInitPhysics(object *obji){
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

void objiBoneSetPhysicsFlags(objInstance *obji, size_t boneID, unsigned char flags){
	if(obji->skeletonPhysics != NULL){
		if((flags & (PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_SIMULATE)) > 0){
			obji->skeletonPhysics[boneID].configuration[0] = obji->skeletonState[0][boneID];
			obji->skeletonPhysics[boneID].configuration[1] = obji->skeletonState[1][boneID];
			++obji->physicsSimulate;
		}else if((obji->skeletonPhysics[boneID].flags & (PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_SIMULATE)) > 0){
			--obji->physicsSimulate;
		}
		obji->skeletonPhysics[boneID].flags = flags;
	}
}

sklAnim *objiGetAnimation(objInstance *obji, const size_t id){
	if(obji->base != NULL){
		if(obji->base->animationNum > id){
			return obji->base->animations[id];
		}
	}
	return NULL;
}

sklAnim *objiFindAnimation(objInstance *obji, const char *name){
	if(obji->base != NULL){
		size_t i;
		for(i = 0; i < obji->base->animationNum; ++i){
			if(strcmp(obji->base->animations[i]->name, name) == 0){
				return obji->base->animations[i];
			}
		}
	}
	return NULL;
}

void objiSetAnimationType(objInstance *obji, const size_t slot, const signed char additive){
	skliSetAnimationType(&obji->animationData, slot, additive);
}

signed char objiChangeAnimation(objInstance *obji, const size_t slot, sklAnim *anim, const size_t frame, const float blendTime){
	return skliChangeAnimation(&obji->animationData, obji->skl, slot, anim, frame, blendTime);
}

void objiClearAnimation(objInstance *obji, const size_t slot){
	skliClearAnimation(&obji->animationData, slot);
}

void objiUpdate(objInstance *obji, const camera *cam, const float elapsedTime, const float dt){

	size_t i;

	/* Update the object's skeletal animations. */
	// Swap the state pointers, so the previous state is in skeletonState[1].
	// We can reuse the old skeletonState[1] for the current state.
	bone *lastState = obji->skeletonState[0];
	obji->skeletonState[0] = obji->skeletonState[1];
	obji->skeletonState[1] = lastState;

	// Update each skeletal animation.
	skliUpdateAnimations(&obji->animationData, elapsedTime, 1.f);

	/* Update the object's skeleton. */
	for(i = 0; i < obji->skl->boneNum; ++i){

		obji->skeletonState[0][i] = obji->configuration[i];
		skliGenerateBoneState(&obji->animationData, &obji->skeletonState[0][i], i);

		if(obji->physicsSimulate && obji->skeletonPhysics != NULL &&
		   (obji->skeletonPhysics[i].flags & PHYSICS_BODY_SIMULATE)){

			/*
			** Simulate the body attached to the bone.
			*/

			// Integrate the body and solve constraints if desired.
			physRBIIntegrateEuler(&obji->skeletonPhysics[i], dt);

		}else{

			/*
			** Apply animation transformations.
			*/

			// Apply the object skeleton's bind offsets.
			/*obji->skeletonState[0][i].position.x += obji->skl->bones[i].defaultState.position.x;
			obji->skeletonState[0][i].position.y += obji->skl->bones[i].defaultState.position.y;
			obji->skeletonState[0][i].position.z += obji->skl->bones[i].defaultState.position.z;*/

			// Apply the object skeleton's bind offsets.
			boneTransformAppendPositionVec(&obji->skeletonState[0][i],
			                               obji->skl->bones[i].defaultState.position.x,
			                               obji->skl->bones[i].defaultState.position.y,
			                               obji->skl->bones[i].defaultState.position.z,
			                               &obji->skeletonState[0][i].position);

			// Apply the parent's transformations to each bone.
			if(obji->skl->bones[i].parent < obji->skl->boneNum && i != obji->skl->bones[i].parent){
				boneTransformAppend(&obji->skeletonState[0][obji->skl->bones[i].parent], &obji->skeletonState[0][i], &obji->skeletonState[0][i]);

			// Apply billboarding to root bones if necessary.
			}else if((obji->tempRndrConfig.flags & (RNDR_BILLBOARD_X | RNDR_BILLBOARD_Y | RNDR_BILLBOARD_Z)) > 0){
				// If any of the flags apart from RNDR_BILLBOARD_TARGET are set, continue.
				/**
				mat4 billboardRotation;
				if((rc->flags & RNDR_BILLBOARD_SPRITE) > 0){
					// Use a less accurate but faster method for billboarding.
					vec3 right, up, forward;
					// Use the camera's X, Y and Z axes for cheap sprite billboarding.
					vec3Set(&right,   cam->viewMatrix.m[0][0], cam->viewMatrix.m[0][1], cam->viewMatrix.m[0][2]);
					vec3Set(&up,      cam->viewMatrix.m[1][0], cam->viewMatrix.m[1][1], cam->viewMatrix.m[1][2]);
					vec3Set(&forward, cam->viewMatrix.m[2][0], cam->viewMatrix.m[2][1], cam->viewMatrix.m[2][2]);
					// Lock certain axes if needed.
					if((rc->flags & RNDR_BILLBOARD_X) == 0){
						right.y   = 0.f;
						up.y      = 1.f;
						forward.y = 0.f;
					}
					if((rc->flags & RNDR_BILLBOARD_Y) == 0){
						right.x   = 1.f;
						up.x      = 0.f;
						forward.x = 0.f;
					}
					if((rc->flags & RNDR_BILLBOARD_Z) == 0){
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
					if((rc->flags & RNDR_BILLBOARD_TARGET) > 0){
						eye = rc->targetPosition.render;
						target = rc->position.render;
						vec3Set(&up, 0.f, 1.f, 0.f);
						quatRotateVec3(&rc->targetOrientation.render, &up);
					}else if((rc->flags & RNDR_BILLBOARD_TARGET_CAMERA) > 0){
						eye = cam->position.render;
						target = rc->position.render;
						up = cam->up.render;
					}else{
						eye = cam->position.render;
						target = cam->targetPosition.render;
						up = cam->up.render;
					}
					// Lock certain axes if needed.
					if((rc->flags & RNDR_BILLBOARD_X) == 0){
						target.y = eye.y;
					}
					if((rc->flags & RNDR_BILLBOARD_Y) == 0){
						target.x = eye.x;
					}
					if((rc->flags & RNDR_BILLBOARD_Z) == 0){
						vec3Set(&up, 0.f, 1.f, 0.f);
					}
					mat4RotateToFace(&billboardRotation, &eye, &target, &up);
				}
				mat4MultMByM2(&billboardRotation, transformMatrix);  // Apply billboard rotation
				**/

			}

			// Update the positions of each rigid body connected to this bone.
			if(obji->skeletonPhysics != NULL){
				obji->skeletonPhysics[i].configuration[0] = obji->skeletonState[0][i];
				obji->skeletonPhysics[i].configuration[1] = obji->skeletonState[1][i];
			}

		}

		// If the body can collide, update its collision mesh.
		if(obji->physicsSimulate && obji->skeletonPhysics != NULL){
			if((obji->skeletonPhysics[i].flags & PHYSICS_BODY_INITIALIZE) > 0){
				obji->skeletonPhysics[i].flags &= ~PHYSICS_BODY_INITIALIZE;
				obji->skeletonPhysics[i].flags |= PHYSICS_BODY_SIMULATE;
			}
			if((obji->skeletonPhysics[i].flags & PHYSICS_BODY_COLLIDE) > 0){
				physRBIUpdateCollisionMesh(&obji->skeletonPhysics[i]);
			}
		}

	}

	for(i = 0; i < obji->skl->boneNum; ++i){

		// Apply the object skeleton's bind offsets.
		/*obji->skeletonState[0][i].position.x += obji->skl->bones[i].defaultState.position.x;
		obji->skeletonState[0][i].position.y += obji->skl->bones[i].defaultState.position.y;
		obji->skeletonState[0][i].position.z += obji->skl->bones[i].defaultState.position.z;*/

		/*bone thing = obji->skl->bones[i].defaultState;
		boneStateInvert(&thing, &thing);
		boneTransformAppend(&obji->skeletonState[0][i], &thing, &obji->skeletonState[0][i]);*/

		// Apply the object skeleton's bind offsets.
		/*boneTransformAppendPositionVec(&obji->skeletonState[0][i],
		                               -obji->skl->bones[i].defaultState.position.x,
		                               -obji->skl->bones[i].defaultState.position.y,
		                               -obji->skl->bones[i].defaultState.position.z,
		                               &obji->skeletonState[0][i]);*/
	}
	/* Transform the bones to global space and update the object's physics skeleton. */
	/*for(i = 0; i < obji->skl->boneNum; ++i){

		**
		*** This is pretty bad, but I'm not sure if I have a choice short of
		*** merging bones and physics bodies or something weird like that.
		**
		// Transform bones from local to global space.
		boneTransformAppendPosition(&obji->skl->bones[i].defaultState, &obji->skeletonState[0][i], &obji->skeletonState[0][i]);

		if(obji->skl->bones[i].parent < obji->skl->boneNum && i != obji->skl->bones[i].parent){
			// Transform parent into local space before applying transformations here.
			boneInvert(&obji->skl->bones[obji->skl->bones[i].parent].defaultState, &transformBone);
			boneTransformAppend(&obji->skeletonState[0][obji->skl->bones[i].parent], &transformBone, &transformBone);
			// Apply parent transformations.
			boneTransformAppend(&transformBone, &obji->skeletonState[0][i], &obji->skeletonState[0][i]);

		// Apply billboarding to root bones if necessary.
		}else if((obji->tempRndrConfig.flags & (RNDR_BILLBOARD_X | RNDR_BILLBOARD_Y | RNDR_BILLBOARD_Z)) > 0){
            //
		}

		// Update the positions of each rigid body connected to this bone.
		if(obji->physicsState != NULL && !obji->physicsSimulate &&
		   (obji->physicsState[i].flags & PHYSICS_BODY_SIMULATE) == 0){

			// Translate the bone into global space for the physics object.
			boneInvert(&obji->skl->bones[obji->skl->bones[i].parent].defaultState, &transformBone);
			boneTransformAppend(&obji->skeletonState[0][i], &transformBone,
			                    &obji->physicsState[i].configuration);

		}

		// Update the positions of each rigid body connected to this bone.
		//for(j = 0; j < obji->renderableNum; ++j){

			// Only bodies that are not being simulated are affected by skeletal animations.
			** Use a lookup, same in renderScene.c. **
			*rndrBone = sklFindBone(obji->renderables[j].mdl->skl, obji->skl->bones[i].name);
			if(rndrBone < obji->renderables[j].mdl->skl->boneNum &&
			   obji->renderables[j].physicsState != NULL && !obji->renderables[j].physicsSimulate &&
			   (obji->renderables[j].physicsState[rndrBone].flags & PHYSICS_BODY_SIMULATE) == 0){

			   	// Translate the bone into global space for the physics object.
			   	boneInvert(&obji->skl->bones[obji->skl->bones[i].parent].defaultState, &transformBone);
			   	boneTransformAppend(&obji->skeletonState[0][i], &transformBone,
				                    &obji->renderables[j].physicsState[rndrBone].configuration);

			}*

		//}

			// If any of the flags apart from RNDR_BILLBOARD_TARGET are set, continue.
			**mat4 billboardRotation;
			if((rc->flags & RNDR_BILLBOARD_SPRITE) > 0){
				// Use a less accurate but faster method for billboarding.
				vec3 right, up, forward;
				// Use the camera's X, Y and Z axes for cheap sprite billboarding.
				vec3Set(&right,   cam->viewMatrix.m[0][0], cam->viewMatrix.m[0][1], cam->viewMatrix.m[0][2]);
				vec3Set(&up,      cam->viewMatrix.m[1][0], cam->viewMatrix.m[1][1], cam->viewMatrix.m[1][2]);
				vec3Set(&forward, cam->viewMatrix.m[2][0], cam->viewMatrix.m[2][1], cam->viewMatrix.m[2][2]);
				// Lock certain axes if needed.
				if((rc->flags & RNDR_BILLBOARD_X) == 0){
					right.y   = 0.f;
					up.y      = 1.f;
					forward.y = 0.f;
				}
				if((rc->flags & RNDR_BILLBOARD_Y) == 0){
					right.x   = 1.f;
					up.x      = 0.f;
					forward.x = 0.f;
				}
				if((rc->flags & RNDR_BILLBOARD_Z) == 0){
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
				if((rc->flags & RNDR_BILLBOARD_TARGET) > 0){
					eye = rc->targetPosition.render;
					target = rc->position.render;
					vec3Set(&up, 0.f, 1.f, 0.f);
					quatRotateVec3(&rc->targetOrientation.render, &up);
				}else if((rc->flags & RNDR_BILLBOARD_TARGET_CAMERA) > 0){
					eye = cam->position.render;
					target = rc->position.render;
					up = cam->up.render;
				}else{
					eye = cam->position.render;
					target = cam->targetPosition.render;
					up = cam->up.render;
				}
				// Lock certain axes if needed.
				if((rc->flags & RNDR_BILLBOARD_X) == 0){
					target.y = eye.y;
				}
				if((rc->flags & RNDR_BILLBOARD_Y) == 0){
					target.x = eye.x;
				}
				if((rc->flags & RNDR_BILLBOARD_Z) == 0){
					vec3Set(&up, 0.f, 1.f, 0.f);
				}
				mat4RotateToFace(&billboardRotation, &eye, &target, &up);
			}
			mat4MultMByM2(&billboardRotation, transformMatrix);  // Apply billboard rotation
		}**

		// Only bodies that are not being simulated are affected by skeletal animations.
		*if(obji->physicsSimulate && (obji->physicsState[i].flags & PHYSICS_BODY_SIMULATE) == 0){
			// Apply the bone's change in position to the physics object.
			obji->physicsState[i].position = obji->skeletonState[0][i].position;
			// Apply the bone's change in orientation to the physics object.
			obji->physicsState[i].orientation = obji->skeletonState[0][i].orientation;
		}*

	}*/


	/* Update each of the object's texture wrappers. */
	for(i = 0; i < obji->renderableNum; ++i){
		twiAnimate(&obji->renderables[i].twi, elapsedTime);
	}

}

signed char objiRenderMethod(objInstance *obji, const float interpT){
	// Update alpha.
	iFloatUpdate(&obji->tempRndrConfig.alpha, interpT);
	if(obji->tempRndrConfig.alpha.render > 0.f){
		size_t i;
		for(i = 0; i < obji->renderableNum; ++i){
			if(obji->tempRndrConfig.alpha.render < 1.f || twiContainsTranslucency(&obji->renderables[i].twi)){
				// The model contains translucency.
				return 1;
			}
		}
		// The model is fully opaque.
		return 0;

	}
	// The model is fully transparent.
	return 2;
}

void objiGenerateSprite(const objInstance *obji, const size_t rndr, const float interpT, const float *texFrag, vertex *vertices){

	/* Generate the base sprite. */
	const float left   = -0.5f - obji->tempRndrConfig.pivot.render.x;
	const float top    = -0.5f - obji->tempRndrConfig.pivot.render.y;
	const float right  = 0.5f - obji->tempRndrConfig.pivot.render.x;
	const float bottom = 0.5f - obji->tempRndrConfig.pivot.render.y;
	const float z      = -obji->tempRndrConfig.pivot.render.z;
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
	boneInterpolate(&obji->skeletonState[1][0], &obji->skeletonState[0][0], interpT, &transform);
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
