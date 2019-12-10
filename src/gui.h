#ifndef GUI_H
#define GUI_H

#include "renderable.h"

#define GUI_ELEMENT_HIDDEN 0x01  // The element is hidden.

typedef uint_least8_t guiElementIndex_t;

typedef struct guiElement guiElement;
typedef struct guiElement {

	// Parent pointer and a linked list of children.
	guiElement *children;
	guiElement *parent;

	// Render variables.
	bone configuration;
	renderable rndr;

	// Various function pointers.
	void (*cursorEnter)(guiElement *const element);
	void (*cursorExit)(guiElement *const element);
	void (*mouseClick)(guiElement *const element, const int button);
	///void (*mouseDoubleClick)(guiElement *const element, const int button);
	void (*mouseRelease)(guiElement *const element, const int button);
	void (*mouseScroll)(guiElement *const element, const int steps);
	void (*keyboardInput)(guiElement *const element, const int key);

	flags_t flags;

} guiElement;

void guiElementTick(guiElement *const element)

#endif
