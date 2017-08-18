#ifndef MODEL_H
#define MODEL_H

#include <SDL2/SDL_opengl.h>
#include "vec3.h"

/** Models need to store a skeleton structure of their own **/
typedef struct {
	vec3  pos;
	float u, v;
	float nx, ny, nz;
	int   bIDs[4];
	float bWeights[4];
} vertex;

typedef struct {
	char *name;
	size_t vertexNum;
	size_t indexNum;
	GLuint vaoID;  // Vertex array object ID
	GLuint vboID;  // Vertex buffer object ID
	GLuint iboID;  // Index buffer object ID
} model;

void vertInit(vertex *v);
void mdlInit(model *mdl);
unsigned char mdlLoadWavefrontObj(model *mdl, const char *prgPath, const char *filePath);
unsigned char mdlCreateSprite(model *mdl, char *name);
void mdlDelete(model *mdl);

#endif
