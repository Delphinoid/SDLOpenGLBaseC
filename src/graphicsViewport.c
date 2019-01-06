#include "graphicsViewport.h"
#include "inline.h"
#include <string.h>

__FORCE_INLINE__ void gfxViewInit(gfxView *const restrict v){
	v->x = 0.f;
	v->y = 0.f;
	v->width = 1.f;
	v->height = 1.f;
}

__FORCE_INLINE__ void gfxViewReset(gfxView *const restrict v){
	memset(v, 0, sizeof(gfxView));
}

__FORCE_INLINE__ void gfxViewSet(gfxView *const restrict v, const float x, const float y, const float width, const float height){
	v->x = x;
	v->y = y;
	v->width = width;
	v->height = height;
}