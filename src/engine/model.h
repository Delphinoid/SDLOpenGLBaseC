#ifndef MODEL_H
#define MODEL_H

#include "modelState.h"
#include "mesh.h"
#include "textureWrapper.h"
#include "skeleton.h"
#include "billboard.h"

typedef uint_least8_t meshIndex_t;
typedef uint_least8_t lodIndex_t;

typedef struct camera camera;
typedef struct graphicsManager graphicsManager;

typedef struct {
	float distance;
	// Technically a uintptr_t, but stored
	// as a void pointer for OpenGL. This is
	// basically the offset of the indices
	// for the LOD in the index buffer.
	const void *offset;
	vertexIndex_t indexNum;
} mdlLOD;

typedef struct modelBase {

	// Skeleton associated with the model.
	const skeleton *skl;
	const textureWrapper *tw;

	/// Mesh and texture data for rendering.
	///
	/// Certain models may be composed
	/// of many distinct meshes.
	///
	/// This array is allocated to include
	/// the LOD and name arrays as well.
	///mesh *meshes;
	///const textureWrapper *textures;
	///meshIndex_t meshNum;
	mesh buffers;

	// Additional model LODs.
	// The distance of the first LOD will be 0.
	// If there is only one LOD, the array will be
	// NULL and lodNum will be 0.
	mdlLOD *lods;
	// Number of LODs per mesh.
	/// The total size of the array is meshNum*lodNum.
	lodIndex_t lodNum;

	char *name;

} modelBase;

typedef struct model {
	/// Array of twInstances, one for each mesh.
	const modelBase *base;
	///twInstance *twi;
	twInstance twi;
	mdlState state;
	billboard billboardData;
} model;

extern modelBase g_mdlDefault;
extern modelBase g_mdlSprite;
extern modelBase g_mdlBillboard;

void mdlBaseInit(modelBase *const __RESTRICT__ base);
return_t mdlBaseLoad(modelBase *const __RESTRICT__ base, const char *const __RESTRICT__ filePath, const size_t filePathLength);
void mdlBaseDelete(modelBase *const __RESTRICT__ base);

void mdlBaseDefaultInit();
void mdlBaseSpriteInit();
void mdlBaseBillboardInit();

void mdlInit(model *const __RESTRICT__ mdl);
void mdlInstantiate(model *const __RESTRICT__ mdl, const modelBase *const base);
void mdlTick(model *const __RESTRICT__ mdl, const float dt_ms);
float mdlAlpha(const model *const __RESTRICT__ mdl, const float interpT);
void mdlDelete(model *const __RESTRICT__ mdl);

void mdlFindCurrentLOD(const mdlLOD *lods, const size_t lodNum, vertexIndex_t *const __RESTRICT__ indexNum, const void **const __RESTRICT__ offset, const float distance, size_t bias);
void mdlRender(const model *const __RESTRICT__ mdl, const skeleton *const __RESTRICT__ skl, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float distance, const vec3 centroid, const float interpT);

#endif
