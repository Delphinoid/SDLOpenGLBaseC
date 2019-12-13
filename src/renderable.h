#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "textureWrapper.h"
#include "billboard.h"

typedef struct model model;

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

#endif
