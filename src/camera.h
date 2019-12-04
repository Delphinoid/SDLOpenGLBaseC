#ifndef CAMERA_H
#define CAMERA_H

#include "graphicsViewport.h"
/** Really want to remove interpState.h. **/
#include "interpState.h"
#include "mat4.h"
#include "flags.h"

#define CAM_INACTIVE                0x00
#define CAM_PROJECTION_FRUSTUM      0x01
#define CAM_PROJECTION_ORTHOGRAPHIC 0x02

#define CAM_BILLBOARD_DISABLED      0x00
#define CAM_BILLBOARD_X             0x01  // Whether or not the object uses the camera's rotated X axis.
#define CAM_BILLBOARD_Y             0x02  // Whether or not the object uses the camera's rotated Y axis.
#define CAM_BILLBOARD_Z             0x04  // Whether or not the object uses the camera's rotated Z axis.
#define CAM_BILLBOARD_XYZ           0x07
#define CAM_BILLBOARD_SCALE         0x08  // Rescale the object based on the distance from the camera.
#define CAM_BILLBOARD_SPRITE        0x10  // A cheap billboard method for sprites.
#define CAM_BILLBOARD_TARGET        0x20  // Billboard towards a specified target.
#define CAM_BILLBOARD_TARGET_CAMERA 0x40  // Billboard towards the camera's position.
#define CAM_BILLBOARD_TARGET_SPRITE 0x80  // Billboard like a sprite with support for axis locking.

#ifndef CAM_BILLBOARD_SCALE_CALIBRATION_DISTANCE
	#define CAM_BILLBOARD_SCALE_CALIBRATION_DISTANCE 7.5f  // How far away an object should be for a 1:1 scale.
#endif

typedef struct {

	/** Should be in a struct? **/
	interpVec3 position;
	interpQuat orientation;

	/** Do we REALLY need previousRotation? **/
	///vec3 previousRotation;
	/** Remove the ResetInterp() functions. **/
	interpVec3 target;
	interpVec3 up;
	interpFloat fovy;

	gfxView view;

	/** Not happy about storing the view matrix here. **/
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 viewProjectionMatrix;

	flags_t flags;

} camera;

void camInit(camera *const restrict cam);
void camResetInterpolation(camera *const restrict cam);

void camCalculateUp(camera *const restrict cam);
void camUpdateViewMatrix(camera *const restrict cam, const float interpT);
void camUpdateProjectionMatrix(camera *const restrict cam, const float windowAspectRatioX, const float windowAspectRatioY, const float interpT);
void camUpdateViewProjectionMatrix(camera *const restrict cam, const unsigned int windowModified, const float windowAspectRatioX, const float windowAspectRatioY, const float interpT);

float camDistance(const camera *const restrict cam, const vec3 target);

mat4 camBillboard(const camera *const restrict cam, const vec3 position, mat4 configuration, const flags_t flags);

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
