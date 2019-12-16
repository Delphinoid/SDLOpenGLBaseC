#ifndef GUI_H
#define GUI_H

#include "renderable.h"
#include "rectangle.h"
#include "bone.h"
#include "vec2.h"

#define GUI_WINDOW_STRETCH_BODY 0x01

// If no type is defined, defaults to renderable.
#define GUI_ELEMENT_TYPE_WINDOW 0x01
#define GUI_ELEMENT_TYPE_TEXT   0x02
#define GUI_ELEMENT_TYPE_OBJECT 0x04
#define GUI_ELEMENT_TYPE_MASK   0x07

typedef struct object object;
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

/*typedef struct {
	// Panel renderable.
	renderable rndr;
	// Offsets for the four corners.
	// We can inherit other properties from the root.
	vec2 configuration[4];
} guiPanel;*/

/**typedef struct {
	char *text;
	rectangle bounds;
} guiText;**/

typedef struct guiElement guiElement;
typedef struct guiElement {

	// Parent pointer and a linked list of children.
	guiElement *children;
	guiElement *parent;

	// Render variables.
	union {
		renderable rndr;
		guiWindow window;
		///guiText text;
		object *obj;
	} data;
	bone root;

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

	flags_t flags;

} guiElement;

void guiElementTick(guiElement *const restrict element, const float elapsedTime);
guiElement *guiElementAddChild(guiElement *const restrict element);
void guiElementRender(const guiElement *const restrict element, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT);
void guiElementDelete(guiElement *element);

#endif
