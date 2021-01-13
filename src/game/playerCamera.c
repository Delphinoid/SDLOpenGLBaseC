#include "playerCamera.h"
#include "../engine/camera.h"
#include "../engine/constantsMath.h"
#include <math.h>
#include <string.h>

#define PLAYER_CAMERA_PITCH_EPSILON 0.001
#define PLAYER_CAMERA_MAX_PITCH (M_PI_on_2 - PLAYER_CAMERA_PITCH_EPSILON)

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

void pcPositionOffsetDynamic(playerCamera *const __RESTRICT__ pc, const vec3 positionOffsetDynamic){
	pc->positionOffsetDynamic = positionOffsetDynamic;
	pc->positionOffsetAngle = atanf(positionOffsetDynamic.y/positionOffsetDynamic.z);
}

void pcTick(playerCamera *const __RESTRICT__ pc, const int mx, const int my){

	quat orientation;

	// Radians to rotate the camera by.
	pc->rx += (float)mx * pc->yaw   * pc->sensitivity * RADIAN_RATIO;
	pc->ry += (float)my * pc->pitch * pc->sensitivity * RADIAN_RATIO;
	if(pc->ry > PLAYER_CAMERA_MAX_PITCH - pc->positionOffsetAngle){
		pc->ry = PLAYER_CAMERA_MAX_PITCH - pc->positionOffsetAngle;
	}else if(pc->ry < -PLAYER_CAMERA_MAX_PITCH - pc->positionOffsetAngle){
		pc->ry = -PLAYER_CAMERA_MAX_PITCH - pc->positionOffsetAngle;
	}

	// Rotate around X, followed by Y.
	orientation = quatNewEuler(-pc->ry, -pc->rx, 0.f);

	// Rotate the target vector.
	if(pc->position != NULL){
		pc->cam->target.value = vec3VAddV(
			*pc->position,
			quatRotateVec3(orientation, pc->targetOffset)
		);
		pc->cam->position.value = vec3VAddV(
			*pc->position,
			vec3VAddV(
				pc->positionOffsetStatic,
				quatRotateVec3(orientation, pc->positionOffsetDynamic)
			)
		);
	}else{
		pc->cam->target.value = quatRotateVec3(orientation, pc->targetOffset);
		pc->cam->position.value = vec3VAddV(pc->positionOffsetStatic, quatRotateVec3(orientation, pc->positionOffsetDynamic));
	}

	/// If the camera is in a wall, move it along the target vector.

}