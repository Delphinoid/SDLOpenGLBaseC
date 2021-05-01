#ifndef GUI_H
#define GUI_H

#include "guiText.h"
#include "guiWindow.h"
#include "guiContainer.h"
#include "model.h"
#include "text.h"
#include "transform.h"
#include "vec2.h"

#define GUI_TEXT_WORD_WRAP 0x01

// If no type is defined, defaults to controller.
#define GUI_ELEMENT_TYPE_CONTROLLER 0x00
#define GUI_ELEMENT_TYPE_TEXT       0x01
#define GUI_ELEMENT_TYPE_WINDOW     0x02
#define GUI_ELEMENT_TYPE_CONTAINER  0x03
#define GUI_ELEMENT_TYPE_RENDERABLE 0x04
#define GUI_ELEMENT_TYPE_OBJECT     0x05

typedef struct object object;
typedef struct graphicsManager graphicsManager;

typedef struct guiElement {

	// Parent pointer and a linked list of children.
	guiElement *children;
	guiElement *parent;

	// Render variables.
	union {
		guiText text;
		guiWindow window;
		guiContainer container;
		model mdl;
		object *obj;  /// Replace with struct containing void pointer and function pointers?
	} data;
	transform root;

	// Called once per tick. Handles all events.
	///void (*tick)(guiElement *const element);

	// Various function pointers.
	/**void (*cursorEnter)(guiElement *const element);
	void (*cursorExit)(guiElement *const element);
	void (*mouseClick)(guiElement *const element, const int button);
	///void (*mouseDoubleClick)(guiElement *const element, const int button);
	void (*mouseDown)(guiElement *const element, const int button);
	void (*mouseRelease)(guiElement *const element, const int button);
	void (*mouseScroll)(guiElement *const element, const int steps);
	void (*keyboardPress)(guiElement *const element, const int key);
	void (*keyboardDown)(guiElement *const element, const int key);
	void (*keyboardRelease)(guiElement *const element, const int key);**/

	flags_t type;

} guiElement;

void guiInit(guiElement *const __RESTRICT__ element, const flags_t type);
guiElement *guiNewChild(guiElement *const element);
void guiTick(guiElement *const element, const float dt_ms);
void guiRender(const guiElement *const element, graphicsManager *const gfxMngr, const camera *const cam, const float distance, const float interpT);
void guiDelete(guiElement *element);

#endif
