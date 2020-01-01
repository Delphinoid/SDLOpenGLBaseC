#define GLEW_STATIC
#include <GL/glew.h>
#include "sprite.h"
#include "spriteState.h"
#include "spriteSettings.h"
#include "vertex.h"
#include <stdio.h>
#include <string.h>

GLuint sprStateBufferID = 0;

typedef struct {
	vec3 position;
	float u, v;
} sprVertex;

// Default sprite.
mesh meshSprite = {
	.vertexNum = 0,
	.indexNum = 0,
	.vaoID = 0,
	.vboID = 0,
	.iboID = 0
};

return_t sprGenerateStateBuffer(){

	GLenum glError;

	// Create and bind the sprite state buffer object.
	glGenBuffers(1, &sprStateBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, sprStateBufferID);
	// Use buffer orphaning and write to the buffer before rendering.
	glBufferData(GL_ARRAY_BUFFER, SPRITE_STATE_BUFFER_SIZE*sizeof(spriteState), NULL, GL_STREAM_DRAW);

	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error generating sprite state buffer: %u\n", glError);
		return 0;
	}

	return 1;

}

void sprPackVertexBuffer(const vertexIndex_t vertexNum, vertex *vertices){
	// Tightly packs an array of vertices for sprites.
	const vertex *i = &vertices[1];
	sprVertex *j = &((sprVertex *)vertices)[1];
	const vertex *const vLast = &vertices[vertexNum];
	for(; i < vLast; ++i, ++j){
		memcpy(j, i, sizeof(sprVertex));
	}
}

static void sprVertexAttributes(){
	// Position offset.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sprVertex), (GLvoid*)offsetof(sprVertex, position));
	glEnableVertexAttribArray(0);
	// Base UV offset.
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(sprVertex), (GLvoid *)offsetof(sprVertex, u));
	glEnableVertexAttribArray(1);
}

static void sprStateAttributes(){
	// First transformation state column offset.
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, transformation.m[0]));
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);
	// Second transformation state column offset.
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, transformation.m[1]));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);
	// Third transformation state column offset.
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, transformation.m[2]));
	glEnableVertexAttribArray(4);
	glVertexAttribDivisor(4, 1);
	// Fourth transformation state column offset.
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, transformation.m[3]));
	glEnableVertexAttribArray(5);
	glVertexAttribDivisor(5, 1);
	// Animated UV offset.
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, frame));
	glEnableVertexAttribArray(6);
	glVertexAttribDivisor(6, 1);
}

return_t sprGenerateBuffers(mesh *const restrict spr, const vertexIndex_t vertexNum, const vertex *const restrict vertices, const vertexIndexNum_t indexNum, const vertexIndexNum_t *const restrict indices){

	if(vertexNum == 4){
		if(indexNum == 6){

			GLenum glError;

			// Create and bind the VAO.
			glGenVertexArrays(1, &spr->vaoID);
			glBindVertexArray(spr->vaoID);

			// Create and bind the VBO.
			glGenBuffers(1, &spr->vboID);
			glBindBuffer(GL_ARRAY_BUFFER, spr->vboID);
			glBufferData(GL_ARRAY_BUFFER, vertexNum*sizeof(sprVertex), vertices, GL_STATIC_DRAW);
			// Check for errors.
			glError = glGetError();
			if(glError != GL_NO_ERROR){
				printf("Error creating vertex buffer for sprite: %u\n", glError);
				return 0;
			}

			// Create and bind the IBO.
			glGenBuffers(1, &spr->iboID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spr->iboID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexNum*sizeof(vertexIndexNum_t), indices, GL_STATIC_DRAW);
			// Check for errors.
			glError = glGetError();
			if(glError != GL_NO_ERROR){
				printf("Error creating index buffer for sprite: %u\n", glError);
				return 0;
			}

			sprVertexAttributes();

			glBindBuffer(GL_ARRAY_BUFFER, sprStateBufferID);
			sprStateAttributes();

			// Check for errors.
			glError = glGetError();
			if(glError != GL_NO_ERROR){
				printf("Error creating vertex array buffer for sprite: %u\n", glError);
				return 0;
			}

		}else{
			printf("Error creating buffers for sprite: sprite does not have 6 indices.\n");
			return 0;
		}

	}else{
		printf("Error creating buffers for sprite: sprite does not have 4 vertices.\n");
		return 0;
	}

	spr->vertexNum = vertexNum;
	spr->indexNum = indexNum;
	return 1;

}

return_t sprDefaultInit(){

	sprVertex vertices[4];
	vertexIndex_t indices[6];

	vertices[0].position = vec3New(-0.5f, 0.5f, 0.f);
	vertices[0].u = 0.f; vertices[0].v = -1.f;
	/**vertices[0].normal = vec3New(0.f, 1.f, 0.f);
	vertices[0].bIDs[0] = 0; vertices[0].bIDs[1] = -1; vertices[0].bIDs[2] = -1; vertices[0].bIDs[3] = -1;
	vertices[0].bWeights[0] = 1.f; vertices[0].bWeights[1] = 0.f; vertices[0].bWeights[2] = 0.f; vertices[0].bWeights[3] = 0.f;**/
	vertices[1].position = vec3New(0.5f, 0.5f, 0.f);
	vertices[1].u = 1.f; vertices[1].v = -1.f;
	/**vertices[1].normal = vec3New(0.f, 1.f, 0.f);
	vertices[1].bIDs[0] = 0; vertices[1].bIDs[1] = -1; vertices[1].bIDs[2] = -1; vertices[1].bIDs[3] = -1;
	vertices[1].bWeights[0] = 1.f; vertices[1].bWeights[1] = 0.f; vertices[1].bWeights[2] = 0.f; vertices[1].bWeights[3] = 0.f;**/
	vertices[2].position = vec3New(-0.5f, -0.5f, 0.f);
	vertices[2].u = 0.f; vertices[2].v = 0.f;
	/**vertices[2].normal = vec3New(0.f, 1.f, 0.f);
	vertices[2].bIDs[0] = 0; vertices[2].bIDs[1] = -1; vertices[2].bIDs[2] = -1; vertices[2].bIDs[3] = -1;
	vertices[2].bWeights[0] = 1.f; vertices[2].bWeights[1] = 0.f; vertices[2].bWeights[2] = 0.f; vertices[2].bWeights[3] = 0.f;**/
	vertices[3].position = vec3New(0.5f, -0.5f, 0.f);
	vertices[3].u = 1.f; vertices[3].v = 0.f;
	/**vertices[3].normal = vec3New(0.f, 1.f, 0.f);
	vertices[3].bIDs[0] = 0; vertices[3].bIDs[1] = -1; vertices[3].bIDs[2] = -1; vertices[3].bIDs[3] = -1;
	vertices[3].bWeights[0] = 1.f; vertices[3].bWeights[1] = 0.f; vertices[3].bWeights[2] = 0.f; vertices[3].bWeights[3] = 0.f;**/

	indices[0] = 2;
	indices[1] = 1;
	indices[2] = 0;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 1;

	if(sprGenerateBuffers(&meshSprite, 4, (const vertex *const restrict)vertices, 6, indices) <= 0){
		return 0;
	}

	return 1;

}

void sprDeleteStateBuffer(){
	if(sprStateBufferID != 0){
		glDeleteBuffers(1, &sprStateBufferID);
	}
}