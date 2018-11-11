#ifndef GRAPHICSRENDERER_H
#define GRAPHICSRENDERER_H

#include "graphicsManager.h"
#include "camera.h"
#include "scene.h"
#include <stdint.h>

typedef struct {

	void *structure;
	uint_least8_t type;

} gfxRenderStructure;

return_t gfxRendererDrawScene(graphicsManager *gfxMngr, camera *cam, scene *scn, const float interpT);

#endif
