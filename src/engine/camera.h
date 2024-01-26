#ifndef CAMERA_H
#define CAMERA_H

#include "graphicsViewport.h"
/** Really want to remove interpState.h. **/
#include "interpState.h"
#include "mat4.h"
#include "flags.h"

#ifndef CAM_Z_THRESHOLD
	#define CAM_Z_THRESHOLD 1000.f
#endif

#define CAM_INACTIVE                0x00
#define CAM_PROJECTION_FRUSTUM      0x01
#define CAM_PROJECTION_ORTHOGRAPHIC 0x02
#define CAM_PROJECTION_FIXED_SIZE   0x04

typedef struct camera {

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

void camInit(camera *const __RESTRICT__ cam);
void camResetInterpolation(camera *const __RESTRICT__ cam);

void camCalculateUp(camera *const __RESTRICT__ cam);
void camUpdateViewMatrix(camera *const __RESTRICT__ cam, const float interpT);
void camUpdateProjectionMatrix(camera *const __RESTRICT__ cam, const float viewportWidth, const float viewportHeight, const float interpT);
void camUpdateViewProjectionMatrix(camera *const __RESTRICT__ cam, const unsigned int viewportModified, const float viewportWidth, const float viewportHeight, const float interpT);

float camDistance(const camera *const __RESTRICT__ cam, const vec3 target);
float camDistanceSquared(const camera *const __RESTRICT__ cam, const vec3 target);
float camSignedDistance(const camera *const __RESTRICT__ cam, const vec3 target);
float camSignedDistanceSquared(const camera *const __RESTRICT__ cam, const vec3 target);

void camDelete(camera *const __RESTRICT__ cam);

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
