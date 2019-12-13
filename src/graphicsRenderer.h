#ifndef GRAPHICSRENDERER_H
#define GRAPHICSRENDERER_H

#include "graphicsManager.h"
#include <stdint.h>

#define GFX_RNDR_ELEMENT_TYPE_OBJECT      0

typedef struct camera camera;
typedef struct scene scene;

typedef struct {

	const void *structure;
	float distance;
	uint_least8_t type;
	///void (*render)(const void *const restrict, graphicsManager *const restrict, const camera *const restrict, const float, const float);

} gfxRenderElement;

return_t gfxRendererDrawScene(graphicsManager *const restrict gfxMngr, camera *const restrict cam, const scene *const restrict scn, const float interpT);

#endif
