#ifndef MODEL_H
#define MODEL_H

#include <SDL2/SDL_opengl.h>
#include "skeleton.h"

/** Models need to store a skeleton structure of their own **/
typedef struct {
	vec3  position;
	float u, v;
	vec3 normal;
	int   bIDs[4];
	float bWeights[4];
} vertex;

typedef struct {
	char *name;
	skeleton skl;
	size_t vertexNum;
	size_t indexNum;
	GLuint vaoID;  // Vertex array object ID
	GLuint vboID;  // Vertex buffer object ID
	GLuint iboID;  // Index buffer object ID
} model;

void vertInit(vertex *v);
void mdlInit(model *mdl);
unsigned char mdlLoad(model *mdl, const char *prgPath, const char *filePath);
unsigned char mdlLoadWavefrontObj(model *mdl, const char *prgPath, const char *filePath);
unsigned char mdlCreateSprite(model *mdl, const char *name);
void mdlDelete(model *mdl);

#endif
