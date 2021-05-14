#ifndef PLAYER_H
#define PLAYER_H

#include "../engine/object.h"
#include "../engine/vec2.h"
#include "../engine/tick.h"

#define PLAYER_MOVEMENT_WALKING 0x01
#define PLAYER_MOVEMENT_JUMPING 0x02

typedef struct playerCamera playerCamera;

typedef struct {

	// Camera basis vectors.
	// These just contain the x and z components
	// and are normalized, so the basis is effectively
	// projected onto the xz plane.
	vec2 rbasis, fbasis;

	// Where on the joystick boundary the stick is.
	// For keyboard input, each of the axes will be
	// -1, 0 or 1. These are later affected by the
	// (projected) camera basis.
	float rwish, fwish;

	// Whether the jump button is being pressed.
	///flags_t jump;

	// Number of ticks since the player left the ground.
	tick_t airborne;

	// Player velocity this tick.
	vec3 velocity;

	// Normalized movement direction.
	vec2 direction;

	// Movement state.
	flags_t state;

} pMove;

typedef struct {

	// How the player appears.
	// You can toggle this from rendering
	// by changing the alpha of the renderables.
	object *obj;

	// Movement controller.
	pMove movement;

} player;

void pInit(player *const __RESTRICT__ p, object *const obj);
void pBasisPC(player *const __RESTRICT__ p, const playerCamera *const __RESTRICT__ pc);
void pBasisC(player *const __RESTRICT__ p, const camera *const __RESTRICT__ cam);
///void pInput(player *const __RESTRICT__ p, const float right, const float forward, const flags_t jump);
void pTick(player *const __RESTRICT__ p, const float dt_s);
void pRotateWish(player *const __RESTRICT__ p);
void pRotateVelocity(player *const __RESTRICT__ p);
void pRotateCamera(player *const __RESTRICT__ p, const playerCamera *const __RESTRICT__ pc);

#endif
