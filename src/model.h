#ifndef MODEL_H
#define MODEL_H

#include <SDL2/SDL_opengl.h>
#include "mesh.h"
#include "skeleton.h"

/** Models need to store a skeleton structure of their own (do they?). **/

typedef struct {
	char *name;
	skeleton *skl;
	size_t vertexNum;
	size_t indexNum;
	GLuint vaoID;  // Vertex array object ID
	GLuint vboID;  // Vertex buffer object ID
	GLuint iboID;  // Index buffer object ID
} model;

void mdlInit(model *mdl);
signed char mdlLoad(model *mdl, const char *prgPath, const char *filePath);
signed char mdlCreateSprite(model *mdl, const char *name);
void mdlDelete(model *mdl);

#endif
