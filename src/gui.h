#ifndef GUI_H
#define GUI_H

#define GUI_ELEMENT_HIDDEN  0x01  // The element is hidden.
#define GUI_ELEMENT_FOCUSED 0x02  // The element is currently focused.
#define GUI_ELEMENT_ACTIVE  0x04  // The element is active; for example, a button has been pressed or a textbox is in use.

/*typedef uint_least8_t guiElementIndex_t;

typedef struct guiElement guiElement;
typedef struct guiElement {

	guiElement *parent;
	guiElement *children;

	renderable *rndr;
	vec2 position;
	vec2 scale;

	// Overwrites for animations and frames.
	// This is used for buttons, which we would
	// prefer to share a renderable.
	animIndex_t animOverwrite;
	frameIndex_t frameOverwrite;

	// Element type. May be useful for programming.
	guiElementIndex_t type;

	flags_t flags;

} guiElement;

typedef struct {

	// Tree of GUI elements.
	guiElement *elements;

	// Linked list of renderables.
	// These will be things like window / panel templates,
	// graphics for interactive elements and so on.
	renderable *renderables;

} gui;*/

#endif
