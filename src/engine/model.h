#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

/** LODs? **/

typedef uint_least8_t lodNum_t;

typedef struct skeleton skeleton;

typedef struct {
	float distance;
	const void *offset;  // Technically a uintptr_t, but stored as a void pointer for OpenGL.
	vertexIndex_t indexNum;
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

extern model g_mdlDefault;
extern model g_mdlSprite;
extern model g_mdlBillboard;

void mdlInit(model *const __RESTRICT__ mdl);
return_t mdlLoad(model *const __RESTRICT__ mdl, const char *const __RESTRICT__ filePath, const size_t filePathLength);
void mdlDefaultInit();
void mdlSpriteInit();
void mdlBillboardInit();
void mdlFindCurrentLOD(const model *const __RESTRICT__ mdl, vertexIndex_t *const __RESTRICT__ indexNum, const void **const __RESTRICT__ offset, const float distance, size_t bias);
void mdlDelete(model *const __RESTRICT__ mdl);

#endif