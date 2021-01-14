#ifndef PLAYER_CAMERA_H
#define PLAYER_CAMERA_H

#include "../engine/colliderPoint.h"

typedef struct camera camera;

// Camera controller.
/// Move sensitivity, yaw and pitch into a mouse event handler.
typedef struct playerCamera {
	camera *cam;
	float sensitivity;
	float yaw, pitch;
	float rx, ry;
	// Camera pivot.
	vec3 *pivot;
	vec3 pivotStatic;
	// Offset of the eye and target from the pivot.
	vec3 eye, target;
	// Angle from the eye to the target.
	float angle;
	cPoint collider;
} playerCamera;

void pcInit(playerCamera *const __RESTRICT__ pc, camera *const cam);
void pcLook(playerCamera *const __RESTRICT__ pc, const vec3 eye, const vec3 target);
void pcTick(playerCamera *const __RESTRICT__ pc, const int mx, const int my);

#endif