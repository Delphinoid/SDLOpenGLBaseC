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

void rndrBaseInit(renderableBase *const __RESTRICT__ base);

void rndrInit(renderable *const __RESTRICT__ rndr);
void rndrInstantiate(renderable *const __RESTRICT__ rndr, const renderableBase *const base);
void rndrTick(renderable *const __RESTRICT__ rndr, const float elapsedTime);
float rndrAlpha(const renderable *const __RESTRICT__ rndr, const float interpT);
void rndrRender(const renderable *const __RESTRICT__ rndr, const skeleton *const __RESTRICT__ skl, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float distance, const vec3 centroid, const float interpT);

#endif
