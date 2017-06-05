#include "camera.h"
#include "mat4.h"
#include "quat.h"
#include "math.h"

#define radianRatio 0.017453292  // = PI / 180, used for converting degrees to radians

void camInit(camera *cam){
	vec3Set(&cam->position, 0.f, 2.f, 7.f);
	vec3Set(&cam->rotation, 0.f, 0.f, 0.f);
	vec3Set(&cam->target, 0.f, 0.f, -1.f);
	vec3Set(&cam->up, 0.f, 1.f, 0.f);
	cam->time = 0.f;
	cam->changed = 1;
}

void camCalculateUp(camera *cam){  /** Probably not entirely necessary **/

	/* Finds the relative up direction based off where the camera is looking */

	// Normalize the target vector
	float magnitude = sqrt(cam->target.x * cam->target.x +
	                       cam->target.y * cam->target.y +
	                       cam->target.z * cam->target.z);

	// If thetarget is (0, 0, 0), the magnitude will be 0 and
	// we'll get a divide by zero error
	if(magnitude != 0){

		vec3 targetUnit = cam->target;
		targetUnit.x /= magnitude;
		targetUnit.y /= magnitude;
		targetUnit.z /= magnitude;

		// Calculate the cross product of the normalized target vector and the
		// vector (0, 1, 0). The result is a vector pointing to the right
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

	}

}

unsigned char camCreateViewMatrix(camera *cam, mat4 *viewMatrix){

	// Only update the view matrix if the camera was changed during the frame
	if(cam->changed){

		// Calculate the up vector
		camCalculateUp(cam);

		// Set the camera to look at something
		mat4LookAt(viewMatrix, cam->position, cam->target, cam->up);

		// Rotate the camera
		vec3 rotationRadians = cam->rotation;
		vec3MultVByS(&rotationRadians, radianRatio);
		mat4Rotate(viewMatrix, quatNewEuler(rotationRadians));

		cam->changed = 0;
		return 1;

	}

	return 0;

}

void camMoveX(camera *cam, float x){
	cam->position.x += x;
	cam->changed = 1;
}

void camMoveY(camera *cam, float y){
	cam->position.y += y;
	cam->changed = 1;
}

void camMoveZ(camera *cam, float z){
	cam->position.z += z;
	cam->changed = 1;
}

void camMove(camera *cam, float x, float y, float z){
	cam->position.x += x;
	cam->position.y += y;
	cam->position.z += z;
	cam->changed = 1;
}

void camSetX(camera *cam, float x){
	cam->position.x = x;
	cam->changed = 1;
}

void camSetY(camera *cam, float y){
	cam->position.y = y;
	cam->changed = 1;
}

void camSetZ(camera *cam, float z){
	cam->position.z = z;
	cam->changed = 1;
}

void camSet(camera *cam, float x, float y, float z){
	cam->position.x = x;
	cam->position.y = y;
	cam->position.z = z;
	cam->changed = 1;
}

void camRotateX(camera *cam, float x){
	cam->rotation.x += x;
	cam->changed = 1;
}

void camRotateY(camera *cam, float y){
	cam->rotation.y += y;
	cam->changed = 1;
}

void camRotateZ(camera *cam, float z){
	cam->rotation.z += z;
	cam->changed = 1;
}

void camRotate(camera *cam, float x, float y, float z){
	cam->rotation.x += x;
	cam->rotation.y += y;
	cam->rotation.z += z;
	cam->changed = 1;
}

void camSetRotationX(camera *cam, float x){
	cam->rotation.x = x;
	cam->changed = 1;
}

void camSetRotationY(camera *cam, float y){
	cam->rotation.y = y;
	cam->changed = 1;
}

void camSetRotationZ(camera *cam, float z){
	cam->rotation.z = z;
	cam->changed = 1;
}

void camSetRotation(camera *cam, float x, float y, float z){
	cam->rotation.x = x;
	cam->rotation.y = y;
	cam->rotation.z = z;
	cam->changed = 1;
}

void camMoveTargetX(camera *cam, float x){
	cam->target.x += x;
	cam->changed = 1;
}

void camMoveTargetY(camera *cam, float y){
	cam->target.y += y;
	cam->changed = 1;
}

void camMoveTargetZ(camera *cam, float z){
	cam->target.z += z;
	cam->changed = 1;
}

void camMoveTarget(camera *cam, float x, float y, float z){
	cam->target.x += x;
	cam->target.y += y;
	cam->target.z += z;
	cam->changed = 1;
}

void camSetTargetX(camera *cam, float x){
	cam->target.x = x;
	cam->changed = 1;
}

void camSetTargetY(camera *cam, float y){
	cam->target.y = y;
	cam->changed = 1;
}

void camSetTargetZ(camera *cam, float z){
	cam->target.z = z;
	cam->changed = 1;
}

void camSetTarget(camera *cam, float x, float y, float z){
	cam->target.x = x;
	cam->target.y = y;
	cam->target.z = z;
	cam->changed = 1;
}
