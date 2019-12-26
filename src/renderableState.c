#include "renderableState.h"
#include "renderableSettings.h"
#include "helpersMath.h"
#include "inline.h"

void rndrStateInit(renderableState *const restrict state){
	state->alpha = 1.f;
	state->alphaCurrent = 1.f;
	state->alphaPrevious = 1.f;
	state->flags = RENDERABLE_DEFAULT_ALPHA_MODE;
}

__FORCE_INLINE__ float rndrStateAlpha(const renderableState *const restrict state, const float interpT){
	return floatLerp(state->alphaPrevious, state->alpha, interpT);
}