#include "model.h"
#include "memoryManager.h"
#include "helpersFileIO.h"
#include "helpersMisc.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>

/** Merge mdlWavefrontObjLoad() with mdlLoad(). **/

#define MDL_VERTEX_START_CAPACITY 1
#define MDL_INDEX_START_CAPACITY 1
#define MDL_LOD_START_CAPACITY 1

/** Remove printf()s **/

#define mdlWavefrontObjFreeReturns() \
	if(*vertices != NULL){ \
		memFree(*vertices); \
	} \
	if(*indices != NULL){ \
		memFree(*indices); \
	} \
	if(*lods != NULL){ \
		memFree(*lods); \
	} \

#define mdlWavefrontObjFreeHelpers() \
	if(tempPositions != NULL){ \
		memFree(tempPositions); \
	} \
	if(tempTexCoords != NULL){ \
		memFree(tempTexCoords); \
	} \
	if(tempNormals != NULL){ \
		memFree(tempNormals); \
	} \
	if(tempBoneIDs != NULL){ \
		memFree(tempBoneIDs); \
	} \
	if(tempBoneWeights != NULL){ \
		memFree(tempBoneWeights); \
	}

return_t mdlWavefrontObjLoad(const char *const restrict filePath, vertexIndex_t *const restrict vertexNum, vertex **const vertices, vertexIndexNum_t *const restrict indexNum, size_t **const indices, size_t *const restrict lodNum, mdlLOD **const lods, char *const restrict sklPath){

	FILE *const restrict mdlInfo = fopen(filePath, "r");

	if(mdlInfo != NULL){

		int i;
		char lineFeed[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		vertex tempVert;                 // Holds a vertex before pushing it into the triangle array
		vertexIndex_t positionIndex[3];  // Holds all the positional information for a face
		vertexIndex_t uvIndex[3];        // Holds all the UV information for a face
		vertexIndex_t normalIndex[3];    // Holds all the normal information for a face

		size_t vertexCapacity = MDL_VERTEX_START_CAPACITY;
		size_t indexCapacity = MDL_INDEX_START_CAPACITY;

		size_t tempPositionsSize = 0;
		size_t tempPositionsCapacity = 1*MDL_VERTEX_START_CAPACITY;
		float *tempPositions;

		size_t tempTexCoordsSize = 0;
		size_t tempTexCoordsCapacity = 1*MDL_VERTEX_START_CAPACITY;
		float *tempTexCoords;

		size_t tempNormalsSize = 0;
		size_t tempNormalsCapacity = 1*MDL_VERTEX_START_CAPACITY;
		float *tempNormals;

		size_t tempBoneIDsSize = 0;
		size_t tempBoneIDsCapacity = 1*MDL_VERTEX_START_CAPACITY;
		int   *tempBoneIDs;

		size_t tempBoneWeightsSize = 0;
		size_t tempBoneWeightsCapacity = 1*MDL_VERTEX_START_CAPACITY;
		float *tempBoneWeights;

		size_t lodCapacity = MDL_LOD_START_CAPACITY;
		mdlLOD lodTemp;


		*vertices = memAllocate(vertexCapacity*sizeof(vertex));
		if(*vertices == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		*indices = memAllocate(indexCapacity*sizeof(vertexIndex_t));
		if(*indices == NULL){
			/** Memory allocation failure. **/
			memFree(*vertices);
			return -1;
		}
		*lods = memAllocate(lodCapacity*sizeof(mdlLOD));
		if(*lods == NULL){
			/** Memory allocation failure. **/
			memFree(*vertices);
			memFree(*indices);
			return -1;
		}
		/**const int generatePhysProperties = (mass != NULL && area != NULL && centroid != NULL);
		if(generatePhysProperties){
			*mass = 0.f;
			*area = 0.f;
			vec3ZeroP(centroid);
		}**/
		*vertexNum = 0;
		*indexNum = 0;
		*lodNum = 0;
		lodTemp.distance = 0.f;
		lodTemp.offset = 0;
		lodTemp.indexNum = 0;

		// Temporarily holds vertex positions before they are pushed into vertices
		tempPositions = memAllocate(tempPositionsCapacity*sizeof(float));
		if(tempPositions == NULL){
			/** Memory allocation failure. **/
			memFree(*vertices);
			memFree(*indices);
			memFree(*lods);
			return -1;
		}
		// Temporarily holds vertex UVs they are pushed into vertices
		tempTexCoords = memAllocate(tempTexCoordsCapacity*sizeof(float));
		if(tempTexCoords == NULL){
			/** Memory allocation failure. **/
			memFree(*vertices);
			memFree(*indices);
			memFree(*lods);
			memFree(tempPositions);
			return -1;
		}
		// Temporarily holds vertex normals before they are pushed into vertices
		tempNormals = memAllocate(tempNormalsCapacity*sizeof(float));
		if(tempNormals == NULL){
			/** Memory allocation failure. **/
			memFree(*vertices);
			memFree(*indices);
			memFree(*lods);
			memFree(tempPositions);
			memFree(tempTexCoords);
			return -1;
		}
		// Temporarily holds bone IDs before they are pushed into vertices
		tempBoneIDs = memAllocate(tempBoneIDsCapacity*sizeof(int));
		if(tempBoneIDs == NULL){
			/** Memory allocation failure. **/
			memFree(*vertices);
			memFree(*indices);
			memFree(*lods);
			memFree(tempPositions);
			memFree(tempTexCoords);
			memFree(tempNormals);
			return -1;
		}
		// Temporarily holds bone weights before they are pushed into vertices
		tempBoneWeights = memAllocate(tempBoneWeightsCapacity*sizeof(float));
		if(tempBoneWeights == NULL){
			/** Memory allocation failure. **/
			memFree(*vertices);
			memFree(*indices);
			memFree(*lods);
			memFree(tempPositions);
			memFree(tempTexCoords);
			memFree(tempNormals);
			memFree(tempBoneIDs);
			return -1;
		}

		while(fileParseNextLine(mdlInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			// Skeleton
			if(sklPath != NULL && sklPath[0] == '\0' && lineLength > 9 && strncmp(line, "skeleton ", 9) == 0){
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
				strncpy(sklPath, line+pathBegin, pathLength);
				sklPath[pathLength] = '\0';

			// LOD
			}else if(lineLength >= 5 && strncmp(line, "lod ", 4) == 0){
				const float lodNewDistance = strtod(line+4, NULL);
				if((*lodNum == 0 && lodNewDistance == 0.f) || ((*lodNum > 0 || lodTemp.distance == 0.f) && lodNewDistance > lodTemp.distance)){
					// The new LOD seems valid.
					// If one was specified before it, add it to the array.
					if(lodNewDistance != 0.f){
						lodTemp.indexNum = *indexNum - lodTemp.indexNum;
						if(pushDynamicArray((void **)lods, &lodTemp, sizeof(mdlLOD), lodNum, &lodCapacity) < 0){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
					}
					// Start the next LOD.
					lodTemp.distance = lodNewDistance;
					lodTemp.offset = (void *)((uintptr_t)*indexNum * sizeof(vertexIndex_t));
					lodTemp.indexNum = *indexNum;
				}

			// Vertex data
			}else if(lineLength >= 7 && strncmp(line, "v ", 2) == 0){
				const char *token = strtok(line+2, " ");
				float curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				/****/
				token = strtok(NULL, " ");
				if(token != NULL){
					int curBoneID = strtoul(token, NULL, 0);
					if(pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity) < 0){
						/** Memory allocation failure. **/
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return -1;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity) < 0){
						/** Memory allocation failure. **/
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return -1;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity) < 0){
						/** Memory allocation failure. **/
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return -1;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity) < 0){
						/** Memory allocation failure. **/
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return -1;
					}
					token = strtok(NULL, " ");
					if(token != NULL){
						curVal = strtod(token, NULL);
						if(pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity) < 0){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity) < 0){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity) < 0){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity) < 0){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
					}
				}

			// UV data
			}else if(lineLength >= 6 && strncmp(line, "vt ", 3) == 0){
				const char *token = strtok(line+3, " ");
				float curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempTexCoords, &curVal, sizeof(curVal), &tempTexCoordsSize, &tempTexCoordsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempTexCoords, &curVal, sizeof(curVal), &tempTexCoordsSize, &tempTexCoordsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}

			// Normal data
			}else if(lineLength >= 8 && strncmp(line, "vn ", 3) == 0){
				const char *token = strtok(line+3, " ");
				float curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempNormals, &curVal, sizeof(curVal), &tempNormalsSize, &tempNormalsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempNormals, &curVal, sizeof(curVal), &tempNormalsSize, &tempNormalsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempNormals, &curVal, sizeof(curVal), &tempNormalsSize, &tempNormalsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}

			// Face data
			}else if(lineLength >= 19 && strncmp(line, "f ", 2) == 0){
				const char *token = strtok(line+2, " /");
				for(i = 0; i < 3; ++i){

					int foundVertex = 0;
					vertexIndex_t j;

					// Load face data
					positionIndex[i] = strtoul(token, NULL, 0)-1;
					token = strtok(NULL, " /");
					uvIndex[i] = strtoul(token, NULL, 0)-1;
					token = strtok(NULL, " /");
					normalIndex[i] = strtoul(token, NULL, 0)-1;
					token = strtok(NULL, " /");

					// Create a vertex from the given data
					// Vertex positional data
					size_t pos = positionIndex[i]*3;
					if(pos+2 < tempPositionsSize){
						tempVert.position.x = tempPositions[pos];
						tempVert.position.y = tempPositions[pos+1];
						tempVert.position.z = tempPositions[pos+2];
					}else{
						vec3ZeroP(&tempVert.position);
					}
					// Vertex UV data
					pos = uvIndex[i]<<1;
					if(pos+1 < tempTexCoordsSize){
						tempVert.u = tempTexCoords[pos];
						tempVert.v = -tempTexCoords[pos+1];
					}else{
						tempVert.u = 0.f;
						tempVert.v = 0.f;
					}
					// Vertex normal data
					pos = normalIndex[i]*3;
					if(pos+2 < tempNormalsSize){
						tempVert.normal.x = tempNormals[pos];
						tempVert.normal.y = tempNormals[pos+1];
						tempVert.normal.z = tempNormals[pos+2];
					}else{
						vec3ZeroP(&tempVert.normal);
					}
					/****/
					pos = positionIndex[i]*4;
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
					pos = positionIndex[i]*4;
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
					for(j = 0; j < *vertexNum; ++j){
						const vertex *const checkVert = &(*vertices)[j];
						/** CHECK BONE DATA HERE **/
						if(memcmp(checkVert, &tempVert, sizeof(vertex)) == 0){
							// Resize indices if there's not enough room
							size_t tempIndexNum = *indexNum;
							if(pushDynamicArray((void **)indices, &j, sizeof(j), &tempIndexNum, &indexCapacity) < 0){
								/** Memory allocation failure. **/
								mdlWavefrontObjFreeHelpers();
								mdlWavefrontObjFreeReturns();
								fclose(mdlInfo);
								return -1;
							}
							*indexNum = tempIndexNum;
							foundVertex = 1;
							break;
						}
					}

					// If the vertex has not yet been loaded, add it to both the vertex vector and the index vector
					if(!foundVertex){
						// Resize indices if there's not enough room
						size_t tempIndexNum = *indexNum;
						size_t tempVertexNum = *vertexNum;
						if(pushDynamicArray((void **)indices, vertexNum, sizeof(*vertexNum), &tempIndexNum, &indexCapacity)  < 0 ||
						   pushDynamicArray((void **)vertices, &tempVert, sizeof(tempVert), &tempVertexNum, &vertexCapacity) < 0){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
						*indexNum = tempIndexNum;
						*vertexNum = tempVertexNum;
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

		// Add the last LOD if it was in progress.
		if(*lodNum > 0 && lodTemp.distance != 0.f){
			// The new LOD seems valid.
			// If one was specified before it, add it to the array.
			lodTemp.indexNum = *indexNum - lodTemp.indexNum;
			if(pushDynamicArray((void **)lods, &lodTemp, sizeof(mdlLOD), lodNum, &lodCapacity) < 0){
				/** Memory allocation failure. **/
				mdlWavefrontObjFreeHelpers();
				mdlWavefrontObjFreeReturns();
				fclose(mdlInfo);
				return -1;
			}
		}

		// Clean up LODs.
		if(*lodNum <= 1){
			memFree(*lods);
			*lods = NULL;
			*lodNum = 0;
		}

	}else{
		printf("Error loading model \"%s\": Could not open file.\n", filePath);
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
	if(*vertices != NULL){ \
		memFree(*vertices); \
	} \
	if(*indices != NULL){ \
		memFree(*indices); \
	} \

#define mdlSMDFreeHelpers() \
	sklDelete(skl);

return_t mdlSMDLoad(const char *filePath, vertexIndex_t *vertexNum, vertex **vertices, vertexIndexNum_t *indexNum, size_t **indices, skeleton *const skl){
	/*
	** Temporary function by 8426THMY.
	*/
	//Load the textureGroup!
	FILE *mdlFile = fopen(filePath, "r");
	if(mdlFile != NULL){
		//Temporarily stores only unique vertices.
		*vertexNum = 0;
		size_t vertexCapacity = MDL_VERTEX_START_CAPACITY;
		*vertices = memAllocate(vertexCapacity * sizeof(**vertices));
		//Temporarily stores vertex indices for faces.
		//We can use the model's totalIndices variable for the size so we don't have to set it later.
		*indexNum = 0;
		size_t indexCapacity = MDL_INDEX_START_CAPACITY;
		*indices = memAllocate(indexCapacity * sizeof(**indices));
		//Temporarily stores bones.
		skl->name = NULL;
		skl->boneNum = 0;
		size_t boneCapacity = 1;
		skl->bones = memAllocate(boneCapacity * sizeof(*skl->bones));
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
						boneCapacity = skl->boneNum;
						skl->bones = memReallocate(skl->bones, boneCapacity * sizeof(*skl->bones));
					}

					dataType = 0;
					data = 0;
				}else{
					if(dataType == 1){
						char *tokPos = line;

						sklNode tempBone;

						//Get this bone's ID.
						size_t boneID = strtoul(tokPos, &tokPos, 10);
						if(boneID == skl->boneNum){
							//Get the bone's name.
							size_t boneNameLength;
							getDelimitedString(tokPos, line + lineLength - tokPos, "\" ", &tokPos, &boneNameLength);
							tempBone.name = memAllocate(boneNameLength + 1);
							memcpy(tempBone.name, tokPos, boneNameLength);
							tempBone.name[boneNameLength] = '\0';

							//Get the ID of this bone's parent.
							tempBone.parent = strtoul(tokPos + boneNameLength + 1, NULL, 10);
							if(tempBone.parent == 255){
								tempBone.parent = boneID;
							}


							//If we're out of space, allocate some more!
							if(skl->boneNum >= boneCapacity){
								boneCapacity = skl->boneNum * 2;
								skl->bones = memReallocate(skl->bones, boneCapacity * sizeof(*skl->bones));
							}
							//Add the bone to our vector!
							skl->bones[skl->boneNum] = tempBone;
							++skl->boneNum;
						}else{
							printf("Error loading model!\n"
							       "Path: %s\n"
							       "Line: %s\n"
							       "Error: Found node %lu when expecting node %u!\n",
							       filePath, line, (unsigned long)boneID, skl->boneNum);
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
							if(boneID < skl->boneNum){
								sklNode *currentBone = &skl->bones[boneID];

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
									if(currentBone->parent != boneID){
										//currentBone->defaultState = boneTransformAppend(skl->bones[currentBone->parent].defaultState, currentBone->defaultState);
									}
								}
							}else{
								printf("Error loading model!\n"
									   "Path: %s\n"
									   "Line: %s\n"
									   "Error: Found skeletal data for bone %lu, which doesn't exist!\n",
									   filePath, line, (unsigned long)boneID);
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
							if(parentBoneID < skl->boneNum){
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

									size_t parentPos = (size_t)-1;
									float totalWeight = 0.f;
									size_t i;
									//Load all of the links!
									for(i = 0; i < numLinks; ++i){
										//Load the bone's ID!
										tempVertex.bIDs[i] = strtoul(tokPos, &tokPos, 10);
										//Make sure it exists!
										if(tempVertex.bIDs[i] > skl->boneNum){
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
										if(parentPos == (size_t)-1){
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
										*vertices = memReallocate(*vertices, vertexCapacity * sizeof(**vertices));
									}
									(*vertices)[*vertexNum] = tempVertex;
									++(*vertexNum);
								}
								//Add an index for the new vertex!
								if(*indexNum >= indexCapacity){
									indexCapacity = *indexNum * 2;
									*indices = memReallocate(*indices, indexCapacity * sizeof(**indices));
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

	}else{
		printf("Unable to open model file!\n"
		       "Path: %s\n", filePath);
		return 0;
	}

	return 1;
}
