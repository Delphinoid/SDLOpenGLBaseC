#ifndef CVARS_H
#define CVARS_H

#include "../engine/command.h"

int CVAR_FORWARD = 0;
int CVAR_BACKWARD = 0;
int CVAR_LEFT = 0;
int CVAR_RIGHT = 0;
int CVAR_JUMP = 0;
int CVAR_INTERACT = 0;

int CVAR_FIRSTPERSON = 0;
int CVAR_THIRDPERSON = 0;

int CVAR_RUNNING = 1;

void c_forward0(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_FORWARD = 0;
}
void c_forward1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_FORWARD = 1;
}
void c_backward0(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_BACKWARD = 0;
}
void c_backward1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_BACKWARD = 1;
}
void c_left0(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_LEFT = 0;
}
void c_left1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_LEFT = 1;
}
void c_right0(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_RIGHT = 0;
}
void c_right1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_RIGHT = 1;
}
void c_jump0(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_JUMP = 0;
}
void c_jump1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_JUMP = 1;
}
void c_interact0(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_INTERACT = 0;
}
void c_interact1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_INTERACT = 1;
}

void c_firstperson(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_FIRSTPERSON = 1;
	CVAR_THIRDPERSON = 0;
}
void c_thirdperson(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_FIRSTPERSON = 0;
	CVAR_THIRDPERSON = 1;
}

void c_exit(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv){
	CVAR_RUNNING = 0;
}

#endif
