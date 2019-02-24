#include "camera.h"
#include "constantsMath.h"
#include <math.h>

void camInit(camera *const restrict cam){
	iVec3Init(&cam->position, 0.f, 0.f, 0.f);
	iQuatInit(&cam->orientation);
	vec3Set(&cam->rotation, 0.f, 0.f, 0.f);
	///vec3Set(cam->previousRotation, 0.f, 0.f, 0.f);
	iVec3Init(&cam->targetPosition, 0.f, 0.f, -1.f);
	iVec3Init(&cam->up, 0.f, 1.f, 0.f);
	iFloatInit(&cam->fovy, 90.f);
	gfxViewInit(&cam->view);
	mat4Identity(&cam->viewMatrix);
	mat4Identity(&cam->projectionMatrix);
	mat4Identity(&cam->viewProjectionMatrix);
	cam->flags = CAM_UPDATE_VIEW | CAM_UPDATE_PROJECTION;
}
void camResetInterpolation(camera *const restrict cam){
	iVec3ResetInterp(&cam->position);
	iQuatResetInterp(&cam->orientation);
	iVec3ResetInterp(&cam->targetPosition);
	iVec3ResetInterp(&cam->up);
	iFloatResetInterp(&cam->fovy);
}

void camCalculateUp(camera *const restrict cam){  /** Probably not entirely necessary **/

	/*
	** Finds the relative up direction based off where the camera is looking
	*/

	/*// Normalize the target vector.
	float magnitude = sqrt(cam->targetPosition.x * cam->targetPosition.x +
	                       cam->targetPosition.y * cam->targetPosition.y +
	                       cam->targetPosition.z * cam->targetPosition.z);

	// If thetarget is (0, 0, 0), the magnitude will be 0 and
	// we'll get a divide by zero error.
	if(magnitude != 0){

		vec3 targetUnit = cam->targetPosition;
		targetUnit.x /= magnitude;
		targetUnit.y /= magnitude;
		targetUnit.z /= magnitude;

		// Calculate the cross product of the normalized target vector and the
		// vector (0, 1, 0). The result is a vector pointing to the right.
		vec3 rightVector;
		rightVector.x = targetUnit.z;
		rightVector.y = 0.f;
		rightVector.z = targetUnit.x;

		// Calculate the cross product of the normalized target vector and the
		// right vector. This is the vector we need.
		cam->up.x = targetUnit.y * rightVector.z;
		cam->up.y = targetUnit.z * rightVector.x - targetUnit.x * rightVector.z;
		cam->up.z = -targetUnit.y * rightVector.x;

	}else{

		cam->up.x = 0.f;
		cam->up.y = 1.f;
		cam->up.z = 0.f;

	}*/

	/*quat camRotation;
	quatSetEuler(&camRotation, cam->rotation.x*RADIAN_RATIO,
	                           cam->rotation.y*RADIAN_RATIO,
	                           cam->rotation.z*RADIAN_RATIO);
	vec3Set(&cam->up, 0.f, 1.f, 0.f);
	quatRotateVec3Fast(&camRotation, &cam->up);*/

}
void camUpdateViewMatrix(camera *const restrict cam, const float interpT){

	// Check if the camera was rotated.
	/**if(cam->rotation.x != cam->previousRotation.x ||
	   cam->rotation.y != cam->previousRotation.y ||
	   cam->rotation.z != cam->previousRotation.z){**/
		// Update orientation.
		quatSetEuler(&cam->orientation.value, cam->rotation.x*RADIAN_RATIO,
		                                      cam->rotation.y*RADIAN_RATIO,
		                                      cam->rotation.z*RADIAN_RATIO);
		/**cam->previousRotation = cam->rotation;
	}**/

	// Only generate a new view matrix if the camera viewport has changed in any way.
	if(iVec3Update(&cam->position,       interpT) |
	   iQuatUpdate(&cam->orientation,    interpT) |
	   iVec3Update(&cam->targetPosition, interpT) |
	   iVec3Update(&cam->up,             interpT) ||
	   flagsAreSet(cam->flags, CAM_UPDATE_VIEW)){

		// Calculate the up vector.
		/**camCalculateUp(cam);**/

		// Set the camera to look at something.
		mat4LookAt(&cam->viewMatrix, &cam->position.render,
		                             &cam->targetPosition.render,
		                             &cam->up.render);

		// Rotate the camera.
		mat4Rotate(&cam->viewMatrix, &cam->orientation.render);

		flagsUnset(cam->flags, CAM_UPDATE_VIEW);

	}

}
void camUpdateProjectionMatrix(camera *const restrict cam, const float windowAspectRatioX, const float windowAspectRatioY, const float interpT){

	if((flagsAreUnset(cam->flags, CAM_PROJECTION_ORTHO) && iFloatUpdate(&cam->fovy, interpT)) || flagsAreSet(cam->flags, CAM_UPDATE_PROJECTION)){

		if(flagsAreUnset(cam->flags, CAM_PROJECTION_ORTHO)){

			// CAM_PROJECTION_TYPE is not set, the camera is using a frustum projection matrix
			mat4Perspective(&cam->projectionMatrix, cam->fovy.render*RADIAN_RATIO, windowAspectRatioX / windowAspectRatioY, 0.1f/cam->fovy.render, 1000.f);

		}else{

			// CAM_PROJECTION_TYPE is set, the camera is using an orthographic projection matrix
			mat4Ortho(&cam->projectionMatrix,
			          0.f, windowAspectRatioX / (windowAspectRatioX < windowAspectRatioY ? windowAspectRatioX : windowAspectRatioY),
			          0.f, windowAspectRatioY / (windowAspectRatioX < windowAspectRatioY ? windowAspectRatioX : windowAspectRatioY),
			          -1000.f, 1000.f);

		}

		flagsUnset(cam->flags, CAM_UPDATE_PROJECTION);

	}

}
void camUpdateViewProjectionMatrix(camera *const restrict cam, const unsigned int windowModified, const float windowAspectRatioX, const float windowAspectRatioY, const float interpT){
	camUpdateViewMatrix(cam, interpT);
	if(windowModified){
		// If the window size changed, update the camera projection matrices as well.
		flagsSet(cam->flags, CAM_UPDATE_PROJECTION);
	}
	camUpdateProjectionMatrix(cam, windowAspectRatioX, windowAspectRatioY, interpT);
	mat4MultMByMR(cam->viewMatrix, cam->projectionMatrix, &cam->viewProjectionMatrix);
}

float camDistance(const camera *const restrict cam, const vec3 *const restrict target){
	return sqrt(fabsf((target->x - cam->position.render.x) * (target->x - cam->position.render.x) +
	                  (target->y - cam->position.render.y) * (target->y - cam->position.render.y) +
	                  (target->z - cam->position.render.z) * (target->z - cam->position.render.z)));
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
	cam->targetPosition.x += x;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camMoveTargetY(camera *cam, const float y){
	cam->targetPosition.y += y;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camMoveTargetZ(camera *cam, const float z){
	cam->targetPosition.z += z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camMoveTarget(camera *cam, const float x, const float y, const float z){
	cam->targetPosition.x += x;
	cam->targetPosition.y += y;
	cam->targetPosition.z += z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}

void camSetTargetX(camera *cam, const float x){
	cam->targetPosition.x = x;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camSetTargetY(camera *cam, const float y){
	cam->targetPosition.y = y;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camSetTargetZ(camera *cam, const float z){
	cam->targetPosition.z = z;
	flagsSet(cam->flags, CAM_UPDATE_VIEW);
}
void camSetTarget(camera *cam, const float x, const float y, const float z){
	cam->targetPosition.x = x;
	cam->targetPosition.y = y;
	cam->targetPosition.z = z;
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
