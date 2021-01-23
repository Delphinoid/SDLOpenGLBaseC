#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "command.h"

#define INPUT_MAX_KEYBOARD_KEYS 256
#define INPUT_MAX_MOUSE_BUTTONS 7

#define INPUT_MBUTTON0   0
#define INPUT_MBUTTON1   1
#define INPUT_MBUTTON2   2
#define INPUT_MBUTTON3   3
#define INPUT_MBUTTON4   4
#define INPUT_MWHEELDOWN 5
#define INPUT_MWHEELUP   6

typedef struct {
	// Button binding.
	int id;
	char *binding;
	size_t bindingLength;
} inputButtonBinding;

typedef struct {

	// Array of keyboard key bindings.
	inputButtonBinding kbKeys[INPUT_MAX_KEYBOARD_KEYS];

	// Array of mouse button bindings.
	inputButtonBinding mButtons[INPUT_MAX_MOUSE_BUTTONS];

	// Relative mouse offset since the last tick.
	int mx, my;

} inputManager;

void inMngrInit(inputManager *const __RESTRICT__ inMngr);
return_t inMngrKeyboardBind(inputManager *const __RESTRICT__ inMngr, const int id, const char *const binding, const size_t bindingLength);
void inMngrKeyboardUnbind(inputManager *const __RESTRICT__ inMngr, const int id);
return_t inMngrMouseBind(inputManager *const __RESTRICT__ inMngr, const int id, const char *const binding, const size_t bindingLength);
void inMngrMouseUnbind(inputManager *const __RESTRICT__ inMngr, const int id);
void inMngrDelete(inputManager *const __RESTRICT__ inMngr);

return_t inMngrTakeInput(inputManager *const __RESTRICT__ inMngr, cmdBuffer *const __RESTRICT__ cmdbuf);
void inMngrMouseDeltas(inputManager *const __RESTRICT__ inMngr, int *const mx, int *const my);

#endif
