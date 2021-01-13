#ifndef PLAYER_CAMERA_H
#define PLAYER_CAMERA_H

#include "../engine/colliderPoint.h"

typedef struct camera camera;

// Camera controller.
/// Move sensitivity, yaw and pitch into a mouse event handler.
typedef struct {
	camera *cam;
	float sensitivity;
	float yaw, pitch;
	float rx, ry;
	vec3 *position;
	vec3 positionOffsetStatic;
	vec3 positionOffsetDynamic;
	float positionOffsetAngle;
	vec3 targetOffset;
	cPoint collider;
} playerCamera;

void pcInit(playerCamera *const __RESTRICT__ pc, camera *const cam);
void pcPositionOffsetDynamic(playerCamera *const __RESTRICT__ pc, const vec3 positionOffsetDynamic);
void pcTick(playerCamera *const __RESTRICT__ pc, const int mx, const int my);

#endif