#include "model.h"
#include "vertex.h"
#include "moduleSkeleton.h"
#include "moduleTextureWrapper.h"
#include "memoryManager.h"
#include "helpersFileIO.h"
/** TEMPORARY **/
#include "helpersMisc.h"
/** TEMPORARY **/
#include <stdlib.h>
#include <string.h>

/**
*** EVERYTHING IN THIS FILE IS TEMPORARY.
**/

#define MODEL_RESOURCE_DIRECTORY_STRING FILE_PATH_RESOURCE_DIRECTORY_SHARED"Resources"FILE_PATH_DELIMITER_STRING"Models"FILE_PATH_DELIMITER_STRING
#define MODEL_RESOURCE_DIRECTORY_LENGTH 19

/** Merge mdlWavefrontObjLoad() with mdlLoad(). **/

#define MODEL_MESH_INITIAL_CAPACITY 1
#define MODEL_VERTEX_INITIAL_CAPACITY 1
#define MODEL_INDEX_INITIAL_CAPACITY 1
#define MODEL_LOD_INITIAL_CAPACITY 1
#define MODEL_BONE_INITIAL_CAPACITY 1

/** Remove printf()s **/

/**#define mdlWavefrontObjFreeReturns() \
	if(*vertices != NULL){ \
		memFree(*vertices); \
	} \
	if(*indices != NULL){ \
		memFree(*indices); \
	} \
	if(*lods != NULL){ \
		memFree(*lods); \
	} \**/

#define mdlWavefrontObjFreeReturns() \
	if(vertices != NULL){ \
		memFree(vertices); \
	} \
	if(indices != NULL){ \
		memFree(indices); \
	} \
	if(lods != NULL){ \
		memFree(lods); \
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

return_t mdlWavefrontObjLoad(modelBase *const __RESTRICT__ base, const char *const __RESTRICT__ filePath, const size_t filePathLength){


	char fullPath[FILE_MAX_PATH_LENGTH];
	fileGenerateFullPath(fullPath, MODEL_RESOURCE_DIRECTORY_STRING, MODEL_RESOURCE_DIRECTORY_LENGTH, filePath, filePathLength);

	mdlBaseInit(base);

	FILE *const __RESTRICT__ mdlInfo = fopen(fullPath, "r");
	if(mdlInfo != NULL){

		char sklPath[FILE_MAX_PATH_LENGTH];
		char twPath[FILE_MAX_PATH_LENGTH];
		size_t sklPathLength = 0;
		size_t twPathLength = 0;

		int i;
		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		vertex tempVert;                 // Holds a vertex before pushing it into the triangle array
		vertexIndex_t positionIndex[3];  // Holds all the positional information for a face
		vertexIndex_t uvIndex[3];        // Holds all the UV information for a face
		vertexIndex_t normalIndex[3];    // Holds all the normal information for a face

		vertexIndex_t vertexNum = 0;
		size_t vertexCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
		vertex *vertices;

		vertexIndex_t indexNum = 0;
		size_t indexCapacity = MODEL_INDEX_INITIAL_CAPACITY;
		vertexIndex_t *indices;

		size_t tempPositionsSize = 0;
		size_t tempPositionsCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
		float *tempPositions;

		size_t tempTexCoordsSize = 0;
		size_t tempTexCoordsCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
		float *tempTexCoords;

		size_t tempNormalsSize = 0;
		size_t tempNormalsCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
		float *tempNormals;

		size_t tempBoneIDsSize = 0;
		size_t tempBoneIDsCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
		int   *tempBoneIDs;

		size_t tempBoneWeightsSize = 0;
		size_t tempBoneWeightsCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
		float *tempBoneWeights;

		size_t lodNum = 0;
		size_t lodCapacity = MODEL_LOD_INITIAL_CAPACITY;
		mdlLOD *lods;
		mdlLOD lodTemp;


		vertices = memAllocate(vertexCapacity*sizeof(vertex));
		if(vertices == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		indices = memAllocate(indexCapacity*sizeof(vertexIndex_t));
		if(indices == NULL){
			/** Memory allocation failure. **/
			memFree(vertices);
			return -1;
		}
		lods = memAllocate(lodCapacity*sizeof(mdlLOD));
		if(lods == NULL){
			/** Memory allocation failure. **/
			memFree(vertices);
			memFree(indices);
			return -1;
		}
		/**const int generatePhysProperties = (mass != NULL && area != NULL && centroid != NULL);
		if(generatePhysProperties){
			*mass = 0.f;
			*area = 0.f;
			centroid = g_vec3Zero;
		}**/
		vertexNum = 0;
		indexNum = 0;
		lodNum = 0;
		lodTemp.distance = 0.f;
		lodTemp.offset = 0;
		lodTemp.indexNum = 0;

		// Temporarily holds vertex positions before they are pushed into vertices
		tempPositions = memAllocate(tempPositionsCapacity*sizeof(float));
		if(tempPositions == NULL){
			/** Memory allocation failure. **/
			memFree(vertices);
			memFree(indices);
			memFree(lods);
			return -1;
		}
		// Temporarily holds vertex UVs they are pushed into vertices
		tempTexCoords = memAllocate(tempTexCoordsCapacity*sizeof(float));
		if(tempTexCoords == NULL){
			/** Memory allocation failure. **/
			memFree(vertices);
			memFree(indices);
			memFree(lods);
			memFree(tempPositions);
			return -1;
		}
		// Temporarily holds vertex normals before they are pushed into vertices
		tempNormals = memAllocate(tempNormalsCapacity*sizeof(float));
		if(tempNormals == NULL){
			/** Memory allocation failure. **/
			memFree(vertices);
			memFree(indices);
			memFree(lods);
			memFree(tempPositions);
			memFree(tempTexCoords);
			return -1;
		}
		// Temporarily holds bone IDs before they are pushed into vertices
		tempBoneIDs = memAllocate(tempBoneIDsCapacity*sizeof(int));
		if(tempBoneIDs == NULL){
			/** Memory allocation failure. **/
			memFree(vertices);
			memFree(indices);
			memFree(lods);
			memFree(tempPositions);
			memFree(tempTexCoords);
			memFree(tempNormals);
			return -1;
		}
		// Temporarily holds bone weights before they are pushed into vertices
		tempBoneWeights = memAllocate(tempBoneWeightsCapacity*sizeof(float));
		if(tempBoneWeights == NULL){
			/** Memory allocation failure. **/
			memFree(vertices);
			memFree(indices);
			memFree(lods);
			memFree(tempPositions);
			memFree(tempTexCoords);
			memFree(tempNormals);
			memFree(tempBoneIDs);
			return -1;
		}

		while(fileParseNextLine(mdlInfo, lineBuffer, sizeof(lineBuffer), &line, &lineLength)){

			// Skeleton
			if(sklPathLength == 0 && lineLength > 9 && strncmp(line, "skeleton ", 9) == 0){
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
				memcpy(sklPath, line+pathBegin, pathLength);
				sklPathLength = pathLength;

			// Texture wrapper
			}else if(twPathLength == 0 && lineLength > 8 && strncmp(line, "texture ", 8) == 0){
				size_t pathBegin;
				size_t pathLength;
				const char *firstQuote = strchr(line+8, '"');
				const char *secondQuote = NULL;
				if(firstQuote != NULL){
					++firstQuote;
					pathBegin = firstQuote-line;
					secondQuote = strrchr(firstQuote, '"');
				}
				if(secondQuote > firstQuote){
					pathLength = secondQuote-firstQuote;
				}else{
					pathBegin = 8;
					pathLength = lineLength-pathBegin;
				}
				memcpy(twPath, line+pathBegin, pathLength);
				twPathLength = pathLength;

			// Sprite
			}else if(lineLength >= 6 && strncmp(line, "sprite ", 7) == 0){
				///*sprite = strtoul(line+6, NULL, 0);

			// LOD
			}else if(lineLength >= 5 && strncmp(line, "lod ", 4) == 0){
				const float lodNewDistance = strtod(line+4, NULL);
				if((lodNum == 0 && lodNewDistance == 0.f) || ((lodNum > 0 || lodTemp.distance == 0.f) && lodNewDistance > lodTemp.distance)){
					// The new LOD seems valid.
					// If one was specified before it, add it to the array.
					if(lodNewDistance != 0.f){
						lodTemp.indexNum = indexNum - lodTemp.indexNum;
						if(pushDynamicArray((void **)&lods, &lodTemp, sizeof(mdlLOD), &lodNum, &lodCapacity) < 0){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
					}
					// Start the next LOD.
					lodTemp.distance = lodNewDistance;
					lodTemp.offset = (void *)((uintptr_t)indexNum * sizeof(vertexIndex_t));
					lodTemp.indexNum = indexNum;
				}

			// Vertex data
			}else if(lineLength >= 7 && strncmp(line, "v ", 2) == 0){
				const char *token = strtok(line+2, " ");
				float curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempPositions, &curVal, sizeof(float), &tempPositionsSize, &tempPositionsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempPositions, &curVal, sizeof(float), &tempPositionsSize, &tempPositionsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempPositions, &curVal, sizeof(float), &tempPositionsSize, &tempPositionsCapacity) < 0){
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
					if(pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(int), &tempBoneIDsSize, &tempBoneIDsCapacity) < 0){
						/** Memory allocation failure. **/
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return -1;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(int), &tempBoneIDsSize, &tempBoneIDsCapacity) < 0){
						/** Memory allocation failure. **/
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return -1;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(int), &tempBoneIDsSize, &tempBoneIDsCapacity) < 0){
						/** Memory allocation failure. **/
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return -1;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(int), &tempBoneIDsSize, &tempBoneIDsCapacity) < 0){
						/** Memory allocation failure. **/
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return -1;
					}
					token = strtok(NULL, " ");
					if(token != NULL){
						curVal = strtod(token, NULL);
						if(pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(float), &tempBoneWeightsSize, &tempBoneWeightsCapacity) < 0){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(float), &tempBoneWeightsSize, &tempBoneWeightsCapacity) < 0){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(float), &tempBoneWeightsSize, &tempBoneWeightsCapacity) < 0){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(float), &tempBoneWeightsSize, &tempBoneWeightsCapacity) < 0){
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
				if(pushDynamicArray((void **)&tempTexCoords, &curVal, sizeof(float), &tempTexCoordsSize, &tempTexCoordsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempTexCoords, &curVal, sizeof(float), &tempTexCoordsSize, &tempTexCoordsCapacity) < 0){
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
				if(pushDynamicArray((void **)&tempNormals, &curVal, sizeof(float), &tempNormalsSize, &tempNormalsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempNormals, &curVal, sizeof(float), &tempNormalsSize, &tempNormalsCapacity) < 0){
					/** Memory allocation failure. **/
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempNormals, &curVal, sizeof(float), &tempNormalsSize, &tempNormalsCapacity) < 0){
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
					size_t j;

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
						tempVert.position = g_vec3Zero;
					}
					// Vertex UV data
					pos = uvIndex[i]<<1;
					if(pos+1 < tempTexCoordsSize){
						tempVert.u = tempTexCoords[pos];
						// Handle OpenGL UV coordinate system.
						tempVert.v = 1.f-tempTexCoords[pos+1];
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
						tempVert.normal = g_vec3Zero;
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
					for(j = 0; j < vertexNum; ++j){
						const vertex *const checkVert = &vertices[j];
						/** CHECK BONE DATA HERE **/
						if(memcmp(checkVert, &tempVert, sizeof(vertex)) == 0){
							// Resize indices if there's not enough room
							size_t tempIndexNum = indexNum;
							if(pushDynamicArray((void **)&indices, &j, sizeof(vertexIndex_t), &tempIndexNum, &indexCapacity) < 0){
								/** Memory allocation failure. **/
								mdlWavefrontObjFreeHelpers();
								mdlWavefrontObjFreeReturns();
								fclose(mdlInfo);
								return -1;
							}
							indexNum = tempIndexNum;
							foundVertex = 1;
							break;
						}
					}

					// If the vertex has not yet been loaded, add it to both the vertex vector and the index vector
					if(!foundVertex){
						// Resize indices if there's not enough room
						size_t tempIndexNum = indexNum;
						size_t tempVertexNum = vertexNum;
						if(pushDynamicArray((void **)&indices, &vertexNum, sizeof(vertexIndex_t), &tempIndexNum, &indexCapacity) < 0 ||
						   pushDynamicArray((void **)&vertices, &tempVert, sizeof(vertex), &tempVertexNum, &vertexCapacity) < 0){
							/** Memory allocation failure. **/
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
						indexNum = tempIndexNum;
						vertexNum = tempVertexNum;
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
		if(lodNum > 0 && lodTemp.distance != 0.f){
			// The new LOD seems valid.
			// If one was specified before it, add it to the array.
			lodTemp.indexNum = indexNum - lodTemp.indexNum;
			if(pushDynamicArray((void **)&lods, &lodTemp, sizeof(mdlLOD), &lodNum, &lodCapacity) < 0){
				/** Memory allocation failure. **/
				mdlWavefrontObjFreeReturns();
				return -1;
			}
		}

		if(vertexNum > 0){

			base->meshNum = 1;
			base->meshes = memAllocate(sizeof(mesh) + sizeof(textureWrapper *) + base->lodNum*sizeof(mdlLOD) + (filePathLength/**-extension**/+1)*sizeof(char));
			if(base->meshes == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			base->textures = (const textureWrapper **)&base->meshes[base->meshNum];
			if(lodNum <= 1){
				base->lods = NULL;
				base->name = (char *)&base->textures[base->meshNum];
			}else{
				base->lods = (mdlLOD *)&base->textures[base->meshNum];
				base->name = (char *)&base->lods[base->meshNum];
			}
			memcpy(base->name, filePath, filePathLength/**-extension**/);
			base->name[filePathLength/**-extension**/] = '\0';

			meshGenerateBuffers(base->meshes, vertices, vertexNum, indices, indexNum);

			if(twPathLength > 0){
				textureWrapper *const tempTw = moduleTextureWrapperAllocate();
				twLoad(tempTw, twPath, twPathLength);
				base->textures[0] = tempTw;
			}else{
				base->textures = &g_twDefaultP;
			}

			if(sklPathLength > 0){
				skeleton *const tempSkl = moduleSkeletonAllocate();
				sklLoad(tempSkl, sklPath, sklPathLength);
				base->skl = tempSkl;
			}else{
				base->skl = &g_sklDefault;
			}

		}

		mdlWavefrontObjFreeReturns();
		return 1;

	}else{
		printf("Error loading model \"%s\": Could not open file.\n", fullPath);
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













#define mdlSMDFreeReturns() \
	if(*vertices != NULL){ \
		memFree(*vertices); \
	} \
	if(*indices != NULL){ \
		memFree(*indices); \
	} \
	sklDelete(skl);

#define mdlSMDFreeHelpers() \
	memFree(nameArray);

// We need to store an array of vertices and indices for each mesh
// we're loading. We use a separate mesh for each texture group.
typedef struct {
	char *texGroupPath;

	vertexIndex_t tempVerticesSize;
	vertexIndex_t tempVerticesCapacity;
	vertex *tempVertices;

	vertexIndex_t tempIndicesSize;
	vertexIndex_t tempIndicesCapacity;
	vertexIndex_t *tempIndices;
} meshData;

/** Temporary! **/
static void sklDefragmentSMD(skeleton *const __RESTRICT__ skl, const char *const __RESTRICT__ resource, const size_t length, char *const __RESTRICT__ nameArray, const size_t nameArrayLength, sklNode *const __RESTRICT__ tempBones, const size_t tempBonesSize){

	frameIndex_t i;
	char *namePtr;

	skl->boneNum = tempBonesSize;
	skl->bones = memReallocate(tempBones, tempBonesSize*sizeof(sklNode) + nameArrayLength + length + 1);
	namePtr = (char *)&skl->bones[skl->boneNum];
	memcpy(namePtr, nameArray, nameArrayLength);
	memFree(nameArray);

	for(i = 0; i < tempBonesSize; ++i){
		/// Fix comments referencing globalBindInverse when removing this function.
		skl->bones[i].globalBindInverse = tfInverse(skl->bones[i].globalBindInverse);
		skl->bones[i].name = namePtr;
		namePtr += strlen(namePtr)+1;
	}

	memcpy(namePtr, resource, length);
	namePtr[length] = '\0';
	skl->name = namePtr;

}

return_t mdlSMDLoad(modelBase *const __RESTRICT__ base, const char *const __RESTRICT__ filePath, const size_t filePathLength){

	char fullPath[FILE_MAX_PATH_LENGTH];
	fileGenerateFullPath(fullPath, MODEL_RESOURCE_DIRECTORY_STRING, MODEL_RESOURCE_DIRECTORY_LENGTH, filePath, filePathLength);

	mdlBaseInit(base);

	// Load the model!
	FILE *const __RESTRICT__ mdlInfo = fopen(fullPath, "r");
	if(mdlInfo != NULL){
		return_t success = 1;

		char *nameArray = NULL;
		size_t nameArrayLength = 0;

		size_t tempBonesSize = 0;
		size_t tempBonesCapacity = MODEL_BONE_INITIAL_CAPACITY;
		sklNode *tempBones;

		size_t tempMeshDataSize = 0;
		size_t tempMeshDataCapacity = MODEL_MESH_INITIAL_CAPACITY;
		meshData *tempMeshData;
		meshData *curMeshData = NULL;

		char *tokPos;
		// This indicates what sort of data we're currently supposed to be reading.
		byte_t dataType = 0;
		// This variable stores data specific to the type we're currently loading.
		unsigned int data = 0;

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;


		// Temporarily stores bones.
		tempBones = memAllocate(MODEL_BONE_INITIAL_CAPACITY * sizeof(*tempBones));
		if(tempBones == NULL){
			/** Memory allocation failure. **/
		}
		// Temporarily stores the vertices, indices
		// and texture group path for each mesh.
		tempMeshData = memAllocate(MODEL_MESH_INITIAL_CAPACITY * sizeof(*tempMeshData));
		if(tempMeshData == NULL){
			/** Memory allocation failure. **/
		}


		while(success && fileParseNextLine(mdlInfo, lineBuffer, sizeof(lineBuffer), &line, &lineLength)){
			if(dataType == 0){
				if(strcmp(line, "nodes") == 0){
					dataType = 1;
				}else if(strcmp(line, "skeleton") == 0){
					dataType = 2;
				}else if(strcmp(line, "triangles") == 0){
					dataType = 3;

				// If this isn't the version number and the line isn't empty, it's something we can't handle!
				}else if(lineLength > 0 && strcmp(line, "version 1") != 0){
					printf(
						"Error loading model!\n"
						"Path: %s\n"
						"Line: %s\n"
						"Error: Unexpected identifier!\n",
						fullPath, line
					);
				}
			}else{
				if(strcmp(line, "end") == 0){
					// If we've finished identifying the skeleton's bones, shrink the vector!
					if(dataType == 1){
						tempBonesCapacity = tempBonesSize;
						tempBones = memReallocate(tempBones, tempBonesCapacity * sizeof(*tempBones));
						if(tempBones == NULL){
							/** Memory allocation failure. **/
						}
					}

					dataType = 0;
					data = 0;
				}else{
					if(dataType == 1){
						// Get this bone's ID.
						const boneIndex_t boneID = strtoul(line, &tokPos, 10);
						if(boneID == tempBonesSize){
							sklNode tempBone;
							// Get the bone's name.
							const size_t boneNameLength = getDelimitedString(tokPos, line + lineLength - tokPos, "\" ", &tokPos);
							nameArray = memReallocate(nameArray, (nameArrayLength+boneNameLength+1)*sizeof(char));
							memcpy(&nameArray[nameArrayLength], tokPos, boneNameLength);
							nameArrayLength += boneNameLength;
							nameArray[nameArrayLength] = '\0';
							++nameArrayLength;

							//Get the ID of this bone's parent.
							tempBone.parent = strtoul(tokPos + boneNameLength + 1, NULL, 10);
							if(tempBone.parent == 255){
								tempBone.parent = boneID;
							}

							// If we're out of space, allocate some more!
							if(tempBonesSize >= tempBonesCapacity){
								tempBonesCapacity = tempBonesSize * 2;
								tempBones = memReallocate(tempBones, tempBonesCapacity * sizeof(*tempBones));
								if(tempBones == NULL){
									/** Memory allocation failure. **/
								}
							}
							// Add the bone to our vector!
							tempBones[tempBonesSize] = tempBone;
							++tempBonesSize;
						}else{
							printf(
								"Error loading model!\n"
								"Path: %s\n"
								"Line: %s\n"
								"Error: Found node %u when expecting node %lu!\n",
								fullPath, line, boneID, (unsigned long)tempBonesSize
							);

							success = 0;
						}
					}else if(dataType == 2){
						// If the line begins with time, get the frame's timestamp!
						if(lineLength >= 6 && memcmp(line, "time ", 5) == 0){
							const unsigned int newTime = strtoul(&line[5], NULL, 10);
							if(newTime >= data){
								data = newTime;
							}else{
								printf(
									"Error loading model!\n"
									"Path: %s\n"
									"Line: %s\n"
									"Error: Frame timestamps do not increment sequentially!\n",
									fullPath, line
								);

								success = 0;
							}

						// Otherwise, we're setting the bone's state!
						}else{
							// Get this bone's ID.
							const boneIndex_t boneID = strtoul(line, &tokPos, 10);
							// Make sure a bone with this ID actually exists.
							if(boneID < tempBonesSize){
								sklNode *currentBone = &tempBones[boneID];

								// If the current frame timestamp is 0, set the bone's initial state!
								if(data == 0){
									// Load the bone's position!
									float x = strtof(tokPos, &tokPos);
									float y = strtof(tokPos, &tokPos);
									float z = strtof(tokPos, &tokPos);
									vec3Set(&currentBone->localBind.position, x, y, z);

									// Load the bone's rotation!
									x = strtof(tokPos, &tokPos);
									y = strtof(tokPos, &tokPos);
									z = strtof(tokPos, NULL);
									quatSetEuler(&currentBone->localBind.orientation, x, y, z);

									// Set the bone's scale!
									vec3Set(&currentBone->localBind.scale, 1.f, 1.f, 1.f);
									currentBone->localBind.shear = g_quatIdentity;

									// Accumulate the global bind poses; NOT the global inverse bind poses.
									// We invert these later on in sklDefragmentSMD.
									if(currentBone->parent == boneID){
										// The root just starts with the local bind pose.
										currentBone->globalBindInverse = currentBone->localBind;
									}else{
										currentBone->globalBindInverse = tfMultiply(tempBones[currentBone->parent].globalBindInverse, currentBone->localBind);
									}
								}
							}else{
								printf(
									"Error loading model!\n"
									"Path: %s\n"
									"Line: %s\n"
									"Error: Found skeletal data for bone %u, which doesn't exist!\n",
									fullPath, line, boneID
								);

								success = 0;
							}
						}
					}else if(dataType == 3){
						// Texture group for the following face.
						if(data == 0){
							char *const texGroupPath = memAllocate((lineLength + 5) * sizeof(*texGroupPath));
							size_t texGroupPathLength;
							if(texGroupPath == NULL){
								/** Memory allocation failure. **/
							}
							texGroupPathLength = fileParseResourcePath(texGroupPath, line, lineLength);
							// Add the file extension!
							memcpy(&texGroupPath[texGroupPathLength], ".tdw", sizeof(".tdw"));

							// If we've seen this texture group before,
							// we should switch to the mesh that uses it.
							{
								const meshData *const lastMeshData = &tempMeshData[tempMeshDataSize];
								for(curMeshData = tempMeshData; curMeshData < lastMeshData; ++curMeshData){
									// If we've seen this texture group before, we'll need
									// to free the memory we allocated for the new path.
									if(strcmp(texGroupPath, curMeshData->texGroupPath) == 0){
										memFree(texGroupPath);
										break;
									}
								}

								// If this texture group is new, create a new mesh for it!
								if(curMeshData == lastMeshData){
									// If we're out of space, allocate some more!
									if(tempMeshDataSize >= tempMeshDataCapacity){
										tempMeshDataCapacity = tempMeshDataSize * 2;
										tempMeshData = memReallocate(
											tempMeshData, tempMeshDataCapacity * sizeof(*tempMeshData)
										);
										if(tempMeshData == NULL){
											/** Memory allocation failure. **/
										}
									}

									// Initialize the new mesh data!
									curMeshData = &tempMeshData[tempMeshDataSize];
									curMeshData->texGroupPath = texGroupPath;
									curMeshData->tempVerticesSize = 0;
									curMeshData->tempVerticesCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
									curMeshData->tempVertices = memAllocate(
										MODEL_VERTEX_INITIAL_CAPACITY * sizeof(*curMeshData->tempVertices)
									);
									if(curMeshData->tempVertices == NULL){
										/** Memory allocation failure. **/
									}
									curMeshData->tempIndicesSize = 0;
									curMeshData->tempIndicesCapacity = MODEL_INDEX_INITIAL_CAPACITY;
									curMeshData->tempIndices = memAllocate(
										MODEL_INDEX_INITIAL_CAPACITY * sizeof(*curMeshData->tempIndices)
									);
									if(curMeshData->tempIndices == NULL){
										/** Memory allocation failure. **/
									}

									++tempMeshDataSize;
								}
							}

						// Vertex.
						}else{
							// Read the vertex data from the line!
							const int parentBoneID = strtoul(line, &tokPos, 10);
							// Make sure a bone with this ID actually exists.
							if(parentBoneID < tempBonesSize){
								vertex tempVertex;
								int numLinks;
								const vertex *checkVertex;
								int i;

								tempVertex.position.x = strtof(tokPos, &tokPos);
								tempVertex.position.y = strtof(tokPos, &tokPos);
								tempVertex.position.z = strtof(tokPos, &tokPos);
								tempVertex.normal.x = strtof(tokPos, &tokPos);
								tempVertex.normal.y = strtof(tokPos, &tokPos);
								tempVertex.normal.z = strtof(tokPos, &tokPos);
								tempVertex.u = strtof(tokPos, &tokPos);
								tempVertex.v = 1.f - strtof(tokPos, &tokPos);

								numLinks = strtoul(tokPos, &tokPos, 10);
								// Make sure some links were specified.
								if(numLinks > 0){
									int parentPos = (boneIndex_t)-1;
									int *curBoneID = tempVertex.bIDs;
									int curLink;
									float totalWeight = 0.f;
									float *curBoneWeight = tempVertex.bWeights;


									// If there are more than the maximum number of
									// supported weights, we'll have to clamp it down!
									if(numLinks > VERTEX_MAX_BONES){
										printf(
											"Error loading model!\n"
											"Path: %s\n"
											"Line: %s\n"
											"Error: Vertex has too many links! All extra links will be ignored.\n",
											fullPath, line
										);

										numLinks = VERTEX_MAX_BONES;
									}

									// Load all of the links!
									for(curLink = 0; curLink < numLinks; ++curLink){
										// Load the bone's ID!
										*curBoneID = strtoul(tokPos, &tokPos, 10);
										// Make sure it exists!
										if(*curBoneID > tempBonesSize){
											printf(
												"Error loading model!\n"
												"Path: %s\n"
												"Line: %s\n"
												"Error: Vertex link bone doesn't exist! The parent bone will be used instead.\n",
												fullPath, line
											);

										// If we're loading the parent bone, remember its position!
										}else if(*curBoneID == parentBoneID){
											parentPos = curLink;
										}

										// Load the bone's weights!
										*curBoneWeight = strtof(tokPos, &tokPos);
										totalWeight += *curBoneWeight;
										// Make sure the total weight doesn't exceed 1!
										if(totalWeight > 1.f){
											*curBoneWeight -= totalWeight - 1.f;
											totalWeight = 1.f;

											++curLink;
											++curBoneID;
											++curBoneWeight;

											break;
										}

										++curBoneID;
										++curBoneWeight;
									}

									// Make sure the total weight isn't less than 1!
									if(totalWeight < 1.f){
										// If we never loaded the parent bone, see if we can add it!
										if(parentPos == (boneIndex_t)-1){
											if(curLink < VERTEX_MAX_BONES){
												*curBoneID = parentBoneID;
												*curBoneWeight = 0.f;
												parentPos = curLink;

												++curLink;
												++curBoneID;
												++curBoneWeight;

											// If there's no room, just use the first bone we loaded.
											}else{
												parentPos = 0;
											}
										}

										tempVertex.bWeights[parentPos] += 1.f - totalWeight;
									}

									// Make sure we fill the rest with invalid values so we know they aren't used.
									memset(curBoneID, (boneIndex_t)-1, (VERTEX_MAX_BONES - curLink) * sizeof(*tempVertex.bIDs));
									memset(curBoneWeight, 0.f, (VERTEX_MAX_BONES - curLink) * sizeof(*tempVertex.bWeights));

								// Otherwise, just bind it to the parent bone.
								}else{
									/**printf(
										"Error loading model!\n"
										"Path: %s\n"
										"Line: %s\n"
										"Error: Vertex has no links! The parent bone will be used.\n",
										fullPath, line
									);**/

									tempVertex.bIDs[0] = parentBoneID;
									memset(&tempVertex.bIDs[1], (boneIndex_t)-1, (VERTEX_MAX_BONES - 1) * sizeof(*tempVertex.bIDs));
									tempVertex.bWeights[0] = 1.f;
									memset(&tempVertex.bWeights[1], 0.f, (VERTEX_MAX_BONES - 1) * sizeof(*tempVertex.bWeights));
								}


								// If we haven't seen any texture groups yet,
								// allocate a new mesh using the default one.
								if(curMeshData == NULL){
									curMeshData = tempMeshData;
									curMeshData->texGroupPath = NULL;
									curMeshData->tempVerticesSize = 0;
									curMeshData->tempVerticesCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
									curMeshData->tempVertices = memAllocate(
										MODEL_VERTEX_INITIAL_CAPACITY * sizeof(*curMeshData->tempVertices)
									);
									if(curMeshData->tempVertices == NULL){
										/** Memory allocation failure. **/
									}
									curMeshData->tempIndicesSize = 0;
									curMeshData->tempIndicesCapacity = MODEL_INDEX_INITIAL_CAPACITY;
									curMeshData->tempIndices = memAllocate(
										MODEL_INDEX_INITIAL_CAPACITY * sizeof(*curMeshData->tempIndices)
									);
									if(curMeshData->tempIndices == NULL){
										/** Memory allocation failure. **/
									}
								}


								checkVertex = curMeshData->tempVertices;
								// Check if this vertex already exists!
								for(i = 0; i < curMeshData->tempVerticesSize; ++i){
									// Looks like it does, so we don't need to store it again!
									if(memcmp(checkVertex, &tempVertex, sizeof(vertex)) == 0){
										break;
									}
									++checkVertex;
								}

								// The vertex does not exist, so add it to the vector!
								if(i == curMeshData->tempVerticesSize){
									// If we're out of space, allocate some more!
									if(curMeshData->tempVerticesSize >= curMeshData->tempVerticesCapacity){
										curMeshData->tempVerticesCapacity = curMeshData->tempVerticesSize * 2;
										curMeshData->tempVertices = memReallocate(
											curMeshData->tempVertices,
											curMeshData->tempVerticesCapacity * sizeof(*curMeshData->tempVertices)
										);
										if(curMeshData->tempVertices == NULL){
											/** Memory allocation failure. **/
										}
									}
									curMeshData->tempVertices[curMeshData->tempVerticesSize] = tempVertex;
									++curMeshData->tempVerticesSize;
								}

								// Add an index for the new vertex!
								if(curMeshData->tempIndicesSize >= curMeshData->tempIndicesCapacity){
									curMeshData->tempIndicesCapacity = curMeshData->tempIndicesSize * 2;
									curMeshData->tempIndices = memReallocate(
										curMeshData->tempIndices,
										curMeshData->tempIndicesCapacity * sizeof(*curMeshData->tempIndices)
									);
									if(curMeshData->tempIndices == NULL){
										/** Memory allocation failure. **/
									}
								}
								curMeshData->tempIndices[curMeshData->tempIndicesSize] = i;
								++curMeshData->tempIndicesSize;
							}else{
								printf(
									"Error loading model!\n"
									"Path: %s\n"
									"Line: %s\n"
									"Error: Vertex parent bone doesn't exist!\n",
									fullPath, line
								);

								success = 0;
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

		fclose(mdlInfo);

		// There's no point keeping the model if no meshes were loaded.
		if(tempMeshDataSize > 0){
			const meshData *const lastMeshData = &tempMeshData[tempMeshDataSize];
			size_t numMeshes = 0;
			// Count the number of valid meshes (meshes with at least one face).
			for(curMeshData = tempMeshData; curMeshData < lastMeshData; ++curMeshData){
				if(curMeshData->tempIndicesSize >= 3){
					++numMeshes;

				// If the mesh is invalid, clean it up
				// now so we don't have to do it later.
				}else{
					memFree(curMeshData->texGroupPath);
					memFree(curMeshData->tempVertices);
					memFree(curMeshData->tempIndices);
				}
			}

			// If there's at least one valid mesh, the model should be valid.
			if(numMeshes > 0){
				mesh *curMesh;
				textureWrapper **curTexGroup;

				curMesh = memAllocate(numMeshes * (sizeof(*base->meshes) + sizeof(*base->textures)) + filePathLength + 1);
				if(curMesh == NULL){
					/** Memory allocation failure. **/
				}
				curTexGroup = (textureWrapper **)&curMesh[numMeshes];
				base->meshes = curMesh;
				base->textures = (const textureWrapper **)curTexGroup;
				base->name = (char *)&curTexGroup[numMeshes];
				base->meshNum = numMeshes;
				memcpy(base->name, filePath, filePathLength + 1);

				// Initialise the model's skeleton!
				if(tempBonesSize > 0){
					skeleton *const tempSkl = moduleSkeletonAllocate();
					sklDefragmentSMD(tempSkl, filePath, filePathLength, nameArray, nameArrayLength, tempBones, tempBonesSize);
					base->skl = tempSkl;
				}else{
					base->skl = &g_sklDefault;
				}
				tempBonesCapacity = 0;

				// Generate buffer objects for each valid
				// mesh and load their texture groups.
				for(curMeshData = tempMeshData; curMeshData < lastMeshData; ++curMeshData){
					if(curMeshData->tempIndicesSize >= 3){
						meshGenerateBuffers(
							curMesh,
							curMeshData->tempVertices, curMeshData->tempVerticesSize,
							curMeshData->tempIndices, curMeshData->tempIndicesSize
						);
						if(curMeshData->texGroupPath == NULL){
							*curTexGroup = &g_twDefault;
						}else{
							textureWrapper *const tempTw = moduleTextureWrapperAllocate();
							twLoad(tempTw, curMeshData->texGroupPath, strlen(curMeshData->texGroupPath));
							*curTexGroup = tempTw;
						}

						// The mesh and texture group should allocate
						// their own memory, so we don't need these anymore.
						memFree(curMeshData->texGroupPath);
						memFree(curMeshData->tempVertices);
						memFree(curMeshData->tempIndices);

						++curMesh;
						++curTexGroup;
					}
				}
			}
		}

		// We've already deleted any arrays stored here.
		memFree(tempMeshData);
		if(tempBonesCapacity > 0){
			memFree(tempBones);
		}

		return 1;

	}else{
		printf(
			"Unable to open model file!\n"
			"Path: %s\n",
			fullPath
		);
		return 0;
	}
}






























/**return_t mdlWavefrontObjLoad(const char *const __RESTRICT__ filePath, vertexIndex_t *const __RESTRICT__ vertexNum, vertex **const vertices, vertexIndex_t *const __RESTRICT__ indexNum, vertexIndex_t **const indices, size_t *const __RESTRICT__ lodNum, mdlLOD **const lods, int *const __RESTRICT__ sprite, char *const __RESTRICT__ sklPath, size_t *const __RESTRICT__ sklPathLength, char *const __RESTRICT__ twPath, size_t *const __RESTRICT__ twPathLength){

	FILE *const __RESTRICT__ mdlInfo = fopen(filePath, "r");

	if(mdlInfo != NULL){

		int i;
		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		vertex tempVert;                 // Holds a vertex before pushing it into the triangle array
		vertexIndex_t positionIndex[3];  // Holds all the positional information for a face
		vertexIndex_t uvIndex[3];        // Holds all the UV information for a face
		vertexIndex_t normalIndex[3];    // Holds all the normal information for a face

		size_t vertexCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
		size_t indexCapacity = MODEL_INDEX_INITIAL_CAPACITY;

		size_t tempPositionsSize = 0;
		size_t tempPositionsCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
		float *tempPositions;

		size_t tempTexCoordsSize = 0;
		size_t tempTexCoordsCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
		float *tempTexCoords;

		size_t tempNormalsSize = 0;
		size_t tempNormalsCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
		float *tempNormals;

		size_t tempBoneIDsSize = 0;
		size_t tempBoneIDsCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
		int   *tempBoneIDs;

		size_t tempBoneWeightsSize = 0;
		size_t tempBoneWeightsCapacity = MODEL_VERTEX_INITIAL_CAPACITY;
		float *tempBoneWeights;

		size_t lodCapacity = MODEL_LOD_INITIAL_CAPACITY;
		mdlLOD lodTemp;


		*vertices = memAllocate(vertexCapacity*sizeof(vertex));
		if(*vertices == NULL){
			/** Memory allocation failure. **
			return -1;
		}
		*indices = memAllocate(indexCapacity*sizeof(vertexIndex_t));
		if(*indices == NULL){
			/** Memory allocation failure. **
			memFree(*vertices);
			return -1;
		}
		*lods = memAllocate(lodCapacity*sizeof(mdlLOD));
		if(*lods == NULL){
			/** Memory allocation failure. **
			memFree(*vertices);
			memFree(*indices);
			return -1;
		}
		/**const int generatePhysProperties = (mass != NULL && area != NULL && centroid != NULL);
		if(generatePhysProperties){
			*mass = 0.f;
			*area = 0.f;
			centroid = g_vec3Zero;
		}**
		*vertexNum = 0;
		*indexNum = 0;
		*lodNum = 0;
		lodTemp.distance = 0.f;
		lodTemp.offset = 0;
		lodTemp.indexNum = 0;

		// Temporarily holds vertex positions before they are pushed into vertices
		tempPositions = memAllocate(tempPositionsCapacity*sizeof(float));
		if(tempPositions == NULL){
			/** Memory allocation failure. **
			memFree(*vertices);
			memFree(*indices);
			memFree(*lods);
			return -1;
		}
		// Temporarily holds vertex UVs they are pushed into vertices
		tempTexCoords = memAllocate(tempTexCoordsCapacity*sizeof(float));
		if(tempTexCoords == NULL){
			/** Memory allocation failure. **
			memFree(*vertices);
			memFree(*indices);
			memFree(*lods);
			memFree(tempPositions);
			return -1;
		}
		// Temporarily holds vertex normals before they are pushed into vertices
		tempNormals = memAllocate(tempNormalsCapacity*sizeof(float));
		if(tempNormals == NULL){
			/** Memory allocation failure. **
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
			/** Memory allocation failure. **
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
			/** Memory allocation failure. **
			memFree(*vertices);
			memFree(*indices);
			memFree(*lods);
			memFree(tempPositions);
			memFree(tempTexCoords);
			memFree(tempNormals);
			memFree(tempBoneIDs);
			return -1;
		}

		while(fileParseNextLine(mdlInfo, lineBuffer, sizeof(lineBuffer), &line, &lineLength)){

			// Skeleton
			if(sklPath != NULL && *sklPathLength == 0 && lineLength > 9 && strncmp(line, "skeleton ", 9) == 0){
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
				memcpy(sklPath, line+pathBegin, pathLength);
				*sklPathLength = pathLength;

			// Texture wrapper
			}else if(twPath != NULL && *twPathLength == 0 && lineLength > 8 && strncmp(line, "texture ", 8) == 0){
				size_t pathBegin;
				size_t pathLength;
				const char *firstQuote = strchr(line+8, '"');
				const char *secondQuote = NULL;
				if(firstQuote != NULL){
					++firstQuote;
					pathBegin = firstQuote-line;
					secondQuote = strrchr(firstQuote, '"');
				}
				if(secondQuote > firstQuote){
					pathLength = secondQuote-firstQuote;
				}else{
					pathBegin = 8;
					pathLength = lineLength-pathBegin;
				}
				memcpy(twPath, line+pathBegin, pathLength);
				*twPathLength = pathLength;

			// Sprite
			}else if(lineLength >= 6 && strncmp(line, "sprite ", 7) == 0){
				*sprite = strtoul(line+6, NULL, 0);

			// LOD
			}else if(lineLength >= 5 && strncmp(line, "lod ", 4) == 0){
				const float lodNewDistance = strtod(line+4, NULL);
				if((*lodNum == 0 && lodNewDistance == 0.f) || ((*lodNum > 0 || lodTemp.distance == 0.f) && lodNewDistance > lodTemp.distance)){
					// The new LOD seems valid.
					// If one was specified before it, add it to the array.
					if(lodNewDistance != 0.f){
						lodTemp.indexNum = *indexNum - lodTemp.indexNum;
						if(pushDynamicArray((void **)lods, &lodTemp, sizeof(mdlLOD), lodNum, &lodCapacity) < 0){
							/** Memory allocation failure. **
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
					/** Memory allocation failure. **
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity) < 0){
					/** Memory allocation failure. **
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity) < 0){
					/** Memory allocation failure. **
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				/****
				token = strtok(NULL, " ");
				if(token != NULL){
					int curBoneID = strtoul(token, NULL, 0);
					if(pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity) < 0){
						/** Memory allocation failure. **
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return -1;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity) < 0){
						/** Memory allocation failure. **
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return -1;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity) < 0){
						/** Memory allocation failure. **
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return -1;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity) < 0){
						/** Memory allocation failure. **
						mdlWavefrontObjFreeHelpers();
						mdlWavefrontObjFreeReturns();
						fclose(mdlInfo);
						return -1;
					}
					token = strtok(NULL, " ");
					if(token != NULL){
						curVal = strtod(token, NULL);
						if(pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity) < 0){
							/** Memory allocation failure. **
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity) < 0){
							/** Memory allocation failure. **
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity) < 0){
							/** Memory allocation failure. **
							mdlWavefrontObjFreeHelpers();
							mdlWavefrontObjFreeReturns();
							fclose(mdlInfo);
							return -1;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity) < 0){
							/** Memory allocation failure. **
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
					/** Memory allocation failure. **
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempTexCoords, &curVal, sizeof(curVal), &tempTexCoordsSize, &tempTexCoordsCapacity) < 0){
					/** Memory allocation failure. **
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
					/** Memory allocation failure. **
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempNormals, &curVal, sizeof(curVal), &tempNormalsSize, &tempNormalsCapacity) < 0){
					/** Memory allocation failure. **
					mdlWavefrontObjFreeHelpers();
					mdlWavefrontObjFreeReturns();
					fclose(mdlInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempNormals, &curVal, sizeof(curVal), &tempNormalsSize, &tempNormalsCapacity) < 0){
					/** Memory allocation failure. **
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
						tempVert.position = g_vec3Zero;
					}
					// Vertex UV data
					pos = uvIndex[i]<<1;
					if(pos+1 < tempTexCoordsSize){
						tempVert.u = tempTexCoords[pos];
						// Handle OpenGL UV coordinate system.
						tempVert.v = 1.f-tempTexCoords[pos+1];
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
						tempVert.normal = g_vec3Zero;
					}
					/****
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
					/****
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
						/** CHECK BONE DATA HERE **
						if(memcmp(checkVert, &tempVert, sizeof(vertex)) == 0){
							// Resize indices if there's not enough room
							size_t tempIndexNum = *indexNum;
							if(pushDynamicArray((void **)indices, &j, sizeof(j), &tempIndexNum, &indexCapacity) < 0){
								/** Memory allocation failure. **
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
							/** Memory allocation failure. **
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
						}**
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
				/** Memory allocation failure. **
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
	}**

	return 1;

}**/




/**return_t mdlSMDLoad(const char *filePath, vertexIndex_t *vertexNum, vertex **vertices, vertexIndex_t *indexNum, vertexIndex_t **indices, skeleton *const skl, char *const __RESTRICT__ twPath, size_t *const __RESTRICT__ twPathLength){
	// Temporary function by 8426THMY.
	//Load the textureGroup!
	FILE *mdlFile = fopen(filePath, "r");
	if(mdlFile != NULL){
		size_t vertexCapacity = MDL_VERTEX_INITIAL_CAPACITY;
		size_t indexCapacity = MDL_INDEX_INITIAL_CAPACITY;
		size_t boneCapacity = 1;
		//This indicates what sort of data we're currently supposed to be reading.
		unsigned char dataType = 0;
		//This variable stores data specific to the type we're currently loading.
		unsigned int data = 0;
		char lineBuffer[1024];
		char *line;
		size_t lineLength;
		char *nameArray = NULL;
		size_t nameArrayLength = 0;
		//Temporarily stores only unique vertices.
		*vertexNum = 0;
		*vertices = memAllocate(vertexCapacity * sizeof(**vertices));
		//Temporarily stores vertex indices for faces.
		//We can use the model's totalIndices variable for the size so we don't have to set it later.
		*indexNum = 0;
		*indices = memAllocate(indexCapacity * sizeof(**indices));
		//Temporarily stores bones.
		skl->name = NULL;
		skl->boneNum = 0;
		skl->bones = memAllocate(boneCapacity * sizeof(*skl->bones));


		while(fileParseNextLine(mdlFile, lineBuffer, sizeof(lineBuffer), &line, &lineLength)){

			// Texture wrapper
			if(twPath != NULL && *twPathLength == 0 && lineLength > 8 && strncmp(line, "texture ", 8) == 0){
				size_t pathBegin;
				size_t pathLength;
				const char *firstQuote = strchr(line+8, '"');
				const char *secondQuote = NULL;
				if(firstQuote != NULL){
					++firstQuote;
					pathBegin = firstQuote-line;
					secondQuote = strrchr(firstQuote, '"');
				}
				if(secondQuote > firstQuote){
					pathLength = secondQuote-firstQuote;
				}else{
					pathBegin = 8;
					pathLength = lineLength-pathBegin;
				}
				memcpy(twPath, line+pathBegin, pathLength);
				*twPathLength = pathLength;

			}else if(dataType == 0){
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
							nameArray = memReallocate(nameArray, (nameArrayLength+boneNameLength+1)*sizeof(char));
							memcpy(&nameArray[nameArrayLength], tokPos, boneNameLength);
							nameArrayLength += boneNameLength;
							nameArray[nameArrayLength] = '\0';
							++nameArrayLength;

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
									vec3Set(&currentBone->localBind.position, x, y, z);

									//Load the bone's rotation!
									x = strtod(tokPos, &tokPos);
									y = strtod(tokPos, &tokPos);
									z = strtod(tokPos, NULL);
									quatSetEuler(&currentBone->localBind.orientation, x, y, z);

									//Set the bone's scale!
									vec3Set(&currentBone->localBind.scale, 1.f, 1.f, 1.f);
									currentBone->localBind.shear = g_quatIdentity;

									// Accumulate the global bind poses; NOT the global inverse bind poses.
									// We invert these later on in sklDefragmentSMD.
									if(currentBone->parent == boneID){
										// The root just starts with the local bind pose.
										currentBone->globalBindInverse = currentBone->localBind;
									}else{
										currentBone->globalBindInverse = tfMultiply(skl->bones[currentBone->parent].globalBindInverse, currentBone->localBind);
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
								// Handle OpenGL UV coordinate system.
								tempVertex.v = 1.f-strtod(tokPos, &tokPos);
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
		sklDefragmentSMD(skl, filePath, strlen(filePath), nameArray, nameArrayLength);

	}else{
		printf("Unable to open model file!\n"
		       "Path: %s\n", filePath);
		return 0;
	}

	return 1;
}**/
