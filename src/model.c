#define GLEW_STATIC
#include <GL/glew.h>
#include "model.h"
#include "cVector.h"
#include <stdio.h>

#define vertexStartCapacity 1024
#define indexStartCapacity 2048

//void generateNameFromPath(char **name, const char *path);
void copyString(char **destination, const char *source, const unsigned int length);

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

	vertex *vertices = malloc(vertexStartCapacity*sizeof(vertex));
	unsigned int *indices = malloc(indexStartCapacity*sizeof(unsigned int));

	if(vertices == NULL){
		printf("Error loading model:\nMemory allocation failure for vertex buffer.\n");
		return 0;
	}else if(indices == NULL){
		printf("Error loading model:\nMemory allocation failure for index buffer.\n");
		free(vertices);
		return 0;
	}

	size_t vertexCapacity = vertexStartCapacity;
	size_t vertexNum = 0;
	size_t indexCapacity = indexStartCapacity;
	size_t indexNum = 0;

	char *fullPath = malloc((strlen(prgPath) + strlen(filePath) + 1) * sizeof(char));
	strcpy(fullPath, prgPath);
	strcat(fullPath, filePath);
	fullPath[strlen(prgPath)+strlen(filePath)] = '\0';
	FILE *mdlInfo = fopen(fullPath, "r");
	char lineFeed[1024];
	char *line;
	char compare[1024];
	unsigned int lineLength;

	cVector tempPositions; cvInit(&tempPositions, 3);  // Holds floats; temporarily holds vertex position data before it is pushed into vertexBuffer
	cVector tempTexCoords; cvInit(&tempTexCoords, 2);  // Holds floats; temporarily holds vertex UV data before it is pushed into vertexBuffer
	cVector tempNorms;     cvInit(&tempNorms, 3);      // Holds floats; temporarily holds vertex normal data before it is pushed into vertexBuffer
	vertex tempVert;  // Holds a vertex before pushing it into the triangle array
	unsigned int positionIndex[3];  // Holds all the positional information for a face
	unsigned int uvIndex[3];        // Holds all the UV information for a face
	unsigned int normalIndex[3];    // Holds all the normal information for a face

	if(mdlInfo != NULL){
		while(!feof(mdlInfo)){

			fgets(lineFeed, sizeof(lineFeed), mdlInfo);
			lineFeed[strcspn(lineFeed, "\r\n")] = 0;
			line = lineFeed;
			lineLength = strlen(line);

			// Remove any comments from the line
			char *commentPos = strstr(line, "//");
			if(commentPos != NULL){
				commentPos = '\0';
			}
			// Remove any indentations from the line
			unsigned int d;
			for(d = 0; d < lineLength; d++){
				if(line[d] != '\t' && line[d] != ' '){
					line += d;
					d = lineLength;
				}
			}

			// Name
			if(lineLength >= 6 && strncpy(compare, line, 5) && (compare[5] = '\0') == 0 && strcmp(compare, "name ") == 0){
				mdl->name = malloc((lineLength-4) * sizeof(char));
				if(mdl->name != NULL){
					strncpy(mdl->name, line+5, lineLength-5);
					mdl->name[lineLength-5] = '\0';
				}

			// Vertex data
			}else if(lineLength >= 7 && strncpy(compare, line, 2) && (compare[2] = '\0') == 0 && strcmp(compare, "v ") == 0){
				char *token = strtok(line+2, " /");
				float curVal = strtof(token, NULL);
				cvPush(&tempPositions, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");
				curVal = strtof(token, NULL);
				cvPush(&tempPositions, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");
				curVal = strtof(token, NULL);
				cvPush(&tempPositions, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");

			// UV data
			}else if(lineLength >= 6 && strncpy(compare, line, 3) && (compare[3] = '\0') == 0 && strcmp(compare, "vt ") == 0){
				char *token = strtok(line+3, " /");
				float curVal = strtof(token, NULL);
				cvPush(&tempTexCoords, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");
				curVal = strtof(token, NULL);
				cvPush(&tempTexCoords, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");

			// Normal data
			}else if(lineLength >= 8 && strncpy(compare, line, 3) && (compare[3] = '\0') == 0 && strcmp(compare, "vn ") == 0){
				char *token = strtok(line+3, " /");
				float curVal = strtof(token, NULL);
				cvPush(&tempNorms, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");
				curVal = strtof(token, NULL);
				cvPush(&tempNorms, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");
				curVal = strtof(token, NULL);
				cvPush(&tempNorms, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");

			// Face data
			}else if(lineLength >= 19 && strncpy(compare, line, 2) && (compare[2] = '\0') == 0 && strcmp(compare, "f ") == 0){
				char *token = strtok(line+2, " /");
				for(d = 0; d < 3; d++){

					// Load face data
					positionIndex[d] = strtoul(token, NULL, 0);
					token = strtok(NULL, " /");
					uvIndex[d] = strtoul(token, NULL, 0);
					token = strtok(NULL, " /");
					normalIndex[d] = strtoul(token, NULL, 0);
					token = strtok(NULL, " /");

					// Create a vertex from the given data
					// Vertex positional data
					void *checkVal = cvGet(&tempPositions, (positionIndex[d]-1)*3);
					if(checkVal != NULL){
						tempVert.pos.x = *((float *)checkVal);
					}else{
						tempVert.pos.x = 0.f;
					}
					checkVal = cvGet(&tempPositions, (positionIndex[d]-1)*3+1);
					if(checkVal != NULL){
						tempVert.pos.y = *((float *)checkVal);
					}else{
						tempVert.pos.y = 0.f;
					}
					checkVal = cvGet(&tempPositions, (positionIndex[d]-1)*3+2);
					if(checkVal != NULL){
						tempVert.pos.z = *((float *)checkVal);
					}else{
						tempVert.pos.z = 0.f;
					}
					// Vertex UV data
					checkVal = cvGet(&tempTexCoords, (uvIndex[d]-1)*2);
					if(checkVal != NULL){
						tempVert.u = *((float *)checkVal);
					}else{
						tempVert.u = 0.f;
					}
					checkVal = cvGet(&tempTexCoords, (uvIndex[d]-1)*2+1);
					if(checkVal != NULL){
						tempVert.v = -*((float *)checkVal);
					}else{
						tempVert.v = 0.f;
					}
					// Vertex normal data
					checkVal = cvGet(&tempNorms, (normalIndex[d]-1)*3);
					if(checkVal != NULL){
						tempVert.nx = *((float *)checkVal);
					}else{
						tempVert.nx = 0.f;
					}
					checkVal = cvGet(&tempNorms, (normalIndex[d]-1)*3+1);
					if(checkVal != NULL){
						tempVert.ny = *((float *)checkVal);
					}else{
						tempVert.ny = 0.f;
					}
					checkVal = cvGet(&tempNorms, (normalIndex[d]-1)*3+2);
					if(checkVal != NULL){
						tempVert.nz = *((float *)checkVal);
					}else{
						tempVert.nz = 0.f;
					}

					// Check if the vertex has already been loaded, and if so add an index
					unsigned char foundVertex = 0;
					unsigned int f;
					for(f = 0; f < vertexNum; f++){
						vertex *checkVert = &vertices[f];
						if(checkVert->pos.x == tempVert.pos.x && checkVert->pos.y == tempVert.pos.y && checkVert->pos.z == tempVert.pos.z &&
						   checkVert->u     == tempVert.u     && checkVert->v     == tempVert.v     &&
						   checkVert->nx    == tempVert.nx    && checkVert->ny    == tempVert.ny    && checkVert->nz    == tempVert.nz){

							// Resize indices if there's not enough room
							if(indexNum == indexCapacity){
								indexCapacity *= 2;
								unsigned int *tempBuffer = realloc(indices, indexCapacity*sizeof(unsigned int));
								if(tempBuffer != NULL){
									indices = tempBuffer;
								}else{
									printf("Error loading model:\nMemory allocation failure for index buffer.\n");
									cvClear(&tempPositions);
									cvClear(&tempTexCoords);
									cvClear(&tempNorms);
									free(fullPath);
									free(indices);
									free(vertices);
									return 0;
								}
							}
							indices[indexNum++] = f;
							f = vertexNum;
							foundVertex = 1;
						}
					}

					// If the vertex has not yet been loaded, add it to both the vertex vector and the index vector
					if(!foundVertex){
						// Resize indices if there's not enough room
						if(indexNum == indexCapacity){
							indexCapacity *= 2;
							unsigned int *tempBuffer = realloc(indices, indexCapacity*sizeof(unsigned int));
							if(tempBuffer != NULL){
								indices = tempBuffer;
							}else{
								printf("Error loading model:\nMemory allocation failure for index buffer.\n");
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
						if(vertexNum == vertexCapacity){
							vertexCapacity *= 2;
							vertex *tempBuffer = realloc(vertices, vertexCapacity*sizeof(vertex));
							if(tempBuffer != NULL){
								vertices = tempBuffer;
							}else{
								printf("Error loading model:\nMemory allocation failure for vertex buffer.\n");
								cvClear(&tempPositions);
								cvClear(&tempTexCoords);
								cvClear(&tempNorms);
								free(fullPath);
								free(indices);
								free(vertices);
								return 0;
							}
						}
						indices[indexNum++] = vertexNum;
						vertices[vertexNum++] = tempVert;
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
	cvClear(&tempTexCoords);
	cvClear(&tempNorms);
	free(fullPath);

	// If no name was given, generate one based off the file name
	if(mdl->name == NULL || strlen(mdl->name) == 0){
		//generateNameFromPath(mdl->name, filePath);
		copyString(&mdl->name, filePath, strlen(filePath));
	}
	/** Should mdlGenBufferObjects() be here? **/
	mdlGenBufferObjects(mdl, vertices, vertexNum, indices, indexNum);
	free(indices);
	free(vertices);
	return 1;

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

	// Position offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, pos));
	glEnableVertexAttribArray(0);
	// UV offset
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, u));
	glEnableVertexAttribArray(1);
	// Normals offset
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, nx));
	glEnableVertexAttribArray(2);
	// We don't want anything else to modify the VAO
	glBindVertexArray(0);

	// Check for errors
	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error creating vertex array buffer:\n%u\n", glError);
	}

	mdl->vertexNum = 4;
	mdl->indexNum = 6;
	copyString(&mdl->name, name, strlen(name));

	return 1;

}

void mdlGenBufferObjects(model *mdl, vertex *vertices, size_t vertexNum, unsigned int *indices, size_t indexNum){

	if(vertexNum > 0){

		GLenum glError;

		// Create and bind the VAO
		glGenVertexArrays(1, &mdl->vaoID);
		glBindVertexArray(mdl->vaoID);

		// Create and bind the VBO
		glGenBuffers(1, &mdl->vboID);
		glBindBuffer(GL_ARRAY_BUFFER, mdl->vboID);
		glBufferData(GL_ARRAY_BUFFER, vertexNum * sizeof(vertex), vertices, GL_STATIC_DRAW);
		// Check for errors
		glError = glGetError();
		if(glError != GL_NO_ERROR){
			printf("Error creating vertex buffer:\n%u\n", glError);
		}else{
			// If there are no errors, set mdl->vertexNum
			mdl->vertexNum = vertexNum;
		}

		if(indexNum > 0){
			// Create and bind the IBO
			glGenBuffers(1, &mdl->iboID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->iboID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexNum * sizeof(unsigned int), indices, GL_STATIC_DRAW);
			// Check for errors
			glError = glGetError();
			if(glError != GL_NO_ERROR){
				printf("Error creating index buffer:\n%u\n", glError);
			}else{
				// If there are no errors, set mdl->indexNum
				mdl->indexNum = indexNum;
			}
		}

		// Position offset
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, pos));
		glEnableVertexAttribArray(0);
		// UV offset
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, u));
		glEnableVertexAttribArray(1);
		// Normals offset
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, nx));
		glEnableVertexAttribArray(2);
		// We don't want anything else to modify the VAO
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
	//free(&mdl->vertices);
	//free(&mdl->indices);
}
