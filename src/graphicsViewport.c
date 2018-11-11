#include "graphicsViewport.h"

void gfxViewInit(gfxView *v){
	v->x = 0.f;
	v->y = 0.f;
	v->width = 1.f;
	v->height = 1.f;
}

void gfxViewReset(gfxView *v){
	v->x = 0.f;
	v->y = 0.f;
	v->width = 0.f;
	v->height = 0.f;
}

void gfxViewSet(gfxView *v, const float x, const float y, const float width, const float height){
	v->x = x;
	v->y = y;
	v->width = width;
	v->height = height;
}