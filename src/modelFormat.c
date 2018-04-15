#include "model.h"
#include "helpersMisc.h"
#include <string.h>
#include <stdio.h>

/** Merge mdlWavefrontObjLoad() with mdlLoad(). **/

#define MDL_VERTEX_START_CAPACITY 1
#define MDL_INDEX_START_CAPACITY 1
#define MDL_BONE_START_CAPACITY 1

/** Remove printf()s **/

#define mdlWavefrontObjFreeReturns() \
	if(*name != NULL){ \
		free(*name); \
	} \
	if(*vertices != NULL){ \
		free(*vertices); \
	} \
	if(*indices != NULL){ \
		free(*indices); \
	} \
	if(*sklPath != NULL){ \
		free(*sklPath); \
	}

#define mdlWavefrontObjFreeHelpers() \
	if(tempPositions != NULL){ \
		free(tempPositions); \
	} \
	if(tempTexCoords != NULL){ \
		free(tempTexCoords); \
	} \
	if(tempNormals != NULL){ \
		free(tempNormals); \
	} \
	if(tempBoneIDs != NULL){ \
		free(tempBoneIDs); \
	} \
	if(tempBoneWeights != NULL){ \
		free(tempBoneWeights); \
	}

signed char mdlWavefrontObjLoad(const char *filePath, size_t *vertexNum, vertex **vertices, size_t *indexNum, size_t **indices, char **name, char **sklPath){

	size_t vertexCapacity = MDL_VERTEX_START_CAPACITY;
	*vertices = malloc(vertexCapacity*sizeof(vertex));
	if(*vertices == NULL){
		/** Memory allocation failure. **/
		return 0;
	}

	size_t indexCapacity = MDL_INDEX_START_CAPACITY;
	*indices = malloc(indexCapacity*sizeof(size_t));
	if(*indices == NULL){
		/** Memory allocation failure. **/
		free(*vertices);
		return 0;
	}

	/**const signed char generatePhysProperties = (mass != NULL && area != NULL && centroid != NULL);
	if(generatePhysProperties){
		*mass = 0.f;
		*area = 0.f;
		vec3SetS(centroid, 0.f);
	}**/
	*vertexNum = 0;
	*indexNum = 0;

	// Temporarily holds vertex positions before they are pushed into vertices
	size_t tempPositionsSize = 0;
	size_t tempPositionsCapacity = 1*MDL_VERTEX_START_CAPACITY;
	float *tempPositions = malloc(tempPositionsCapacity*sizeof(float));
	if(tempPositions == NULL){
		/** Memory allocation failure. **/
		free(*vertices);
		free(*indices);
		return 0;
	}
	// Temporarily holds vertex UVs they are pushed into vertices
	size_t tempTexCoordsSize = 0;
	size_t tempTexCoordsCapacity = 1*MDL_VERTEX_START_CAPACITY;
	float *tempTexCoords = malloc(tempTexCoordsCapacity*sizeof(float));
	if(tempTexCoords == NULL){
		/** Memory allocation failure. **/
		free(*vertices);
		free(*indices);
		free(tempPositions);
		return 0;
	}
	// Temporarily holds vertex normals before they are pushed into vertices
	size_t tempNormalsSize = 0;
	size_t tempNormalsCapacity = 1*MDL_VERTEX_START_CAPACITY;
	float *tempNormals = malloc(tempNormalsCapacity*sizeof(float));
	if(tempNormals == NULL){
		/** Memory allocation failure. **/
		free(*vertices);
		free(*indices);
		free(tempPositions);
		free(tempTexCoords);
		return 0;
	}
	// Temporarily holds bone IDs before they are pushed into vertices
	size_t tempBoneIDsSize = 0;
	size_t tempBoneIDsCapacity = 1*MDL_VERTEX_START_CAPACITY;
	int   *tempBoneIDs = malloc(tempBoneIDsCapacity*sizeof(int));
	if(tempBoneIDs == NULL){
		/** Memory allocation failure. **/
		free(*vertices);
		free(*indices);
		free(tempPositions);
		free(tempTexCoords);
		free(tempNormals);
		return 0;
	}
	// Temporarily holds bone weights before they are pushed into vertices
	size_t tempBoneWeightsSize = 0;
	size_t tempBoneWeightsCapacity = 1*MDL_VERTEX_START_CAPACITY;
	float *tempBoneWeights = malloc(tempBoneWeightsCapacity*sizeof(float));
	if(tempBoneWeights == NULL){
		/** Memory allocation failure. **/
		free(*vertices);
		free(*indices);
		free(tempPositions);
		free(tempTexCoords);
		free(tempNormals);
		free(tempBoneIDs);
		return 0;
	}

	vertex tempVert;          // Holds a vertex before pushing it into the triangle array
	size_t positionIndex[3];  // Holds all the positional information for a face
	size_t uvIndex[3];        // Holds all the UV information for a face
	size_t normalIndex[3];    // Holds all the normal information for a face


	FILE *mdlInfo = fopen(filePath, "r");
	char lineFeed[1024];
	char *line;
	size_t lineLength;

	if(mdlInfo != NULL){
		while(fileParseNextLine(mdlInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			size_t i;

			// Name
			if(name != NULL && *name == NULL && lineLength >= 6 && strncmp(line, "name ", 5) == 0){
				*name = malloc((lineLength-4) * sizeof(char));
				if(*name == NULL){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return 0;
				}
				strncpy(*name, line+5, lineLength-5);
				(*name)[lineLength-5] = '\0';

			// Skeleton
			}else if(sklPath != NULL && *sklPath == NULL && lineLength > 9 && strncmp(line, "skeleton ", 9) == 0){
				const char *firstQuote = strchr(line, '"');
				const char *lastQuote = strrchr(line, '"');
				size_t pathBegin;
				size_t pathLength;
				if(firstQuote != NULL && lastQuote > firstQuote){
					pathBegin = firstQuote-line+1;
					pathLength = lastQuote-line-pathBegin;
				}else{  // If the skeleton path wasn't surrounded by quotes, don't give up:
					pathBegin = 9;
					pathLength = lineLength-9;
				}
				*sklPath = malloc((pathLength+1) * sizeof(char));
				if(*sklPath == NULL){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return 0;
				}
				strncpy(*sklPath, line+pathBegin, pathLength);
				(*sklPath)[pathLength] = '\0';

			// Vertex data
			}else if(lineLength >= 7 && strncmp(line, "v ", 2) == 0){
				char *token = strtok(line+2, " ");
				float curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity)){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return 0;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity)){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return 0;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity)){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return 0;
				}
				/****/
				token = strtok(NULL, " ");
				if(token != NULL){
					int curBoneID = strtoul(token, NULL, 0);
					if(!pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity)){
						/** Memory allocation failure. **/
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return 0;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(!pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity)){
						/** Memory allocation failure. **/
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return 0;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(!pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity)){
						/** Memory allocation failure. **/
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return 0;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(!pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity)){
						/** Memory allocation failure. **/
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return 0;
					}
					token = strtok(NULL, " ");
					if(token != NULL){
						curVal = strtod(token, NULL);
						if(!pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity)){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return 0;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(!pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity)){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return 0;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(!pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity)){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return 0;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(!pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity)){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return 0;
						}
					}
				}

			// UV data
			}else if(lineLength >= 6 && strncmp(line, "vt ", 3) == 0){
				char *token = strtok(line+3, " ");
				float curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempTexCoords, &curVal, sizeof(curVal), &tempTexCoordsSize, &tempTexCoordsCapacity)){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return 0;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempTexCoords, &curVal, sizeof(curVal), &tempTexCoordsSize, &tempTexCoordsCapacity)){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return 0;
				}

			// Normal data
			}else if(lineLength >= 8 && strncmp(line, "vn ", 3) == 0){
				char *token = strtok(line+3, " ");
				float curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempNormals, &curVal, sizeof(curVal), &tempNormalsSize, &tempNormalsCapacity)){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return 0;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempNormals, &curVal, sizeof(curVal), &tempNormalsSize, &tempNormalsCapacity)){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return 0;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempNormals, &curVal, sizeof(curVal), &tempNormalsSize, &tempNormalsCapacity)){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return 0;
				}

			// Face data
			}else if(lineLength >= 19 && strncmp(line, "f ", 2) == 0){
				char *token = strtok(line+2, " /");
				for(i = 0; i < 3; ++i){

					// Load face data
					positionIndex[i] = strtoul(token, NULL, 0);
					token = strtok(NULL, " /");
					uvIndex[i] = strtoul(token, NULL, 0);
					token = strtok(NULL, " /");
					normalIndex[i] = strtoul(token, NULL, 0);
					token = strtok(NULL, " /");

					// Create a vertex from the given data
					// Vertex positional data
					size_t pos = (positionIndex[i]-1)*3;
					if(pos+2 < tempPositionsSize){
						tempVert.position.x = tempPositions[pos];
						tempVert.position.y = tempPositions[pos+1];
						tempVert.position.z = tempPositions[pos+2];
					}else{
						tempVert.position.x = 0.f;
						tempVert.position.y = 0.f;
						tempVert.position.z = 0.f;
					}
					// Vertex UV data
					pos = (uvIndex[i]-1)<<1;
					if(pos+1 < tempTexCoordsSize){
						tempVert.u = tempTexCoords[pos];
						tempVert.v = -tempTexCoords[pos+1];
					}else{
						tempVert.u = 0.f;
						tempVert.v = 0.f;
					}
					// Vertex normal data
					pos = (normalIndex[i]-1)*3;
					if(pos+2 < tempNormalsSize){
						tempVert.normal.x = tempNormals[pos];
						tempVert.normal.y = tempNormals[pos+1];
						tempVert.normal.z = tempNormals[pos+2];
					}else{
						tempVert.normal.x = 0.f;
						tempVert.normal.y = 0.f;
						tempVert.normal.z = 0.f;
					}
					/****/
					pos = (positionIndex[i]-1)*4;
					if(pos+3 < tempBoneIDsSize){
						tempVert.bIDs[0] = tempBoneIDs[pos];
						tempVert.bIDs[1] = tempBoneIDs[pos+1];
						tempVert.bIDs[2] = tempBoneIDs[pos+2];
						tempVert.bIDs[3] = tempBoneIDs[pos+3];
					}else{
						tempVert.bIDs[0] = 0;
						tempVert.bIDs[1] = -1;
						tempVert.bIDs[2] = -1;
						tempVert.bIDs[3] = -1;
					}
					/****/
					pos = (positionIndex[i]-1)*4;
					if(pos+3 < tempBoneWeightsSize){
						tempVert.bWeights[0] = tempBoneWeights[pos];
						tempVert.bWeights[1] = tempBoneWeights[pos+1];
						tempVert.bWeights[2] = tempBoneWeights[pos+2];
						tempVert.bWeights[3] = tempBoneWeights[pos+3];
					}else{
						tempVert.bWeights[0] = 1.f;
						tempVert.bWeights[1] = 0.f;
						tempVert.bWeights[2] = 0.f;
						tempVert.bWeights[3] = 0.f;
					}

					// Check if the vertex has already been loaded, and if so add an index
					signed char foundVertex = 0;
					size_t j;
					for(j = 0; j < *vertexNum; ++j){
						vertex *checkVert = &(*vertices)[j];
						/** CHECK BONE DATA HERE **/
						if(memcmp(checkVert, &tempVert, sizeof(vertex)) == 0){
							// Resize indices if there's not enough room
							if(!pushDynamicArray((void **)indices, &j, sizeof(j), indexNum, &indexCapacity)){
								/** Memory allocation failure. **/
								mdlWavefrontObjFreeHelpers();
								mdlWavefrontObjFreeReturns();
								fclose(mdlInfo);
								return 0;
							}
							foundVertex = 1;
							break;
						}
					}

					// If the vertex has not yet been loaded, add it to both the vertex vector and the index vector
					if(!foundVertex){
						// Resize indices if there's not enough room
						if(!pushDynamicArray((void **)indices, vertexNum, sizeof(*vertexNum), indexNum, &indexCapacity) ||
						   !pushDynamicArray((void **)vertices, &tempVert, sizeof(tempVert), vertexNum, &vertexCapacity)){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return 0;
						}
						// Generate physics properties if necessary
						/**if(generatePhysProperties && *vertexNum > 1){
							float temp = (*vertices)[(*vertexNum)-2].position.x * (*vertices)[(*vertexNum)-1].position.y -
							             (*vertices)[(*vertexNum)-2].position.y * (*vertices)[(*vertexNum)-1].position.x;
							centroid->x += ((*vertices)[(*vertexNum)-2].position.x + (*vertices)[(*vertexNum)-1].position.x)*temp;
							centroid->y += ((*vertices)[(*vertexNum)-2].position.y + (*vertices)[(*vertexNum)-1].position.y)*temp;
							centroid->z += ((*vertices)[(*vertexNum)-2].position.z + (*vertices)[(*vertexNum)-1].position.z)*temp;
							*area += temp;
							*mass += DEFAULT_VERTEX_MASS;
						}**/
					}

				}

			}

		}

		fclose(mdlInfo);
		mdlWavefrontObjFreeHelpers();

	}else{
		printf("Error loading model: Couldn't open %s\n", filePath);
		mdlWavefrontObjFreeHelpers();
		mdlWavefrontObjFreeReturns();
		return 0;
	}

	/**if(generatePhysProperties){
		// Final iteration with the last and first vertices.
		float temp = (*vertices)[(*vertexNum)-1].position.x * (*vertices)[0].position.y -
		             (*vertices)[(*vertexNum)-1].position.y * (*vertices)[0].position.x;
		centroid->x += ((*vertices)[(*vertexNum)-1].position.x + (*vertices)[0].position.x)*temp;
		centroid->y += ((*vertices)[(*vertexNum)-1].position.y + (*vertices)[0].position.y)*temp;
		centroid->z += ((*vertices)[(*vertexNum)-1].position.z + (*vertices)[0].position.z)*temp;
		*area += temp;
		*mass += DEFAULT_VERTEX_MASS;

		// Calculate the mesh's final area and center of mass.
		*area *= 0.5f;
		temp = 1.f/(6.f*(*area));
		centroid->x *= temp;
		centroid->y *= temp;
		centroid->z *= temp;
	}**/

	return 1;

}





/**
*** EVERYTHING BELOW THIS COMMENT IS TEMPORARY.
**/





#define mdlSMDFreeReturns() \
	if(*name != NULL){ \
		free(*name); \
	} \
	if(*vertices != NULL){ \
		free(*vertices); \
	} \
	if(*indices != NULL){ \
		free(*indices); \
	} \

#define mdlSMDFreeHelpers() \
	sklDelete(&tempSkl);

signed char mdlSMDLoad(const char *filePath, size_t *vertexNum, vertex **vertices, size_t *indexNum, size_t **indices, char **name, cVector *allSkeletons){
	/*
	** Temporary function by 8426THMY.
	*/
	//Load the textureGroup!
	FILE *mdlFile = fopen(filePath, "r");
	if(mdlFile != NULL){
		//Temporarily stores only unique vertices.
		*vertexNum = 0;
		size_t vertexCapacity = MDL_VERTEX_START_CAPACITY;
		*vertices = malloc(vertexCapacity * sizeof(**vertices));
		//Temporarily stores vertex indices for faces.
		//We can use the model's totalIndices variable for the size so we don't have to set it later.
		*indexNum = 0;
		size_t indexCapacity = MDL_INDEX_START_CAPACITY;
		*indices = malloc(indexCapacity * sizeof(**indices));
		//Temporarily stores bones.
		skeleton tempSkl;
		tempSkl.name = NULL;
		tempSkl.boneNum = 0;
		size_t boneCapacity = MDL_BONE_START_CAPACITY;
		tempSkl.bones = malloc(boneCapacity * sizeof(*tempSkl.bones));
		//This indicates what sort of data we're currently supposed to be reading.
		unsigned char dataType = 0;
		//This variable stores data specific to the type we're currently loading.
		unsigned int data = 0;

		char lineFeed[1024];
		char *line;
		size_t lineLength;


		while(fileParseNextLine(mdlFile, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			if(dataType == 0){
				if(strcmp(line, "nodes") == 0){
					dataType = 1;
				}else if(strcmp(line, "skeleton") == 0){
					dataType = 2;
				}else if(strcmp(line, "triangles") == 0){
					dataType = 3;

				//If this isn't the version number and the line isn't empty, it's something we can't handle!
				}else if(lineLength > 0 && strcmp(line, "version 1") != 0){
					printf("Error loading model!\n"
					       "Path: %s\n"
					       "Line: %s\n"
					       "Error: Unexpected identifier!\n", filePath, line);
					mdlSMDFreeReturns();
					mdlSMDFreeHelpers();
					return 0;
				}
			}else{
				if(strcmp(line, "end") == 0){
					//If we've finished identifying the skeleton's bones, shrink the vector!
					if(dataType == 1){
						boneCapacity = tempSkl.boneNum;
						tempSkl.bones = realloc(tempSkl.bones, boneCapacity * sizeof(*tempSkl.bones));
					}

					dataType = 0;
					data = 0;
				}else{
					if(dataType == 1){
						char *tokPos = line;

						sklNode tempBone;

						//Get this bone's ID.
						size_t boneID = strtoul(tokPos, &tokPos, 10);
						if(boneID == tempSkl.boneNum){
							//Get the bone's name.
							size_t boneNameLength;
							getDelimitedString(tokPos, line + lineLength - tokPos, "\" ", &tokPos, &boneNameLength);
							tempBone.name = malloc(boneNameLength + 1);
							memcpy(tempBone.name, tokPos, boneNameLength);
							tempBone.name[boneNameLength] = '\0';

							//Get the ID of this bone's parent.
							tempBone.parent = strtoul(tokPos + boneNameLength + 1, NULL, 10);


							//If we're out of space, allocate some more!
							if(tempSkl.boneNum >= boneCapacity){
								boneCapacity = tempSkl.boneNum * 2;
								tempSkl.bones = realloc(tempSkl.bones, boneCapacity * sizeof(*tempSkl.bones));
							}
							//Add the bone to our vector!
							tempSkl.bones[tempSkl.boneNum] = tempBone;
							++tempSkl.boneNum;
						}else{
							printf("Error loading model!\n"
							       "Path: %s\n"
							       "Line: %s\n"
							       "Error: Found node %u when expecting node %u!\n",
							       filePath, line, boneID, tempSkl.boneNum);
							mdlSMDFreeReturns();
							mdlSMDFreeHelpers();
							return 0;
						}
					}else if(dataType == 2){
						//If the line begins with time, get the frame's timestamp!
						if(memcmp(line, "time ", 5) == 0){
							unsigned int newTime = strtoul(&line[5], NULL, 10);
							if(newTime >= data){
								data = newTime;
							}else{
								printf("Error loading model!\n"
								       "Path: %s\n"
								       "Line: %s\n"
								       "Error: Frame timestamps do not increment sequentially!\n",
								       filePath, line);
								mdlSMDFreeReturns();
								mdlSMDFreeHelpers();
								return 0;
							}

						//Otherwise, we're setting the bone's state!
						}else{
							char *tokPos = line;

							//Get this bone's ID.
							size_t boneID = strtoul(tokPos, &tokPos, 10);
							//Make sure a bone with this ID actually exists.
							if(boneID < tempSkl.boneNum){
								sklNode *currentBone = &tempSkl.bones[boneID];

								//If the current frame timestamp is 0, set the bone's initial state!
								if(data == 0){
									//Load the bone's position!
									float x = strtod(tokPos, &tokPos) * 0.05f;
									float y = strtod(tokPos, &tokPos) * 0.05f;
									float z = strtod(tokPos, &tokPos) * 0.05f;
									vec3Set(&currentBone->defaultState.position, x, y, z);

									//Load the bone's rotation!
									x = strtod(tokPos, &tokPos);
									y = strtod(tokPos, &tokPos);
									z = strtod(tokPos, NULL);
									quatSetEuler(&currentBone->defaultState.orientation, x, y, z);

									//Set the bone's scale!
									vec3Set(&currentBone->defaultState.scale, 1.f, 1.f, 1.f);


									//If this bone has a parent, append its state to its parent's state!
									if(currentBone->parent != -1){
										//boneTransformAppend(&tempSkl.bones[currentBone->parent].defaultState, &currentBone->defaultState, &currentBone->defaultState);
									}
								}
							}else{
								printf("Error loading model!\n"
									   "Path: %s\n"
									   "Line: %s\n"
									   "Error: Found skeletal data for bone %u, which doesn't exist!\n",
									   filePath, line, boneID);
								mdlSMDFreeReturns();
								mdlSMDFreeHelpers();
								return 0;
							}
						}
					}else if(dataType == 3){
						if(data == 0){
							//This indicates the texture that the face uses.
						}else{
							char *tokPos = line;

							vertex tempVertex;
							memset(&tempVertex, 0, sizeof(tempVertex));

							//Read the vertex data from the line!
							size_t parentBoneID = strtoul(tokPos, &tokPos, 10);
							//Make sure a bone with this ID actually exists.
							if(parentBoneID < tempSkl.boneNum){
								tempVertex.position.x = strtod(tokPos, &tokPos) * 0.05f;
								tempVertex.position.y = strtod(tokPos, &tokPos) * 0.05f;
								tempVertex.position.z = strtod(tokPos, &tokPos) * 0.05f;
								tempVertex.normal.x = strtod(tokPos, &tokPos);
								tempVertex.normal.y = strtod(tokPos, &tokPos);
								tempVertex.normal.z = strtod(tokPos, &tokPos);
								tempVertex.u = strtod(tokPos, &tokPos);
								tempVertex.v = -strtod(tokPos, &tokPos);
								size_t numLinks = strtoul(tokPos, &tokPos, 10);
								//Make sure some links were specified.
								if(numLinks > 0){
									//If there are more than the maximum number of supported weights, we'll have to clamp it down!
									if(numLinks > VERTEX_MAX_BONES){
										printf("Error loading model!\n"
										       "Path: %s\n"
										       "Line: %s\n"
										       "Error: Vertex has too many links! All extra links will be ignored.\n",
										       filePath, line);

										numLinks = VERTEX_MAX_BONES;
									}

									size_t parentPos = -1;
									float totalWeight = 0.f;
									size_t i;
									//Load all of the links!
									for(i = 0; i < numLinks; ++i){
										//Load the bone's ID!
										tempVertex.bIDs[i] = strtoul(tokPos, &tokPos, 10);
										//Make sure it exists!
										if(tempVertex.bIDs[i] > tempSkl.boneNum){
											printf("Error loading model!\n"
											       "Path: %s\n"
											       "Line: %s\n"
											       "Error: Vertex link bone doesn't exist! The parent bone will be used instead.\n",
											       filePath, line);

										//If we're loading the parent bone, remember its position!
										}else if(tempVertex.bIDs[i] == parentBoneID){
											parentPos = i;
										}

										//Load the bone's weights!
										tempVertex.bWeights[i] = strtod(tokPos, &tokPos);
										totalWeight += tempVertex.bWeights[i];
										//Make sure the total weight doesn't exceed 1!
										if(totalWeight > 1.f){
											tempVertex.bWeights[i] -= totalWeight - 1.f;
											totalWeight = 1.f;

											++i;
											break;
										}
									}

									//Make sure the total weight isn't less than 1!
									if(totalWeight < 1.f){
										//If we never loaded the parent bone, see if we can add it!
										if(parentPos == -1){
											if(i < VERTEX_MAX_BONES){
												tempVertex.bIDs[i] = parentBoneID;
												tempVertex.bWeights[i] = 0.f;
												parentPos = i;
												++i;

											//If there's no room, just use the first bone we loaded.
											}else{
												parentPos = 0;
											}
										}

										tempVertex.bWeights[parentPos] += 1.f - totalWeight;
									}

									//Make sure we fill the rest with invalid values so we know they aren't used.
									memset(&tempVertex.bIDs[i], -1, (VERTEX_MAX_BONES - i) * sizeof(tempVertex.bIDs[0]));
									memset(&tempVertex.bWeights[i], 0.f, (VERTEX_MAX_BONES - i) * sizeof(tempVertex.bWeights[0]));

								//Otherwise, just bind it to the parent bone.
								}else{
									printf("Error loading model!\n"
									       "Path: %s\n"
									       "Line: %s\n"
									       "Error: Vertex has no links! The parent bone will be used.\n",
									       filePath, line);

									tempVertex.bIDs[0] = parentBoneID;
									memset(&tempVertex.bIDs[1], -1, (VERTEX_MAX_BONES - 1) * sizeof(tempVertex.bIDs[0]));
									tempVertex.bWeights[0] = 1.f;
									memset(&tempVertex.bWeights[1], 0.f, (VERTEX_MAX_BONES - 1) * sizeof(tempVertex.bWeights[0]));
								}


								size_t i;
								//Check if this vertex already exists!
								for(i = 0; i < *vertexNum; ++i){
									//Looks like it does, so we don't need to store it again!
									if(memcmp(&(*vertices)[i], &tempVertex, sizeof(vertex)) == 0){
										break;
									}
								}
								//The vertex does not exist, so add it to the vector!
								if(i == *vertexNum){
									//If we're out of space, allocate some more!
									if(*vertexNum >= vertexCapacity){
										vertexCapacity = *vertexNum * 2;
										*vertices = realloc(*vertices, vertexCapacity * sizeof(**vertices));
									}
									(*vertices)[*vertexNum] = tempVertex;
									++(*vertexNum);
								}
								//Add an index for the new vertex!
								if(*indexNum >= indexCapacity){
									indexCapacity = *indexNum * 2;
									*indices = realloc(*indices, indexCapacity * sizeof(**indices));
								}
								(*indices)[*indexNum] = i;
								++(*indexNum);
							}else{
								printf("Error loading model!\n"
									   "Path: %s\n"
									   "Line: %s\n"
									   "Error: Vertex parent bone doesn't exist!\n",
									   filePath, line);
								mdlSMDFreeReturns();
								mdlSMDFreeHelpers();
								return 0;
							}
						}

						if(data < 3){
							++data;
						}else{
							data = 0;
						}
					}
				}
			}
		}

		fclose(mdlFile);

		size_t fileLen = strlen(filePath);
		tempSkl.name = malloc((fileLen+1)*sizeof(char));
		if(tempSkl.name == NULL){
			/** Memory allocation failure. **/
			mdlSMDFreeReturns();
			mdlSMDFreeHelpers();
			return 0;
		}
		memcpy(tempSkl.name, filePath, fileLen);
		tempSkl.name[fileLen] = '\0';
		cvPush(allSkeletons, (void *)&tempSkl, sizeof(tempSkl));

	}else{
		printf("Unable to open model file!\n"
		       "Path: %s\n", filePath);
		return 0;
	}

	return 1;
}
