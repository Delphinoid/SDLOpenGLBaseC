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
	///void (*render)(const void *const __RESTRICT__, graphicsManager *const __RESTRICT__, const camera *const __RESTRICT__, const float, const float);

} gfxRenderElement;

return_t gfxRendererDrawScene(graphicsManager *const __RESTRICT__ gfxMngr, camera *const __RESTRICT__ cam, const scene *const __RESTRICT__ scn, const float interpT);

#endif
