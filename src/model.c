#define GLEW_STATIC
#include <GL/glew.h>
#include "model.h"
#include <stdio.h>

signed char mdlWavefrontObjLoad(const char *filePath, size_t *vertexNum, vertex **vertices, size_t *indexNum, size_t **indices, char **name, char **sklPath);

static void mdlVertexAttributes();
static void mdlGenBufferObjects(model *mdl, const size_t vertexNum, const vertex *vertices, const size_t indexNum, const size_t *indices);

void mdlInit(model *mdl){
	mdl->name = NULL;
	mdl->vertexNum = 0;
	mdl->indexNum = 0;
	mdl->vaoID = 0;
	mdl->vboID = 0;
	mdl->iboID = 0;
	/**mdl->boneNum = 0;
	mdl->boneNames = NULL;**/
	mdl->skl = NULL;
	mdl->bodies = NULL;
	mdl->hitboxes = NULL;
}

signed char mdlLoad(model *mdl, const char *prgPath, const char *filePath, cVector *allSkeletons){

	/** Create a proper model file that loads a specified mesh, a name and a skeleton. **/

	mdlInit(mdl);

	size_t pathLen = strlen(prgPath);
	size_t fileLen = strlen(filePath);
	char *fullPath = malloc((pathLen+fileLen+1)*sizeof(char));
	if(fullPath == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	memcpy(fullPath, prgPath, pathLen);
	memcpy(fullPath+pathLen, filePath, fileLen);
	fullPath[pathLen+fileLen] = '\0';

	size_t vertexNum;
	vertex *vertices;
	size_t indexNum;
	size_t *indices;
	char *sklPath = NULL;
	/** Replace and move the loading function here. **/
	if(!mdlWavefrontObjLoad(fullPath, &vertexNum, &vertices, &indexNum, &indices, &mdl->name, &sklPath)){
		free(fullPath);
		return 0;
	}
	if(sklPath == NULL){
		// Use the default skeleton.
        mdl->skl = (skeleton *)cvGet(allSkeletons, 0);
	}else{
		/** Check if the skeleton already exists. If not, load it. **/
		skeleton tempSkl;
		sklLoad(&tempSkl, prgPath, sklPath);
		cvPush(allSkeletons, (void *)&tempSkl, sizeof(tempSkl));
		mdl->skl = (skeleton *)cvGet(allSkeletons, allSkeletons->size-1);
		free(sklPath);
	}
	free(fullPath);

	/** Should mdlGenBufferObjects() be here? **/
	mdlGenBufferObjects(mdl, vertexNum, vertices, indexNum, indices);
	free(vertices);
	free(indices);

	// If no name was given, generate one based off the file name
	if(mdl->name == NULL || mdl->name[0] == '\0'){
		mdl->name = malloc((fileLen+1)*sizeof(char));
		if(mdl->name == NULL){
			/** Memory allocation failure. **/
			mdlDelete(mdl);
			return 0;
		}
		memcpy(mdl->name, filePath, fileLen);
		mdl->name[fileLen] = '\0';
	}

	return 1;

}

/** Change this function later **/
signed char mdlCreateSprite(model *mdl, cVector *allSkeletons){

	GLenum glError;

	mdlInit(mdl);
	// Use the default skeleton.
	mdl->skl = (skeleton *)cvGet(allSkeletons, 0);

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
	mdl->name = NULL;

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

static void mdlGenBufferObjects(model *mdl, const size_t vertexNum, const vertex *vertices, const size_t indexNum, const size_t *indices){

	if(vertexNum > 0){

		GLenum glError;

		// Create and bind the VAO
		glGenVertexArrays(1, &mdl->vaoID);
		glBindVertexArray(mdl->vaoID);

		// Create and bind the VBO
		glGenBuffers(1, &mdl->vboID);
		glBindBuffer(GL_ARRAY_BUFFER, mdl->vboID);
		glBufferData(GL_ARRAY_BUFFER, vertexNum*sizeof(vertex), vertices, GL_STATIC_DRAW);
		// Check for errors
		glError = glGetError();
		if(glError != GL_NO_ERROR){
			printf("Error creating vertex buffer: %u\n", glError);
		}else{
			mdl->vertexNum = vertexNum;
		}

		if(indexNum > 0){
			// Create and bind the IBO
			glGenBuffers(1, &mdl->iboID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->iboID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexNum*sizeof(size_t), indices, GL_STATIC_DRAW);
			// Check for errors
			glError = glGetError();
			if(glError != GL_NO_ERROR){
				printf("Error creating index buffer: %u\n", glError);
			}else{
				mdl->indexNum = indexNum;
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
	size_t i;
	if(mdl->name != NULL){
		free(mdl->name);
	}
	if(mdl->vaoID != 0){
		glDeleteBuffers(1, &mdl->vaoID);
	}
	if(mdl->vboID != 0){
		glDeleteBuffers(1, &mdl->vboID);
	}
	if(mdl->iboID != 0){
		glDeleteBuffers(1, &mdl->iboID);
	}
	/**if(mdl->bones != NULL){
		for(i = 0; i < mdl->boneNum; ++i){
			free(mdl->boneNames[i]);
		}
		free(mdl->boneNames);
	}
	if(mdl->bodies != NULL){
		for(i = 0; i < mdl->boneNum; ++i){
			physRigidBodyDelete(&mdl->bodies[i]);
		}
		free(mdl->bodies);
	}
	if(mdl->hitboxes != NULL){
		for(i = 0; i < mdl->boneNum; ++i){
			free(mdl->hitboxes[i]);
		}
		free(mdl->hitboxes);
	}**/
}
