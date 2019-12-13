#include "camera.h"
#include "constantsMath.h"
#include <math.h>

void camInit(camera *const restrict cam){
	iVec3Init(&cam->position, 0.f, 0.f, 0.f);
	iQuatInit(&cam->orientation);
	///vec3Set(cam->previousRotation, 0.f, 0.f, 0.f);
	iVec3Init(&cam->target, 0.f, 0.f, -1.f);
	iVec3Init(&cam->up, 0.f, 1.f, 0.f);
	iFloatInit(&cam->fovy, 90.f);
	gfxViewInit(&cam->view);
	cam->viewMatrix = mat4Identity();
	cam->projectionMatrix = mat4Identity();
	cam->viewProjectionMatrix = mat4Identity();
	cam->flags = 0;
}
void camResetInterpolation(camera *const restrict cam){
	iVec3ResetInterp(&cam->position);
	iQuatResetInterp(&cam->orientation);
	iVec3ResetInterp(&cam->target);
	iVec3ResetInterp(&cam->up);
	iFloatResetInterp(&cam->fovy);
}

void camUpdateViewMatrix(camera *const restrict cam, const float interpT){
	// Set the camera to look at something.
	cam->viewMatrix = mat4LookAt(
		cam->position.render,
		cam->target.render,
		cam->up.render
	);
	// Rotate the camera.
	cam->viewMatrix = mat4MMultM(mat4RotationMatrix(cam->orientation.render), cam->viewMatrix);
}
void camUpdateProjectionMatrix(camera *const restrict cam, const float windowWidth, const float windowHeight, const float interpT){
	if(flagsAreSet(cam->flags, CAM_PROJECTION_FRUSTUM)){
		// CAM_PROJECTION_FRUSTUM is set, the camera is using a frustum projection matrix
		cam->projectionMatrix = mat4Perspective(cam->fovy.render*RADIAN_RATIO, windowWidth / windowHeight, 0.1f/cam->fovy.render, 1000.f);
	}else if(flagsAreSet(cam->flags, CAM_PROJECTION_ORTHOGRAPHIC)){
		// CAM_PROJECTION_ORTHOGRAPHIC is set, the camera is using an orthographic projection matrix
		cam->projectionMatrix = mat4Ortho(0.f, windowWidth / (windowWidth < windowHeight ? windowWidth : windowHeight),
										  0.f, windowHeight / (windowWidth < windowHeight ? windowWidth : windowHeight),
										  -1000.f, 1000.f);
	}else if(flagsAreSet(cam->flags, CAM_PROJECTION_OVERLAY)){
		// OpenGL coordinates have the center at 0 with the sides
		// at -1 and 1, resulting in a total window size of 2x2.
		cam->projectionMatrix = mat4ScaleMatrix(2.f/windowWidth, 2.f/windowHeight, 0.f);
	}else{
		cam->projectionMatrix = mat4Identity();
	}
}
void camUpdateViewProjectionMatrix(camera *const restrict cam, const unsigned int windowModified, const float windowWidth, const float windowHeight, const float interpT){

	// Only generate a new view matrix if the camera viewport has changed in any way.
	const int viewUpdate = iVec3Update(&cam->position, interpT) | iVec3Update(&cam->target, interpT) |
	                       iQuatUpdate(&cam->orientation, interpT) | iVec3Update(&cam->up, interpT);
	// If the FOV or window size changed, update the camera projection matrices as well.
	const int projectionUpdate = (flagsAreSet(cam->flags, CAM_PROJECTION_FRUSTUM) && iFloatUpdate(&cam->fovy, interpT)) || windowModified;

	if(viewUpdate){
		camUpdateViewMatrix(cam, interpT);
	}
	if(projectionUpdate){
		camUpdateProjectionMatrix(cam, windowWidth, windowHeight, interpT);
	}
	if(viewUpdate || projectionUpdate){
		//cam->viewProjectionMatrix = mat4MMultM(cam->viewMatrix, cam->projectionMatrix);
		cam->viewProjectionMatrix = mat4MMultM(cam->projectionMatrix, cam->viewMatrix);
	}

}

float camDistance(const camera *const restrict cam, const vec3 target){
	return sqrt(fabsf((target.x - cam->position.render.x) * (target.x - cam->position.render.x) +
	                  (target.y - cam->position.render.y) * (target.y - cam->position.render.y) +
	                  (target.z - cam->position.render.z) * (target.z - cam->position.render.z)));
}

void camDelete(camera *const restrict cam){
	//
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
