#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include "textureWrapper.h"
#include "flags.h"

typedef struct guiElement guiElement;
typedef struct camera camera;
typedef struct graphicsManager graphicsManager;

typedef struct {
	// Border element UV offsets for rendering.
	/// Move this to guiWindowBase.
	rectangle offsets[8];
	// Texture wrappers for the window body and border.
	twInstance body;
	twInstance border;
	flags_t flags;
} guiWindow;

void guiElementRenderWindow(const guiElement *const restrict element, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT);

#endif
