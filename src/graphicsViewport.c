#include "graphicsViewport.h"
#include <string.h>

__FORCE_INLINE__ void gfxViewInit(gfxView *const __RESTRICT__ v){
	v->x = 0.f;
	v->y = 0.f;
	v->width = 1.f;
	v->height = 1.f;
}

__FORCE_INLINE__ void gfxViewReset(gfxView *const __RESTRICT__ v){
	memset(v, 0, sizeof(gfxView));
}

__FORCE_INLINE__ void gfxViewSet(gfxView *const __RESTRICT__ v, const float x, const float y, const float width, const float height){
	v->x = x;
	v->y = y;
	v->width = width;
	v->height = height;
}