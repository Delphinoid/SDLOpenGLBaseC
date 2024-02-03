#include "player.h"
#include "playerCamera.h"
#include "cvars_player.h"
#include "../engine/camera.h"
#include "../engine/physicsRigidBody.h"
#include "../engine/physicsContact.h"
#include "../engine/physicsCollider.h"
#include "../engine/memorySLink.h"
#include "../engine/helpersMath.h"
#include <math.h>
#include <string.h>

#define PLAYER_STEEPEST_SLOPE_ANGLE 0.7f  // Cosine of the maximum slope angle.
#define	PLAYER_MOVE_STOP_EPSILON 0.001f

// Physics engine friction for different states.
///#define PLAYER_PHYSICS_GROUND_FRICTION 1.f
///#define PLAYER_PHYSICS_AIR_FRICTION    1.f

// Note that these friction values do not account for
// the added friction from the physics simulation.
// We use separate friction values while stationary
// and in motion to reduce excessive acceleration while
// strafing while still keeping the slippery, "socks
// on timber" feeling.
#define PLAYER_FRICTION 2.75f
#define PLAYER_FRICTION_WISH 6.f
// This combination gives a jump height of almost exactly 1 unit (1.0125).
#define PLAYER_GRAVITY 20.f
#define PLAYER_JUMP 9.f
// Note: air control only affects you when you're not strafing.
// You're probably looking for PLAYER_AIR_STRAFE_ACCELERATION.
#define PLAYER_AIR_CONTROL 4.f

#define PLAYER_GROUND_MAX_SPEED    20.f
#define PLAYER_GROUND_ACCELERATION 15.f
#define PLAYER_GROUND_DECELERATION 8.f

#define PLAYER_AIR_MAX_SPEED    10.f
#define PLAYER_AIR_ACCELERATION 1.f
#define PLAYER_AIR_DECELERATION 2.5f

/// Currently unimplemented.
#define PLAYER_COYOTE_TIME 0

// A low air strafe speed and high acceleration allows for good
// air control while stopping the player from easily climbing
// slopes that are much too steep for them, like in Quake. It
// also helps surfing feel more natural, similar to TF2.
#define PLAYER_AIR_STRAFE_MAX_SPEED    1.f
#define PLAYER_AIR_STRAFE_ACCELERATION 120.f

static __HINT_INLINE__ float pMoveScale(const pMove *const __RESTRICT__ movement, const float speed){
	// Scale to reduce sqrt(2) movement along diagonals.
	// A little bit of zigzagging is still nice.
	if(movement->rwish == 0.f && movement->fwish == 0.f){
		return 0.f;
	}else{
		return speed*rsqrtAccurate(
			movement->rwish*movement->rwish + movement->fwish*movement->fwish
		);
	}
}

static __HINT_INLINE__ void pMoveClipVelocity(pMove *const __RESTRICT__ movement, const vec3 normal, const float restitution){
	// Clip the player velocity such
	// that it stays out of contacts
	// and doesn't bounce when landing.
	float backoff = vec3Dot(movement->velocity, normal);
	if(restitution == 0.f || backoff < 0.f){
		// If the restitution is 0, always clip to prevent bouncing.
		// We also always clip if we're moving into the contact.
		movement->velocity.x -= normal.x*backoff;
		movement->velocity.y -= normal.y*backoff;
		movement->velocity.z -= normal.z*backoff;
	}
}

static __HINT_INLINE__ void pMoveAccelerate(
	pMove *const __RESTRICT__ movement, const vec2 wishdir, const float wishspeed, const float accel, const float friction, const float dt_s
){

	// Subtract the dot product of the velocity vector with the (normalized) wish direction.
	// This is just the magnitude of the projection of the velocity vector onto the
	// wish direction; thus addspeed represents the difference between how much we want to
	// move in the wish direction and how much we're already moving in the wish direction,
	// and hence the maximum amount of speed we can add before we breach the max speed limit.
	const float addspeed = wishspeed - movement->velocity.x*wishdir.x - movement->velocity.z*wishdir.y;

	// If addspeed is zero or negative, we're moving at max or greater than max speed.
	if(addspeed > 0.f){
		// We multiply by wishspeed here to overcome multiplying by control in pMoveFriction.
		float accelspeed = accel * wishspeed * friction * dt_s;
		if(addspeed < accelspeed){
			accelspeed = addspeed;
		}
		movement->velocity.x += wishdir.x * accelspeed;
		movement->velocity.z += wishdir.y * accelspeed;
	}

}

static __HINT_INLINE__ void pMoveFriction(pMove *const __RESTRICT__ movement, const float t, const float dt_s){

	const vec2 velocity = {
		.x = movement->velocity.x,
		.y = movement->velocity.z
	};
	const float speed = vec2Magnitude(velocity);

	if(speed > 0.f){

		float newSpeed = speed;

		// Only apply friction when grounded.
		// This function is only called when we're grounded, so it's fine.
		//if(!movement->airborne){
			const float control = floatMax(speed, PLAYER_GROUND_DECELERATION);
			if(movement->fwish != 0.f || movement->rwish != 0.f){
				newSpeed -= control * t * PLAYER_FRICTION_WISH * dt_s;
			}else{
				newSpeed -= control * t * PLAYER_FRICTION * dt_s;
			}
		//}

		if(newSpeed < 0.f){
			movement->velocity.x = 0.f;
			movement->velocity.z = 0.f;
		}else{
			// Normalize the velocity and scale it by newSpeed.
			newSpeed /= speed;
			movement->velocity.x *= newSpeed;
			movement->velocity.z *= newSpeed;
		}

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

static __HINT_INLINE__ void pMoveGround(pMove *const __RESTRICT__ movement, const float friction, const float dt_s){

	// Transform the joystick direction
	// into the projected camera basis.
	vec2 wishdir = {
		.x = movement->rwish*movement->rbasis.x + movement->fwish*movement->fbasis.x,
		.y = movement->rwish*movement->rbasis.y + movement->fwish*movement->fbasis.y,
	};
	// Divides wishdir*PLAYER_GROUND_MAX_SPEED by the magnitude of (rwish, fwish).
	const float wishspeed = vec2Magnitude(wishdir) * pMoveScale(movement, PLAYER_GROUND_MAX_SPEED);

	//if(wishspeed > 0.f){
		wishdir = vec2NormalizeFastAccurate(wishdir);
		movement->direction = wishdir;
		pMoveAccelerate(movement, wishdir, wishspeed, PLAYER_GROUND_ACCELERATION, friction, dt_s);
	//}

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
	const float wishspeed2 = vec2Magnitude(wishdir) * pMoveScale(movement, PLAYER_AIR_MAX_SPEED);

	if(wishspeed2 > 0.f){

		float wishspeed = wishspeed2;
		float accel;
		wishdir = vec2NormalizeFastAccurate(wishdir);
		movement->direction = wishdir;

		// If we're trying to move in the opposite direction we're already moving in, decelerate.
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
		pMoveAccelerate(movement, wishdir, wishspeed, accel, 1.f, dt_s);
		if(PLAYER_AIR_CONTROL > 0.f){
			pMoveAirControl(movement, wishdir, wishspeed2, dt_s);
		}

	}

}

/**static __HINT_INLINE__ void pSetPhysicsFriction(player *const __RESTRICT__ p, const float friction){
	// Set the friction of the player's physics colliders.
	// This value depends on the current state of the player.
	physCollider *c = p->obj->skeletonBodies->hull;
	while(c != NULL){
		c->friction = friction;
		c = (physCollider *)memSLinkNext(c);
	}
}**/

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

void pRotateCamera(player *const __RESTRICT__ p, const playerCamera *const __RESTRICT__ pc){
	// Rotate the player to face the direction of the camera.
	const vec3 direction = vec3New(
		pc->cam->target.value.x - pc->cam->position.value.x,
		0.f,
		pc->cam->target.value.z - pc->cam->position.value.z
	);
	p->obj->skeletonBodies->configuration.orientation = quatNewRotation(
		vec3New(0.f, 0.f, 1.f), vec3NormalizeFastAccurate(direction)
	);
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

/**void pInput(player *const __RESTRICT__ p, const float right, const float forward, const flags_t jump){
	p->movement.rwish = right;
	p->movement.fwish = forward;
	p->movement.jump = jump;
}**/

void pTick(player *const __RESTRICT__ p, const float dt_s){

	// The object is automatically updated by the module.

	// Check whether or not the body is on the ground.
	// We do this by finding the contact normal with
	// the greatest y-coordinate of the and checking if
	// this is greater than some maximum slope threshold.
	const physCollider *lastCollider = NULL;
	const physContactPair *lastContact = NULL;
	vec3 maxNormal = {.x = 0.f, .y = 0.f, .z = 0.f};
	float maxFriction = 1.f;
	float maxRestitution = 0.f;
	vec3 maxVelocity = g_vec3Zero;
	vec3 frame = g_vec3Zero;
	while(physRigidBodyCheckContact(p->obj->skeletonBodies, 0xFFFF, &lastCollider, &lastContact)){
		float friction, restitution;
		vec3 velocity;
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
			friction = lastContact->colliderB->friction;
			restitution = lastContact->colliderB->restitution;
			velocity = ((const physRigidBody *)lastContact->colliderB->body)->linearVelocity;
		}else{
			friction = lastContact->colliderA->friction;
			restitution = lastContact->colliderA->restitution;
			velocity = ((const physRigidBody *)lastContact->colliderB->body)->linearVelocity;
		}
		if(normal.y > maxNormal.y){
			maxNormal = normal;
			maxFriction = friction;
			maxRestitution = restitution;
			maxVelocity = velocity;
		}
	}

	// Check the greatest contact normal to determine
	// whether or not we're in the air. We should also
	// change the physics friction accordingly!
	if(maxNormal.y >= PLAYER_STEEPEST_SLOPE_ANGLE){
		p->movement.airborne = 0;
		// This reference frame represents the linear velocity of any
		// moving platform that the player is currently standing on.
		frame = maxVelocity;
	}else if(p->movement.airborne != (tick_t)-1){
		// We set CVAR_JUMP to 0 here rather than when we
		// execute a jump to prevent jumps from being eaten
		// while the player is still moving out of the ground.
		if(p->movement.airborne == 1){
			CVAR_JUMP = 0;
		}
		++p->movement.airborne;
	}

	// Capture console input.
	p->movement.rwish = (float)(CVAR_RIGHT-CVAR_LEFT);
	p->movement.fwish = (float)(CVAR_FORWARD-CVAR_BACKWARD);
	///p->movement.jump = CVAR_JUMP;

	// Handle movement related input.
	p->movement.velocity = vec3VSubV(p->obj->skeletonBodies->linearVelocity, frame);
	if(!p->movement.airborne){
		if(CVAR_JUMP == 0){

			// Handle friction.
			pMoveFriction(&p->movement, maxFriction, dt_s);
			flagsUnset(p->movement.state, PLAYER_MOVEMENT_JUMPING);

			if(p->movement.fwish != 0.f || p->movement.rwish != 0.f){
				// Move along the ground.
				flagsSet(p->movement.state, PLAYER_MOVEMENT_WALKING);
				pMoveGround(&p->movement, maxFriction, dt_s);
				///pMoveClipVelocity(&p->movement, maxNormal);
			}else{
				flagsUnset(p->movement.state, PLAYER_MOVEMENT_WALKING);
			}
			// We do this regardless of whether the player is
			// moving or stationary to account for bouncing.
			pMoveClipVelocity(&p->movement, maxNormal, floatMax(p->obj->skeletonBodies->hull->restitution, maxRestitution));

		}else{

			// A jump has been input.
			p->movement.airborne = 1;
			// Only overwrite the velocity if we wouldn't
			// otherwise bounce higher, for instance due to
			// a particularly bouncy surface.
			if(p->movement.velocity.y < PLAYER_JUMP){
				p->movement.velocity.y = PLAYER_JUMP;
			}
			flagsSet(p->movement.state, PLAYER_MOVEMENT_JUMPING);
			pMoveAir(&p->movement, dt_s);

		}
	}else{
		pMoveAir(&p->movement, dt_s);
	}
	p->movement.velocity.y -= PLAYER_GRAVITY * dt_s;
	p->obj->skeletonBodies->linearVelocity = vec3VAddV(p->movement.velocity, frame);

	// Handle physics friction.
	/**if(!p->movement.airborne){
		pSetPhysicsFriction(p, PLAYER_PHYSICS_GROUND_FRICTION);
	}else{
		pSetPhysicsFriction(p, PLAYER_PHYSICS_AIR_FRICTION);
	}**/

}
