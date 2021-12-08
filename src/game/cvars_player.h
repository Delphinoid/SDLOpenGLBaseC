#ifndef CVARS_PLAYER_H
#define CVARS_PLAYER_H

#include "../engine/command.h"

extern int CVAR_FORWARD;
extern int CVAR_BACKWARD;
extern int CVAR_LEFT;
extern int CVAR_RIGHT;
extern int CVAR_JUMP;
extern int CVAR_INTERACT;

extern int CVAR_FIRSTPERSON;
extern int CVAR_THIRDPERSON;

extern int CVAR_CHANGE;

void c_void(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);

void c_forward0(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_forward1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_backward0(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_backward1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_left0(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_left1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_right0(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_right1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_jump0(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_jump1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_interact0(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_interact1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);

void c_firstperson(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
void c_thirdperson(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);

void c_change1(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);

#endif
