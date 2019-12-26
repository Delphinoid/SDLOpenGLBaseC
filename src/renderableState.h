#ifndef RENDERABLESTATE_H
#define RENDERABLESTATE_H

#include "flags.h"

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

void rndrStateInit(renderableState *const restrict state);
float rndrStateAlpha(const renderableState *const restrict state, const float interpT);

#endif
