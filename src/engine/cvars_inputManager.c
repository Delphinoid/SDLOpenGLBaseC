#include "cvars_inputManager.h"
#include <stdlib.h>
#include <string.h>

// Relative mouse offset since the last tick.
int CVAR_MOUSE_DX = 0;
int CVAR_MOUSE_DY = 0;

int CVAR_RUNNING = 1;

static int c_str2int(const char *str){
	// Used when invoking a mousemove command.
    unsigned int curIndex = 0;
    const char flagChar = *str - 60;
    int i;
    memcpy(&i, &str[1], 4);
    for(; curIndex < 4; ++curIndex, ++str){
        if(flagChar & 1 << curIndex){
            i -= 60 << (curIndex << 3);
        }
    }
    return i;
}
void c_mousemove(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	// If the first character is greater than the command limit,
	// it must be a flag byte and not a numerical value.
	if(argc == 2){
		if(**argv >= 60){
			// If the first character is greater than the command limit,
			// it must be a flag byte and not a numerical value.
			CVAR_MOUSE_DX += c_str2int(argv[0]);
			CVAR_MOUSE_DY += c_str2int(argv[1]);
		}else{
			// Otherwise, handle the arguments normally.
			CVAR_MOUSE_DX += strtol(argv[0], NULL, 10);
			CVAR_MOUSE_DY += strtol(argv[1], NULL, 10);
		}
	}
}

void c_exit(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_RUNNING = 0;
}