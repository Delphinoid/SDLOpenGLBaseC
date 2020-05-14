#include "billboard.h"
#include "camera.h"
#include "constantsMath.h"

#define BILLBOARD_ANIMATION_ANGLE (2.f*M_PI / ((float)BILLBOARD_ANIMATION_ANGLE_GRANULARITY))

void billboardInit(billboard *const __RESTRICT__ data){
	data->flags = BILLBOARD_DISABLED;
	data->axis = NULL;
	data->target = NULL;
	data->scale = 1.f/BILLBOARD_SCALE_CALIBRATION_DISTANCE;
}

mat4 billboardState(const billboard data, const camera *const __RESTRICT__ cam, const vec3 centroid, mat4 configuration){

	// Generates a billboard transformation matrix.
	// If no flags are set, returns the identity matrix.

	// Use a faster method for billboarding. Doesn't support individual axis locking.
	if(flagsAreSet(data.flags, BILLBOARD_SPRITE)){

		// Use the camera's X, Y and Z axes for cheap sprite billboarding.
		// This is technically an inverse matrix, so we need rows rather than columns.
		*((vec3 *)&configuration.m[0][0]) = vec3New(cam->viewMatrix.m[0][0], cam->viewMatrix.m[1][0], cam->viewMatrix.m[2][0]);

		if(data.axis != NULL){
			*((vec3 *)&configuration.m[1][0]) = *data.axis;
		}else{
			*((vec3 *)&configuration.m[1][0]) = vec3New(cam->viewMatrix.m[0][1], cam->viewMatrix.m[1][1], cam->viewMatrix.m[2][1]);
		}

		*((vec3 *)&configuration.m[2][0]) = vec3New(cam->viewMatrix.m[0][2], cam->viewMatrix.m[1][2], cam->viewMatrix.m[2][2]);

		configuration.m[0][3] = 0.f; configuration.m[1][3] = 0.f; configuration.m[2][3] = 0.f; configuration.m[3][3] = 1.f;

		if(flagsAreSet(data.flags, BILLBOARD_SCALE)){
			const float distance = camDistance(cam, centroid) * data.scale;
			configuration = mat4ScalePre(configuration, distance, distance, distance);
		}

	}else if(flagsAreSet(data.flags, BILLBOARD_LOCK_XYZ)){

		vec3 eye, target, up;

		///const vec3 translation = *((vec3 *)&configuration.m[3][0]);

		// Set the up vector. This corresponds
		// to the billboard rotation axis.
		if(data.axis != NULL){
			up = *data.axis;
		}else{
			up = cam->up.render;
		}

		// Set the eye and the target.
		if(flagsAreSet(data.flags, BILLBOARD_TARGET_SPRITE)){
			eye = cam->target.render;
			target = cam->position.render;
		}else if(data.target != NULL){
			eye = centroid;
			target = *data.target;
		}else{
			eye = centroid;
			target = cam->position.render;
		}

		// Lock certain axes if needed.
		if(flagsAreUnset(data.flags, BILLBOARD_LOCK_X)){
			target.y = eye.y;
		}
		if(flagsAreUnset(data.flags, BILLBOARD_LOCK_Y)){
			target.x = eye.x;
		}
		if(flagsAreUnset(data.flags, BILLBOARD_LOCK_Z)){
			up = vec3New(0.f, 1.f, 0.f);
		}

		// Translate the transformation to the "origin".
		configuration = mat4TranslatePre(configuration, -centroid.x, -centroid.y, -centroid.z);
		///vec3ZeroP((vec3 *)&configuration.m[3][0]);

		// Scale based on distance if necessary.
		if(flagsAreSet(data.flags, BILLBOARD_SCALE)){
			const float distance = camDistance(cam, centroid) * data.scale;
			configuration = mat4ScalePre(configuration, distance, distance, distance);
		}

		// Rotate to face and translate back.
		configuration = mat4TranslatePre(
			mat4MMultM(
				mat4RotateToFace(eye, target, up),
				configuration
			),
			centroid.x, centroid.y, centroid.z
		);

		// Translate back.
		///*((vec3 *)&configuration.m[3][0]) = translation;

	}else{

		if(flagsAreSet(data.flags, BILLBOARD_SCALE)){
			const float distance = camDistance(cam, centroid) * data.scale;
			configuration = mat4ScalePre(configuration, distance, distance, distance);
		}

	}

	return configuration;

}