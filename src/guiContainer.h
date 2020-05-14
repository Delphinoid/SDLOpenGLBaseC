#ifndef GUICONTAINER_H
#define GUICONTAINER_H

typedef struct {
	// Viewport dimensions.
	float width;
	float height;
	// Content boundaries.
	rectangle boundaries;
	// Scroll bar UV offsets for rendering.
	/// Move this to guiMaskBase?
	rectangle offsets[6];
	// Texture wrappers for scroll bars.
	twInstance body;
	twInstance border;
} guiContainer;

#endif
