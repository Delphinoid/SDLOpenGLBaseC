#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "textureWrapper.h"
#include "billboard.h"

#define RENDERABLE_ALPHA_BLEND  0x00
#define RENDERABLE_ALPHA_DITHER 0x01

typedef struct model model;
typedef struct skeleton skeleton;
typedef struct camera camera;
typedef struct graphicsManager graphicsManager;

/** Add default state data to renderable base. **/
typedef struct renderableBase {
	model *mdl;          // A pointer to the renderable's associated model.
	textureWrapper *tw;  // A pointer to the renderable's associated texture wrapper.
} renderableBase;

typedef struct {

	// Render data.
	float alpha;          // Updated alpha.
	/** Is this really worth it? **/
	float alphaCurrent;   // Current alpha for rendering.
	float alphaPrevious;  // Previous alpha for rendering.
	flags_t flags;

} rndrState;

typedef struct renderable {
	model *mdl;
	twInstance twi;
	rndrState stateData;
	billboard billboardData;
} renderable;

void rndrBaseInit(renderableBase *const restrict base);

void rndrInit(renderable *const restrict rndr);
void rndrInstantiate(renderable *const restrict rndr, const renderableBase *const base);
void rndrTick(renderable *const restrict rndr, const float elapsedTime);
float rndrAlpha(const renderable *const restrict rndr, const float interpT);
void rndrRender(const renderable *const restrict rndr, const skeleton *const restrict skl, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const vec3 centroid, const float interpT);

#endif
