#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "mat4.h"

#define CAM_PROJECTION_ORTHO    0x01
#define CAM_UPDATE_VIEW         0x02
#define CAM_UPDATE_PROJECTION   0x04

typedef struct {

	vec3 position;
	vec3 rotation;
	vec3 target;
	vec3 up;
	float fovy;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	unsigned char flags;

} camera;

void camInit(camera *cam);

void camCalculateUp(camera *cam);
void camUpdateViewMatrix(camera *cam);
void camUpdateProjectionMatrix(camera *cam, unsigned char aspectRatioX, unsigned char aspectRatioY);

void camMoveX(camera *cam, float x);
void camMoveY(camera *cam, float y);
void camMoveZ(camera *cam, float z);
void camMove(camera *cam, float x, float y, float z);

void camSetX(camera *cam, float x);
void camSetY(camera *cam, float y);
void camSetZ(camera *cam, float z);
void camSet(camera *cam, float x, float y, float z);

void camRotateX(camera *cam, float x);
void camRotateY(camera *cam, float y);
void camRotateZ(camera *cam, float z);
void camRotate(camera *cam, float x, float y, float z);

void camSetRotationX(camera *cam, float x);
void camSetRotationY(camera *cam, float y);
void camSetRotationZ(camera *cam, float z);
void camSetRotation(camera *cam, float x, float y, float z);

void camMoveTargetX(camera *cam, float x);
void camMoveTargetY(camera *cam, float y);
void camMoveTargetZ(camera *cam, float z);
void camMoveTarget(camera *cam, float x, float y, float z);

void camSetTargetX(camera *cam, float x);
void camSetTargetY(camera *cam, float y);
void camSetTargetZ(camera *cam, float z);
void camSetTarget(camera *cam, float x, float y, float z);

void camSetUp(camera *cam, float x, float y, float z);

void camSetFOV(camera *cam, float fov);

#endif
