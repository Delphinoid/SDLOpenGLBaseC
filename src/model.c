#define GLEW_STATIC
#include <GL/glew.h>
#include "model.h"
#include <stdio.h>

signed char mdlWavefrontObjLoad(const char *filePath, vertexIndex_t *vertexNum, vertex **vertices, vertexIndexNum_t *indexNum, vertexIndex_t **indices, char **name, char **sklPath);
signed char mdlSMDLoad(const char *filePath, vertexIndex_t *vertexNum, vertex **vertices, vertexIndexNum_t *indexNum, vertexIndex_t **indices, char **name, cVector *allSkeletons);

static void mdlVertexAttributes();
static signed char mdlGenBufferObjects(model *mdl, const char *filePath, const vertexIndex_t vertexNum, const vertex *vertices, const vertexIndexNum_t indexNum, const vertexIndex_t *indices);

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
	/**mdl->bodies = NULL;
	mdl->hitboxes = NULL;**/
}

signed char mdlLoad(model *mdl, const char *prgPath, const char *filePath, cVector *allSkeletons){

	/** Create a proper model file that loads a specified mesh, a name and a skeleton. **/
	signed char r;
	mdlInit(mdl);

	size_t pathLen = strlen(prgPath);
	size_t fileLen = strlen(filePath);
	char *fullPath = malloc((pathLen+fileLen+1)*sizeof(char));
	if(fullPath == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	memcpy(fullPath, prgPath, pathLen);
	memcpy(fullPath+pathLen, filePath, fileLen);
	fullPath[pathLen+fileLen] = '\0';

	vertexIndex_t vertexNum;
	vertex *vertices;
	vertexIndexNum_t indexNum;
	vertexIndex_t *indices;
	char *sklPath = NULL;
	r = mdlWavefrontObjLoad(fullPath, &vertexNum, &vertices, &indexNum, &indices, &mdl->name, &sklPath);
	//r = mdlSMDLoad(fullPath, &vertexNum, &vertices, &indexNum, &indices, &mdl->name, allSkeletons);
	/** Replace and move the loading function here. **/
	if(r <= 0){
		free(fullPath);
		return r;
	}
	if(sklPath == NULL){
		// Use the default skeleton.
        mdl->skl = (skeleton *)cvGet(allSkeletons, 0);
		//mdl->skl = (skeleton *)cvGet(allSkeletons, allSkeletons->size-1);
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
	r = mdlGenBufferObjects(mdl, fullPath, vertexNum, vertices, indexNum, indices);
	free(vertices);
	free(indices);

	if(r > 0){

		// If no name was given, generate one based off the file path.
		if(mdl->name == NULL || mdl->name[0] == '\0'){
			if(mdl->name != NULL){
				free(mdl->name);
			}
			mdl->name = malloc((fileLen+1)*sizeof(char));
			if(mdl->name == NULL){
				/** Memory allocation failure. **/
				mdlDelete(mdl);
				return -1;
			}
			memcpy(mdl->name, filePath, fileLen);
			mdl->name[fileLen] = '\0';
		}

	}else{
		mdlDelete(mdl);
	}

	return r;

}

signed char mdlDefault(model *mdl, cVector *allSkeletons){

	vertex vertices[24];
	vertexIndex_t indices[36];

	mdlInit(mdl);
	mdl->vertexNum = 24;
	mdl->indexNum = 36;

	vec3Set(&vertices[0].position, 1.f, -1.f, 1.f);
	vertices[0].u = 0.f; vertices[0].v = 0.f;
	vec3Set(&vertices[0].normal, 0.f, -1.f, 0.f);
	vertices[0].bIDs[0] = 0; vertices[0].bIDs[1] = -1; vertices[0].bIDs[2] = -1; vertices[0].bIDs[3] = -1;
	vertices[0].bWeights[0] = 1.f; vertices[0].bWeights[1] = 0.f; vertices[0].bWeights[2] = 0.f; vertices[0].bWeights[3] = 0.f;
	vec3Set(&vertices[1].position, 1.f, -1.f, -1.f);
	vertices[1].u = 1.f; vertices[1].v = 0.f;
	vec3Set(&vertices[1].normal, 0.f, -1.f, 0.f);
	vertices[1].bIDs[0] = 0; vertices[1].bIDs[1] = -1; vertices[1].bIDs[2] = -1; vertices[1].bIDs[3] = -1;
	vertices[1].bWeights[0] = 1.f; vertices[1].bWeights[1] = 0.f; vertices[1].bWeights[2] = 0.f; vertices[1].bWeights[3] = 0.f;
	vec3Set(&vertices[2].position, 1.f, 1.f, 1.f);
	vertices[2].u = 0.f; vertices[2].v = -1.f;
	vec3Set(&vertices[2].normal, 0.f, -1.f, 0.f);
	vertices[2].bIDs[0] = 0; vertices[2].bIDs[1] = -1; vertices[2].bIDs[2] = -1; vertices[2].bIDs[3] = -1;
	vertices[2].bWeights[0] = 1.f; vertices[2].bWeights[1] = 0.f; vertices[2].bWeights[2] = 0.f; vertices[2].bWeights[3] = 0.f;
	vec3Set(&vertices[3].position, 1.f, 1.f, -1.f);
	vertices[3].u = 1.f; vertices[3].v = -1.f;
	vec3Set(&vertices[3].normal, 0.f, -1.f, 0.f);
	vertices[3].bIDs[0] = 0; vertices[3].bIDs[1] = -1; vertices[3].bIDs[2] = -1; vertices[3].bIDs[3] = -1;
	vertices[3].bWeights[0] = 1.f; vertices[3].bWeights[1] = 0.f; vertices[3].bWeights[2] = 0.f; vertices[3].bWeights[3] = 0.f;
	vec3Set(&vertices[4].position, 1.f, 1.f, 1.f);
	vertices[4].u = 1.f; vertices[4].v = 0.f;
	vec3Set(&vertices[4].normal, 0.f, 1.f, 0.f);
	vertices[4].bIDs[0] = 0; vertices[4].bIDs[1] = -1; vertices[4].bIDs[2] = -1; vertices[4].bIDs[3] = -1;
	vertices[4].bWeights[0] = 1.f; vertices[4].bWeights[1] = 0.f; vertices[4].bWeights[2] = 0.f; vertices[4].bWeights[3] = 0.f;
	vec3Set(&vertices[5].position, 1.f, 1.f, -1.f);
	vertices[5].u = 1.f; vertices[5].v = -1.f;
	vec3Set(&vertices[5].normal, 0.f, 1.f, 0.f);
	vertices[5].bIDs[0] = 0; vertices[5].bIDs[1] = -1; vertices[5].bIDs[2] = -1; vertices[5].bIDs[3] = -1;
	vertices[5].bWeights[0] = 1.f; vertices[5].bWeights[1] = 0.f; vertices[5].bWeights[2] = 0.f; vertices[5].bWeights[3] = 0.f;
	vec3Set(&vertices[6].position, -1.f, 1.f, 1.f);
	vertices[6].u = 0.f; vertices[6].v = 0.f;
	vec3Set(&vertices[6].normal, 0.f, 1.f, 0.f);
	vertices[6].bIDs[0] = 0; vertices[6].bIDs[1] = -1; vertices[6].bIDs[2] = -1; vertices[6].bIDs[3] = -1;
	vertices[6].bWeights[0] = 1.f; vertices[6].bWeights[1] = 0.f; vertices[6].bWeights[2] = 0.f; vertices[6].bWeights[3] = 0.f;
	vec3Set(&vertices[7].position, -1.f, 1.f, -1.f);
	vertices[7].u = 0.f; vertices[7].v = -1.f;
	vec3Set(&vertices[7].normal, 0.f, 1.f, 0.f);
	vertices[7].bIDs[0] = 0; vertices[7].bIDs[1] = -1; vertices[7].bIDs[2] = -1; vertices[7].bIDs[3] = -1;
	vertices[7].bWeights[0] = 1.f; vertices[7].bWeights[1] = 0.f; vertices[7].bWeights[2] = 0.f; vertices[7].bWeights[3] = 0.f;
	vec3Set(&vertices[8].position, -1.f, 1.f, 1.f);
	vertices[8].u = 1.f; vertices[8].v = -1.f;
	vec3Set(&vertices[8].normal, 1.f, 0.f, 0.f);
	vertices[8].bIDs[0] = 0; vertices[8].bIDs[1] = -1; vertices[8].bIDs[2] = -1; vertices[8].bIDs[3] = -1;
	vertices[8].bWeights[0] = 1.f; vertices[8].bWeights[1] = 0.f; vertices[8].bWeights[2] = 0.f; vertices[8].bWeights[3] = 0.f;
	vec3Set(&vertices[9].position, -1.f, 1.f, -1.f);
	vertices[9].u = 0.f; vertices[9].v = -1.f;
	vec3Set(&vertices[9].normal, 1.f, 0.f, 0.f);
	vertices[9].bIDs[0] = 0; vertices[9].bIDs[1] = -1; vertices[9].bIDs[2] = -1; vertices[9].bIDs[3] = -1;
	vertices[9].bWeights[0] = 1.f; vertices[9].bWeights[1] = 0.f; vertices[9].bWeights[2] = 0.f; vertices[9].bWeights[3] = 0.f;
	vec3Set(&vertices[10].position, -1.f, -1.f, 1.f);
	vertices[10].u = 1.f; vertices[10].v = 0.f;
	vec3Set(&vertices[10].normal, 1.f, 0.f, 0.f);
	vertices[10].bIDs[0] = 0; vertices[10].bIDs[1] = -1; vertices[10].bIDs[2] = -1;
	vertices[10].bIDs[3] = -1;
	vertices[10].bWeights[0] = 1.f; vertices[10].bWeights[1] = 0.f; vertices[10].bWeights[2] = 0.f; vertices[10].bWeights[3] = 0.f;
	vec3Set(&vertices[11].position, -1.f, -1.f, -1.f);
	vertices[11].u = 0.f; vertices[11].v = 0.f;
	vec3Set(&vertices[11].normal, 1.f, 0.f, 0.f);
	vertices[11].bIDs[0] = 0; vertices[11].bIDs[1] = -1; vertices[11].bIDs[2] = -1;
	vertices[11].bIDs[3] = -1;
	vertices[11].bWeights[0] = 1.f; vertices[11].bWeights[1] = 0.f; vertices[11].bWeights[2] = 0.f; vertices[11].bWeights[3] = 0.f;
	vec3Set(&vertices[12].position, -1.f, -1.f, 1.f);
	vertices[12].u = 0.f; vertices[12].v = -1.f;
	vec3Set(&vertices[12].normal, 0.f, 0.f, 1.f);
	vertices[12].bIDs[0] = 0; vertices[12].bIDs[1] = -1; vertices[12].bIDs[2] = -1;
	vertices[12].bIDs[3] = -1;
	vertices[12].bWeights[0] = 1.f; vertices[12].bWeights[1] = 0.f; vertices[12].bWeights[2] = 0.f; vertices[12].bWeights[3] = 0.f;
	vec3Set(&vertices[13].position, -1.f, -1.f, -1.f);
	vertices[13].u = 0.f; vertices[13].v = 0.f;
	vec3Set(&vertices[13].normal, 0.f, 0.f, 1.f);
	vertices[13].bIDs[0] = 0; vertices[13].bIDs[1] = -1; vertices[13].bIDs[2] = -1;
	vertices[13].bIDs[3] = -1;
	vertices[13].bWeights[0] = 1.f; vertices[13].bWeights[1] = 0.f; vertices[13].bWeights[2] = 0.f; vertices[13].bWeights[3] = 0.f;
	vec3Set(&vertices[14].position, 1.f, -1.f, 1.f);
	vertices[14].u = 1.f; vertices[14].v = -1.f;
	vec3Set(&vertices[14].normal, 0.f, 0.f, 1.f);
	vertices[14].bIDs[0] = 0; vertices[14].bIDs[1] = -1; vertices[14].bIDs[2] = -1;
	vertices[14].bIDs[3] = -1;
	vertices[14].bWeights[0] = 1.f; vertices[14].bWeights[1] = 0.f; vertices[14].bWeights[2] = 0.f; vertices[14].bWeights[3] = 0.f;
	vec3Set(&vertices[15].position, 1.f, -1.f, -1.f);
	vertices[15].u = 1.f; vertices[15].v = 0.f;
	vec3Set(&vertices[15].normal, 0.f, 0.f, 1.f);
	vertices[15].bIDs[0] = 0; vertices[15].bIDs[1] = -1; vertices[15].bIDs[2] = -1;
	vertices[15].bIDs[3] = -1;
	vertices[15].bWeights[0] = 1.f; vertices[15].bWeights[1] = 0.f; vertices[15].bWeights[2] = 0.f; vertices[15].bWeights[3] = 0.f;
	vec3Set(&vertices[16].position, -1.f, -1.f, 1.f);
	vertices[16].u = 0.f; vertices[16].v = 0.f;
	vec3Set(&vertices[16].normal, -1.f, 0.f, 0.f);
	vertices[16].bIDs[0] = 0; vertices[16].bIDs[1] = -1; vertices[16].bIDs[2] = -1;
	vertices[16].bIDs[3] = -1;
	vertices[16].bWeights[0] = 1.f; vertices[16].bWeights[1] = 0.f; vertices[16].bWeights[2] = 0.f; vertices[16].bWeights[3] = 0.f;
	vec3Set(&vertices[17].position, 1.f, -1.f, 1.f);
	vertices[17].u = 1.f; vertices[17].v = 0.f;
	vec3Set(&vertices[17].normal, -1.f, 0.f, 0.f);
	vertices[17].bIDs[0] = 0; vertices[17].bIDs[1] = -1; vertices[17].bIDs[2] = -1;
	vertices[17].bIDs[3] = -1;
	vertices[17].bWeights[0] = 1.f; vertices[17].bWeights[1] = 0.f; vertices[17].bWeights[2] = 0.f; vertices[17].bWeights[3] = 0.f;
	vec3Set(&vertices[18].position, -1.f, 1.f, 1.f);
	vertices[18].u = 0.f; vertices[18].v = -1.f;
	vec3Set(&vertices[18].normal, -1.f, 0.f, 0.f);
	vertices[18].bIDs[0] = 0; vertices[18].bIDs[1] = -1; vertices[18].bIDs[2] = -1;
	vertices[18].bIDs[3] = -1;
	vertices[18].bWeights[0] = 1.f; vertices[18].bWeights[1] = 0.f; vertices[18].bWeights[2] = 0.f; vertices[18].bWeights[3] = 0.f;
	vec3Set(&vertices[19].position, 1.f, 1.f, 1.f);
	vertices[19].u = 1.f; vertices[19].v = -1.f;
	vec3Set(&vertices[19].normal, -1.f, 0.f, 0.f);
	vertices[19].bIDs[0] = 0; vertices[19].bIDs[1] = -1; vertices[19].bIDs[2] = -1;
	vertices[19].bIDs[3] = -1;
	vertices[19].bWeights[0] = 1.f; vertices[19].bWeights[1] = 0.f; vertices[19].bWeights[2] = 0.f; vertices[19].bWeights[3] = 0.f;
	vec3Set(&vertices[20].position, 1.f, -1.f, -1.f);
	vertices[20].u = 0.f; vertices[20].v = 0.f;
	vec3Set(&vertices[20].normal, 0.f, 0.f, -1.f);
	vertices[20].bIDs[0] = 0; vertices[20].bIDs[1] = -1; vertices[20].bIDs[2] = -1;
	vertices[20].bIDs[3] = -1;
	vertices[20].bWeights[0] = 1.f; vertices[20].bWeights[1] = 0.f; vertices[20].bWeights[2] = 0.f; vertices[20].bWeights[3] = 0.f;
	vec3Set(&vertices[21].position, -1.f, -1.f, -1.f);
	vertices[21].u = 1.f; vertices[21].v = 0.f;
	vec3Set(&vertices[21].normal, 0.f, 0.f, -1.f);
	vertices[21].bIDs[0] = 0; vertices[21].bIDs[1] = -1; vertices[21].bIDs[2] = -1;
	vertices[21].bIDs[3] = -1;
	vertices[21].bWeights[0] = 1.f; vertices[21].bWeights[1] = 0.f; vertices[21].bWeights[2] = 0.f; vertices[21].bWeights[3] = 0.f;
	vec3Set(&vertices[22].position, 1.f, 1.f, -1.f);
	vertices[22].u = 0.f; vertices[22].v = -1.f;
	vec3Set(&vertices[22].normal, 0.f, 0.f, -1.f);
	vertices[22].bIDs[0] = 0; vertices[22].bIDs[1] = -1; vertices[22].bIDs[2] = -1;
	vertices[22].bIDs[3] = -1;
	vertices[22].bWeights[0] = 1.f; vertices[22].bWeights[1] = 0.f; vertices[22].bWeights[2] = 0.f; vertices[22].bWeights[3] = 0.f;
	vec3Set(&vertices[23].position, -1.f, 1.f, -1.f);
	vertices[23].u = 1.f; vertices[23].v = -1.f;
	vec3Set(&vertices[23].normal, 0.f, 0.f, -1.f);
	vertices[23].bIDs[0] = 0; vertices[23].bIDs[1] = -1; vertices[23].bIDs[2] = -1;
	vertices[23].bIDs[3] = -1;
	vertices[23].bWeights[0] = 1.f; vertices[23].bWeights[1] = 0.f; vertices[23].bWeights[2] = 0.f; vertices[23].bWeights[3] = 0.f;

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;
	indices[6] = 4;
	indices[7] = 5;
	indices[8] = 6;
	indices[9] = 6;
	indices[10] = 5;
	indices[11] = 7;
	indices[12] = 8;
	indices[13] = 9;
	indices[14] = 10;
	indices[15] = 10;
	indices[16] = 9;
	indices[17] = 11;
	indices[18] = 12;
	indices[19] = 13;
	indices[20] = 14;
	indices[21] = 14;
	indices[22] = 13;
	indices[23] = 15;
	indices[24] = 16;
	indices[25] = 17;
	indices[26] = 18;
	indices[27] = 18;
	indices[28] = 17;
	indices[29] = 19;
	indices[30] = 20;
	indices[31] = 21;
	indices[32] = 22;
	indices[33] = 22;
	indices[34] = 21;
	indices[35] = 23;

	mdl->name = malloc(8*sizeof(char));
	if(mdl->name == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	mdl->name[0] = 'd';
	mdl->name[1] = 'e';
	mdl->name[2] = 'f';
	mdl->name[3] = 'a';
	mdl->name[4] = 'u';
	mdl->name[5] = 'l';
	mdl->name[6] = 't';
	mdl->name[7] = '\0';

	// Use the default skeleton.
	mdl->skl = (skeleton *)cvGet(allSkeletons, 0);

	if(mdlGenBufferObjects(mdl, NULL, mdl->vertexNum, vertices, mdl->indexNum, indices) <= 0){
		mdlDelete(mdl);
		return 0;
	}

	return 1;

}

/** Change this function later **/
signed char mdlCreateSprite(model *mdl, cVector *allSkeletons){

	GLenum glError;

	mdlInit(mdl);

	mdl->name = malloc(7*sizeof(char));
	if(mdl->name == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	mdl->name[0] = 's';
	mdl->name[1] = 'p';
	mdl->name[2] = 'r';
	mdl->name[3] = 'i';
	mdl->name[4] = 't';
	mdl->name[5] = 'e';
	mdl->name[6] = '\0';

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
	glVertexAttribIPointer(3, VERTEX_MAX_BONES, GL_INT, sizeof(vertex), (GLvoid*)offsetof(vertex, bIDs));
	glEnableVertexAttribArray(3);
	// Bone weight offset
	glVertexAttribPointer(4, VERTEX_MAX_BONES, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, bWeights));
	glEnableVertexAttribArray(4);
}

static signed char mdlGenBufferObjects(model *mdl, const char *filePath, const vertexIndex_t vertexNum, const vertex *vertices, const vertexIndexNum_t indexNum, const vertexIndex_t *indices){

	if(vertexNum > 0){
		if(indexNum > 0){

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
				printf("Error creating vertex buffer for model");
				if(filePath != NULL){
					printf(" \"%s\"", filePath);
				}
				printf(": %u\n", glError);
				return 0;
			}
			mdl->vertexNum = vertexNum;

			// Create and bind the IBO
			glGenBuffers(1, &mdl->iboID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->iboID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexNum*sizeof(vertexIndex_t), indices, GL_STATIC_DRAW);
			// Check for errors
			glError = glGetError();
			if(glError != GL_NO_ERROR){
				printf("Error creating index buffer for model");
				if(filePath != NULL){
					printf(" \"%s\"", filePath);
				}
				printf(": %u\n", glError);
				return 0;
			}
			mdl->indexNum = indexNum;

			mdlVertexAttributes();
			glBindVertexArray(0);

			// Check for errors
			glError = glGetError();
			if(glError != GL_NO_ERROR){
				printf("Error creating vertex array buffer for model");
				if(filePath != NULL){
					printf(" \"%s\"", filePath);
				}
				printf(": %u\n", glError);
				return 0;
			}

		}else{
			printf("Error creating buffers for model");
			if(filePath != NULL){
				printf(" \"%s\"", filePath);
			}
			printf(": model has no indices.\n");
			return 0;
		}

	}else{
		printf("Error creating buffers for model");
		if(filePath != NULL){
			printf(" \"%s\"", filePath);
		}
		printf(": model has no vertices.\n");
		return 0;
	}

	return 1;

}

void mdlDelete(model *mdl){
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
