#ifndef RENDERABLESTATE_H
#define RENDERABLESTATE_H

#include "flags.h"
#include "qualifiers.h"

#define RENDERABLE_STATE_ALPHA_BLEND  0x00
#define RENDERABLE_STATE_ALPHA_DITHER 0x01

typedef struct {

	// Render data.
	float alpha;          // Updated alpha.
	/** Is this really worth it? **/
	float alphaCurrent;   // Current alpha for rendering.
	float alphaPrevious;  // Previous alpha for rendering.
	flags_t flags;

} renderableState;

void rndrStateInit(renderableState *const __RESTRICT__ state);
float rndrStateAlpha(const renderableState *const __RESTRICT__ state, const float interpT);

#endif
