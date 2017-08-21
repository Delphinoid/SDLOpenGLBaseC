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
void camUpdateProjectionMatrix(camera *cam, const unsigned char aspectRatioX, const unsigned char aspectRatioY);

void camMoveX(camera *cam, const float x);
void camMoveY(camera *cam, const float y);
void camMoveZ(camera *cam, const float z);
void camMove(camera *cam, const float x, const float y, const float z);

void camSetX(camera *cam, const float x);
void camSetY(camera *cam, const float y);
void camSetZ(camera *cam, const float z);
void camSet(camera *cam, const float x, const float y, const float z);

void camRotateX(camera *cam, const float x);
void camRotateY(camera *cam, const float y);
void camRotateZ(camera *cam, const float z);
void camRotate(camera *cam, const float x, const float y, const float z);

void camSetRotationX(camera *cam, const float x);
void camSetRotationY(camera *cam, const float y);
void camSetRotationZ(camera *cam, const float z);
void camSetRotation(camera *cam, const float x, const float y, const float z);

void camMoveTargetX(camera *cam, const float x);
void camMoveTargetY(camera *cam, const float y);
void camMoveTargetZ(camera *cam, const float z);
void camMoveTarget(camera *cam, const float x, const float y, const float z);

void camSetTargetX(camera *cam, const float x);
void camSetTargetY(camera *cam, const float y);
void camSetTargetZ(camera *cam, const float z);
void camSetTarget(camera *cam, const float x, const float y, const float z);

void camSetUp(camera *cam, const float x, const float y, const float z);

void camSetFOV(camera *cam, const float fov);

#endif
