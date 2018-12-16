#ifndef GRAPHICSRENDERER_H
#define GRAPHICSRENDERER_H

#include "graphicsManager.h"
#include "camera.h"
#include "scene.h"
#include <stdint.h>

#define GFX_RNDR_ELEMENT_TYPE_OBJECT 0

typedef struct {

	const void *structure;
	uint_least8_t type;
	float distance;

} gfxRenderElement;

return_t gfxRendererDrawScene(graphicsManager *const restrict gfxMngr, camera *const restrict cam, const scene *const restrict scn, const float interpT);

#endif
