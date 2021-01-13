#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <SDL2/SDL.h>
#include "command.h"

#define INPUT_KEY_BINDING_LENGTH 1024

#define INPUT_KEY_STATE_UP       0x00
#define INPUT_KEY_STATE_RELEASED 0x01
#define INPUT_KEY_STATE_DOWN     0x02
#define INPUT_KEY_STATE_PRESSED  0x03
#define INPUT_KEY_STATE_MASK     0x02

typedef struct inputKeyBinding {
	SDL_Scancode scancode;
	char binding[INPUT_KEY_BINDING_LENGTH];
	// This is 32-bit so that we can store
	// a float, in the case of controllers.
	flags32_t state;
} inputKeyBinding;

typedef struct {

	// Internal keyboard state.
	// Written to by SDL_PumpEvents().
	const Uint8 *internal;

	// Singly-linked list of keys to poll.
	inputKeyBinding *keys;

} inputManager;

void inMngrInit(inputManager *const __RESTRICT__ inMngr);
return_t inMngrBind(inputManager *const __RESTRICT__ inMngr, const inputKeyBinding binding);
void inMngrUnbind(inputManager *const __RESTRICT__ inMngr, const SDL_Scancode scancode);
void inMngrUnbindLast(inputManager *const __RESTRICT__ inMngr, const SDL_Scancode scancode);
void inMngrTakeInput(inputManager *const __RESTRICT__ inMngr);

#endif
