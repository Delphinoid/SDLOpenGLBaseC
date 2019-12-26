#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "renderableState.h"
#include "textureWrapper.h"
#include "billboard.h"

typedef struct model model;
typedef struct skeleton skeleton;
typedef struct camera camera;
typedef struct graphicsManager graphicsManager;

/** Add default state data to renderable base. **/
typedef struct renderableBase {
	model *mdl;          // A pointer to the renderable's associated model.
	textureWrapper *tw;  // A pointer to the renderable's associated texture wrapper.
} renderableBase;

typedef struct renderable {
	model *mdl;
	twInstance twi;
	renderableState state;
	billboard billboardData;
} renderable;

void rndrBaseInit(renderableBase *const restrict base);

void rndrInit(renderable *const restrict rndr);
void rndrInstantiate(renderable *const restrict rndr, const renderableBase *const base);
void rndrTick(renderable *const restrict rndr, const float elapsedTime);
float rndrAlpha(const renderable *const restrict rndr, const float interpT);
void rndrRender(const renderable *const restrict rndr, const skeleton *const restrict skl, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const vec3 centroid, const float interpT);

#endif
