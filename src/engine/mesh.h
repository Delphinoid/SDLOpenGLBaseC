#ifndef MESH_H
#define MESH_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "return.h"
#include "qualifiers.h"

typedef GLsizei vertexIndex_t;

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
	///vertexIndex_t vertexNum;
	vertexIndex_t indexNum;

} mesh;

extern mesh g_meshDefault;
extern mesh g_meshSprite;
extern mesh g_meshBillboard;

void meshInit(mesh *const __RESTRICT__ m);
return_t meshDefaultInit();
return_t meshBillboardInit();
return_t meshGenerateBuffers(mesh *const __RESTRICT__ m, const vertex *const __RESTRICT__ vertices, const vertexIndex_t vertexNum, const vertexIndex_t *const __RESTRICT__ indices, const vertexIndex_t indexNum);
///void meshFindCurrentLOD(const mesh *const __RESTRICT__ m, vertexIndex_t *const __RESTRICT__ indexNum, const void **const __RESTRICT__ offset, const float distance, size_t bias);
void meshDelete(mesh *const __RESTRICT__ m);

#endif
