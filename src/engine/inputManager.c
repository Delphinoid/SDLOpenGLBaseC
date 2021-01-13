#include "inputManager.h"
#include "moduleInput.h"

void inMngrInit(inputManager *const __RESTRICT__ inMngr){
	inMngr->internal = SDL_GetKeyboardState(NULL);
	inMngr->keys = NULL;
}

return_t inMngrBind(inputManager *const __RESTRICT__ inMngr, const inputKeyBinding binding){
	inputKeyBinding *const bind = moduleInputKeyBindingPrepend(&inMngr->keys);
	if(bind == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	*bind = binding;
	return 1;
}

void inMngrUnbind(inputManager *const __RESTRICT__ inMngr, const SDL_Scancode scancode){
	inputKeyBinding *prev = NULL;
	inputKeyBinding *key = inMngr->keys;
	while(key != NULL){
		if(key->scancode == scancode){
			moduleInputKeyBindingFree(&inMngr->keys, key, prev);
			key = prev;
		}
		prev = key;
		key = (inputKeyBinding *)memSLinkNext(key);
	}
}

void inMngrUnbindLast(inputManager *const __RESTRICT__ inMngr, const SDL_Scancode scancode){
	inputKeyBinding *prev = NULL;
	inputKeyBinding *key = inMngr->keys;
	while(key != NULL){
		if(key->scancode == scancode){
			moduleInputKeyBindingFree(&inMngr->keys, key, prev);
			return;
		}
		prev = key;
		key = (inputKeyBinding *)memSLinkNext(key);
	}
}

void inMngrTakeInput(inputManager *const __RESTRICT__ inMngr){

	inputKeyBinding *key = inMngr->keys;

	// Update the internal keyboard state buffer.
	SDL_PumpEvents();

	// Update each key state we're expecting input from.
	while(key != NULL){

		if(inMngr->internal[key->scancode]){

			// Key has been pressed.
			/**const command *cmd = key->binding;
			if(flagsAreSet(key->state, INPUT_KEY_STATE_MASK)){
				if(key->state == INPUT_KEY_STATE_PRESSED){
					--key->state;
				}
			}else{
				key->state = INPUT_KEY_STATE_PRESSED;
			}
			// Execute each command bound to the key.
			while(cmd->cmd != NULL){
				cmd->cmd(cmd, key->state);
				++cmd;
			}**/

		}else{

			// Key has been released.
			if(flagsAreSet(key->state, INPUT_KEY_STATE_MASK)){
				key->state = INPUT_KEY_STATE_RELEASED;
			}else{
				if(key->state == INPUT_KEY_STATE_RELEASED){
					--key->state;
				}
			}

		}

		// Get the next key.
		key = (inputKeyBinding *)memSLinkNext(key);

	}

}
