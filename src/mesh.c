#include "mesh.h"
#include <string.h>
#include <stdio.h>

#define DEFAULT_VERTEX_MASS 1
#define VERTEX_START_CAPACITY 1
#define INDEX_START_CAPACITY 1

/** Remove printf()s **/

#define freeHelpersWavefrontObj() \
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
	} \

unsigned char pushDynamicArray(void **vector, const void *element, const size_t bytes, size_t *size, size_t *capacity);

void vertInit(vertex *v){
	vec3SetS(&v->position, 0.f);
	v->u  = 0.f; v->v  = 0.f;
	vec3SetS(&v->normal, 0.f);
	v->bIDs[0]     = -1;  v->bIDs[1]     = -1;  v->bIDs[2]     = -1;  v->bIDs[3] = -1;
	v->bWeights[0] = 0.f; v->bWeights[1] = 0.f; v->bWeights[2] = 0.f; v->bWeights[3] = 0.f;
}

/** Should meshes really be storing physical properties? **/
unsigned char meshLoadWavefrontObj(mesh *m, const char *filePath/**, const unsigned char generatePhysProperties**/){

	size_t vertexCapacity = VERTEX_START_CAPACITY;
	m->vertices = malloc(vertexCapacity*sizeof(vertex));
	if(m->vertices == NULL){
		printf("Error loading mesh: Memory allocation failure.\n");
		return 0;
	}

	size_t indexCapacity = INDEX_START_CAPACITY;
	m->indices = malloc(indexCapacity*sizeof(size_t));
	if(m->indices == NULL){
		printf("Error loading mesh: Memory allocation failure.\n");
		free(m->vertices);
		return 0;
	}

	/**if(generatePhysProperties){
		m->area = 0.f;
		m->mass = 0.f;
		vec3SetS(&m->centroid, 0.f);
	}**/
	m->vertexNum = 0;
	m->indexNum = 0;

	// Temporarily holds vertex positions before they are pushed into vertices
	size_t tempPositionsSize = 0;
	size_t tempPositionsCapacity = 1*VERTEX_START_CAPACITY;
	float *tempPositions = malloc(tempPositionsCapacity*sizeof(float));
	if(tempPositions == NULL){
		printf("Error loading mesh: Memory allocation failure.\n");
		free(m->vertices);
		free(m->indices);
		return 0;
	}
	// Temporarily holds vertex UVs they are pushed into vertices
	size_t tempTexCoordsSize = 0;
	size_t tempTexCoordsCapacity = 1*VERTEX_START_CAPACITY;
	float *tempTexCoords = malloc(tempTexCoordsCapacity*sizeof(float));
	if(tempTexCoords == NULL){
		printf("Error loading mesh: Memory allocation failure.\n");
		free(m->vertices);
		free(m->indices);
		free(tempPositions);
		return 0;
	}
	// Temporarily holds vertex normals before they are pushed into vertices
	size_t tempNormalsSize = 0;
	size_t tempNormalsCapacity = 1*VERTEX_START_CAPACITY;
	float *tempNormals = malloc(tempNormalsCapacity*sizeof(float));
	if(tempNormals == NULL){
		printf("Error loading mesh: Memory allocation failure.\n");
		free(m->vertices);
		free(m->indices);
		free(tempPositions);
		free(tempTexCoords);
		return 0;
	}
	// Temporarily holds bone IDs before they are pushed into vertices
	size_t tempBoneIDsSize = 0;
	size_t tempBoneIDsCapacity = 1*VERTEX_START_CAPACITY;
	int   *tempBoneIDs = malloc(tempBoneIDsCapacity*sizeof(int));
	if(tempBoneIDs == NULL){
		printf("Error loading mesh: Memory allocation failure.\n");
		free(m->vertices);
		free(m->indices);
		free(tempPositions);
		free(tempTexCoords);
		free(tempNormals);
		return 0;
	}
	// Temporarily holds bone weights before they are pushed into vertices
	size_t tempBoneWeightsSize = 0;
	size_t tempBoneWeightsCapacity = 1*VERTEX_START_CAPACITY;
	float *tempBoneWeights = malloc(tempBoneWeightsCapacity*sizeof(float));
	if(tempBoneWeights == NULL){
		printf("Error loading mesh: Memory allocation failure.\n");
		free(m->vertices);
		free(m->indices);
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
		while(fgets(lineFeed, sizeof(lineFeed), mdlInfo)){

			line = lineFeed;
			lineLength = strlen(line);

			// Remove new line and carriage return
			if(line[lineLength-1] == '\n'){
				line[--lineLength] = '\0';
			}
			if(line[lineLength-1] == '\r'){
				line[--lineLength] = '\0';
			}
			// Remove any comments from the line
			char *commentPos = strstr(line, "//");
			if(commentPos != NULL){
				lineLength -= commentPos-line;
				*commentPos = '\0';
			}
			// Remove any indentations from the line, as well as any trailing spaces and tabs
			unsigned char doneFront = 0, doneEnd = 0;
			size_t newOffset = 0;
			size_t i;
			for(i = 0; (i < lineLength && !doneFront && !doneEnd); ++i){
				if(!doneFront && line[i] != '\t' && line[i] != ' '){
					newOffset = i;
					doneFront = 1;
				}
				if(!doneEnd && i > 1 && i < lineLength && line[lineLength-i] != '\t' && line[lineLength-i] != ' '){
					lineLength -= i-1;
					line[lineLength] = '\0';
					doneEnd = 1;
				}
			}
			line += newOffset;
			lineLength -= newOffset;

			// Name
			if(lineLength >= 6 && strncmp(line, "name ", 5) == 0){
				/** *name = malloc((lineLength-4) * sizeof(char));
				if(*name == NULL){
					printf("Error loading model: Memory allocation failure.\n");
					freeHelpersWavefrontObj();
					fclose(mdlInfo);
					return 0;
				}
				strncpy(*name, line+5, lineLength-5);
				(*name)[lineLength-5] = '\0';**/

			// Skeleton
			}else if(lineLength > 9 && strncmp(line, "skeleton ", 9) == 0){
				/**const char *firstQuote = strchr(line, '"');
				const char *lastQuote = strrchr(line, '"');
				size_t pathBegin;
				size_t pathLength;
				if(firstQuote != NULL && lastQuote > firstQuote){
					pathBegin = firstQuote-line+1;
					pathLength = lastQuote-line-pathBegin;
				}else{  // If the skeleton path wasn't surrounded by quotes, don't give up:
					pathBegin = 8;
					pathLength = lineLength-8;
				}
				char *sklPath = malloc((pathLength+1) * sizeof(char));
				strncpy(sklPath, line+pathBegin, pathLength);
				sklPath[pathLength] = '\0';
				sklLoad(&mdl->skl, prgPath, sklPath);
				free(sklPath);**/

			// Vertex data
			}else if(lineLength >= 7 && strncmp(line, "v ", 2) == 0){
				char *token = strtok(line+2, " ");
				float curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity)){
					printf("Error loading mesh: Memory allocation failure.\n");
					freeHelpersWavefrontObj();
					meshDelete(m);
					fclose(mdlInfo);
					return 0;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity)){
					printf("Error loading mesh: Memory allocation failure.\n");
					freeHelpersWavefrontObj();
					meshDelete(m);
					fclose(mdlInfo);
					return 0;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity)){
					printf("Error loading mesh: Memory allocation failure.\n");
					freeHelpersWavefrontObj();
					meshDelete(m);
					fclose(mdlInfo);
					return 0;
				}
				/****/
				token = strtok(NULL, " ");
				if(token != NULL){
					int curBoneID = strtoul(token, NULL, 0);
					if(!pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity)){
						printf("Error loading mesh: Memory allocation failure.\n");
						freeHelpersWavefrontObj();
						meshDelete(m);
						fclose(mdlInfo);
						return 0;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(!pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity)){
						printf("Error loading mesh: Memory allocation failure.\n");
						freeHelpersWavefrontObj();
						meshDelete(m);
						fclose(mdlInfo);
						return 0;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(!pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity)){
						printf("Error loading mesh: Memory allocation failure.\n");
						freeHelpersWavefrontObj();
						meshDelete(m);
						fclose(mdlInfo);
						return 0;
					}
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					if(!pushDynamicArray((void **)&tempBoneIDs, &curBoneID, sizeof(curBoneID), &tempBoneIDsSize, &tempBoneIDsCapacity)){
						printf("Error loading mesh: Memory allocation failure.\n");
						freeHelpersWavefrontObj();
						meshDelete(m);
						fclose(mdlInfo);
						return 0;
					}
					token = strtok(NULL, " ");
					if(token != NULL){
						curVal = strtod(token, NULL);
						if(!pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity)){
							printf("Error loading mesh: Memory allocation failure.\n");
							freeHelpersWavefrontObj();
							meshDelete(m);
							fclose(mdlInfo);
							return 0;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(!pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity)){
							printf("Error loading mesh: Memory allocation failure.\n");
							freeHelpersWavefrontObj();
							meshDelete(m);
							fclose(mdlInfo);
							return 0;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(!pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity)){
							printf("Error loading mesh: Memory allocation failure.\n");
							freeHelpersWavefrontObj();
							meshDelete(m);
							fclose(mdlInfo);
							return 0;
						}
						token = strtok(NULL, " ");
						curVal = strtod(token, NULL);
						if(!pushDynamicArray((void **)&tempBoneWeights, &curVal, sizeof(curVal), &tempBoneWeightsSize, &tempBoneWeightsCapacity)){
							printf("Error loading mesh: Memory allocation failure.\n");
							freeHelpersWavefrontObj();
							meshDelete(m);
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
					printf("Error loading mesh: Memory allocation failure.\n");
					freeHelpersWavefrontObj();
					meshDelete(m);
					fclose(mdlInfo);
					return 0;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempTexCoords, &curVal, sizeof(curVal), &tempTexCoordsSize, &tempTexCoordsCapacity)){
					printf("Error loading mesh: Memory allocation failure.\n");
					freeHelpersWavefrontObj();
					meshDelete(m);
					fclose(mdlInfo);
					return 0;
				}

			// Normal data
			}else if(lineLength >= 8 && strncmp(line, "vn ", 3) == 0){
				char *token = strtok(line+3, " ");
				float curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempNormals, &curVal, sizeof(curVal), &tempNormalsSize, &tempNormalsCapacity)){
					printf("Error loading mesh: Memory allocation failure.\n");
					freeHelpersWavefrontObj();
					meshDelete(m);
					fclose(mdlInfo);
					return 0;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempNormals, &curVal, sizeof(curVal), &tempNormalsSize, &tempNormalsCapacity)){
					printf("Error loading mesh: Memory allocation failure.\n");
					freeHelpersWavefrontObj();
					meshDelete(m);
					fclose(mdlInfo);
					return 0;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(!pushDynamicArray((void **)&tempNormals, &curVal, sizeof(curVal), &tempNormalsSize, &tempNormalsCapacity)){
					printf("Error loading mesh: Memory allocation failure.\n");
					freeHelpersWavefrontObj();
					meshDelete(m);
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

					// Reset tempVert member variables
					vertInit(&tempVert);

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
						tempVert.bIDs[0] = -1;
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
						tempVert.bWeights[0] = 0.f;
						tempVert.bWeights[1] = 0.f;
						tempVert.bWeights[2] = 0.f;
						tempVert.bWeights[3] = 0.f;
					}

					// Check if the vertex has already been loaded, and if so add an index
					unsigned char foundVertex = 0;
					size_t j;
					for(j = 0; j < m->vertexNum; ++j){
						vertex *checkVert = &m->vertices[j];
						/** CHECK BONE DATA HERE **/
						if(checkVert->position.x  == tempVert.position.x  && checkVert->position.y  == tempVert.position.y  && checkVert->position.z == tempVert.position.z &&
						   checkVert->u           == tempVert.u           && checkVert->v           == tempVert.v           &&
						   checkVert->normal.x    == tempVert.normal.x    && checkVert->normal.y    == tempVert.normal.y    && checkVert->normal.z   == tempVert.normal.z   &&
						   checkVert->bIDs[0]     == tempVert.bIDs[0]     && checkVert->bIDs[1]     == tempVert.bIDs[1]     &&
						   checkVert->bIDs[2]     == tempVert.bIDs[2]     && checkVert->bIDs[3]     == tempVert.bIDs[3]     &&
						   checkVert->bWeights[0] == tempVert.bWeights[0] && checkVert->bWeights[1] == tempVert.bWeights[1] &&
						   checkVert->bWeights[2] == tempVert.bWeights[2] && checkVert->bWeights[3] == tempVert.bWeights[3]){

							// Resize indices if there's not enough room
							if(!pushDynamicArray((void **)&m->indices, &j, sizeof(j), &m->indexNum, &indexCapacity)){
								printf("Error loading mesh: Memory allocation failure. %i\n", indexCapacity);
								freeHelpersWavefrontObj();
								meshDelete(m);
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
						if(!pushDynamicArray((void **)&m->indices, &m->vertexNum, sizeof(m->vertexNum), &m->indexNum, &indexCapacity) ||
						   !pushDynamicArray((void **)&m->vertices, &tempVert, sizeof(tempVert), &m->vertexNum, &vertexCapacity)){
							printf("Error loading mesh: Memory allocation failure.\n");
							freeHelpersWavefrontObj();
							meshDelete(m);
							fclose(mdlInfo);
							return 0;
						}
						/**if(generatePhysProperties && m->vertexNum > 1){
							float temp = m->vertices[m->vertexNum-2].position.x * m->vertices[m->vertexNum-1].position.y -
							             m->vertices[m->vertexNum-2].position.y * m->vertices[m->vertexNum-1].position.x;
							m->centroid.x += (m->vertices[m->vertexNum-2].position.x + m->vertices[m->vertexNum-1].position.x)*temp;
							m->centroid.y += (m->vertices[m->vertexNum-2].position.y + m->vertices[m->vertexNum-1].position.y)*temp;
							m->centroid.z += (m->vertices[m->vertexNum-2].position.z + m->vertices[m->vertexNum-1].position.z)*temp;
							m->area += temp;
							m->mass += DEFAULT_VERTEX_MASS;
						}**/
					}

				}

			}

		}

		fclose(mdlInfo);
		freeHelpersWavefrontObj();

	}else{
		printf("Error loading model: Couldn't open %s\n", filePath);
		freeHelpersWavefrontObj();
		meshDelete(m);
		return 0;
	}

	/**if(generatePhysProperties){
		// Final iteration with the last and first vertices.
		float temp = m->vertices[m->vertexNum-1].position.x * m->vertices[0].position.y -
		             m->vertices[m->vertexNum-1].position.y * m->vertices[0].position.x;
		m->centroid.x += (m->vertices[m->vertexNum-1].position.x + m->vertices[0].position.x)*temp;
		m->centroid.y += (m->vertices[m->vertexNum-1].position.y + m->vertices[0].position.y)*temp;
		m->centroid.z += (m->vertices[m->vertexNum-1].position.z + m->vertices[0].position.z)*temp;
		m->area += temp;
		m->mass += DEFAULT_VERTEX_MASS;

		// Calculate the mesh's final area and center of mass.
		m->area *= 0.5f;
		temp = 1.f/(6.f*m->area);
		m->centroid.x *= temp;
		m->centroid.y *= temp;
		m->centroid.z *= temp;
	}**/

	return 1;

}

size_t meshGetFarthestVertex(const mesh *m, const vec3 *axis){
	/*
	** Finds the vertex in mesh that is farthest in
	** the direction of axis by projecting each
	** vertex onto the axis.
	*/
	/** Generate a world state for each vertex? **/
	size_t r = 0;
	float max = vec3Dot(&m->vertices[0].position, axis);
	for(size_t i = 1; i < m->vertexNum; ++i){
		float s = vec3Dot(&m->vertices[i].position, axis);
		if(s > max){
			max = s;
			r = i;
		}
	}
	return r;
}

void meshDelete(mesh *m){
	if(m->vertices != NULL){
		free(m->vertices);
	}
	if(m->indices != NULL){
		free(m->indices);
	}
}
