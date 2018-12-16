#ifndef GRAPHICSVIEWPORT_H
#define GRAPHICSVIEWPORT_H

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

void gfxViewInit(gfxView *const restrict v);
void gfxViewReset(gfxView *const restrict v);
void gfxViewSet(gfxView *const restrict v, const float x, const float y, const float width, const float height);

#endif
