#ifndef MODEL_H
#define MODEL_H

#include <SDL2/SDL_opengl.h>
#include "vertex.h"
#include "skeleton.h"

typedef uint_least32_t vertexIndex_t;
typedef uint_least32_t vertexIndexNum_t;

typedef struct {

	char *name;

	/* Skeleton associated with the model. */
	skeleton *skl;

	/* Model data for rendering. */
	vertexIndex_t vertexNum;
	vertexIndexNum_t indexNum;
	GLuint vaoID;  // Vertex array object ID
	GLuint vboID;  // Vertex buffer object ID
	GLuint iboID;  // Index buffer object ID

} model;

void mdlInit(model *mdl);
return_t mdlLoad(model *mdl, const char *prgPath, const char *filePath);
return_t mdlDefault(model *mdl);
return_t mdlCreateSprite(model *mdl);
void mdlDelete(model *mdl);

#endif
