#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "mat4.h"
#include "scene.h"

#define CAM_PROJECTION_ORTHO    0x01
#define CAM_UPDATE_PROJECTION   0x02
/** Is CAM_UPDATE_VIEW still needed? What about the others? **/
#define CAM_UPDATE_VIEW         0x04

typedef struct {
	/** Should be in a struct? **/
	interpVec3 position;
	interpQuat orientation;
	vec3 rotation;
	/** Do we REALLY need previousRotation? **/
	vec3 previousRotation;
	/** Remove the ResetInterp() functions. **/
	interpVec3 targetPosition;
	interpVec3 up;
	interpFloat fovy;
	/** Not happy about storing the view matrix here. **/
	mat4 viewMatrix;
	mat4 projectionMatrix;
	scene **targetScene;
	unsigned char flags;
} camera;

void camInit(camera *cam);
void camStateCopy(const camera *o, camera *c);
void camResetInterpolation(camera *cam);
void camCalculateUp(camera *cam);
void camUpdateViewMatrix(camera *cam, const float interpT);
void camUpdateProjectionMatrix(camera *cam, const unsigned char aspectRatioX, const unsigned char aspectRatioY, const float interpT);
void camDelete(camera *cam);

/**void camMoveX(camera *cam, const float x);
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

void camSetFOV(camera *cam, const float fov);**/

#endif
