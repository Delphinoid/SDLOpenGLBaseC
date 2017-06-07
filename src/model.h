#ifndef MODEL_H
#define MODEL_H

#include <SDL2/SDL_opengl.h>
#include "vertex.h"

typedef struct model {
	char *name;
	//vertex *vertices;
	size_t vertexNum;
	//unsigned int *indices;
	size_t indexNum;
	//bone *bones;
	//size_t boneNum;
	GLuint vaoID;  // Vertex array object ID
	GLuint vboID;  // Vertex buffer object ID
	GLuint iboID;  // Index buffer object ID
} model;

void mdlInit(model *mdl);
unsigned char mdlLoadWavefrontObj(model *mdl, const char *prgPath, const char *filePath);
unsigned char mdlCreateSprite(model *mdl, char *name);
void mdlGenBufferObjects(model *mdl, vertex *vertices, size_t vertexNum, unsigned int *indices, size_t indexNum);
void mdlDelete(model *mdl);

#endif
