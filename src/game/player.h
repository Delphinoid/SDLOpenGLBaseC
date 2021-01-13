#ifndef PLAYER_H
#define PLAYER_H

#include "../engine/inputManager.h"
#include "../engine/object.h"
#include "../engine/vec2.h"

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
	flags_t jump;

	// Number of ticks since the player left the ground.
	uint_least32_t airborne;

	// Player velocity this tick.
	vec3 velocity;

	// Movement direction.
	///vec3 direction;

} pMove;

typedef struct {

	// How the player appears.
	// You can toggle this from rendering
	// by changing the alpha of the renderables.
	object *obj;

	// Movement controller.
	// Based on both QuakeWorld and CPM movement.
	pMove movement;

} player;

void pInit(player *const __RESTRICT__ p, object *const obj);
void pBasis(player *const __RESTRICT__ p, const camera *const __RESTRICT__ cam);
void pInput(player *const __RESTRICT__ p, const float right, const float forward, const flags_t jump);
void pTick(player *const __RESTRICT__ p, const float dt_s);

#endif
