#ifndef GUITEXT_H
#define GUITEXT_H

#include "text.h"

typedef struct guiElement guiElement;
typedef struct camera camera;
typedef struct graphicsManager graphicsManager;

typedef struct {
	// Text stream.
	txtStream stream;
	// Text box boundaries.
	rectangle bounds;
	// How far down the text has scrolled.
	float scroll;
	flags_t flags;
} guiText;

void guiElementRenderText(const guiElement *const __RESTRICT__ element, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float distance, const float interpT);

#endif
