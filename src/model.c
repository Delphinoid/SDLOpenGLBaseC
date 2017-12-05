#define GLEW_STATIC
#include <GL/glew.h>
#include "model.h"
#include <stdio.h>

/** Remove printf()s **/

static void mdlGenBufferObjects(model *mdl, const mesh *m);

void mdlInit(model *mdl){
	mdl->name = NULL;
	mdl->skl = NULL;
	mdl->vertexNum = 0;
	mdl->indexNum = 0;
	mdl->vaoID = 0;
	mdl->vboID = 0;
	mdl->iboID = 0;
}

unsigned char mdlLoad(model *mdl, const char *prgPath, const char *filePath){

	/** Create a proper model file that loads a specified mesh, a name and a skeleton. **/

	mdlInit(mdl);

	size_t pathLen = strlen(prgPath);
	size_t fileLen = strlen(filePath);
	char *fullPath = malloc((pathLen+fileLen+1)*sizeof(char));
	if(fullPath == NULL){
		/** Remove printf()s **/
		printf("Error loading model: Memory allocation failure.\n");
		return 0;
	}
	memcpy(fullPath, prgPath, pathLen);
	memcpy(fullPath+pathLen, filePath, fileLen);
	fullPath[pathLen+fileLen] = '\0';

	mesh m;
	if(!meshLoadWavefrontObj(&m, fullPath)){
		meshDelete(&m);
		free(fullPath);
		return 0;
	}
	free(fullPath);

	/** Should mdlGenBufferObjects() be here? **/
	mdlGenBufferObjects(mdl, &m);
	meshDelete(&m);

	// If no name was given, generate one based off the file name
	if(mdl->name == NULL || mdl->name[0] == '\0'){
		mdl->name = malloc((fileLen+1)*sizeof(char));
		if(mdl->name == NULL){
			printf("Error loading model: Memory allocation failure.\n");
			mdlDelete(mdl);
			return 0;
		}
		memcpy(mdl->name, filePath, fileLen);
		mdl->name[fileLen] = '\0';
	}

	return 1;

}

static void mdlVertexAttributes(){
	// Position offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, position));
	glEnableVertexAttribArray(0);
	// UV offset
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, u));
	glEnableVertexAttribArray(1);
	// Normals offset
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, normal));
	glEnableVertexAttribArray(2);
	// Bone index offset
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(vertex), (GLvoid*)offsetof(vertex, bIDs));
	glEnableVertexAttribArray(3);
	// Bone weight offset
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, bWeights));
	glEnableVertexAttribArray(4);
}

/** Change this function later **/
unsigned char mdlCreateSprite(model *mdl, const char *name){

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
		printf("Error creating vertex buffer: %u\n", glError);
		return 0;
	}

	/** Should sprites use IBOs? Probably, but they're not working at the moment **/
	// Create and bind the IBO
	/*glGenBuffers(1, &mdl->iboID);
	glBindBuffer(GL_ARRAY_BUFFER, mdl->iboID);
	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error creating index buffer: %u\n", glError);
		return 0;
	}*/

	mdlVertexAttributes();
	glBindVertexArray(0);

	// Check for errors
	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error creating vertex array buffer: %u\n", glError);
	}

	mdl->vertexNum = 4;
	mdl->indexNum = 6;
	const size_t nameLen = strlen(name);
	mdl->name = malloc((nameLen+1)*sizeof(char));
	memcpy(mdl->name, name, nameLen);
	mdl->name[nameLen] = '\0';

	return 1;

}

static void mdlGenBufferObjects(model *mdl, const mesh *m){

	if(m->vertexNum > 0){

		GLenum glError;

		// Create and bind the VAO
		glGenVertexArrays(1, &mdl->vaoID);
		glBindVertexArray(mdl->vaoID);

		// Create and bind the VBO
		glGenBuffers(1, &mdl->vboID);
		glBindBuffer(GL_ARRAY_BUFFER, mdl->vboID);
		glBufferData(GL_ARRAY_BUFFER, m->vertexNum * sizeof(vertex), m->vertices, GL_STATIC_DRAW);
		// Check for errors
		glError = glGetError();
		if(glError != GL_NO_ERROR){
			printf("Error creating vertex buffer: %u\n", glError);
		}else{
			mdl->vertexNum = m->vertexNum;
		}

		if(m->indexNum > 0){
			// Create and bind the IBO
			glGenBuffers(1, &mdl->iboID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->iboID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->indexNum * sizeof(size_t), m->indices, GL_STATIC_DRAW);
			// Check for errors
			glError = glGetError();
			if(glError != GL_NO_ERROR){
				printf("Error creating index buffer: %u\n", glError);
			}else{
				mdl->indexNum = m->indexNum;
			}
		}

		mdlVertexAttributes();
		glBindVertexArray(0);

		// Check for errors
		glError = glGetError();
		if(glError != GL_NO_ERROR){
			printf("Error creating vertex array buffer: %u\n", glError);
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
