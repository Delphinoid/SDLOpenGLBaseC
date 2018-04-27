#include "hitboxConvexMesh.h"
#include "helpersMisc.h"
#include <string.h>
#include <stdio.h>

/** Merge hbMeshWavefrontObjLoad() with mdlLoad(). **/

#define HITBOX_VERTEX_START_CAPACITY 1
#define HITBOX_INDEX_START_CAPACITY 1

/** Remove printf()s **/

#define hbMeshWavefrontObjFreeHelpers() \
	if(tempPositions != NULL){ \
		free(tempPositions); \
	}

signed char hbMeshWavefrontObjLoad(hbMesh *hbm, const char *filePath){

	size_t vertexCapacity = HITBOX_VERTEX_START_CAPACITY;
	hbm->vertices = malloc(vertexCapacity*sizeof(vec3));
	if(hbm->vertices == NULL){
		/** Memory allocation failure. **/
		return -1;
	}

	size_t indexCapacity = HITBOX_INDEX_START_CAPACITY;
	hbm->indices = malloc(indexCapacity*sizeof(size_t));
	if(hbm->indices == NULL){
		/** Memory allocation failure. **/
		free(hbm->vertices);
		return -1;
	}

	/**const signed char generatePhysProperties = (mass != NULL && area != NULL && centroid != NULL);
	if(generatePhysProperties){
		*mass = 0.f;
		*area = 0.f;
		vec3SetS(centroid, 0.f);
	}**/

	// Temporarily holds vertex positions before they are pushed into vertices
	size_t tempPositionsSize = 0;
	size_t tempPositionsCapacity = 1*HITBOX_VERTEX_START_CAPACITY;
	float *tempPositions = malloc(tempPositionsCapacity*sizeof(float));
	if(tempPositions == NULL){
		/** Memory allocation failure. **/
		free(hbm->vertices);
		free(hbm->indices);
		return -1;
	}

	vec3 tempVert;            // Holds a vertex before pushing it into the triangle array
	size_t positionIndex[3];  // Holds all the positional information for a face


	FILE *hbmInfo = fopen(filePath, "r");
	char lineFeed[1024];
	char *line;
	size_t lineLength;

	if(hbmInfo != NULL){
		while(fileParseNextLine(hbmInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			size_t i;

			// Vertex data
			if(lineLength >= 7 && strncmp(line, "v ", 2) == 0){
				char *token = strtok(line+2, " ");
				float curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity) == -1){
					/** Memory allocation failure. **/
					hbMeshWavefrontObjFreeHelpers();
					hbMeshDelete(hbm);
					fclose(hbmInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity) == -1){
					/** Memory allocation failure. **/
					hbMeshWavefrontObjFreeHelpers();
					hbMeshDelete(hbm);
					fclose(hbmInfo);
					return -1;
				}
				token = strtok(NULL, " ");
				curVal = strtod(token, NULL);
				if(pushDynamicArray((void **)&tempPositions, &curVal, sizeof(curVal), &tempPositionsSize, &tempPositionsCapacity) == -1){
					/** Memory allocation failure. **/
					hbMeshWavefrontObjFreeHelpers();
					hbMeshDelete(hbm);
					fclose(hbmInfo);
					return -1;
				}

			// Face data
			}else if(lineLength >= 19 && strncmp(line, "f ", 2) == 0){
				char *token = strtok(line+2, " ");
				for(i = 0; i < 3; ++i){

					// Load face data
					positionIndex[i] = strtoul(token, NULL, 0);
					token = strtok(NULL, " ");

					// Create a vertex from the given data
					// Vertex positional data
					size_t pos = (positionIndex[i]-1)*3;
					if(pos+2 < tempPositionsSize){
						tempVert.x = tempPositions[pos];
						tempVert.y = tempPositions[pos+1];
						tempVert.z = tempPositions[pos+2];
					}else{
						tempVert.x = 0.f;
						tempVert.y = 0.f;
						tempVert.z = 0.f;
					}

					// Check if the vertex has already been loaded, and if so add an index
					signed char foundVertex = 0;
					size_t j;
					for(j = 0; j < hbm->vertexNum; ++j){
						vec3 *checkVert = &hbm->vertices[j];
						if(memcmp(checkVert, &tempVert, sizeof(vec3)) == 0){
							// Resize indices if there's not enough room
							if(pushDynamicArray((void **)&hbm->indices, &j, sizeof(j), &hbm->indexNum, &indexCapacity) == -1){
								/** Memory allocation failure. **/
								hbMeshWavefrontObjFreeHelpers();
								hbMeshDelete(hbm);
								fclose(hbmInfo);
								return -1;
							}
							foundVertex = 1;
							break;
						}
					}

					// If the vertex has not yet been loaded, add it to both the vertex vector and the index vector
					if(!foundVertex){
						// Resize indices if there's not enough room
						if(pushDynamicArray((void **)&hbm->indices, &hbm->vertexNum, sizeof(hbm->vertexNum), &hbm->indexNum, &indexCapacity) == -1 ||
						   pushDynamicArray((void **)&hbm->vertices, &tempVert, sizeof(tempVert), &hbm->vertexNum, &vertexCapacity) == -1){
							/** Memory allocation failure. **/
							hbMeshWavefrontObjFreeHelpers();
							hbMeshDelete(hbm);
							fclose(hbmInfo);
							return -1;
						}
						/**if(generatePhysProperties && hbm->vertexNum > 1){
							// Generate physics properties if necessary
							float temp = hbm->vertices[hbm->vertexNum-2].x * hbm->vertices[hbm->vertexNum-1].y -
							             hbm->vertices[hbm->vertexNum-2].y * hbm->vertices[hbm->vertexNum-1].x;
							centroid->x += (hbm->vertices[hbm->vertexNum-2].x + hbm->vertices[hbm->vertexNum-1].x)*temp;
							centroid->y += (hbm->vertices[hbm->vertexNum-2].y + hbm->vertices[hbm->vertexNum-1].y)*temp;
							centroid->z += (hbm->vertices[hbm->vertexNum-2].z + hbm->vertices[hbm->vertexNum-1].z)*temp;
							*area += temp;
							*mass += MESH_DEFAULT_VERTEX_MASS;
						}**/
					}

				}

			}

		}

		fclose(hbmInfo);
		hbMeshWavefrontObjFreeHelpers();

	}else{
		printf("Error loading model: Couldn't open %s\n", filePath);
		hbMeshWavefrontObjFreeHelpers();
		hbMeshDelete(hbm);
		return 0;
	}

	/**if(generatePhysProperties){
		// Final iteration with the last and first vertices.
		float temp = hbm->vertices[hbm->vertexNum-1].x * hbm->vertices[0].y -
		             hbm->vertices[hbm->vertexNum-1].y * hbm->vertices[0].x;
		centroid->x += (hbm->vertices[hbm->vertexNum-1].x + hbm->vertices[0].x)*temp;
		centroid->y += (hbm->vertices[hbm->vertexNum-1].y + hbm->vertices[0].y)*temp;
		centroid->z += (hbm->vertices[hbm->vertexNum-1].z + hbm->vertices[0].z)*temp;
		*area += temp;
		*mass += MESH_DEFAULT_VERTEX_MASS;

		// Calculate the mesh's final area and center of mass.
		*area *= 0.5f;
		temp = 1.f/(6.f*(*area));
		centroid->x *= temp;
		centroid->y *= temp;
		centroid->z *= temp;
	}**/

	return 1;

}
