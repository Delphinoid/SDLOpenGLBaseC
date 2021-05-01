#include "modelState.h"
#include "modelSettings.h"
#include "helpersMath.h"

void mdlStateInit(mdlState *const __RESTRICT__ state){
	state->alpha = 1.f;
	state->alphaCurrent = 1.f;
	state->alphaPrevious = 1.f;
	state->flags = MODEL_DEFAULT_ALPHA_MODE;
}

__FORCE_INLINE__ float mdlStateAlpha(const mdlState *const __RESTRICT__ state, const float interpT){
	return floatLerp(state->alphaPrevious, state->alpha, interpT);
}

__FORCE_INLINE__ float mdlStateAlphaRender(const mdlState *const __RESTRICT__ state, const float interpT){
	if(flagsAreSet(state->flags, MODEL_STATE_ALPHA_DITHER)){
		// Negative alpha values indicate dithering.
		return -floatLerp(state->alphaPrevious, state->alpha, interpT);
	}
	return floatLerp(state->alphaPrevious, state->alpha, interpT);
}