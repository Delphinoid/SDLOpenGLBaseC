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

void gfxViewInit(gfxView *v);
void gfxViewReset(gfxView *v);
void gfxViewSet(gfxView *v, const float x, const float y, const float width, const float height);

#endif
