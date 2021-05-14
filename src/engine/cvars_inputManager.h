#ifndef CVARS_INPUTMANAGER_H
#define CVARS_INPUTMANAGER_H

#include "../engine/command.h"

// Relative mouse offset since the last tick.
extern int CVAR_MOUSE_DX;
extern int CVAR_MOUSE_DY;

extern int CVAR_RUNNING;

void c_mousemove(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_exit(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);

#endif
