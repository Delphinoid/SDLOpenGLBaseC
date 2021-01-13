#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include "textureWrapper.h"
#include "flags.h"

#define GUI_WINDOW_STRETCH_BORDER 0x01
#define GUI_WINDOW_STRETCH_BODY   0x02

typedef struct guiElement guiElement;
typedef struct camera camera;
typedef struct graphicsManager graphicsManager;

typedef struct {
	// Border element UV offsets for rendering.
	/// Move this to guiWindowBase?
	rectangle offsets[8];
	// Texture wrappers for the window body and border.
	twInstance body;
	twInstance border;
	flags_t flags;
} guiWindow;

void guiWindowTick(guiElement *const element, const float dt_ms);
void guiWindowRender(const guiElement *const element, graphicsManager *const gfxMngr, const camera *const cam, const float distance, const float interpT);

#endif
