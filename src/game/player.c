#include "player.h"
#include "playerCamera.h"
#include "../engine/camera.h"
#include "../engine/physicsRigidBody.h"
#include "../engine/physicsCollision.h"
#include "../engine/physicsCollider.h"
#include "../engine/memorySLink.h"
#include <math.h>
#include <string.h>

#define PLAYER_STEEPEST_SLOPE_ANGLE 0.7f  // Cosine of the maximum slope angle.
#define	PLAYER_MOVE_STOP_EPSILON 0.001f

// Physics engine friction for different states.
#define PLAYER_PHYSICS_GROUND_FRICTION 1.f
#define PLAYER_PHYSICS_AIR_FRICTION    0.f

// Note that this friction value does not account for
// the added friction from the physics simulation.
#define PLAYER_FRICTION 1.5f
// This combination gives a jump height of almost exactly 1 unit (1.0125).
#define PLAYER_GRAVITY 20.f
#define PLAYER_JUMP 9.f
// Note: air control only affects you when you're not strafing.
// You're probably looking for PLAYER_AIR_STRAFE_MAX_SPEED.
#define PLAYER_AIR_CONTROL 4.f

#define PLAYER_GROUND_MAX_SPEED    10.f
#define PLAYER_GROUND_ACCELERATION 15.f
#define PLAYER_GROUND_DECELERATION 8.f

#define PLAYER_AIR_MAX_SPEED    10.f
#define PLAYER_AIR_ACCELERATION 1.f
#define PLAYER_AIR_DECELERATION 2.5f

#define PLAYER_AIR_STRAFE_MAX_SPEED    1.f
#define PLAYER_AIR_STRAFE_ACCELERATION 120.f

static __HINT_INLINE__ void pMoveClipVelocity(pMove *const __RESTRICT__ movement, const vec3 normal){
	// Clip the player velocity such
	// that it stays out of contacts.
	const float backoff = vec3Dot(movement->velocity, normal);
	movement->velocity.x -= normal.x*backoff;
	movement->velocity.y -= normal.y*backoff;
	movement->velocity.z -= normal.z*backoff;
}

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
	if(movement->rwish == 0.f && wishspeed > PLAYER_MOVE_STOP_EPSILON){

		vec2 velocity = {
			.x = movement->velocity.x,
			.y = movement->velocity.z
		};
		const float speed = vec2Magnitude(velocity);
		velocity = vec2NormalizeFastAccurate(velocity);

		{
			// Can't change direction if we're slowing down.
			const float dot = vec2Dot(velocity, wishdir);
			if(dot > 0.f){
				const float k = 32.f*PLAYER_AIR_CONTROL*dot*dot*dt_s;
				velocity.x = velocity.x*speed + wishdir.x*k;
				velocity.y = velocity.y*speed + wishdir.y*k;
				velocity = vec2NormalizeFastAccurate(velocity);
				movement->direction = velocity;
			}
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

	if(wishspeed > 0.f){
		wishdir = vec2NormalizeFastAccurate(wishdir);
		movement->direction = wishdir;
		pMoveAccelerate(movement, wishdir, wishspeed, PLAYER_GROUND_ACCELERATION, dt_s);
	}

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
	const float wishspeed2 = vec2Magnitude(wishdir) * PLAYER_AIR_MAX_SPEED;

	if(wishspeed2 > 0.f){

		float wishspeed = wishspeed2;
		float accel;
		wishdir = vec2NormalizeFastAccurate(wishdir);
		movement->direction = wishdir;

		if(movement->velocity.x*wishdir.x + movement->velocity.z*wishdir.y < 0.f){
			accel = PLAYER_AIR_DECELERATION;
		}else{
			accel = PLAYER_AIR_ACCELERATION;
		}

		// Handle strafing movement.
		if(movement->rwish != 0.f && movement->fwish == 0.f){
			if(wishspeed > PLAYER_AIR_STRAFE_MAX_SPEED){
				wishspeed = PLAYER_AIR_STRAFE_MAX_SPEED;
			}
			accel = PLAYER_AIR_STRAFE_ACCELERATION;
		}

		// Accelerate.
		pMoveAccelerate(movement, wishdir, wishspeed, accel, dt_s);
		if(PLAYER_AIR_CONTROL > 0.f){
			pMoveAirControl(movement, wishdir, wishspeed2, dt_s);
		}

	}

}

static __HINT_INLINE__ void pSetPhysicsFriction(player *const __RESTRICT__ p, const float friction){
	// Set the friction of the player's physics colliders.
	// This value depends on the current state of the player.
	physCollider *c = p->obj->skeletonBodies->hull;
	while(c != NULL){
		c->friction = friction;
		c = (physCollider *)memSLinkNext(c);
	}
}

void pRotateWish(player *const __RESTRICT__ p){
	// Rotate the player based off their wish direction.
	p->obj->skeletonBodies->configuration.orientation = quatNewRotation(
		vec3New(0.f, 0.f, 1.f), vec3New(p->movement.direction.x, 0.f, p->movement.direction.y)
	);
}

void pRotateVelocity(player *const __RESTRICT__ p){
	// Rotate the player based off their velocity.
	const vec3 direction = vec3New(p->movement.velocity.x, 0.f, p->movement.velocity.z);
	if(vec3Magnitude(direction) > PLAYER_MOVE_STOP_EPSILON){
		// Only update the direction if we've moved far enough.
		p->obj->skeletonBodies->configuration.orientation = quatNewRotation(
			vec3New(0.f, 0.f, 1.f), vec3NormalizeFastAccurate(direction)
		);
	}
}

void pInit(player *const __RESTRICT__ p, object *const obj){
	memset(p, 0, sizeof(player));
	p->obj = obj;
	p->movement.direction = vec2New(0.f, 1.f);
}

void pBasisPC(player *const __RESTRICT__ p, const playerCamera *const __RESTRICT__ pc){
	// Project the camera's basis onto the xz plane and normalize.
	// This gives us our movement basis, that is used for moving
	// the player based on the direction the camera is facing.
	// Note that the z-coordinates must be negated.
	const quat orientation = quatNewEuler(0.f, -pc->rx, 0.f);
	const vec3 rbasis = quatRotateVec3(orientation, vec3New(1.f, 0.f, 0.f));
	const vec3 fbasis = quatRotateVec3(orientation, vec3New(0.f, 0.f, -1.f));
	p->movement.rbasis.x = rbasis.x; p->movement.rbasis.y = rbasis.z;
	p->movement.fbasis.x = fbasis.x; p->movement.fbasis.y = fbasis.z;
}

void pBasisC(player *const __RESTRICT__ p, const camera *const __RESTRICT__ cam){
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
	vec3 maximum_normal = {.x = 0.f, .y = 0.f, .z = 0.f};
	while(physRigidBodyCheckContact(p->obj->skeletonBodies, 0xFFFF, &lastCollider, &lastContact)){
		#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT
		vec3 normal = lastContact->data.normal;
		#else
		vec3 normal = lastContact->data.frictionConstraint.normal;
		#endif
		if(lastContact->colliderA == lastCollider){
			// The contact normal is taken from
			// collider A to collider B. If the
			// player is collider A, valid normals
			// will be negative: make them positive.
			normal = vec3Negate(normal);
		}
		if(normal.y > maximum_normal.y){
			maximum_normal = normal;
		}
	}
	// Check the greatest contact normal to determine
	// whether or not we're in the air. We should also
	// change the physics friction accordingly!
	if(maximum_normal.y >= PLAYER_STEEPEST_SLOPE_ANGLE){
		p->movement.airborne = 0;
	}else if(p->movement.airborne != (uint_least32_t)-1){
		++p->movement.airborne;
	}

	// Handle movement related input.
	p->movement.velocity = p->obj->skeletonBodies->linearVelocity;
	if(!p->movement.airborne){
		if(p->movement.jump < INPUT_KEY_STATE_DOWN){
			pMoveFriction(&p->movement, 1.f, dt_s);
			pMoveGround(&p->movement, dt_s);
			pMoveClipVelocity(&p->movement, maximum_normal);
		}else{
			p->movement.airborne = 1;
			p->movement.velocity.y = PLAYER_JUMP;
			pMoveAir(&p->movement, dt_s);
		}
	}else{
		pMoveAir(&p->movement, dt_s);
	}
	p->movement.velocity.y -= PLAYER_GRAVITY * dt_s;
	p->obj->skeletonBodies->linearVelocity = p->movement.velocity;

	// Handle physics friction.
	if(!p->movement.airborne){
		pSetPhysicsFriction(p, PLAYER_PHYSICS_GROUND_FRICTION);
	}else{
		pSetPhysicsFriction(p, PLAYER_PHYSICS_AIR_FRICTION);
	}

}