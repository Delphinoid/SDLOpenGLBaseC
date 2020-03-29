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
	float width;
	float height;
	// How far down the text has scrolled.
	float scroll;
	flags_t flags;
} guiText;

void guiTickText(guiElement *const element, const float elapsedTime);
void guiRenderText(const guiElement *const element, graphicsManager *const gfxMngr, const camera *const cam, const float distance, const float interpT);

#endif
