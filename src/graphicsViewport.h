#ifndef GRAPHICSVIEWPORT_H
#define GRAPHICSVIEWPORT_H

#include "qualifiers.h"

typedef struct {
	int x;
	int y;
	unsigned int width;
	unsigned int height;
} gfxViewport;

typedef struct {
	float x;
	float y;
	float width;
	float height;
} gfxView;

void gfxViewInit(gfxView *const __RESTRICT__ v);
void gfxViewReset(gfxView *const __RESTRICT__ v);
void gfxViewSet(gfxView *const __RESTRICT__ v, const float x, const float y, const float width, const float height);

#endif
