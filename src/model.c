#define GLEW_STATIC
#include <GL/glew.h>
#include "model.h"
#include "cVector.h"
#include <stdio.h>

#define VERTEX_START_CAPACITY 1024
#define INDEX_START_CAPACITY 2048

static void mdlGenBufferObjects(model *mdl, vertex *vertices, size_t *indices);

void vertInit(vertex *v){
	vec3SetS(&v->pos, 0.f);
	v->u  = 0.f; v->v  = 0.f;
	v->nx = 0.f; v->ny = 0.f; v->nz = 0.f;
	v->bIDs[0]     = -1;  v->bIDs[1]     = -1;  v->bIDs[2]     = -1;  v->bIDs[3] = -1;
	v->bWeights[0] = 0.f; v->bWeights[1] = 0.f; v->bWeights[2] = 0.f; v->bWeights[3] = 0.f;
}

void mdlInit(model *mdl){
	mdl->name = NULL;
	mdl->vertexNum = 0;
	mdl->indexNum = 0;
	mdl->vaoID = 0;
	mdl->vboID = 0;
	mdl->iboID = 0;
}

unsigned char mdlLoadWavefrontObj(model *mdl, const char *prgPath, const char *filePath){

	mdlInit(mdl);

	size_t vertexCapacity = VERTEX_START_CAPACITY;
	vertex *vertices = malloc(vertexCapacity*sizeof(vertex));
	if(vertices == NULL){
		printf("Error loading model:\nMemory allocation failure.\n");
		return 0;
	}

	size_t indexCapacity = INDEX_START_CAPACITY;
	size_t *indices = malloc(indexCapacity*sizeof(size_t));
	if(indices == NULL){
		printf("Error loading model:\nMemory allocation failure.\n");
		free(vertices);
		return 0;
	}

	cVector tempPositions;   cvInit(&tempPositions, 5);  // Holds floats; temporarily holds vertex position data before it is pushed into vertexBuffer
	cVector tempBoneIDs;     cvInit(&tempBoneIDs, 4);
	cVector tempBoneWeights; cvInit(&tempBoneWeights, 4);
	cVector tempTexCoords;   cvInit(&tempTexCoords, 2);  // Holds floats; temporarily holds vertex UV data before it is pushed into vertexBuffer
	cVector tempNorms;       cvInit(&tempNorms, 3);      // Holds floats; temporarily holds vertex normal data before it is pushed into vertexBuffer
	vertex tempVert;  // Holds a vertex before pushing it into the triangle array
	size_t positionIndex[3];  // Holds all the positional information for a face
	size_t uvIndex[3];        // Holds all the UV information for a face
	size_t normalIndex[3];    // Holds all the normal information for a face


	size_t pathLen = strlen(prgPath);
	size_t fileLen = strlen(filePath);
	char *fullPath = malloc((pathLen+fileLen+1)*sizeof(char));
	memcpy(fullPath, prgPath, pathLen);
	memcpy(fullPath+pathLen, filePath, fileLen);
	fullPath[pathLen+fileLen] = '\0';
	FILE *mdlInfo = fopen(fullPath, "r");
	char lineFeed[1024];
	char *line;
	char compare[1024];
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
			for(i = 0; (i < lineLength && !doneFront && !doneEnd); i++){
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
			if(lineLength >= 6 && strncpy(compare, line, 5) && (compare[5] = '\0') == 0 && strcmp(compare, "name ") == 0){
				mdl->name = malloc((lineLength-4) * sizeof(char));
				if(mdl->name != NULL){
					strncpy(mdl->name, line+5, lineLength-5);
					mdl->name[lineLength-5] = '\0';
				}

			// Vertex data
			}else if(lineLength >= 7 && strncpy(compare, line, 2) && (compare[2] = '\0') == 0 && strcmp(compare, "v ") == 0){
				char *token = strtok(line+2, " ");
				float curVal = strtof(token, NULL);
				cvPush(&tempPositions, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " ");
				curVal = strtof(token, NULL);
				cvPush(&tempPositions, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " ");
				curVal = strtof(token, NULL);
				cvPush(&tempPositions, (void *)&curVal, sizeof(curVal));
				/****/
				token = strtok(NULL, " ");
				if(token != NULL){
					int curBoneID = strtoul(token, NULL, 0);
					cvPush(&tempBoneIDs, (void *)&curBoneID, sizeof(curBoneID));
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					cvPush(&tempBoneIDs, (void *)&curBoneID, sizeof(curBoneID));
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					cvPush(&tempBoneIDs, (void *)&curBoneID, sizeof(curBoneID));
					token = strtok(NULL, " ");
					curBoneID = strtoul(token, NULL, 0);
					cvPush(&tempBoneIDs, (void *)&curBoneID, sizeof(curBoneID));
					token = strtok(NULL, " ");
					if(token != NULL){
						curVal = strtof(token, NULL);
						cvPush(&tempBoneWeights, (void *)&curVal, sizeof(curVal));
						token = strtok(NULL, " ");
						curVal = strtof(token, NULL);
						cvPush(&tempBoneWeights, (void *)&curVal, sizeof(curVal));
						token = strtok(NULL, " ");
						curVal = strtof(token, NULL);
						cvPush(&tempBoneWeights, (void *)&curVal, sizeof(curVal));
						token = strtok(NULL, " ");
						curVal = strtof(token, NULL);
						cvPush(&tempBoneWeights, (void *)&curVal, sizeof(curVal));
					}
				}

			// UV data
			}else if(lineLength >= 6 && strncpy(compare, line, 3) && (compare[3] = '\0') == 0 && strcmp(compare, "vt ") == 0){
				char *token = strtok(line+3, " ");
				float curVal = strtof(token, NULL);
				cvPush(&tempTexCoords, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " ");
				curVal = strtof(token, NULL);
				cvPush(&tempTexCoords, (void *)&curVal, sizeof(curVal));

			// Normal data
			}else if(lineLength >= 8 && strncpy(compare, line, 3) && (compare[3] = '\0') == 0 && strcmp(compare, "vn ") == 0){
				char *token = strtok(line+3, " ");
				float curVal = strtof(token, NULL);
				cvPush(&tempNorms, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " ");
				curVal = strtof(token, NULL);
				cvPush(&tempNorms, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " ");
				curVal = strtof(token, NULL);
				cvPush(&tempNorms, (void *)&curVal, sizeof(curVal));

			// Face data
			}else if(lineLength >= 19 && strncpy(compare, line, 2) && (compare[2] = '\0') == 0 && strcmp(compare, "f ") == 0){
				char *token = strtok(line+2, " /");
				for(i = 0; i < 3; i++){

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
					void *checkVal = cvGet(&tempPositions, (positionIndex[i]-1)*3);
					if(checkVal != NULL){
						tempVert.pos.x = *((float *)checkVal);
					}else{
						tempVert.pos.x = 0.f;
					}
					checkVal = cvGet(&tempPositions, (positionIndex[i]-1)*3+1);
					if(checkVal != NULL){
						tempVert.pos.y = *((float *)checkVal);
					}else{
						tempVert.pos.y = 0.f;
					}
					checkVal = cvGet(&tempPositions, (positionIndex[i]-1)*3+2);
					if(checkVal != NULL){
						tempVert.pos.z = *((float *)checkVal);
					}else{
						tempVert.pos.z = 0.f;
					}
					// Vertex UV data
					checkVal = cvGet(&tempTexCoords, (uvIndex[i]-1)*2);
					if(checkVal != NULL){
						tempVert.u = *((float *)checkVal);
					}else{
						tempVert.u = 0.f;
					}
					checkVal = cvGet(&tempTexCoords, (uvIndex[i]-1)*2+1);
					if(checkVal != NULL){
						tempVert.v = -*((float *)checkVal);
					}else{
						tempVert.v = 0.f;
					}
					// Vertex normal data
					checkVal = cvGet(&tempNorms, (normalIndex[i]-1)*3);
					if(checkVal != NULL){
						tempVert.nx = *((float *)checkVal);
					}else{
						tempVert.nx = 0.f;
					}
					checkVal = cvGet(&tempNorms, (normalIndex[i]-1)*3+1);
					if(checkVal != NULL){
						tempVert.ny = *((float *)checkVal);
					}else{
						tempVert.ny = 0.f;
					}
					checkVal = cvGet(&tempNorms, (normalIndex[i]-1)*3+2);
					if(checkVal != NULL){
						tempVert.nz = *((float *)checkVal);
					}else{
						tempVert.nz = 0.f;
					}
					/****/
					checkVal = cvGet(&tempBoneIDs, (positionIndex[i]-1)*4);
					if(checkVal != NULL){
						tempVert.bIDs[0] = *((int *)checkVal);
					}else{
						tempVert.bIDs[0] = -1;
					}
					checkVal = cvGet(&tempBoneIDs, (positionIndex[i]-1)*4+1);
					if(checkVal != NULL){
						tempVert.bIDs[1] = *((int *)checkVal);
					}else{
						tempVert.bIDs[1] = -1;
					}
					checkVal = cvGet(&tempBoneIDs, (positionIndex[i]-1)*4+2);
					if(checkVal != NULL){
						tempVert.bIDs[2] = *((int *)checkVal);
					}else{
						tempVert.bIDs[2] = -1;
					}
					checkVal = cvGet(&tempBoneIDs, (positionIndex[i]-1)*4+2);
					if(checkVal != NULL){
						tempVert.bIDs[3] = *((int *)checkVal);
					}else{
						tempVert.bIDs[3] = -1;
					}
					/****/
					checkVal = cvGet(&tempBoneWeights, (positionIndex[i]-1)*4);
					if(checkVal != NULL){
						tempVert.bWeights[0] = *((float *)checkVal);
					}else{
						tempVert.bWeights[0] = 0.f;
					}
					checkVal = cvGet(&tempBoneWeights, (positionIndex[i]-1)*4+1);
					if(checkVal != NULL){
						tempVert.bWeights[1] = *((float *)checkVal);
					}else{
						tempVert.bWeights[1] = 0.f;
					}
					checkVal = cvGet(&tempBoneWeights, (positionIndex[i]-1)*4+2);
					if(checkVal != NULL){
						tempVert.bWeights[2] = *((float *)checkVal);
					}else{
						tempVert.bWeights[2] = 0.f;
					}
					checkVal = cvGet(&tempBoneWeights, (positionIndex[i]-1)*4+2);
					if(checkVal != NULL){
						tempVert.bWeights[3] = *((float *)checkVal);
					}else{
						tempVert.bWeights[3] = 0.f;
					}

					// Check if the vertex has already been loaded, and if so add an index
					unsigned char foundVertex = 0;
					size_t j;
					for(j = 0; j < mdl->vertexNum; j++){
						vertex *checkVert = &vertices[j];
						/** CHECK BONE DATA HERE **/
						if(checkVert->pos.x == tempVert.pos.x && checkVert->pos.y == tempVert.pos.y && checkVert->pos.z == tempVert.pos.z &&
						   checkVert->u     == tempVert.u     && checkVert->v     == tempVert.v     &&
						   checkVert->nx    == tempVert.nx    && checkVert->ny    == tempVert.ny    && checkVert->nz    == tempVert.nz){

							// Resize indices if there's not enough room
							if(mdl->indexNum == indexCapacity){
								indexCapacity *= 2;
								size_t *tempBuffer = realloc(indices, indexCapacity*sizeof(size_t));
								if(tempBuffer != NULL){
									indices = tempBuffer;
								}else{
									printf("Error loading model:\nMemory allocation failure.\n");
									cvClear(&tempPositions);
									cvClear(&tempTexCoords);
									cvClear(&tempNorms);
									free(fullPath);
									free(indices);
									free(vertices);
									return 0;
								}
							}
							indices[mdl->indexNum++] = j;
							j = mdl->vertexNum;
							foundVertex = 1;
						}
					}

					// If the vertex has not yet been loaded, add it to both the vertex vector and the index vector
					if(!foundVertex){
						// Resize indices if there's not enough room
						if(mdl->indexNum == indexCapacity){
							indexCapacity *= 2;
							size_t *tempBuffer = realloc(indices, indexCapacity*sizeof(size_t));
							if(tempBuffer != NULL){
								indices = tempBuffer;
							}else{
								printf("Error loading model:\nMemory allocation failure.\n");
								cvClear(&tempPositions);
								cvClear(&tempTexCoords);
								cvClear(&tempNorms);
								free(fullPath);
								free(indices);
								free(vertices);
								return 0;
							}
						}
						// Resize vertices if there's not enough room
						if(mdl->vertexNum == vertexCapacity){
							vertexCapacity *= 2;
							vertex *tempBuffer = realloc(vertices, vertexCapacity*sizeof(vertex));
							if(tempBuffer != NULL){
								vertices = tempBuffer;
							}else{
								printf("Error loading model:\nMemory allocation failure.\n");
								cvClear(&tempPositions);
								cvClear(&tempTexCoords);
								cvClear(&tempNorms);
								free(fullPath);
								free(indices);
								free(vertices);
								return 0;
							}
						}
						indices[mdl->indexNum++] = mdl->vertexNum;
						vertices[mdl->vertexNum++] = tempVert;
					}

				}

			}

		}

		fclose(mdlInfo);

	}else{
		printf("Error loading model:\nCouldn't open %s\n", fullPath);
		cvClear(&tempPositions);
		cvClear(&tempTexCoords);
		cvClear(&tempNorms);
		free(fullPath);
		free(indices);
		free(vertices);
		return 0;
	}

	cvClear(&tempPositions);
	cvClear(&tempBoneIDs);
	cvClear(&tempBoneWeights);
	cvClear(&tempTexCoords);
	cvClear(&tempNorms);
	free(fullPath);

	// If no name was given, generate one based off the file name
	if(mdl->name == NULL || strlen(mdl->name) == 0){
		mdl->name = malloc((fileLen+1)*sizeof(char));
		memcpy(mdl->name, filePath, fileLen);
		mdl->name[fileLen] = '\0';
	}
	/** Should mdlGenBufferObjects() be here? **/
	mdlGenBufferObjects(mdl, vertices, indices);
	free(indices);
	free(vertices);
	return 1;

}

static void mdlVertexAttributes(){
	// Position offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, pos));
	glEnableVertexAttribArray(0);
	// UV offset
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, u));
	glEnableVertexAttribArray(1);
	// Normals offset
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, nx));
	glEnableVertexAttribArray(2);
	// Bone index offset
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(vertex), (GLvoid*)offsetof(vertex, bIDs));
	glEnableVertexAttribArray(3);
	// Bone weight offset
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, bWeights));
	glEnableVertexAttribArray(4);
}

/** Change this function later **/
unsigned char mdlCreateSprite(model *mdl, char *name){

	mdlInit(mdl);
	GLenum glError;

	// Create and bind the VAO
	glGenVertexArrays(1, &mdl->vaoID);
	glBindVertexArray(mdl->vaoID);

	// Create and bind the VBO
	glGenBuffers(1, &mdl->vboID);
	glBindBuffer(GL_ARRAY_BUFFER, mdl->vboID);
	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error creating vertex buffer:\n%u\n", glError);
		return 0;
	}

	/** Should sprites use IBOs? Probably, but they're not working at the moment **/
	// Create and bind the IBO
	/*glGenBuffers(1, &mdl->iboID);
	glBindBuffer(GL_ARRAY_BUFFER, mdl->iboID);
	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error creating index buffer:\n%u\n", glError);
		return 0;
	}*/

	mdlVertexAttributes();
	glBindVertexArray(0);

	// Check for errors
	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error creating vertex array buffer:\n%u\n", glError);
	}

	mdl->vertexNum = 4;
	mdl->indexNum = 6;
	size_t nameLen = strlen(name);
	mdl->name = malloc((nameLen+1)*sizeof(char));
	memcpy(mdl->name, name, nameLen);
	mdl->name[nameLen] = '\0';

	return 1;

}

static void mdlGenBufferObjects(model *mdl, vertex *vertices, size_t *indices){

	if(mdl->vertexNum > 0){

		GLenum glError;

		// Create and bind the VAO
		glGenVertexArrays(1, &mdl->vaoID);
		glBindVertexArray(mdl->vaoID);

		// Create and bind the VBO
		glGenBuffers(1, &mdl->vboID);
		glBindBuffer(GL_ARRAY_BUFFER, mdl->vboID);
		glBufferData(GL_ARRAY_BUFFER, mdl->vertexNum * sizeof(vertex), vertices, GL_STATIC_DRAW);
		// Check for errors
		glError = glGetError();
		if(glError != GL_NO_ERROR){
			printf("Error creating vertex buffer:\n%u\n", glError);
			mdl->vertexNum = 0;
		}

		if(mdl->indexNum > 0){
			// Create and bind the IBO
			glGenBuffers(1, &mdl->iboID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->iboID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mdl->indexNum * sizeof(size_t), indices, GL_STATIC_DRAW);
			// Check for errors
			glError = glGetError();
			if(glError != GL_NO_ERROR){
				printf("Error creating index buffer:\n%u\n", glError);
				mdl->indexNum = 0;
			}
		}

		mdlVertexAttributes();
		glBindVertexArray(0);

		// Check for errors
		glError = glGetError();
		if(glError != GL_NO_ERROR){
			printf("Error creating vertex array buffer:\n%u\n", glError);
		}

	}

}

void mdlDelete(model *mdl){
	if(mdl->vaoID != 0){
		glDeleteBuffers(1, &mdl->vaoID);
	}
	if(mdl->vboID != 0){
		glDeleteBuffers(1, &mdl->vboID);
	}
	if(mdl->iboID != 0){
		glDeleteBuffers(1, &mdl->iboID);
	}
}
