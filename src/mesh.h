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
	// Total number of vertices for
	// every LOD, and the number of
	// indices for the default LOD.
	vertexIndex_t vertexNum;
	vertexIndexNum_t indexNum;
} mesh;

extern mesh meshDefault;
extern mesh meshSprite;
extern mesh meshBillboard;

return_t meshDefaultInit();
return_t meshBillboardInit();
return_t meshGenerateBuffers(mesh *const restrict m, const vertexIndex_t vertexNum, const vertex *const restrict vertices, const vertexIndexNum_t indexNum, const vertexIndexNum_t *const restrict indices);
void meshDelete(mesh *const restrict m);

#endif