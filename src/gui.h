#ifndef GUI_H
#define GUI_H

#define GUI_ELEMENT_HIDDEN   0x01  // The element is hidden.
#define GUI_ELEMENT_FOCUSED  0x02  // The element is currently focused.
#define GUI_ELEMENT_HOVER    0x04  // The element is being hovered over.
#define GUI_ELEMENT_ACTIVATE 0x08  // The element is being activated, or clicked on.
#define GUI_ELEMENT_ACTIVE   0x10  // The element is active; for example, a button has been pressed or a textbox is in use.

typedef uint_least8_t guiElementIndex_t;

typedef struct guiElement guiElement;
typedef struct guiElement {

	// Parent pointer and a linked list of children.
	guiElement *children;
	guiElement *parent;



	flags_t flags;

} guiElement;

typedef struct {

	// Tree of GUI elements.
	guiElement root;

	// Linked list of renderables.
	// These will be things like window / panel templates,
	// graphics for interactive elements and so on.
	renderable *renderables;

} gui;

#endif
