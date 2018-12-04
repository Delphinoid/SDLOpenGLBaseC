#ifndef GRAPHICSRENDERER_H
#define GRAPHICSRENDERER_H

#include "graphicsManager.h"
#include "camera.h"
#include "scene.h"
#include <stdint.h>

#define GFX_RNDR_ELEMENT_TYPE_OBJECT 0

typedef struct {

	void *structure;
	uint_least8_t type;
	float distance;

} gfxRenderElement;

return_t gfxRendererDrawScene(graphicsManager *gfxMngr, camera *cam, const scene *scn, const float interpT);

#endif
