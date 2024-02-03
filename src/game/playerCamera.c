#include "playerCamera.h"
#include "../engine/constantsMath.h"
#include <math.h>
#include <string.h>

#define PLAYER_CAMERA_PITCH_EPSILON 0.0001
#define PLAYER_CAMERA_MAX_PITCH (M_PI_2 - PLAYER_CAMERA_PITCH_EPSILON)

/// Move sensitivity, yaw and pitch into a mouse event handler.
#define PLAYER_CAMERA_DEFAULT_SENSITIVITY 12.f
#define PLAYER_CAMERA_DEFAULT_YAW   0.022f
#define PLAYER_CAMERA_DEFAULT_PITCH 0.022f

void pcInit(playerCamera *const __RESTRICT__ pc, camera *const cam){
	memset(pc, 0, sizeof(playerCamera));
	pc->cam = cam;
	pc->sensitivity = PLAYER_CAMERA_DEFAULT_SENSITIVITY;
	pc->yaw = PLAYER_CAMERA_DEFAULT_YAW;
	pc->pitch = PLAYER_CAMERA_DEFAULT_PITCH;
}

void pcLook(playerCamera *const __RESTRICT__ pc, const vec3 eye, const vec3 target){
	const vec3 look = vec3VSubV(target, eye);
	pc->eye = eye;
	pc->target = target;
	if(look.z != 0.f){
		pc->angle = atanf(look.y/look.z);
	}else{
		pc->angle = 0.f;
	}
}

void pcTick(playerCamera *const __RESTRICT__ pc, const int mx, const int my){

	quat orientation;

	// Radians to rotate the camera by.
	pc->rx += (float)mx * pc->yaw   * pc->sensitivity * RADIAN_RATIO;
	pc->ry += (float)my * pc->pitch * pc->sensitivity * RADIAN_RATIO;
	if(pc->ry > PLAYER_CAMERA_MAX_PITCH - pc->angle){
		pc->ry = PLAYER_CAMERA_MAX_PITCH - pc->angle;
	}else if(pc->ry < -PLAYER_CAMERA_MAX_PITCH - pc->angle){
		pc->ry = -PLAYER_CAMERA_MAX_PITCH - pc->angle;
	}

	// Rotate around X, followed by Y.
	orientation = quatNewEuler(-pc->ry, -pc->rx, 0.f);

	// Rotate the position and target vectors.
	if(pc->pivot != NULL){
		pc->cam->position.value = vec3VAddV(
			*pc->pivot,
			vec3VAddV(
				pc->pivotStatic,
				quatRotateVec3(orientation, pc->eye)
			)
		);
		pc->cam->target.value = vec3VAddV(
			*pc->pivot,
			vec3VAddV(
				pc->pivotStatic,
				quatRotateVec3(orientation, pc->target)
			)
		);
	}else{
		pc->cam->position.value = vec3VAddV(pc->pivotStatic, quatRotateVec3(orientation, pc->eye));
		pc->cam->target.value   = vec3VAddV(pc->pivotStatic, quatRotateVec3(orientation, pc->target));
	}

	/// If the camera is in a wall, move it along the eye vector.

}
