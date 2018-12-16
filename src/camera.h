#ifndef CAMERA_H
#define CAMERA_H

#include "graphicsViewport.h"
#include "interpState.h"
#include "mat4.h"
#include "flags.h"

#define CAM_INACTIVE            0x00
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
	///vec3 previousRotation;
	/** Remove the ResetInterp() functions. **/
	interpVec3 targetPosition;
	interpVec3 up;
	interpFloat fovy;

	gfxView view;

	/** Not happy about storing the view matrix here. **/
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 viewProjectionMatrix;

	flags_t flags;

} camera;

return_t camInit(camera *const restrict cam);
void camResetInterpolation(camera *const restrict cam);

void camCalculateUp(camera *const restrict cam);
void camUpdateViewMatrix(camera *const restrict cam, const float interpT);
void camUpdateProjectionMatrix(camera *const restrict cam, const float windowAspectRatioX, const float windowAspectRatioY, const float interpT);
void camUpdateViewProjectionMatrix(camera *const restrict cam, const unsigned int windowModified, const float windowAspectRatioX, const float windowAspectRatioY, const float interpT);

float camDistance(const camera *const restrict cam, const vec3 *const restrict target);

void camDelete(camera *const restrict cam);

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
