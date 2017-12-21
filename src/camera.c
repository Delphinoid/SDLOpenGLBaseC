#include "camera.h"
#include "mat4.h"
#include <math.h>

#define RADIAN_RATIO 0.017453292  /* = PI / 180, used for converting degrees to radians */

void camInit(camera *cam){
	iVec3Init(&cam->position, 0.f, 0.f, 0.f);
	iQuatInit(&cam->orientation);
	vec3Set(&cam->rotation, 0.f, 0.f, 0.f);
	vec3Set(&cam->previousRotation, 0.f, 0.f, 0.f);
	iVec3Init(&cam->targetPosition, 0.f, 0.f, -1.f);
	iVec3Init(&cam->up, 0.f, 1.f, 0.f);
	iFloatInit(&cam->fovy, 90.f);
	mat4Identity(&cam->viewMatrix);
	mat4Identity(&cam->projectionMatrix);
	cam->targetScene = NULL;
	cam->flags = CAM_UPDATE_VIEW | CAM_UPDATE_PROJECTION;
}

void camStateCopy(const camera *o, camera *c){
	c->position = o->position;
	c->orientation = o->orientation;
	c->rotation = o->rotation;
	c->previousRotation = o->previousRotation;
	c->targetPosition = o->targetPosition;
	c->up = o->up;
	c->fovy = o->fovy;
	c->viewMatrix = o->viewMatrix;
	if((o->flags & CAM_UPDATE_VIEW) == 0){
		/*
		** Never copy the view matrix as it is always generated before a render,
		** depending on the interp values.
		** Only copy the projection matrix if it is not likely to change before
		** a render.
		*/
		c->projectionMatrix = o->projectionMatrix;
	}
	c->targetScene = o->targetScene;
	c->flags = o->flags;
}

void camResetInterpolation(camera *cam){
	iVec3ResetInterp(&cam->position);
	iQuatResetInterp(&cam->orientation);
	iVec3ResetInterp(&cam->targetPosition);
	iVec3ResetInterp(&cam->up);
	iFloatResetInterp(&cam->fovy);
}
void camCalculateUp(camera *cam){  /** Probably not entirely necessary **/

	/*
	** Finds the relative up direction based off where the camera is looking
	*/

	/* Normalize the target vector. *
	float magnitude = sqrt(cam->target.x * cam->target.x +
	                       cam->target.y * cam->target.y +
	                       cam->target.z * cam->target.z);

	* If thetarget is (0, 0, 0), the magnitude will be 0 and */
	/* we'll get a divide by zero error *
	if(magnitude != 0){

		vec3 targetUnit = cam->target;
		targetUnit.x /= magnitude;
		targetUnit.y /= magnitude;
		targetUnit.z /= magnitude;

		* Calculate the cross product of the normalized target vector and the */
		/* vector (0, 1, 0). The result is a vector pointing to the right *
		vec3 rightVector;
		rightVector.x = targetUnit.z;
		rightVector.y = 0.f;
		rightVector.z = targetUnit.x;

		* Calculate the cross product of the normalized target vector and the */
		/* right vector. This is the vector we need *
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
	quatRotateVec3(&camRotation, &cam->up);*/

}
void camUpdateViewMatrix(camera *cam, const float interpT){

	// Check if the camera was rotated.
	if(cam->rotation.x != cam->previousRotation.x ||
	   cam->rotation.y != cam->previousRotation.y ||
	   cam->rotation.z != cam->previousRotation.z){
		// Update orientation.
		quatSetEuler(&cam->orientation.value, cam->rotation.x*RADIAN_RATIO,
		                                      cam->rotation.y*RADIAN_RATIO,
		                                      cam->rotation.z*RADIAN_RATIO);
		cam->previousRotation = cam->rotation;
	}

	/* Only generate a new view matrix if the camera viewport has changed in any way. */
	if(iVec3Update(&cam->position,       interpT) |
	   iQuatUpdate(&cam->orientation,    interpT) |
	   iVec3Update(&cam->targetPosition, interpT) |
	   iVec3Update(&cam->up,             interpT) ||
	   (cam->flags & CAM_UPDATE_VIEW) > 0){

		/* Calculate the up vector */
		/**camCalculateUp(cam);**/

		/* Set the camera to look at something */
		mat4LookAt(&cam->viewMatrix, &cam->position.render,
		                             &cam->targetPosition.render,
		                             &cam->up.render);

		/* Rotate the camera */
		mat4Rotate(&cam->viewMatrix, &cam->orientation.render);

		cam->flags &= ~CAM_UPDATE_VIEW;

	}

}
void camUpdateProjectionMatrix(camera *cam, const unsigned char aspectRatioX, const unsigned char aspectRatioY, const float interpT){

	if(((cam->flags & CAM_PROJECTION_ORTHO) == 0 && iFloatUpdate(&cam->fovy, interpT)) || (cam->flags & CAM_UPDATE_PROJECTION) > 0){

		if((cam->flags & CAM_PROJECTION_ORTHO) == 0){

			// CAM_PROJECTION_TYPE is not set, the camera is using a frustum projection matrix
			mat4Perspective(&cam->projectionMatrix, cam->fovy.render*RADIAN_RATIO, (float)aspectRatioX / (float)aspectRatioY, 0.1f/cam->fovy.render, 1000.f);

		}else{

			// CAM_PROJECTION_TYPE is set, the camera is using an orthographic projection matrix
			mat4Ortho(&cam->projectionMatrix,
			          0.f, (float)aspectRatioX / (float)(aspectRatioX < aspectRatioY ? aspectRatioX : aspectRatioY),
			          0.f, (float)aspectRatioY / (float)(aspectRatioX < aspectRatioY ? aspectRatioX : aspectRatioY),
			          -1000.f, 1000.f);

		}

		cam->flags &= ~CAM_UPDATE_PROJECTION;

	}

}

void camDelete(camera *cam){
	//
}

/**void camMoveX(camera *cam, const float x){
	cam->position.x += x;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camMoveY(camera *cam, const float y){
	cam->position.y += y;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camMoveZ(camera *cam, const float z){
	cam->position.z += z;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camMove(camera *cam, const float x, const float y, const float z){
	cam->position.x += x;
	cam->position.y += y;
	cam->position.z += z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camSetX(camera *cam, const float x){
	cam->position.x = x;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetY(camera *cam, const float y){
	cam->position.y = y;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetZ(camera *cam, const float z){
	cam->position.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSet(camera *cam, const float x, const float y, const float z){
	cam->position.x = x;
	cam->position.y = y;
	cam->position.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camRotateX(camera *cam, const float x){
	cam->rotation.x += x;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camRotateY(camera *cam, const float y){
	cam->rotation.y += y;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camRotateZ(camera *cam, const float z){
	cam->rotation.z += z;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camRotate(camera *cam, const float x, const float y, const float z){
	cam->rotation.x += x;
	cam->rotation.y += y;
	cam->rotation.z += z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camSetRotationX(camera *cam, const float x){
	cam->rotation.x = x;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetRotationY(camera *cam, const float y){
	cam->rotation.y = y;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetRotationZ(camera *cam, const float z){
	cam->rotation.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetRotation(camera *cam, const float x, const float y, const float z){
	cam->rotation.x = x;
	cam->rotation.y = y;
	cam->rotation.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camMoveTargetX(camera *cam, const float x){
	cam->target.x += x;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camMoveTargetY(camera *cam, const float y){
	cam->target.y += y;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camMoveTargetZ(camera *cam, const float z){
	cam->target.z += z;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camMoveTarget(camera *cam, const float x, const float y, const float z){
	cam->target.x += x;
	cam->target.y += y;
	cam->target.z += z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camSetTargetX(camera *cam, const float x){
	cam->target.x = x;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetTargetY(camera *cam, const float y){
	cam->target.y = y;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetTargetZ(camera *cam, const float z){
	cam->target.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetTarget(camera *cam, const float x, const float y, const float z){
	cam->target.x = x;
	cam->target.y = y;
	cam->target.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camSetUp(camera *cam, const float x, const float y, const float z){
	cam->up.x = x;
	cam->up.y = y;
	cam->up.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camSetFOV(camera *cam, const float fov){
	cam->fovy = fov;
	cam->flags |= CAM_UPDATE_PROJECTION;
}**/
