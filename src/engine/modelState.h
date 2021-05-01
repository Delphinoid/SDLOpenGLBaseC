#ifndef MODELSTATE_H
#define MODELSTATE_H

#include "state.h"
#include "flags.h"
#include "qualifiers.h"

#define MODEL_STATE_ALPHA_BLEND  0x00
#define MODEL_STATE_ALPHA_DITHER 0x01

typedef struct {

	// Render data.
	float alpha;          // Updated alpha.
	/** Is this really worth it? **/
	float alphaCurrent;   // Current alpha for rendering.
	float alphaPrevious;  // Previous alpha for rendering.
	flags_t flags;

} mdlState;

void mdlStateInit(mdlState *const __RESTRICT__ state);
float mdlStateAlpha(const mdlState *const __RESTRICT__ state, const float interpT);
float mdlStateAlphaRender(const mdlState *const __RESTRICT__ state, const float interpT);

#endif
