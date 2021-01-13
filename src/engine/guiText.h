#ifndef GUITEXT_H
#define GUITEXT_H

#include "text.h"

typedef struct guiElement guiElement;
typedef struct camera camera;
typedef struct graphicsManager graphicsManager;

typedef struct {
	// Text stream.
	txtStream stream;
	txtFormat format;
	// Text box dimensions on the screen.
	float width;
	float height;
} guiText;

void guiTextTick(guiElement *const element, const float dt_ms);
void guiTextRender(const guiElement *const element, graphicsManager *const gfxMngr, const camera *const cam, const float distance, const float interpT);

#endif
