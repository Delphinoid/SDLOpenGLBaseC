#include "inputManager.h"
#include "cvars_inputManager.h"
#include "memoryManager.h"
#include <SDL2/SDL.h>
#include <string.h>

static __HINT_INLINE__ void inputButtonBindingDelete(inputButtonBinding *const __RESTRICT__ bind){
	if(bind->binding != NULL){
		memFree(bind->binding);
	}
	memset(bind, 0, sizeof(inputButtonBinding));
}

void inMngrInit(inputManager *const __RESTRICT__ inMngr){
	memset(inMngr, 0, sizeof(inputManager));
}
return_t inMngrKeyboardBind(inputManager *const __RESTRICT__ inMngr, const int id, const char *const binding, const size_t bindingLength){

	inputButtonBinding *const btn = &inMngr->kbKeys[id];

	if(btn->binding != NULL){
		// Key already bound!
		return 0;
	}else{

		btn->id = id;
		btn->binding = memAllocate(bindingLength*sizeof(char));
		if(btn->binding == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		btn->bindingLength = bindingLength;
		memcpy(btn->binding, binding, bindingLength*sizeof(char));

		return 1;

	}

}
void inMngrKeyboardUnbind(inputManager *const __RESTRICT__ inMngr, const int id){
	inputButtonBinding *const btn = &inMngr->kbKeys[id];
	inputButtonBindingDelete(btn);
}
return_t inMngrMouseBind(inputManager *const __RESTRICT__ inMngr, const int id, const char *const binding, const size_t bindingLength){

	inputButtonBinding *const btn = &inMngr->mButtons[id];

	if(btn->binding != NULL){
		// Key already bound!
		return 0;
	}else{

		btn->id = id;
		btn->binding = memAllocate(bindingLength*sizeof(char));
		if(btn->binding == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		btn->bindingLength = bindingLength;
		memcpy(btn->binding, binding, bindingLength*sizeof(char));

		return 1;

	}

}
void inMngrMouseUnbind(inputManager *const __RESTRICT__ inMngr, const int id){
	inputButtonBinding *const btn = &inMngr->mButtons[id];
	inputButtonBindingDelete(btn);
}
void inMngrDelete(inputManager *const __RESTRICT__ inMngr){
	inputButtonBinding *b = inMngr->kbKeys;
	const inputButtonBinding *bLast = &inMngr->kbKeys[INPUT_MAX_KEYBOARD_KEYS];
	while(b < bLast){
		inputButtonBindingDelete(b);
		++b;
	}
	b = inMngr->mButtons;
	bLast = &inMngr->mButtons[INPUT_MAX_MOUSE_BUTTONS];
	while(b < bLast){
		inputButtonBindingDelete(b);
		++b;
	}
}

static void c_int2str(char *str, const int i){
	// Used when adding a mousemove command.
    unsigned int curIndex = 1;
    char *flagChar = str;
    ++str;
    *flagChar = 60;
    memcpy(str, &i, 4);
    for(; curIndex < 16; curIndex <<= 1, ++str){
        if((unsigned char)*str < 60){
            *str += 60;
            *flagChar += curIndex;
        }
    }
}
static void inMngrMouseMotionAddCommand(
	cmdBuffer *const restrict buffer,
	int *const restrict mx, int *const restrict my,
	const tick_t mt
){
	// Only send a command if the mouse has moved.
	if(*mx | *my){
		char cmd[22];
		// Construct the command string.
		memcpy(&cmd[0], "mousemove ", 10);
		c_int2str(&cmd[10], *mx);
		cmd[15] = ' ';
		c_int2str(&cmd[16], *my);
		cmd[21] = '\0';
		// Add the command to the command buffer.
		cmdBufferTokenize(buffer, cmd, 22, mt, 0);
		// Reset the mouse deltas.
		*mx = 0; *my = 0;
	}
}

return_t inMngrTakeInput(inputManager *const __RESTRICT__ inMngr, cmdBuffer *const __RESTRICT__ buffer){

	inputButtonBinding *btn;
	int mx = 0; int my = 0; tick_t mt = 0;
	SDL_Event e;

	CVAR_MOUSE_DX = 0; CVAR_MOUSE_DY = 0;

	while(SDL_PollEvent(&e)){
		switch(e.type){

			case SDL_QUIT:
				c_exit(NULL, 0, NULL);
			break;

			case SDL_KEYDOWN:
				// A key has been pressed.
				// Make sure the key isn't being held down.
				btn = &inMngr->kbKeys[e.key.keysym.scancode];
				if(!e.key.repeat && btn->binding != NULL){
					if(cmdBufferTokenize(buffer, btn->binding, btn->bindingLength, e.key.timestamp, 0) < 0){
						/** Memory allocation failure. **/
						return -1;
					}
				}
			break;

			case SDL_KEYUP:
				// A key has been released.
				// Make sure the key isn't being held down.
				btn = &inMngr->kbKeys[e.key.keysym.scancode];
				if(!e.key.repeat && btn->binding != NULL){
					// If the command begins with a '+', execute its '-' pair.
					if(btn->binding[0] == '+'){
						btn->binding[0] = '-';
						if(cmdBufferTokenize(buffer, btn->binding, btn->bindingLength, e.key.timestamp, 0) < 0){
							/** Memory allocation failure. **/
							return -1;
						}
						btn->binding[0] = '+';
					}
				}
			break;

			case SDL_MOUSEMOTION:
				// The cursor was moved.
				// Buffer the mouse movement until the next mouse event.
				// This allows us to trigger events during mouse movements.
				inMngr->mx += e.motion.xrel;
				inMngr->my += e.motion.yrel;
				mx += e.motion.xrel;
				my += e.motion.yrel;
				mt = e.motion.timestamp;
			break;

			case SDL_MOUSEBUTTONDOWN:
				// A button has been pressed.
				inMngrMouseMotionAddCommand(buffer, &mx, &my, mt);
				// Make sure the button isn't being held down.
				btn = &inMngr->mButtons[e.button.button-1];
				if(btn->binding != NULL){
					if(cmdBufferTokenize(buffer, btn->binding, btn->bindingLength, e.button.timestamp, 0) < 0){
						/** Memory allocation failure. **/
						return -1;
					}
				}
			break;

			case SDL_MOUSEBUTTONUP:
				// A button has been released.
				inMngrMouseMotionAddCommand(buffer, &mx, &my, mt);
				// Make sure the button isn't being held down.
				btn = &inMngr->mButtons[e.button.button-1];
				if(btn->binding != NULL){
					// If the command begins with a '+', execute its '-' pair.
					if(btn->binding[0] == '+'){
						btn->binding[0] = '-';
						if(cmdBufferTokenize(buffer, btn->binding, btn->bindingLength, e.button.timestamp, 0) < 0){
							/** Memory allocation failure. **/
							return -1;
						}
						btn->binding[0] = '+';
					}
				}
			break;

			case SDL_MOUSEWHEEL:
				// The mouse wheel was scrolled.
				inMngrMouseMotionAddCommand(buffer, &mx, &my, mt);
				if(e.wheel.y < 0){
					btn = &inMngr->mButtons[INPUT_MWHEELDOWN];
				}else if(e.wheel.y > 0){
					btn = &inMngr->mButtons[INPUT_MWHEELUP];
				}else{
					break;
				}
				if(btn->binding != NULL){
					// We perform both the press and release events.
					if(cmdBufferTokenize(buffer, btn->binding, btn->bindingLength, e.wheel.timestamp, 0) < 0){
						/** Memory allocation failure. **/
						return -1;
					}
					// If the command begins with a '+', execute its '-' pair.
					if(btn->binding[0] == '+'){
						btn->binding[0] = '-';
						if(cmdBufferTokenize(buffer, btn->binding, btn->bindingLength, e.wheel.timestamp, 0) < 0){
							/** Memory allocation failure. **/
							return -1;
						}
						btn->binding[0] = '+';
					}
				}
			break;

		}
	}

	// Handle residual mouse motion events.
	inMngrMouseMotionAddCommand(buffer, &mx, &my, mt);
	return 1;

}

__HINT_INLINE__ void inMngrMouseDeltas(inputManager *const __RESTRICT__ inMngr, int *const mx, int *const my){
	// Reset the accumulated mouse deltas.
	*mx = inMngr->mx;
	*my = inMngr->my;
	inMngr->mx = 0;
	inMngr->my = 0;
}
