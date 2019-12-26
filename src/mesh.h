#ifndef MESH_H
#define MESH_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "return.h"

typedef GLsizei vertexIndex_t;
typedef GLsizei vertexIndexNum_t;

typedef struct vertex vertex;

typedef struct {
	// Vertex array object ID.
	GLuint vaoID;
	// Vertex buffer object ID.
	GLuint vboID;
	// Index buffer object ID.
	GLuint iboID;
	vertexIndex_t vertexNum;
	vertexIndexNum_t indexNum;
} mesh;

extern mesh meshDefault;
extern mesh meshSprite;

return_t meshDefaultInit();
return_t meshSpriteInit();
return_t meshGenerateBuffers(mesh *const restrict m, const vertexIndex_t vertexNum, const vertex *const restrict vertices, const vertexIndexNum_t indexNum, const vertexIndexNum_t *const restrict indices, const char *const restrict filePath);
void meshDelete(mesh *const restrict m);

#endif