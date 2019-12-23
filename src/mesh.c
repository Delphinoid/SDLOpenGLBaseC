#define GLEW_STATIC
#include <GL/glew.h>
#include "mesh.h"
#include "vertex.h"
#include <stdio.h>

// Default meshes.
mesh meshDefault = {
	.vertexNum = 0,
	.indexNum = 0,
	.vaoID = 0,
	.vboID = 0,
	.iboID = 0
};
mesh meshSprite = {
	.vertexNum = 0,
	.indexNum = 0,
	.vaoID = 0,
	.vboID = 0,
	.iboID = 0
};

static void meshVertexAttributes(){
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

return_t meshGenerateBuffers(mesh *const restrict m, const vertexIndex_t vertexNum, const vertex *const restrict vertices, const vertexIndexNum_t indexNum, const vertexIndexNum_t *const restrict indices, const char *const restrict filePath){

	if(vertexNum > 0){
		if(indexNum > 0){

			GLenum glError;

			// Create and bind the VAO
			glGenVertexArrays(1, &m->vaoID);
			glBindVertexArray(m->vaoID);

			// Create and bind the VBO
			glGenBuffers(1, &m->vboID);
			glBindBuffer(GL_ARRAY_BUFFER, m->vboID);
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

			// Create and bind the IBO
			glGenBuffers(1, &m->iboID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->iboID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexNum*sizeof(vertexIndexNum_t), indices, GL_STATIC_DRAW);
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

			meshVertexAttributes();
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

	m->vertexNum = vertexNum;
	m->indexNum = indexNum;
	return 1;

}



return_t meshDefaultInit(){

	/** 24 vertices and 36 indices for a cube? What was I thinking? **/

	vertex vertices[24];
	vertexIndex_t indices[36];

	vertices[0].position = vec3New(1.f, -1.f, 1.f);
	vertices[0].u = 0.f; vertices[0].v = 0.f;
	vertices[0].normal = vec3New(0.f, -1.f, 0.f);
	vertices[0].bIDs[0] = 0; vertices[0].bIDs[1] = -1; vertices[0].bIDs[2] = -1; vertices[0].bIDs[3] = -1;
	vertices[0].bWeights[0] = 1.f; vertices[0].bWeights[1] = 0.f; vertices[0].bWeights[2] = 0.f; vertices[0].bWeights[3] = 0.f;
	vertices[1].position = vec3New(1.f, -1.f, -1.f);
	vertices[1].u = 1.f; vertices[1].v = 0.f;
	vertices[1].normal = vec3New(0.f, -1.f, 0.f);
	vertices[1].bIDs[0] = 0; vertices[1].bIDs[1] = -1; vertices[1].bIDs[2] = -1; vertices[1].bIDs[3] = -1;
	vertices[1].bWeights[0] = 1.f; vertices[1].bWeights[1] = 0.f; vertices[1].bWeights[2] = 0.f; vertices[1].bWeights[3] = 0.f;
	vertices[2].position = vec3New(1.f, 1.f, 1.f);
	vertices[2].u = 0.f; vertices[2].v = -1.f;
	vertices[2].normal = vec3New(0.f, -1.f, 0.f);
	vertices[2].bIDs[0] = 0; vertices[2].bIDs[1] = -1; vertices[2].bIDs[2] = -1; vertices[2].bIDs[3] = -1;
	vertices[2].bWeights[0] = 1.f; vertices[2].bWeights[1] = 0.f; vertices[2].bWeights[2] = 0.f; vertices[2].bWeights[3] = 0.f;
	vertices[3].position = vec3New(1.f, 1.f, -1.f);
	vertices[3].u = 1.f; vertices[3].v = -1.f;
	vertices[3].normal = vec3New(0.f, -1.f, 0.f);
	vertices[3].bIDs[0] = 0; vertices[3].bIDs[1] = -1; vertices[3].bIDs[2] = -1; vertices[3].bIDs[3] = -1;
	vertices[3].bWeights[0] = 1.f; vertices[3].bWeights[1] = 0.f; vertices[3].bWeights[2] = 0.f; vertices[3].bWeights[3] = 0.f;
	vertices[4].position = vec3New(1.f, 1.f, 1.f);
	vertices[4].u = 1.f; vertices[4].v = 0.f;
	vertices[4].normal = vec3New(0.f, 1.f, 0.f);
	vertices[4].bIDs[0] = 0; vertices[4].bIDs[1] = -1; vertices[4].bIDs[2] = -1; vertices[4].bIDs[3] = -1;
	vertices[4].bWeights[0] = 1.f; vertices[4].bWeights[1] = 0.f; vertices[4].bWeights[2] = 0.f; vertices[4].bWeights[3] = 0.f;
	vertices[5].position = vec3New(1.f, 1.f, -1.f);
	vertices[5].u = 1.f; vertices[5].v = -1.f;
	vertices[5].normal = vec3New(0.f, 1.f, 0.f);
	vertices[5].bIDs[0] = 0; vertices[5].bIDs[1] = -1; vertices[5].bIDs[2] = -1; vertices[5].bIDs[3] = -1;
	vertices[5].bWeights[0] = 1.f; vertices[5].bWeights[1] = 0.f; vertices[5].bWeights[2] = 0.f; vertices[5].bWeights[3] = 0.f;
	vertices[6].position = vec3New(-1.f, 1.f, 1.f);
	vertices[6].u = 0.f; vertices[6].v = 0.f;
	vertices[6].normal = vec3New(0.f, 1.f, 0.f);
	vertices[6].bIDs[0] = 0; vertices[6].bIDs[1] = -1; vertices[6].bIDs[2] = -1; vertices[6].bIDs[3] = -1;
	vertices[6].bWeights[0] = 1.f; vertices[6].bWeights[1] = 0.f; vertices[6].bWeights[2] = 0.f; vertices[6].bWeights[3] = 0.f;
	vertices[7].position = vec3New(-1.f, 1.f, -1.f);
	vertices[7].u = 0.f; vertices[7].v = -1.f;
	vertices[7].normal = vec3New(0.f, 1.f, 0.f);
	vertices[7].bIDs[0] = 0; vertices[7].bIDs[1] = -1; vertices[7].bIDs[2] = -1; vertices[7].bIDs[3] = -1;
	vertices[7].bWeights[0] = 1.f; vertices[7].bWeights[1] = 0.f; vertices[7].bWeights[2] = 0.f; vertices[7].bWeights[3] = 0.f;
	vertices[8].position = vec3New(-1.f, 1.f, 1.f);
	vertices[8].u = 1.f; vertices[8].v = -1.f;
	vertices[8].normal = vec3New(1.f, 0.f, 0.f);
	vertices[8].bIDs[0] = 0; vertices[8].bIDs[1] = -1; vertices[8].bIDs[2] = -1; vertices[8].bIDs[3] = -1;
	vertices[8].bWeights[0] = 1.f; vertices[8].bWeights[1] = 0.f; vertices[8].bWeights[2] = 0.f; vertices[8].bWeights[3] = 0.f;
	vertices[9].position = vec3New(-1.f, 1.f, -1.f);
	vertices[9].u = 0.f; vertices[9].v = -1.f;
	vertices[9].normal = vec3New(1.f, 0.f, 0.f);
	vertices[9].bIDs[0] = 0; vertices[9].bIDs[1] = -1; vertices[9].bIDs[2] = -1; vertices[9].bIDs[3] = -1;
	vertices[9].bWeights[0] = 1.f; vertices[9].bWeights[1] = 0.f; vertices[9].bWeights[2] = 0.f; vertices[9].bWeights[3] = 0.f;
	vertices[10].position = vec3New(-1.f, -1.f, 1.f);
	vertices[10].u = 1.f; vertices[10].v = 0.f;
	vertices[10].normal = vec3New(1.f, 0.f, 0.f);
	vertices[10].bIDs[0] = 0; vertices[10].bIDs[1] = -1; vertices[10].bIDs[2] = -1;
	vertices[10].bIDs[3] = -1;
	vertices[10].bWeights[0] = 1.f; vertices[10].bWeights[1] = 0.f; vertices[10].bWeights[2] = 0.f; vertices[10].bWeights[3] = 0.f;
	vertices[11].position = vec3New(-1.f, -1.f, -1.f);
	vertices[11].u = 0.f; vertices[11].v = 0.f;
	vertices[11].normal = vec3New(1.f, 0.f, 0.f);
	vertices[11].bIDs[0] = 0; vertices[11].bIDs[1] = -1; vertices[11].bIDs[2] = -1;
	vertices[11].bIDs[3] = -1;
	vertices[11].bWeights[0] = 1.f; vertices[11].bWeights[1] = 0.f; vertices[11].bWeights[2] = 0.f; vertices[11].bWeights[3] = 0.f;
	vertices[12].position = vec3New(-1.f, -1.f, 1.f);
	vertices[12].u = 0.f; vertices[12].v = -1.f;
	vertices[12].normal = vec3New(0.f, 0.f, 1.f);
	vertices[12].bIDs[0] = 0; vertices[12].bIDs[1] = -1; vertices[12].bIDs[2] = -1;
	vertices[12].bIDs[3] = -1;
	vertices[12].bWeights[0] = 1.f; vertices[12].bWeights[1] = 0.f; vertices[12].bWeights[2] = 0.f; vertices[12].bWeights[3] = 0.f;
	vertices[13].position = vec3New(-1.f, -1.f, -1.f);
	vertices[13].u = 0.f; vertices[13].v = 0.f;
	vertices[13].normal = vec3New(0.f, 0.f, 1.f);
	vertices[13].bIDs[0] = 0; vertices[13].bIDs[1] = -1; vertices[13].bIDs[2] = -1;
	vertices[13].bIDs[3] = -1;
	vertices[13].bWeights[0] = 1.f; vertices[13].bWeights[1] = 0.f; vertices[13].bWeights[2] = 0.f; vertices[13].bWeights[3] = 0.f;
	vertices[14].position = vec3New(1.f, -1.f, 1.f);
	vertices[14].u = 1.f; vertices[14].v = -1.f;
	vertices[14].normal = vec3New(0.f, 0.f, 1.f);
	vertices[14].bIDs[0] = 0; vertices[14].bIDs[1] = -1; vertices[14].bIDs[2] = -1;
	vertices[14].bIDs[3] = -1;
	vertices[14].bWeights[0] = 1.f; vertices[14].bWeights[1] = 0.f; vertices[14].bWeights[2] = 0.f; vertices[14].bWeights[3] = 0.f;
	vertices[15].position = vec3New(1.f, -1.f, -1.f);
	vertices[15].u = 1.f; vertices[15].v = 0.f;
	vertices[15].normal = vec3New(0.f, 0.f, 1.f);
	vertices[15].bIDs[0] = 0; vertices[15].bIDs[1] = -1; vertices[15].bIDs[2] = -1;
	vertices[15].bIDs[3] = -1;
	vertices[15].bWeights[0] = 1.f; vertices[15].bWeights[1] = 0.f; vertices[15].bWeights[2] = 0.f; vertices[15].bWeights[3] = 0.f;
	vertices[16].position = vec3New(-1.f, -1.f, 1.f);
	vertices[16].u = 0.f; vertices[16].v = 0.f;
	vertices[16].normal = vec3New(-1.f, 0.f, 0.f);
	vertices[16].bIDs[0] = 0; vertices[16].bIDs[1] = -1; vertices[16].bIDs[2] = -1;
	vertices[16].bIDs[3] = -1;
	vertices[16].bWeights[0] = 1.f; vertices[16].bWeights[1] = 0.f; vertices[16].bWeights[2] = 0.f; vertices[16].bWeights[3] = 0.f;
	vertices[17].position = vec3New(1.f, -1.f, 1.f);
	vertices[17].u = 1.f; vertices[17].v = 0.f;
	vertices[17].normal = vec3New(-1.f, 0.f, 0.f);
	vertices[17].bIDs[0] = 0; vertices[17].bIDs[1] = -1; vertices[17].bIDs[2] = -1;
	vertices[17].bIDs[3] = -1;
	vertices[17].bWeights[0] = 1.f; vertices[17].bWeights[1] = 0.f; vertices[17].bWeights[2] = 0.f; vertices[17].bWeights[3] = 0.f;
	vertices[18].position = vec3New(-1.f, 1.f, 1.f);
	vertices[18].u = 0.f; vertices[18].v = -1.f;
	vertices[18].normal = vec3New(-1.f, 0.f, 0.f);
	vertices[18].bIDs[0] = 0; vertices[18].bIDs[1] = -1; vertices[18].bIDs[2] = -1;
	vertices[18].bIDs[3] = -1;
	vertices[18].bWeights[0] = 1.f; vertices[18].bWeights[1] = 0.f; vertices[18].bWeights[2] = 0.f; vertices[18].bWeights[3] = 0.f;
	vertices[19].position = vec3New(1.f, 1.f, 1.f);
	vertices[19].u = 1.f; vertices[19].v = -1.f;
	vertices[19].normal = vec3New(-1.f, 0.f, 0.f);
	vertices[19].bIDs[0] = 0; vertices[19].bIDs[1] = -1; vertices[19].bIDs[2] = -1;
	vertices[19].bIDs[3] = -1;
	vertices[19].bWeights[0] = 1.f; vertices[19].bWeights[1] = 0.f; vertices[19].bWeights[2] = 0.f; vertices[19].bWeights[3] = 0.f;
	vertices[20].position = vec3New(1.f, -1.f, -1.f);
	vertices[20].u = 0.f; vertices[20].v = 0.f;
	vertices[20].normal = vec3New(0.f, 0.f, -1.f);
	vertices[20].bIDs[0] = 0; vertices[20].bIDs[1] = -1; vertices[20].bIDs[2] = -1;
	vertices[20].bIDs[3] = -1;
	vertices[20].bWeights[0] = 1.f; vertices[20].bWeights[1] = 0.f; vertices[20].bWeights[2] = 0.f; vertices[20].bWeights[3] = 0.f;
	vertices[21].position = vec3New(-1.f, -1.f, -1.f);
	vertices[21].u = 1.f; vertices[21].v = 0.f;
	vertices[21].normal = vec3New(0.f, 0.f, -1.f);
	vertices[21].bIDs[0] = 0; vertices[21].bIDs[1] = -1; vertices[21].bIDs[2] = -1;
	vertices[21].bIDs[3] = -1;
	vertices[21].bWeights[0] = 1.f; vertices[21].bWeights[1] = 0.f; vertices[21].bWeights[2] = 0.f; vertices[21].bWeights[3] = 0.f;
	vertices[22].position = vec3New(1.f, 1.f, -1.f);
	vertices[22].u = 0.f; vertices[22].v = -1.f;
	vertices[22].normal = vec3New(0.f, 0.f, -1.f);
	vertices[22].bIDs[0] = 0; vertices[22].bIDs[1] = -1; vertices[22].bIDs[2] = -1;
	vertices[22].bIDs[3] = -1;
	vertices[22].bWeights[0] = 1.f; vertices[22].bWeights[1] = 0.f; vertices[22].bWeights[2] = 0.f; vertices[22].bWeights[3] = 0.f;
	vertices[23].position = vec3New(-1.f, 1.f, -1.f);
	vertices[23].u = 1.f; vertices[23].v = -1.f;
	vertices[23].normal = vec3New(0.f, 0.f, -1.f);
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

	if(meshGenerateBuffers(&meshDefault, 24, vertices, 36, indices, NULL) <= 0){
		return 0;
	}

	return 1;

}

/** Change this function later **/
return_t meshSpriteInit(){

	/**GLenum glError;

	// Create and bind the VAO
	glGenVertexArrays(1, &mdlSprite.vaoID);
	glBindVertexArray(mdlSprite.vaoID);

	// Create and bind the VBO
	glGenBuffers(1, &mdlSprite.vboID);
	glBindBuffer(GL_ARRAY_BUFFER, mdlSprite.vboID);
	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error creating vertex buffer: %u\n", glError);
		return 0;
	}

	** Should sprites use IBOs? Probably, but they're not working at the moment **
	// Create and bind the IBO
	*glGenBuffers(1, &mdlSprite.iboID);
	glBindBuffer(GL_ARRAY_BUFFER, mdlSprite.iboID);
	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error creating index buffer: %u\n", glError);
		return 0;
	}*

	mdlVertexAttributes();
	glBindVertexArray(0);

	// Check for errors
	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error creating vertex array buffer: %u\n", glError);
	}

	mdlSprite.vertexNum = 4;
	mdlSprite.indexNum = 6;

	return 1;**/

	vertex vertices[4];
	vertexIndex_t indices[6];

	vertices[0].position = vec3New(-0.5f, 0.5f, 0.f);
	vertices[0].u = 0.f; vertices[0].v = -1.f;
	vertices[0].normal = vec3New(0.f, 1.f, 0.f);
	vertices[0].bIDs[0] = 0; vertices[0].bIDs[1] = -1; vertices[0].bIDs[2] = -1; vertices[0].bIDs[3] = -1;
	vertices[0].bWeights[0] = 1.f; vertices[0].bWeights[1] = 0.f; vertices[0].bWeights[2] = 0.f; vertices[0].bWeights[3] = 0.f;
	vertices[1].position = vec3New(0.5f, 0.5f, 0.f);
	vertices[1].u = 1.f; vertices[1].v = -1.f;
	vertices[1].normal = vec3New(0.f, 1.f, 0.f);
	vertices[1].bIDs[0] = 0; vertices[1].bIDs[1] = -1; vertices[1].bIDs[2] = -1; vertices[1].bIDs[3] = -1;
	vertices[1].bWeights[0] = 1.f; vertices[1].bWeights[1] = 0.f; vertices[1].bWeights[2] = 0.f; vertices[1].bWeights[3] = 0.f;
	vertices[2].position = vec3New(-0.5f, -0.5f, 0.f);
	vertices[2].u = 0.f; vertices[2].v = 0.f;
	vertices[2].normal = vec3New(0.f, 1.f, 0.f);
	vertices[2].bIDs[0] = 0; vertices[2].bIDs[1] = -1; vertices[2].bIDs[2] = -1; vertices[2].bIDs[3] = -1;
	vertices[2].bWeights[0] = 1.f; vertices[2].bWeights[1] = 0.f; vertices[2].bWeights[2] = 0.f; vertices[2].bWeights[3] = 0.f;
	vertices[3].position = vec3New(0.5f, -0.5f, 0.f);
	vertices[3].u = 1.f; vertices[3].v = 0.f;
	vertices[3].normal = vec3New(0.f, 1.f, 0.f);
	vertices[3].bIDs[0] = 0; vertices[3].bIDs[1] = -1; vertices[3].bIDs[2] = -1; vertices[3].bIDs[3] = -1;
	vertices[3].bWeights[0] = 1.f; vertices[3].bWeights[1] = 0.f; vertices[3].bWeights[2] = 0.f; vertices[3].bWeights[3] = 0.f;

	indices[0] = 2;
	indices[1] = 1;
	indices[2] = 0;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 1;

	if(meshGenerateBuffers(&meshSprite, 4, vertices, 6, indices, NULL) <= 0){
		return 0;
	}

	return 1;

}

void meshDelete(mesh *const restrict m){
	if(m->vaoID != 0){
		glDeleteBuffers(1, &m->vaoID);
	}
	if(m->vboID != 0){
		glDeleteBuffers(1, &m->vboID);
	}
	if(m->iboID != 0){
		glDeleteBuffers(1, &m->iboID);
	}
}