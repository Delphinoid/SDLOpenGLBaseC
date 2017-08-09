#include "camera.h"
#include "mat4.h"
#include <math.h>

#define RADIAN_RATIO 0.017453292  /* = PI / 180, used for converting degrees to radians */

void camInit(camera *cam){
	vec3Set(&cam->position, 0.f, 0.f, 0.f);
	vec3Set(&cam->rotation, 0.f, 0.f, 0.f);
	vec3Set(&cam->target, 0.f, 0.f, -1.f);
	vec3Set(&cam->up, 0.f, 1.f, 0.f);
	cam->fovy = 90.f;
	mat4Identity(&cam->viewMatrix);
	mat4Identity(&cam->projectionMatrix);
	cam->flags = CAM_UPDATE_VIEW | CAM_UPDATE_PROJECTION;
}

void camCalculateUp(camera *cam){  /** Probably not entirely necessary **/

	/*
	** Finds the relative up direction based off where the camera is looking
	*/

	/* Normalize the target vector. */
	float magnitude = sqrt(cam->target.x * cam->target.x +
	                       cam->target.y * cam->target.y +
	                       cam->target.z * cam->target.z);

	/* If thetarget is (0, 0, 0), the magnitude will be 0 and */
	/* we'll get a divide by zero error */
	if(magnitude != 0){

		vec3 targetUnit = cam->target;
		targetUnit.x /= magnitude;
		targetUnit.y /= magnitude;
		targetUnit.z /= magnitude;

		/* Calculate the cross product of the normalized target vector and the */
		/* vector (0, 1, 0). The result is a vector pointing to the right */
		vec3 rightVector;
		rightVector.x = targetUnit.z;
		rightVector.y = 0.f;
		rightVector.z = targetUnit.x;

		/* Calculate the cross product of the normalized target vector and the */
		/* right vector. This is the vector we need */
		cam->up.x = targetUnit.y * rightVector.z;
		cam->up.y = targetUnit.z * rightVector.x - targetUnit.x * rightVector.z;
		cam->up.z = -targetUnit.y * rightVector.x;

	}else{

		cam->up.x = 0.f;
		cam->up.y = 1.f;
		cam->up.z = 0.f;

	}

}
void camUpdateViewMatrix(camera *cam){

	if((cam->flags & CAM_UPDATE_VIEW) > 0){

		/* Calculate the up vector */
		camCalculateUp(cam);

		/* Set the camera to look at something */
		mat4LookAt(&cam->viewMatrix, cam->position, cam->target, cam->up);

		/* Rotate the camera */
		mat4Rotate(&cam->viewMatrix, quatNewEuler(cam->rotation.x*RADIAN_RATIO, cam->rotation.y*RADIAN_RATIO, cam->rotation.z*RADIAN_RATIO));

		cam->flags &= ~CAM_UPDATE_VIEW;

	}

}
void camUpdateProjectionMatrix(camera *cam, unsigned char aspectRatioX, unsigned char aspectRatioY){

	if((cam->flags & CAM_UPDATE_PROJECTION) > 0){

		if((cam->flags & CAM_PROJECTION_ORTHO) == 0){

			// CAM_PROJECTION_TYPE is not set, the camera is using a frustum projection matrix
			mat4Perspective(&cam->projectionMatrix, cam->fovy*RADIAN_RATIO, (float)aspectRatioX / (float)aspectRatioY, 0.001f, 1000.f);

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

void camMoveX(camera *cam, float x){
	cam->position.x += x;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camMoveY(camera *cam, float y){
	cam->position.y += y;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camMoveZ(camera *cam, float z){
	cam->position.z += z;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camMove(camera *cam, float x, float y, float z){
	cam->position.x += x;
	cam->position.y += y;
	cam->position.z += z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camSetX(camera *cam, float x){
	cam->position.x = x;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetY(camera *cam, float y){
	cam->position.y = y;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetZ(camera *cam, float z){
	cam->position.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSet(camera *cam, float x, float y, float z){
	cam->position.x = x;
	cam->position.y = y;
	cam->position.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camRotateX(camera *cam, float x){
	cam->rotation.x += x;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camRotateY(camera *cam, float y){
	cam->rotation.y += y;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camRotateZ(camera *cam, float z){
	cam->rotation.z += z;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camRotate(camera *cam, float x, float y, float z){
	cam->rotation.x += x;
	cam->rotation.y += y;
	cam->rotation.z += z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camSetRotationX(camera *cam, float x){
	cam->rotation.x = x;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetRotationY(camera *cam, float y){
	cam->rotation.y = y;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetRotationZ(camera *cam, float z){
	cam->rotation.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetRotation(camera *cam, float x, float y, float z){
	cam->rotation.x = x;
	cam->rotation.y = y;
	cam->rotation.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camMoveTargetX(camera *cam, float x){
	cam->target.x += x;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camMoveTargetY(camera *cam, float y){
	cam->target.y += y;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camMoveTargetZ(camera *cam, float z){
	cam->target.z += z;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camMoveTarget(camera *cam, float x, float y, float z){
	cam->target.x += x;
	cam->target.y += y;
	cam->target.z += z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camSetTargetX(camera *cam, float x){
	cam->target.x = x;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetTargetY(camera *cam, float y){
	cam->target.y = y;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetTargetZ(camera *cam, float z){
	cam->target.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}
void camSetTarget(camera *cam, float x, float y, float z){
	cam->target.x = x;
	cam->target.y = y;
	cam->target.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camSetUp(camera *cam, float x, float y, float z){
	cam->up.x = x;
	cam->up.y = y;
	cam->up.z = z;
	cam->flags |= CAM_UPDATE_VIEW;
}

void camSetFOV(camera *cam, float fov){
	cam->fovy = fov;
	cam->flags |= CAM_UPDATE_PROJECTION;
}
