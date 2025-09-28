#include "camera.h"
#include "constantsMath.h"
#include "helpersMath.h"
#include <math.h>

void camInit(camera *const __RESTRICT__ cam){
	iVec3Init(&cam->position, 0.f, 0.f, 0.f);
	iQuatInit(&cam->orientation);
	///vec3Set(cam->previousRotation, 0.f, 0.f, 0.f);
	iVec3Init(&cam->target, 0.f, 0.f, -1.f);
	iVec3Init(&cam->up, 0.f, 1.f, 0.f);
	iFloatInit(&cam->fovy, 90.f);
	gfxViewInit(&cam->view);
	cam->viewMatrix = g_mat4Identity;
	cam->projectionMatrix = g_mat4Identity;
	cam->viewProjectionMatrix = g_mat4Identity;
	cam->flags = 0;
}
void camResetInterpolation(camera *const __RESTRICT__ cam){
	iVec3ResetInterp(&cam->position);
	iQuatResetInterp(&cam->orientation);
	iVec3ResetInterp(&cam->target);
	iVec3ResetInterp(&cam->up);
	iFloatResetInterp(&cam->fovy);
}

void camUpdateViewMatrix(camera *const __RESTRICT__ cam, const float interpT){
	// Set the camera to look at something.
	cam->viewMatrix = mat4LookAt(
		cam->position.render,
		cam->target.render,
		cam->up.render
	);
	// Rotate the camera.
	cam->viewMatrix = mat4MMultM(mat4Quaternion(cam->orientation.render), cam->viewMatrix);
}
void camUpdateProjectionMatrix(camera *const __RESTRICT__ cam, const float viewportWidth, const float viewportHeight, const float interpT){
	if(flagsAreSet(cam->flags, CAM_PROJECTION_FRUSTUM)){
		// CAM_PROJECTION_FRUSTUM is set, the camera is using a frustum projection matrix
		cam->projectionMatrix = mat4Perspective(cam->fovy.render*RADIAN_RATIO, viewportWidth / viewportHeight, 0.1f/cam->fovy.render, CAM_Z_THRESHOLD);
	}else if(flagsAreSet(cam->flags, CAM_PROJECTION_ORTHOGRAPHIC)){
		// CAM_PROJECTION_ORTHOGRAPHIC is set, the camera is using an orthographic projection matrix
		cam->projectionMatrix = mat4Ortho(0.f, viewportWidth / (viewportWidth < viewportHeight ? viewportWidth : viewportHeight),
										  0.f, viewportHeight / (viewportWidth < viewportHeight ? viewportWidth : viewportHeight),
										  CAM_Z_THRESHOLD, -CAM_Z_THRESHOLD);
	}else if(flagsAreSet(cam->flags, CAM_PROJECTION_FIXED_SIZE)){
		// OpenGL coordinates have the center at 0 with the sides
		// at -1 and 1, resulting in a total window size of 2x2.
		// We also need to take into account GL pixel coordinates
		// being in the center of the pixel.
		cam->projectionMatrix = mat4Ortho(-viewportWidth*0.5f + 0.5f, viewportWidth*0.5f - 0.5f, -viewportHeight*0.5f + 0.5f, viewportHeight*0.5f - 0.5f, CAM_Z_THRESHOLD, -CAM_Z_THRESHOLD);
		///cam->projectionMatrix = mat4Ortho(0.f, viewportWidth, -viewportHeight, 0.f, CAM_Z_THRESHOLD, -CAM_Z_THRESHOLD);
	}else{
		cam->projectionMatrix = g_mat4Identity;
	}
}
void camUpdateViewProjectionMatrix(camera *const __RESTRICT__ cam, const unsigned int viewportModified, const float viewportWidth, const float viewportHeight, const float interpT){

	// Only generate a new view matrix if the camera viewport has changed in any way.
	const int viewUpdate = iVec3Update(&cam->position, interpT) | iVec3Update(&cam->target, interpT) |
	                       iQuatUpdate(&cam->orientation, interpT) | iVec3Update(&cam->up, interpT);
	// If the FOV or window size changed, update the camera projection matrices as well.
	const int projectionUpdate = (flagsAreSet(cam->flags, CAM_PROJECTION_FRUSTUM) && iFloatUpdate(&cam->fovy, interpT)) || viewportModified;

	if(viewUpdate){
		camUpdateViewMatrix(cam, interpT);
	}
	if(projectionUpdate){
		camUpdateProjectionMatrix(cam, viewportWidth, viewportHeight, interpT);
	}
	if(viewUpdate || projectionUpdate){
		///cam->viewProjectionMatrix = mat4MMultM(cam->viewMatrix, cam->projectionMatrix);
		cam->viewProjectionMatrix = mat4MMultM(cam->projectionMatrix, cam->viewMatrix);
	}

}

float camDistance(const camera *const __RESTRICT__ cam, const vec3 target){
	return vec3Magnitude(vec3VSubV(target, cam->position.render));
}
float camDistanceSquared(const camera *const __RESTRICT__ cam, const vec3 target){
	return vec3MagnitudeSquared(vec3VSubV(target, cam->position.render));
}
float camSignedDistance(const camera *const __RESTRICT__ cam, const vec3 target){
	const vec3 displacement = vec3VSubV(target, cam->position.render);
	return copySign(vec3Magnitude(displacement), vec3Dot(displacement, vec3New(cam->viewMatrix.m[0][2], cam->viewMatrix.m[1][2], cam->viewMatrix.m[2][2])));
}
float camSignedDistanceSquared(const camera *const __RESTRICT__ cam, const vec3 target){
	const vec3 displacement = vec3VSubV(target, cam->position.render);
	return copySign(vec3MagnitudeSquared(displacement), vec3Dot(displacement, vec3New(cam->viewMatrix.m[0][2], cam->viewMatrix.m[1][2], cam->viewMatrix.m[2][2])));
}

void camDelete(camera *const __RESTRICT__ cam){
	///
}

/**void camMoveX(camera *cam, const float x){
	cam->position.x += x;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camMoveY(camera *cam, const float y){
	cam->position.y += y;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camMoveZ(camera *cam, const float z){
	cam->position.z += z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camMove(camera *cam, const float x, const float y, const float z){
	cam->position.x += x;
	cam->position.y += y;
	cam->position.z += z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}

void camSetX(camera *cam, const float x){
	cam->position.x = x;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camSetY(camera *cam, const float y){
	cam->position.y = y;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camSetZ(camera *cam, const float z){
	cam->position.z = z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camSet(camera *cam, const float x, const float y, const float z){
	cam->position.x = x;
	cam->position.y = y;
	cam->position.z = z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}

void camRotateX(camera *cam, const float x){
	cam->rotation.x += x;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camRotateY(camera *cam, const float y){
	cam->rotation.y += y;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camRotateZ(camera *cam, const float z){
	cam->rotation.z += z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camRotate(camera *cam, const float x, const float y, const float z){
	cam->rotation.x += x;
	cam->rotation.y += y;
	cam->rotation.z += z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}

void camSetRotationX(camera *cam, const float x){
	cam->rotation.x = x;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camSetRotationY(camera *cam, const float y){
	cam->rotation.y = y;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camSetRotationZ(camera *cam, const float z){
	cam->rotation.z = z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camSetRotation(camera *cam, const float x, const float y, const float z){
	cam->rotation.x = x;
	cam->rotation.y = y;
	cam->rotation.z = z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}

void camMoveTargetX(camera *cam, const float x){
	cam->target.x += x;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camMoveTargetY(camera *cam, const float y){
	cam->target.y += y;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camMoveTargetZ(camera *cam, const float z){
	cam->target.z += z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camMoveTarget(camera *cam, const float x, const float y, const float z){
	cam->target.x += x;
	cam->target.y += y;
	cam->target.z += z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}

void camSetTargetX(camera *cam, const float x){
	cam->target.x = x;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camSetTargetY(camera *cam, const float y){
	cam->target.y = y;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camSetTargetZ(camera *cam, const float z){
	cam->target.z = z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camSetTarget(camera *cam, const float x, const float y, const float z){
	cam->target.x = x;
	cam->target.y = y;
	cam->target.z = z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}

void camSetUp(camera *cam, const float x, const float y, const float z){
	cam->up.x = x;
	cam->up.y = y;
	cam->up.z = z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}

void camSetFOV(camera *cam, const float fov){
	cam->fovy = fov;
	flagsSet(cam->flags, CAM_UPDATE_PROJECTION);
}**/
