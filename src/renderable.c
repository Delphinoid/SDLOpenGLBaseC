#include "renderable.h"
#include "helpersMath.h"
#include "inline.h"

void rndrBaseInit(renderableBase *const restrict rndr){
	rndr->mdl = NULL;
	rndr->tw = NULL;
}

void rndrStateInit(rndrState *const restrict state){
	state->alpha = 1.f;
	state->alphaCurrent = 1.f;
	state->alphaPrevious = 1.f;
}

void rndrInit(renderable *const restrict rndr){
	rndr->mdl = NULL;
	twiInit(&rndr->twi, NULL);
	rndrStateInit(&rndr->stateData);
	billboardInit(&rndr->billboardData);
}

void rndrInstantiate(renderable *const restrict rndr, const renderableBase *const base){
	rndr->mdl = base->mdl;
	twiInit(&rndr->twi, base->tw);
	rndrStateInit(&rndr->stateData);
	billboardInit(&rndr->billboardData);
}

__FORCE_INLINE__ void rndrTick(renderable *const restrict rndr, const float elapsedTime){
	twiTick(&rndr->twi, elapsedTime);
	rndr->stateData.alphaPrevious = rndr->stateData.alphaCurrent;
	rndr->stateData.alphaCurrent = rndr->stateData.alpha;
}

__FORCE_INLINE__ float rndrAlpha(const renderable *const restrict rndr, const float interpT){
	return floatLerp(rndr->stateData.alphaPrevious, rndr->stateData.alpha, interpT);
}