#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

/** LODs? **/

typedef uint_least8_t lodNum_t;

typedef struct skeleton skeleton;

typedef struct {
	float distance;
	const void *offset;  // Technically a uintptr_t, but stored as a void pointer for OpenGL.
	vertexIndexNum_t indexNum;
} mdlLOD;

typedef struct model {

	// Model data for rendering.
	mesh buffers;

	// Skeleton associated with the model.
	const skeleton *skl;

	// Model LODs.
	// The distance of the first LOD will be 0,
	// If there is only one LOD, the array will be NULL.
	lodNum_t lodNum;
	mdlLOD *lods;

	char *name;

} model;

extern model mdlDefault;
extern model mdlSprite;

void mdlInit(model *const restrict mdl);
return_t mdlLoad(model *const restrict mdl, const char *const restrict prgPath, const char *const restrict filePath);
void mdlDefaultInit();
void mdlSpriteInit();
void mdlFindCurrentLOD(const model *const restrict mdl, vertexIndexNum_t *const restrict indexNum, const void **const restrict offset, const float distance, size_t bias);
void mdlDelete(model *const restrict mdl);

#endif
