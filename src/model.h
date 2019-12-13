#ifndef MODEL_H
#define MODEL_H

#include <SDL2/SDL_opengl.h>
#include "return.h"

/** LODs? **/

typedef uint_least8_t lodNum_t;
typedef GLsizei vertexIndex_t;
typedef GLsizei vertexIndexNum_t;

typedef struct skeleton skeleton;
typedef struct vertex vertex;

typedef struct {
	float distance;
	const void *offset;  // Technically a uintptr_t, but stored as a void pointer for OpenGL.
	vertexIndexNum_t indexNum;
} mdlLOD;

typedef struct model {

	// Skeleton associated with the model.
	const skeleton *skl;

	// Model LODs.
	// The distance of the first LOD will be 0,
	// If there is only one LOD, the array will be NULL.
	lodNum_t lodNum;
	mdlLOD *lods;

	// Model data for rendering.
	vertexIndex_t vertexNum;
	vertexIndexNum_t indexNum;
	GLuint vaoID;  // Vertex array object ID.
	GLuint vboID;  // Vertex buffer object ID.
	GLuint iboID;  // Index buffer object ID.

	char *name;

} model;

extern model mdlDefault;
extern model mdlSprite;

void mdlInit(model *const restrict mdl);
return_t mdlLoad(model *const restrict mdl, const char *const restrict prgPath, const char *const restrict filePath);
return_t mdlGenerateBuffers(const vertexIndex_t vertexNum, const vertex *const restrict vertices, const vertexIndexNum_t indexNum, const vertexIndex_t *const restrict indices, GLuint *vaoID, GLuint *vboID, GLuint *iboID, const char *const restrict filePath);
return_t mdlDefaultInit();
return_t mdlSpriteInit();
void mdlFindCurrentLOD(const model *const restrict mdl, GLsizei *const restrict indexNum, const void **const restrict offset, const float distance, size_t bias);
void mdlDelete(model *const restrict mdl);

#endif
