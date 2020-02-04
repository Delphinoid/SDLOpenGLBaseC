#include "renderableState.h"
#include "renderableSettings.h"
#include "helpersMath.h"

void rndrStateInit(renderableState *const __RESTRICT__ state){
	state->alpha = 1.f;
	state->alphaCurrent = 1.f;
	state->alphaPrevious = 1.f;
	state->flags = RENDERABLE_DEFAULT_ALPHA_MODE;
}

__FORCE_INLINE__ float rndrStateAlpha(const renderableState *const __RESTRICT__ state, const float interpT){
	return floatLerp(state->alphaPrevious, state->alpha, interpT);
}