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
	void (*cursorEnter)();
	void (*cursorExit)();
	void (*mouseClick)(const int button);
	void (*mouseDoubleClick)(const int button);
	void (*mouseScroll)(const int steps);
	void (*mouseRelease)(const int button);
	void (*keyboardInput)(const int key);

	flags_t flags;

} guiElement;

typedef struct {

	// Tree of GUI elements.
	guiElement root;

	// Linked list of renderables.
	// These will be things like window / panel templates,
	// graphics for interactive elements and so on.
	renderableBase *renderables;

} gui;

#endif
