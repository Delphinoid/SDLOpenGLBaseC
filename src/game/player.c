#include "player.h"
#include "../engine/camera.h"
#include "../engine/physicsRigidBody.h"
#include "../engine/physicsCollision.h"
#include <math.h>
#include <string.h>

#define PLAYER_STEEPEST_SLOPE_ANGLE 0.6f  // Sine of the maximum slope angle.
#define	PLAYER_MOVE_STOP_EPSILON 0.001f

#define PLAYER_FRICTION 4.f
#define PLAYER_GRAVITY 20.f
#define PLAYER_JUMP 8.f
#define PLAYER_AIR_CONTROL 0.4f

#define PLAYER_GROUND_MAX_SPEED    10.f
#define PLAYER_GROUND_ACCELERATION 15.f
#define PLAYER_GROUND_DECELERATION 10.f

#define PLAYER_AIR_MAX_SPEED    10.f
#define PLAYER_AIR_ACCELERATION 2.f
#define PLAYER_AIR_DECELERATION 3.f

#define PLAYER_STRAFE_MAX_SPEED    1.f
#define PLAYER_STRAFE_ACCELERATION 70.f
#define PLAYER_STRAFE_DECELERATION 70.f

static __HINT_INLINE__ void pMoveAccelerate(
	pMove *const __RESTRICT__ movement, const vec2 wishdir, const float wishspeed, const float accel, const float dt_s
){

	const float addspeed = wishspeed - movement->velocity.x*wishdir.x - movement->velocity.z*wishdir.y;
	if(addspeed > 0.f){
		float accelspeed = accel * wishspeed * dt_s;
		if(accelspeed > addspeed){
			accelspeed = addspeed;
		}
		movement->velocity.x += wishdir.x * accelspeed;
		movement->velocity.z += wishdir.y * accelspeed;
	}

}

static __HINT_INLINE__ void pMoveFriction(pMove *const __RESTRICT__ movement, float t, const float dt_s){

	const vec2 velocity = {
		.x = movement->velocity.x,
		.y = movement->velocity.z
	};
	const float speed = vec2Magnitude(velocity);
	float newSpeed = speed;

	// Only apply friction when grounded.
	if(!movement->airborne){
		const float control = speed < PLAYER_GROUND_DECELERATION ? PLAYER_GROUND_DECELERATION : speed;
		newSpeed -= control * t * PLAYER_FRICTION * dt_s;
	}

	if(newSpeed < 0.f){
		movement->velocity.x = 0.f;
		movement->velocity.z = 0.f;
	}else{
		if(speed > 0.f){
			newSpeed /= speed;
		}
		movement->velocity.x *= newSpeed;
		movement->velocity.z *= newSpeed;
	}

}

static __HINT_INLINE__ void pMoveAirControl(
	pMove *const __RESTRICT__ movement, const vec2 wishdir, const float wishspeed, const float dt_s
){

	// Only control air movement when moving forward or backward.
	if(
		fabsf(movement->fwish) >= PLAYER_MOVE_STOP_EPSILON &&
		fabsf(wishspeed)       >= PLAYER_MOVE_STOP_EPSILON
	){

		vec2 velocity = {
			.x = movement->velocity.x,
			.y = movement->velocity.z
		};
		const float speed = vec2Magnitude(velocity);
		velocity = vec2NormalizeFastAccurate(velocity);

		{
			// Change direction while slowing down.
			const float dot = vec2Dot(velocity, wishdir);
			//if(dot > 0.f){
				const float k = 480.f*PLAYER_AIR_CONTROL*dot*dot*dt_s;
				velocity.x = velocity.x*speed + wishdir.x*k;
				velocity.y = velocity.y*speed + wishdir.y*k;
				velocity = vec2NormalizeFastAccurate(velocity);
			//}
		}

		movement->velocity.x = velocity.x * speed;
		movement->velocity.z = velocity.y * speed;

	}

}

static __HINT_INLINE__ void pMoveGround(pMove *const __RESTRICT__ movement, const float dt_s){

	// Transform the joystick direction
	// into the projected camera basis.
	vec2 wishdir = {
		.x = movement->rwish*movement->rbasis.x + movement->fwish*movement->fbasis.x,
		.y = movement->rwish*movement->rbasis.y + movement->fwish*movement->fbasis.y,
	};
	// Calculate the speed.
	const float wishspeed = vec2Magnitude(wishdir) * PLAYER_GROUND_MAX_SPEED;
	wishdir = vec2NormalizeFastAccurate(wishdir);

	pMoveAccelerate(movement, wishdir, wishspeed, PLAYER_GROUND_ACCELERATION, dt_s);

}

static __HINT_INLINE__ void pMoveAir(pMove *const __RESTRICT__ movement, const float dt_s){

	// Handle airborne movement.

	// Transform the joystick direction
	// into the projected camera basis.
	vec2 wishdir = {
		.x = movement->rwish*movement->rbasis.x + movement->fwish*movement->fbasis.x,
		.y = movement->rwish*movement->rbasis.y + movement->fwish*movement->fbasis.y,
	};
	// Calculate the speed.
	const float wishspeed = vec2Magnitude(wishdir) * PLAYER_AIR_MAX_SPEED;
	float wishspeed_strafe = wishspeed;
	float accel;
	wishdir = vec2NormalizeFastAccurate(wishdir);

	// Use CPM-style air control.
	if(movement->velocity.x*wishdir.x + movement->velocity.z*wishdir.y < 0.f){
		accel = PLAYER_AIR_DECELERATION;
	}else{
		accel = PLAYER_AIR_ACCELERATION;
	}

	// Handle strafing movement.
	if(movement->rwish != 0.f && movement->fwish == 0.f){
		if(wishspeed_strafe > PLAYER_STRAFE_MAX_SPEED){
			wishspeed_strafe = PLAYER_STRAFE_MAX_SPEED;
		}
		accel = PLAYER_STRAFE_ACCELERATION;
	}

	// Accelerate.
	pMoveAccelerate(movement, wishdir, wishspeed_strafe, accel, dt_s);
	if(PLAYER_AIR_CONTROL > 0.f){
		pMoveAirControl(movement, wishdir, wishspeed, dt_s);
	}

}

static __HINT_INLINE__ void pMoveRotate(player *const __RESTRICT__ p){
	// Only update the direction if we've moved far enough.
	const vec3 direction = vec3New(p->movement.velocity.x, 0.f, p->movement.velocity.z);
	if(vec3Magnitude(direction) >= PLAYER_MOVE_STOP_EPSILON){
		p->obj->skeletonBodies[0].configuration.orientation = quatNewRotation(
			vec3New(0.f, 0.f, 1.f), vec3NormalizeFastAccurate(direction)
		);
	}
}

void pInit(player *const __RESTRICT__ p, object *const obj){
	memset(p, 0, sizeof(player));
	p->obj = obj;
}

void pBasis(player *const __RESTRICT__ p, const camera *const __RESTRICT__ cam){
	// Project the camera's basis onto the xz plane and normalize.
	// This gives us our movement basis, that is used for moving
	// the player based on the direction the camera is facing.
	// Note that the z-coordinates must be negated.
	const vec2 right = {
		.x = cam->viewMatrix.m[0][0],
		.y = -cam->viewMatrix.m[0][2]
	};
	const vec2 forward = {
		.x = cam->viewMatrix.m[2][0],
		.y = -cam->viewMatrix.m[2][2]
	};
	p->movement.rbasis = vec2NormalizeFastAccurate(right);
	p->movement.fbasis = vec2NormalizeFastAccurate(forward);
}

void pInput(player *const __RESTRICT__ p, const float right, const float forward, const flags_t jump){
	p->movement.rwish = right;
	p->movement.fwish = forward;
	p->movement.jump = jump;
}

void pTick(player *const __RESTRICT__ p, const float dt_s){

	// The object is automatically updated by the module.

	// Check whether or not the body is on the ground.
	// We do this by finding the contact normal with
	// the greatest y-coordinate of the and checking if
	// this is greater than some maximum slope threshold.
	const physCollider *lastCollider = NULL;
	const physContactPair *lastContact = NULL;
	float maximum_angle = 0.f;
	while(physRigidBodyCheckContact(p->obj->skeletonBodies, 0xFFFF, &lastCollider, &lastContact)){
		#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT
		float angle = lastContact->data.normal.y;
		#else
		float angle = lastContact->data.frictionConstraint.normal.y;
		#endif
		if(lastContact->colliderA == lastCollider){
			// The contact normal is taken from
			// collider A to collider B. If the
			// player is collider A, valid normals
			// will be negative: make them positive.
			angle = -angle;
		}
		if(angle > maximum_angle){
			maximum_angle = angle;
		}
	}
	// Check the greatest contact normal.
	if(maximum_angle >= PLAYER_STEEPEST_SLOPE_ANGLE){
		p->movement.airborne = 0;
	}else if(p->movement.airborne != (uint_least32_t)-1){
		++p->movement.airborne;
	}

	// Handle movement related input.
	p->movement.velocity = p->obj->skeletonBodies->linearVelocity;
	if(!p->movement.airborne){
		if(p->movement.jump < INPUT_KEY_STATE_DOWN){
			pMoveFriction(&p->movement, 1.f, dt_s);
		}else{
			p->movement.airborne = 1;
			p->movement.velocity.y = PLAYER_JUMP;
		}
		pMoveGround(&p->movement, dt_s);
		if(!p->movement.airborne){
			p->movement.velocity.y = -PLAYER_GRAVITY * dt_s;
		}
	}else{
		pMoveAir(&p->movement, dt_s);
		p->movement.velocity.y -= PLAYER_GRAVITY * dt_s;
	}
	p->obj->skeletonBodies->linearVelocity = p->movement.velocity;

	// Rotate to face the direction of the (projected) velocity vector.
	pMoveRotate(p);

}